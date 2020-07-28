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


struct ModelConstantBuffer
{
	bool isIndex16bit;
	int padd1;
	int padd2[2];
};

struct VtxAttrib
{
    float3  Normal;
    float3  BaseColor;
    float2  Uv;
};




#endif //INCLUDE_RAYTRACING_H

