#ifndef INCLUDE_RAYTRACING_H
#define INCLUDE_RAYTRACING_H






struct Viewport
{
    float left;
    float top;
    float right;
    float bottom;
};

struct RayGenConstantBuffer
{
    Viewport viewport;
    Viewport stencil;
    float4x4 mtxCamera; //  ÉJÉÅÉâçsóÒ
};

struct VtxAttrib
{
    float3  Normal;
    float3  BaseColor;
    float2  Uv;
};




#endif //INCLUDE_RAYTRACING_H

