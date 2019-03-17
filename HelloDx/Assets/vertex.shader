cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectMatrix;
};
struct VertexInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
};
PixelInput VShader(VertexInput vertex)
{
    PixelInput output;
    
    vertex.position.w = 1.0f;

    output.position = mul(vertex.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectMatrix);

    output.tex = vertex.tex;

    output.normal = mul(vertex.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
	return output;
}