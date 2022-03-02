#include "AniBase.hlsli"

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
    // 애니메이션 된다는건
    // 정점의 포지션이 바뀌는 것이다.
    // _In.Pos이 바뀌어서 나올수 있다.
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