cbuffer perFrame : register( b0 )
{
    matrix  worldViewProjection : packoffset( c0 );
    matrix  view    			: packoffset( c4 );
}

struct VS_INPUT
{
    float4 Position     : POSITION;
    float4 Normal       : NORMAL;  
    float4 Color		: COLOR;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float4 Normal		: NORMAL;     
    float4 Color		: COLOR;  
};

VS_OUTPUT VS_Main (VS_INPUT input)
{
    VS_OUTPUT Output;

    Output.Position 	= mul (worldViewProjection, input.Position);
	Output.Color 		= input.Color;
	Output.Normal 		= float4(input.Normal.xyz, 0);
    
    return Output;    
}

float4 PS_Main (VS_OUTPUT In) : SV_TARGET
{ 
	return In.Color * saturate (dot (normalize (float3 (1, 1, 1)), In.Normal.xyz) + 0.1f);
}
