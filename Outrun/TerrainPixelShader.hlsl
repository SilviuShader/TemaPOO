cbuffer TerrainShaderParameters : register(b0)
{
	float  roadWidth;
	float  sideWidth;
	float  translation;
	float  drawDistance;

	int    segmentLength;
	float2 segment1;
	int    padding2;
	float2 segment2;

	int    screenHeight;
	float  positionX;
}

Texture2D<float4> BaseTexture : register(t0);
sampler TextureSampler : register(s0);

int ScreenHeight(float texY)
{
	return (int)((float)texY * (float)screenHeight);
}

float GetRoadX(float2 texCoord, float z)
{
	float result = 0.0f;
	int crtHeight  = ScreenHeight(texCoord.y);
	int topSegH    = segment2.x;
	int bottomSegH = segment1.x;

	int linesDiff = screenHeight - max(crtHeight, topSegH);
	float dx = segment1.y;
	float ddx = dx * linesDiff * z;

	result = linesDiff * (dx + ddx) / 2.0f;

	if (crtHeight <= topSegH)
	{
		linesDiff = topSegH - crtHeight;
		dx = segment2.y;
		float prevStr = ddx;
		ddx += dx * linesDiff * z;
		result += linesDiff * (prevStr + ddx) / 2.0f;
	}

	return result;
}

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
	float roadY = -1.0f;

	float2 screenPos = texCoord * 2.0f;
	screenPos -= float2(1.0f, 1.0f);
	screenPos.y = -screenPos.y;

	float z = roadY / screenPos.y;
	float4 col = float4(0.2f, 0.4f, 0.6f, 1.0f);

	if (z < 15.0f)
	{
		float roadX = GetRoadX(texCoord, z);
		float x = screenPos.x * z;
		
		int div = (z + translation) / segmentLength;

		if (z > 0.0f)
		{
			if (abs(roadX - x) <= roadWidth)
			{
				if (div % 2 == 0)
					col = float4(0.5f, 0.5f, 0.5f, 1.0f);
				else
					col = float4(0.4f, 0.4f, 0.4f, 1.0f);
			}
			else
			{
				float lx = roadX - roadWidth - (sideWidth / 2.0f);
				float rx = roadX + roadWidth + (sideWidth / 2.0f);

				if (abs(lx - x) <= sideWidth || abs(rx - x) <= sideWidth)
					col = float4(1.0f, 0.0f, 0.4f, 1.0f);
				else if (div % 2 == 0)
					col = float4(0.0f, 1.0f, 0.0f, 1.0f);
				else
					col = float4(0.0f, 0.6f, 0.5f, 1.0f);
			}
		}
	}
	return col;
}