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

VTX3DOUT VS_3DStaticDefferdTexture(VTX3DIN _In)
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
    Out.ViewNormal = normalize(mul(_In.Normal, WV));

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

DEFFERDOUT PS_3DStaticDefferdTexture(VTX3DOUT _In)
{
    DEFFERDOUT Out = (DEFFERDOUT) 0;
    Out.DifColor = Tex.Sample(Smp, _In.Uv.xy);
    Out.NorColor = _In.ViewNormal;
    Out.PosColor = _In.ViewPos;
    Out.DepColor.x = _In.ViewPos.z;
    
    Out.DifColor.w = 1.0f;
    Out.DepColor.w = Out.PosColor.w = Out.NorColor.w = 1.0f;
    return Out;
}