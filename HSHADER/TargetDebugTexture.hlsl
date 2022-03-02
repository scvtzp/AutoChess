// #include "header"

struct VTX2DIMGIN
{
    float4 Pos : POSITION;
    float4 Uv  : TEXCOORD;
};

struct VTX2DIMGOUT
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
};

// ������۴� ������� �ؾ� �Ѵ�.
// b�� �νĵǴµ� �������̳ĸ�?

// �� �����ʹ� cpu���� ���ƿðŴ�.
// 
cbuffer WVPMATRIX : register(b0)
{
    matrix WVP;
}

// HVTX2DCOLOR
VTX2DIMGOUT VS_TARGETDEBUG(VTX2DIMGIN _In)
{
    VTX2DIMGOUT Out = (VTX2DIMGOUT)0;
    Out.Pos = mul(_In.Pos, WVP);
    Out.Uv = _In.Uv;
    return Out;
}

Texture2D Tex : register(t0);
SamplerState Smp : register(s0);
float4 PS_TARGETDEBUG(VTX2DIMGOUT _In) : SV_Target0
{
    float4 Color = Tex.Sample(Smp, _In.Uv.xy);
    return Color;
}