#if MSAA_SAMPLES > 1
	Texture2DMS<float4, MSAA_SAMPLES>	
#else
	Texture2D
#endif
	source;
	
struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
	float2 UV			: TEXCOORD0;
};

VS_OUTPUT VS_Main (uint id : SV_VertexID)
{
    VS_OUTPUT Output;
    Output.UV = float2((id << 1) & 2, id & 2);
    Output.Position = float4(Output.UV * float2(2,-2) + float2(-1,1), 0, 1);
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
