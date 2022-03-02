// #include "header"

struct VTX2DIMGIN
{
    float4 Pos : POSITION;
};

struct VTX2DIMGOUT
{
    float4 Pos : SV_Position;
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
VTX2DIMGOUT VS_TARGETDEBUGBACK(VTX2DIMGIN _In)
{
    VTX2DIMGOUT Out = (VTX2DIMGOUT)0;
    Out.Pos = mul(_In.Pos, WVP);
    return Out;
}

cbuffer BACKCOLOR : register(b8)
{
    float4 DrawColor;
}

float4 PS_TARGETDEBUGBACK(VTX2DIMGOUT _In) : SV_Target0
{
    return DrawColor;
}