


#include "SimpleDef.hlsli"


Texture2D<float4>	tex2d : register(t0);
SamplerState		ss	  : register(s0);


PSOutput PSMain(VSOutput input)
{
	PSOutput output = (PSOutput)0;

	output.Color = input.Color;


	float4 t = tex2d.Sample(ss, input.TexCoord);

	output.Color *= t;

	return output;
}









