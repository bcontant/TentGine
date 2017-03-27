//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
cbuffer WVPBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
}

struct VS_INPUT
{
	float4 vPosition    : POSITION;
	float2 vTexcoord    : TEXCOORD0;
	float4 vColor		: COLOR0;
};

struct VS_OUTPUT
{
	float2 vTexcoord    : TEXCOORD0;
	float4 vColor		: COLOR0;

	float4 vPosition    : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT vs_simple_2d_ptc(VS_INPUT Input)
{
	VS_OUTPUT Output;

	float4 pos = mul(Input.vPosition, World);
	pos = mul(pos, View);
	pos = mul(pos, Proj);
	Output.vPosition = pos;

	Output.vColor = Input.vColor;
	Output.vTexcoord = Input.vTexcoord;

	return Output;
}