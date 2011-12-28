#if MSAA_SAMPLES > 1
	Texture2DMS<float4, MSAA_SAMPLES>	
#else
	Texture2D
#endif
	source;

struct VS_INPUT
{
    float4 Position     : POSITION;
	float2 UV			: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
	float2 UV			: TEXCOORD0;
};

VS_OUTPUT VS_Main (VS_INPUT input)
{
    VS_OUTPUT Output;

    Output.Position 	= input.Position;
	Output.UV			= input.UV;
    
    return Output;    
}

#include "fw.hlsl"

float4 PS_Main (VS_OUTPUT In) : SV_TARGET
{ 
	int2 pos = int2(In.Position.xy);
	
	#ifdef COPY
		return source.Load (int3 (pos, 0));
	#else
		return FILTER (pos);
	#endif
}
