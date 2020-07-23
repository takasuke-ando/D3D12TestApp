
#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

#include "Raytracing.h"

// Global Root Signature
RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);

//  Local Root Signature
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);

//  [MaterialID]
ByteAddressBuffer   g_geomIndexBuffer[]   :   register(t16, space0);


typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};


uint3   GetIndices(uint materialID , uint primitiveID)
{


    //  16bit

    uint indexOffset = primitiveID * 2 * 3;
    uint dwordIndex = indexOffset & ~0x3;
    uint2 packedIndices = g_geomIndexBuffer[materialID].Load2(dwordIndex);

    uint3 indices;
    if (dwordIndex == indexOffset) {

        indices.x = ( packedIndices.x ) & 0xffff;
        indices.y = (packedIndices.x >> 16) & 0xffff;
        indices.z = (packedIndices.y  ) & 0xffff;

    } else {

        indices.x = (packedIndices.x >> 16) & 0xffff;
        indices.y = (packedIndices.y) & 0xffff;
        indices.z = (packedIndices.y >> 16) & 0xffff;

    }

    return indices;

}



bool IsInsideViewport(float2 p, Viewport viewport)
{
    return (p.x >= viewport.left && p.x <= viewport.right)
        && (p.y >= viewport.top && p.y <= viewport.bottom);
}

[shader("raygeneration")]
void MyRaygenShader()
{
    float2 lerpValues = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();

    // Orthographic projection since we're raytracing in screen space.
    /*
    float3 rayDir = float3(0, 0, 1);
    float3 origin = float3(
        lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
        lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
        0.0f);
       /*/
    float3 rayDir = float3(
        lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
        lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
        1.0f);
    float3 origin = float3(0, 0, 0);
    //*/

    rayDir = normalize(rayDir);

    rayDir = mul(float4(rayDir, 0.f), g_rayGenCB.mtxCamera).xyz;
    origin = mul( float4(origin,1.f), g_rayGenCB.mtxCamera).xyz;

    //if (IsInsideViewport(origin.xy, g_rayGenCB.stencil))
    {
        /*
            https://microsoft.github.io/DirectX-Specs/d3d/Raytracing.html#addressing-calculations-within-shader-tables
        */
        // Trace the ray.
        // Set the ray's extents.
        RayDesc ray;
        ray.Origin = origin;
        ray.Direction = rayDir;
        // Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
        // TMin should be kept small to prevent missing geometry at close contact areas.
        ray.TMin = 0.001;
        ray.TMax = 10000.0;
        RayPayload payload = { float4(0, 0, 0, 0) };
        //TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
        TraceRay(Scene, RAY_FLAG_NONE, 
            ~0,     //  Instance Masks
            0,      //  RayContributionToHitGroupIndex                  :   
            1,      //  MultiplierForGeometryContributionToHitGroupIndex :  BLASのインデックスに、この値を掛けた結果がHitGroupのインデックスとなる
            0,      //  MissShaderIndex
            ray, 
            payload);

        // Write the raytraced color to the output texture.
        RenderTarget[DispatchRaysIndex().xy] = payload.color;
    }// else
    //{
        // Render interpolated DispatchRaysIndex outside the stencil window
    //    RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
    //}
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{

    uint MaterialID = 0;

    uint  primIndex = PrimitiveIndex();
    float3 Indices =  (float3)GetIndices(MaterialID, primIndex);

    float intepolatedIndices = Indices.x + attr.barycentrics.x * (Indices.y - Indices.x) + attr.barycentrics.y * (Indices.z - Indices.x);

    intepolatedIndices /= 8;

    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);

    payload.color = float4(barycentrics, 1);
    //payload.color = float4(intepolatedIndices, intepolatedIndices, intepolatedIndices, 1);

}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    payload.color = float4(0, 0, 0, 1);
}

#endif // RAYTRACING_HLSL