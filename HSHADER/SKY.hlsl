#include "CalBase.hlsli"

struct VTX3DIN
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
    float4 Normal : NORMAL; // �̳༮�� Y��
    float4 BiNormal : BINORMAL; // �̳༮�� X��
    float4 Tangent : TANGENT; // �̳༮�� Z�� // ���߿� �ٲܼ� �ִ�.
    float4 Weight : BLENDWEIGHT; // �󸶳� ������ �޾ƾ� �ϴ°�? 0.5 ������ �޴´�.
    int4 Index : BLENDINDICES; // 21��° ����
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

// ī�޶��� ��ġ�� ����
// ���� Ŀ����.

VTX3DOUT VS_SKY(VTX3DIN _In)
{
    // �ִϸ��̼� �ȴٴ°�
    // ������ �������� �ٲ�� ���̴�.
    // _In.Pos�� �ٲ� ���ü� �ִ�.
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

// �ؽ�ó�� ����ϰڴٰ� �Ѱ�.
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