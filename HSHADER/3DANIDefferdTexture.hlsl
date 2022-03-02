#include "LightBase.hlsli"

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

// �Ʊ� ������� 134�� ���� �ִϸ��̼� �������� ��������
// �ؽ�ó�� ������µ�. ���⿡ �־��ִ� �ſ���.
Texture2D FrameAniTex : register(t0);

float4x4 TexToMat(int _Idx)
{
    // Load�� �������� �ؽ�ó�� Index�� ���ؼ� 
    // ���� ������ �Լ��̴�.
    // �ؽ�ó�� ���� ������ �Լ��� 
    // DifTex.Sample(Smp, _In.Uv.xy);
    // ��İ��� �������°Ŷ�.
    // ������ �������� �ǳ���?
    // 17�� �� ������
    // 17 16�� ������ ������ � ������ �帱����.
    // 17�� ���� ����� �����;���?
    // 134 * 4
    // 17�� ��� ����������?
    // 17 * 4 �� ����� �� ����� ����������.
    //                     R G B A
    // ������ ���ϱ� + 0 16 1 0 0 0
    // ������ ���ϱ� + 1 16 0 1 0 0
    // ������ ���ϱ� + 2 16 0 0 1 0
    // ������ ���ϱ� + 3 16 0 0 0 1
    
    // 0 0                  0 1 
    // 0.1 0.1 0.1 0.0      0.5 0.5 0.5
    
    // 100 100
    // 0 0
    
    // 4  10
    // 4 * 10 
    
    float4x4 BoneMat =
    {
        FrameAniTex.Load(int3((_Idx * 4) + 0, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 1, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 2, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 3, 0, 0)),
    };
    return BoneMat;
}

// inout ���̴� ���۷���
// �ٱ������� ��������.
void SkinningTex(inout float4 _Pos, inout float4 _Normal, inout float4 _Weight, inout int4 _Indices)
{
    float4 CalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 CalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float fWeight[4];
    fWeight[0] = _Weight.x;
    fWeight[1] = _Weight.y;
    fWeight[2] = _Weight.z;
    fWeight[3] = 1.f - _Weight.x - _Weight.y - _Weight.z;
    
    _Pos.w = 1.0f;
    
    _Normal.w = 0.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        matrix matBone = TexToMat(_Indices[i]);
        CalPos += fWeight[i] * mul(_Pos, matBone);
        CalNormal += fWeight[i] * mul(_Normal, matBone);
    }
    
    _Pos = CalPos;
    _Normal.xyz = CalNormal.xyz;

    return;
}

VTX3DOUT VS_3DAniDefferdTexture(VTX3DIN _In)
{
    // �ִϸ��̼� �ȴٴ°�
    // ������ �������� �ٲ�� ���̴�.
    // _In.Pos�� �ٲ� ���ü� �ִ�.
    _In.Normal.w = 0.0f;
    SkinningTex(_In.Pos, _In.Normal, _In.Weight, _In.Index);
    
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, WVP);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    
    _In.Pos.w = 1.0f;
    Out.ViewPos = mul(_In.Pos, WV);
    
    Out.ViewNormal = normalize(mul(_In.Normal, WV));
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

DEFFERDOUT PS_3DAniDefferdTexture(VTX3DOUT _In)
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