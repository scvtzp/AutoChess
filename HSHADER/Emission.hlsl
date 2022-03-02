#include "AniBase.hlsli"

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
};

struct DEFFERDOUT
{
    float4 OutColor : SV_Target6;
};

Texture2D FrameAniTex : register(t0);

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


VTX3DOUT VS_Emission(VTX3DIN _In)
{
    // �ִϸ��̼� �ȴٴ°�
    // ������ �������� �ٲ�� ���̴�.
    // _In.Pos�� �ٲ� ���ü� �ִ�.
    SkinningPos(_In.Pos, _In.Weight, _In.Index, FrameAniTex);
    
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, WVP);

    return Out;
}

DEFFERDOUT PS_Emission(VTX3DOUT _In)
{
    DEFFERDOUT Out = (DEFFERDOUT) 0.0f;
    Out.OutColor.xw = 1.0f;
    return Out;
}