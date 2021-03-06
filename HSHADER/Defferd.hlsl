#include "LightBase.hlsli"
#include "AniBase.hlsli"
#include "RenderBase.hlsli"

struct BaseVertexIn
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
};

cbuffer HTRANSDATA : register(b0)
{
    TransDataStruct MatrixData;
}

cbuffer RenderOption : register(b7)
{
    RenderOptionStruct RenderOptionData;
}
Texture2D FrameAniTex : register(t0);

VTX3DOUT VS_Defferd(BaseVertexIn _In)
{
    // 애니메이션 된다는건
    // 정점의 포지션이 바뀌는 것이다.
    // _In.Pos이 바뀌어서 나올수 있다.
    _In.Normal.w = 0.0f;
    if (0 != RenderOptionData.IsAni)
    {
        SkinningTex(_In.Pos, _In.Normal, _In.Weight, _In.Index, FrameAniTex);
    }
    
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, MatrixData.WVP);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    
    _In.Pos.w = 1.0f;
    Out.ViewPos = mul(_In.Pos, MatrixData.WV);
    
    Out.ViewNormal = normalize(mul(_In.Normal, MatrixData.WV));
    _In.Normal.w = 0.0f;

    return Out;
}

struct DEFFERDOUT
{
    float4 DifColor : SV_Target0;
    float4 PosColor : SV_Target1;
    float4 NorColor : SV_Target2;
    float4 DepColor : SV_Target3;
};

Texture2D Tex : register(t0);
SamplerState Smp : register(s0);

DEFFERDOUT PS_Defferd(VTX3DOUT _In)
{
    DEFFERDOUT Out = (DEFFERDOUT) 0;
    
    if (0 != RenderOptionData.IsDifTexture)
    {
        Out.DifColor = Tex.Sample(Smp, _In.Uv.xy);
    }
    else
    {
        Out.DifColor = RenderOptionData.BasicColor;
    }
        
    
    Out.NorColor = _In.ViewNormal;
    Out.PosColor = _In.ViewPos;
    Out.DepColor.x = _In.ViewPos.z;
    Out.DifColor.w = 1.0f;
    Out.DepColor.w = Out.PosColor.w = Out.NorColor.w = 1.0f;
    return Out;
}