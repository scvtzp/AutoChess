// #include "header"

struct VTX2DIMGIN
{
    float4 Pos : POSITION;
};

struct VTX2DIMGOUT
{
    float4 Pos : SV_Position;
};

// 상수버퍼는 선언부터 해야 한다.
// b로 인식되는데 무슨말이냐면?

// 이 데이터는 cpu에서 날아올거다.
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