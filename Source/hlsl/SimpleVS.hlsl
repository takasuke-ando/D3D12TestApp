



#include "SimpleDef.hlsli"


VSOutput		VSMain(VSInput In)
{

	VSOutput Out = (VSOutput)0;


	float4 localPos = float4(In.Position, 1.f);


	float4	worldPos = mul(localPos, World);
	float4	viewPos = mul(worldPos, View);
	float4	projPos = mul(viewPos, Proj);
	
	Out.Position = projPos;
	Out.Normal = mul( In.Normal , (float3x3)World );
	Out.TexCoord = In.TexCoord;
	Out.Color = In.Color;




	return Out;
}


