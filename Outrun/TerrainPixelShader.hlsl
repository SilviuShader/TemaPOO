cbuffer ConstantTerrainShaderParameters : register(b0)
{
	float roadWidth;
	float sideWidth;
	int   screenHeight;
	float drawDistance;
	int   segmentLength;
	float roadHeight;
}

cbuffer TerrainShaderParameters : register(b1)
{
	float maxRoadX;
	float translation;
	float positionX;
}

Texture2D<float4> BaseTexture    : register(t0);
sampler           TextureSampler : register(s0);

float4 main(float4 color    : COLOR0,
	        float2 texCoord : TEXCOORD0) : SV_TARGET
{
	const float  Z_STRIPES_SPACING_FACTOR = 0.1f;
    const float  X_STRIPES_SPACING_FACTOR = 0.05f;

	const float4 SKY_COLOR                = float4(0.0f, 0.0f, 0.0f, 0.0f);
	const float4 STRIPE_COLOR             = float4(0.250, 0, 0.113, 1.0f);
	const float4 SIDE_COLOR               = float4(1, 0, 0.447, 1.0f);
	const float4 TERRAIN_LINE_COLOR1      = float4(0.023, 0, 0.188, 1.0f);
	const float4 TERRAIN_LINE_COLOR2      = float4(0, 0.145, 0.188, 1.0f);;

	float2 screenPos = texCoord * 2.0f;
	screenPos  -= float2(1.0f, 1.0f);
	screenPos.y = -screenPos.y;

	float z    = roadHeight / screenPos.y;
	float4 col = SKY_COLOR;

	if (z < drawDistance && z >= 0.0f)
	{
		float4 sampledData = BaseTexture.Sample(TextureSampler, texCoord);
		float roadX        = ((sampledData.x * 2.0f) - 1.0f) * maxRoadX + positionX;
		float x            = screenPos.x * z;
		
		int div = (z + translation) / segmentLength;

		if (abs(roadX - x) <= roadWidth)
		{
			int roadDiv  = (z + translation) / (segmentLength * Z_STRIPES_SPACING_FACTOR);
			int roadDivX = (x - roadX) / (segmentLength * X_STRIPES_SPACING_FACTOR);
			int modZ = (1.0f / Z_STRIPES_SPACING_FACTOR) / 2.0f;
			int modX = (1.0f / X_STRIPES_SPACING_FACTOR) / 2.0f;

			if (roadDiv  % modZ == 0 || 
				roadDivX % modX == 0)
				col = STRIPE_COLOR;
			else
				col = SKY_COLOR;
		}
		else
		{
			float lx = roadX - roadWidth - (sideWidth / 2.0f);
			float rx = roadX + roadWidth + (sideWidth / 2.0f);

			if (abs(lx - x) <= sideWidth || abs(rx - x) <= sideWidth)
				col = SIDE_COLOR;
			else if ((div) % 2 == 0)
				col = TERRAIN_LINE_COLOR1;
			else
				col = TERRAIN_LINE_COLOR2;
		}
	}
	return col;
}