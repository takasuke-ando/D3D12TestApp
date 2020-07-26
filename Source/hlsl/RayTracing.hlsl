
#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

#include "Raytracing.h"
#include "Lighting.hlslh"

// Global Root Signature
RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);

//  Local Root Signature
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);

//  [MaterialID]
ByteAddressBuffer           g_geomIndexBuffer[]     :   register(t16, space0);
StructuredBuffer<VtxAttrib> g_geomAttrib[]          :   register(t16, space1);



#define     HITGROUPOFFSET_RADIANCE      (0)
#define     HITGROUPOFFSET_SHADOW        (1)



typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};

struct ShadowPayload
{
    bool isHit;
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
        TraceRay(Scene, 
            RAY_FLAG_NONE, 
            ~0,     //  Instance Masks
            0,      //  RayContributionToHitGroupIndex                  :   
            0,      //  MultiplierForGeometryContributionToHitGroupIndex :  BLAS内Geometryのインデックスに、この値を掛けた結果がHitGroupのインデックスとなる
            0,      //  MissShaderIndex
            ray, 
            payload);


        float3 color = payload.color.rgb;

        // Tone Maping
        color = color / (color+1.f);

        // OETF
        color = pow(color, 1 / 2.2f);


        // Write the raytraced color to the output texture.
        RenderTarget[DispatchRaysIndex().xy] = float4(color,1);
    }// else
    //{
        // Render interpolated DispatchRaysIndex outside the stencil window
    //    RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
    //}
}



bool        ShadowRayHitTest(float3 worldPosition,float3 lightdir)
{


    RayDesc ray;
    ray.Origin = worldPosition;
    ray.Direction = lightdir;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;
    ShadowPayload payload = { false };
    //TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
    TraceRay(Scene,
        RAY_FLAG_NONE,
        ~0,     //  Instance Masks
        HITGROUPOFFSET_SHADOW,      //  RayContributionToHitGroupIndex                  :   
        0,                          //  MultiplierForGeometryContributionToHitGroupIndex :  BLAS内Geometryのインデックスに、この値を掛けた結果がHitGroupのインデックスとなる
        HITGROUPOFFSET_SHADOW,      //  MissShaderIndex
        ray,
        payload);

    return payload.isHit;

}



VtxAttrib   GetVtxAttrib(in uint MaterialID, in uint3 Indices, float2 barycentrics)
{

    VtxAttrib attr = (VtxAttrib)0;


    VtxAttrib attr0 = g_geomAttrib[MaterialID].Load(Indices.x);
    VtxAttrib attr1 = g_geomAttrib[MaterialID].Load(Indices.y);
    VtxAttrib attr2 = g_geomAttrib[MaterialID].Load(Indices.z);


    float3  factor = float3(1 - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);

    attr.Normal    = attr0.Normal * factor.x + attr1.Normal * factor.y + attr2.Normal * factor.z;
    attr.BaseColor = attr0.BaseColor * factor.x + attr1.BaseColor * factor.y + attr2.BaseColor * factor.z;

    return attr;

}



[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{

    const uint MaterialID = 0;

    const uint  primIndex = PrimitiveIndex();
    const float3 Indices =  (float3)GetIndices(MaterialID, primIndex);

    float intepolatedIndices = Indices.x + attr.barycentrics.x * (Indices.y - Indices.x) + attr.barycentrics.y * (Indices.z - Indices.x);

    intepolatedIndices /= 8;

    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);

    VtxAttrib vtx = GetVtxAttrib(MaterialID,Indices, attr.barycentrics);

    //payload.color = float4(barycentrics, 1);
    //payload.color = float4(intepolatedIndices, intepolatedIndices, intepolatedIndices, 1);


    float3  worldPosition = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();

    float3 radiance = (float3)0;

    // Directional Light

    Material mat;

    mat.DiffuseAlbedo = vtx.BaseColor;
    mat.SpecularAlbedo = float3(0.04f, 0.04f, 0.04f);
    mat.Roughness = 0.2f;
    mat.Normal = vtx.Normal;

    {
        DirectionalLight   lit;

        //lit.Dir = float3(0.f, 0.707106f, 0.707106f);
        lit.Dir = float3(0.f, 0.5f, sqrt(3)/2.f);
        lit.Irradiance = float3(1.f, 1.f, 1.f);


        bool isShadow = ShadowRayHitTest(worldPosition, lit.Dir);

        if (!isShadow) {
            radiance += ComputeDirectionalLight(mat, lit, normalize(-WorldRayDirection()));
        }
    }


    payload.color = float4(radiance, 1);
    //payload.color = float4(vtx.BaseColor, 1);

}


// Shadow Closest Hit Shader

[shader("closesthit")]
void MyClosestHitShader_Shadow(inout ShadowPayload payload, in MyAttributes attr)
{
    payload.isHit = true;
}


[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    payload.color = float4(0, 0, 0, 1);
}


[shader("miss")]
void MyMissShader_Shadow(inout ShadowPayload payload)
{
    payload.isHit = false;
}



#endif // RAYTRACING_HLSL