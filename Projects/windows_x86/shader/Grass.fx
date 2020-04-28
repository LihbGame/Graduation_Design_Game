

cbuffer cbPerFrame
{
	float3 gEyePosW;
	float gGameTime;
	float4x4 gViewProj; 
};


cbuffer cbGrass
{
	float2 gGrassTexC[12] =
	{
		float2(0.0f,1.0f),
		float2(1.0f,1.0f),
				   
		float2(0.0f,0.8f),
		float2(1.0f,0.8f),
				   
		float2(0.0f,0.6f),
		float2(1.0f,0.6f),
				   
		float2(0.0f,0.4f),
		float2(1.0f,0.4f),
				
		float2(0.0f,0.2f),
		float2(1.0f,0.2f),

		float2(0.0f,0.0f),
		float2(1.0f,0.0f)
	};
};

 
Texture2D gGrassTex;
Texture2D gGrassBlendTex;
Texture2D gTerrainTex;
// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex;
 
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState samRandom
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = CLAMP;
	AddressV = CLAMP;
};


float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	// project onto unit sphere
	return normalize(v);
}
 
struct VertexIn
{
	float3 PosW  : POSITION;
};


struct VertexOut
{
	float4 PosW  : SV_POSITION;
};

VertexOut DrawVS(VertexIn vin)
{
	VertexOut vout;
	//banding eye position
	vout.PosW.rgb = vin.PosW;// +gEyePosW;
	vout.PosW.a = 1.0f;

	


	return vout;
}

struct GeoOut
{
	float4 PosH  : SV_Position;
	float2 Tex   : TEXCOORD;
};

// The draw GS just expands points into Grass.

	// v10--v11
	//   |\ |
	//   | \|
	//  v8--v9
	//   |\ |
	//   | \|
	//  v6--v7
	//   |\ |
	//   | \|
	//  v4--v5
	//   |\ |
	//   | \|
	//  v2--v3
	//   |\ |
	//   | \|
	//  v0--v1
