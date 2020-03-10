cbuffer TerrainShaderParameters : register(b0)
{
	float  roadWidth;
	float  sideWidth;
	float  translation;
	float  drawDistance;

	int    segmentLength;
	float2 segment1;
	float  maxRoadX;
	float2 segment2;

	int    screenHeight;
	float  positionX;
}

Texture2D<float4> BaseTexture : register(t0);
sampler TextureSampler : register(s0);

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
	float roadY = -1.0f;

	float2 screenPos = texCoord * 2.0f;
	screenPos -= float2(1.0f, 1.0f);
	screenPos.y = -screenPos.y;

	float z = roadY / screenPos.y;
	float4 col = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (z < 15.0f)
	{
		float4 sampledData = BaseTexture.Sample(TextureSampler, texCoord);
		float roadX = sampledData.x * maxRoadX + positionX;
		float4 sampledBottomX = BaseTexture.Sample(TextureSampler, float2(0.0f, 1.0f));
		float bottomRoadX = sampledBottomX.x * maxRoadX + positionX;
		float x = screenPos.x * z;
		
		int div = (z + translation) / segmentLength;

		if (z > 0.0f)
		{
			if (abs(roadX - x) <= roadWidth)
			{
				int roadDiv = (z + translation) / (segmentLength / 10.0f);
				int roadDivX = (x - roadX) / (segmentLength / 20.0f);
				if ((roadDiv % 5 == 0 || (roadDivX + 1) % 10 == 0))
					col = float4(0.250, 0, 0.113, 1.0f);
				else
					col = float4(0.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				float lx = roadX - roadWidth - (sideWidth / 2.0f);
				float rx = roadX + roadWidth + (sideWidth / 2.0f);

				int div2 = (x - bottomRoadX) / segmentLength;

				if (abs(lx - x) <= sideWidth || abs(rx - x) <= sideWidth)
					col = float4(1, 0, 0.447, 1.0f);
				else if ((div) % 2 == 0)
					col = float4(0.023, 0, 0.188, 1.0f);
				else
					col = float4(0, 0.145, 0.188, 1.0f);
			}
		}
	}
	return col;
}