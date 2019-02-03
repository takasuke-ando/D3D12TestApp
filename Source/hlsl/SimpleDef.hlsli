#ifndef __INCLUDE_SIMPLEDEF_HLSLI_H__
#define	__INCLUDE_SIMPLEDEF_HLSLI_H__


struct VSInput
{

	float3	Position:POSITION;
	float3	Normal	:NORMAL;
	float2	TexCoord:	TEXCOORD;
	float4	Color	:	VTX_COLOR;

};



struct VSOutput
{
	float4		Position:	SV_POSITION;
	float3		Normal	:	NORMAL;
	float2		TexCoord:	TEXCOORD;
	float4		Color	:	VTX_COLOR;

};



struct PSOutput
{

	float4	Color	:	SV_TARGET0;

};


cbuffer		TransformBuffer:	register(b0)
{
	float4x4	World:	packoffset(c0);
	float4x4	View:	packoffset(c4);
	float4x4	Proj:	packoffset(c8);
}




#endif // !__INCLUDE_SIMPLEDEF_HLSLI_H__
