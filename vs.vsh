
struct VS_IN
{
	float4 pos : POSITION0;
	float4 nor : NORMAL0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 posw : POSITION0;
	float4 norw : NORMAL0;
};

cbuffer ConstantBuffer
{
	float4x4 World;         
	float4x4 View;          
	float4x4 Projection;    
	float4 Light;
	float4 Attenuation;
}

VS_OUT vs_main( VS_IN input ) 
{
	VS_OUT output;
	float3 nor;
	float col;

	output.posw = mul(input.pos, World);
	output.pos = mul(output.posw, View);
	output.pos = mul(output.pos, Projection);

	output.norw = mul(input.nor, World);
	return output;
}