cbuffer TerrainShaderParameters : register(b0)
{
	int screenWidth;
	int screenHeight;
	int segmentLength;
	int padding;
}

Texture2D<float4> BaseTexture : register(t0);
sampler TextureSampler : register(s0);

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
	float z = BaseTexture.Sample(TextureSampler, texCoord).x;
	float valid = BaseTexture.Sample(TextureSampler, texCoord).y;
	
	float4 col = float4(0.0f, 1.0f, 0.0f, 1.0f);
	if (z > 0.5f)
		col = float4(0.0f, 0.6f, 0.4f, 1.0f);

	if (valid < 0.5f)
		col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	return col;
}