[maxvertexcount(12)]
void DrawGS(point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	// Compute world matrix so that billboard faces the camera.
	//float3 look = RandUnitVec3(gin[0].PosW.x+gin[0].PosW.z);
	//look = normalize(float3(look.x, 0.0f, look.z));
	//float3 up = float3(0.0f, 1.0f, 0.0f);
	//float3 right = normalize(cross(up, look));

	
	
		float4 PosW = gin[0].PosW;

		float c = (PosW.x + 0.5f * 2048);
		float d = -1*(PosW.z - 0.5f * 2048);
		float2 UVoffset = float2(c / 2048.0f, d / 2048.0f);
		float y = gTerrainTex.SampleLevel(samLinear, UVoffset, 0).r;

		PosW.g = y;
		if (length(gEyePosW - PosW.rgb) <= 100.0f && y > 0.0f)
		{
			float time = gGameTime / 10.0f;


			float3 randomRGB = gRandomTex.SampleLevel(samRandom, UVoffset.x * UVoffset.y, 0).rgb;// sin(frac(PosW.x) + frac(PosW.z));
			float random = sin(randomRGB.r + randomRGB.b + randomRGB.g);
			//风的影响系数
			float windCoEff = 0.0f;
			float2 wind = float2(sin(randomRGB.r * time * 3.14 * 5), sin(randomRGB.g * time * 3.14 * 5));
			wind.x += (sin(time + gin[0].PosW.x / 25) + sin((time + gin[0].PosW.x / 15) + 50)) * 0.5;
			wind.y += cos(time + gin[0].PosW.z / 80);
			wind *= lerp(0.7, 1.0, 1.0 - random);

			float oscillationStrength = 2.5f;
			float sinSkewCoeff = random;
			float lerpCoeff = (sin(oscillationStrength * time + sinSkewCoeff) + 1.0) / 2;
			float2 leftWindBound = wind * (1.0 - 0.05);
			float2 rightWindBound = wind * (1.0 + 0.05);

			wind = lerp(leftWindBound, rightWindBound, lerpCoeff);

			float randomAngle = lerp(-3.14, 3.14, random);
			float randomMagnitude = lerp(0, 1., random);
			float2 randomWindDir = float2(sin(randomAngle), cos(randomAngle));
			wind += randomWindDir * randomMagnitude;

			float windForce = length(wind);



			float3 look = normalize(gEyePosW - gin[0].PosW.rgb);
			look = normalize(float3(look.x, 0.0f, look.z));
			float3 right = normalize(cross(float3(0, 1, 0), look));
			float3 up = cross(look, right);


			// Compute triangle strip vertices (quad) in world space.
			float HalfWidth = 0.5f;
			float PerHeight = HalfWidth * 8;

			float4 v[12];
			float3 Height = PerHeight * up;
			float3 Width = HalfWidth * right;


			v[0] = float4(PosW - Width + Height * 0, 1.0f);
			v[0].xz += wind.xy * windCoEff;
			v[0].y -= windForce * windCoEff * 0.8;
			v[1] = float4(PosW + Width + Height * 0, 1.0f);
			v[1].xz += wind.xy * windCoEff;
			v[1].y -= windForce * windCoEff * 0.8;
			windCoEff += 1 - gGrassTexC[0].y;

			v[2] = float4(PosW - Width + Height * 1, 1.0f);
			v[2].xz += wind.xy * windCoEff;
			v[2].y -= windForce * windCoEff * 0.8;
			v[3] = float4(PosW + Width + Height * 1, 1.0f);
			v[3].xz += wind.xy * windCoEff;
			v[3].y -= windForce * windCoEff * 0.8;
			windCoEff += 1 - gGrassTexC[2].y;

			v[4] = float4(PosW - Width + Height * 2, 1.0f);
			v[4].xz += wind.xy * windCoEff;
			v[4].y -= windForce * windCoEff * 0.8;
			v[5] = float4(PosW + Width + Height * 2, 1.0f);
			v[5].xz += wind.xy * windCoEff;
			v[5].y -= windForce * windCoEff * 0.8;
			windCoEff += 1 - gGrassTexC[4].y;

			v[6] = float4(PosW - Width + Height * 3, 1.0f);
			v[6].xz += wind.xy * windCoEff;
			v[6].y -= windForce * windCoEff * 0.8;
			v[7] = float4(PosW + Width + Height * 3, 1.0f);
			v[7].xz += wind.xy * windCoEff;
			v[7].y -= windForce * windCoEff * 0.8;
			windCoEff += 1 - gGrassTexC[6].y;

			v[8] = float4(PosW - Width + Height * 4, 1.0f);
			v[8].xz += wind.xy * windCoEff;
			v[8].y -= windForce * windCoEff * 0.8;
			v[9] = float4(PosW + Width + Height * 4, 1.0f);
			v[9].xz += wind.xy * windCoEff;
			v[9].y -= windForce * windCoEff * 0.8;
			windCoEff += 1 - gGrassTexC[8].y;

			v[10] = float4(PosW - Width + Height * 5, 1.0f);
			v[10].xz += wind.xy * windCoEff;
			v[10].y -= windForce * windCoEff * 0.8;
			v[11] = float4(PosW + Width + Height * 5, 1.0f);
			v[11].xz += wind.xy * windCoEff;
			v[11].y -= windForce * windCoEff * 0.8;
			windCoEff += 1 - gGrassTexC[10].y;





			// Transform quad vertices to world space and output 
			// them as a triangle strip.
			GeoOut gout;
			[unroll]
			for (int i = 0; i < 12; ++i)
			{
				gout.PosH = mul(v[i], gViewProj);
				gout.Tex = gGrassTexC[i];
				triStream.Append(gout);
			}

		}
}

float4 DrawPS(GeoOut pin) : SV_TARGET
{
	float4 color = gGrassTex.Sample(samLinear, pin.Tex);
	clip(gGrassBlendTex.Sample(samAnisotropic, pin.Tex).r - 0.1f);
	return color;
}

RasterizerState NoCull
{
	CullMode = None;
};

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_5_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_5_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_5_0, DrawPS() ) );

		SetRasterizerState(NoCull);
    }
}