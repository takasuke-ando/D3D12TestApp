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
    float4x4 mtxCamera; //  カメラ行列
};


struct ModelConstantBuffer
{
	bool isIndex16bit;
	uint primitiveOffset;   //  モデル全体でのプリミティブ開始位置
	int padd2[2];
};

struct VtxAttrib
{
    float3  Normal;
    float3  BaseColor;
    float2  Uv;
};




#endif //INCLUDE_RAYTRACING_H

