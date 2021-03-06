#include "CalBase.hlsli"

struct VTX3DIN
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
    float4 Normal : NORMAL; // 이녀석이 Y축
    float4 BiNormal : BINORMAL; // 이녀석이 X축
    float4 Tangent : TANGENT; // 이녀석이 Z축 // 나중에 바꿀수 있다.
    float4 Weight : BLENDWEIGHT; // 얼마나 영향을 받아야 하는가? 0.5 영향을 받는다.
    int4 Index : BLENDINDICES; // 21번째 본에
};

struct VTX3DOUT
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
    
    float4 ViewPos : POSITION;
    float4 ViewNormal : NORMAL;
	float4 ViewBiNormal : BINORMAL;
	float4 ViewTangent : TANGENT;
};

cbuffer HTRANSDATA : register(b0)
{
    matrix POS;
    matrix SCALE;
    matrix ROT;
    matrix REVOL;
    matrix PARENT;
    matrix LWORLD;
    matrix WWORLD;
    matrix VIEW;
    matrix PROJ;
    matrix WV;
    matrix VP;
    matrix WVP;
}

// 카메라의 위치에 따라서
// 점점 커진다.

VTX3DOUT VS_SKY(VTX3DIN _In)
{
    // 애니메이션 된다는건
    // 정점의 포지션이 바뀌는 것이다.
    // _In.Pos이 바뀌어서 나올수 있다.
    _In.Normal.w = 0.0f;
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, WVP);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    
    _In.Pos.w = 1.0f;
    Out.ViewPos = mul(_In.Pos, WV);
    
    
	_In.Normal.w = 0.0f;
	Out.ViewNormal = mul(_In.Normal, WV);

	_In.BiNormal.w = 0.0f;
	Out.ViewBiNormal = mul(_In.BiNormal, WV);

	_In.Tangent.w = 0.0f;
	Out.ViewTangent = mul(_In.Tangent, WV);

    return Out;
}

struct FORWARDOUT
{
	float4 ForwardColor : SV_Target0;
};

cbuffer RenderOption : register(b8)
{
    int IsDifTexture;
    int IsNormalTexture;
    int IsOutLine;
    int IsShadow;
    int IsAni;
    int IsDummy0;
    int IsDummy1;
    int IsDummy2;
	float4 BasicColor;
}

// 텍스처를 사용하겠다고 한것.
Texture2D Tex : register(t0);
// Texture2D NormalTexture : register(t1);
SamplerState Smp : register(s0);

FORWARDOUT PS_SKY(VTX3DOUT _In)
{
	FORWARDOUT Out = (FORWARDOUT) 0;
    
	//if (0 != IsNormalTexture)
	//{
	//	_In.ViewNormal = CalBump(_In.ViewNormal, _In.ViewBiNormal, _In.ViewTangent, _In.Uv.xy, NormalTexture, Smp);
	//}

    
	if (IsDifTexture != 0)
	{
		Out.ForwardColor = Tex.Sample(Smp, _In.Uv.xy);
	}
	else
	{
		Out.ForwardColor = BasicColor;
	}
    return Out;
}