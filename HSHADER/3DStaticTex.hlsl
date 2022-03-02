struct VTX3DIN
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
    float4 Normal : NORMAL;
    float4 BiNormal : BINORMAL;
    float4 Tangent : TANGENT;
    float4 Weight : BLENDWEIGHT;
    int4 Index : BLENDINDICES;
};

struct VTX3DOUT
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
};

cbuffer TRANSDATA : register(b0)
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

/* ������۷� �����ϴ�. 
�ٸ�, ������۴� ���� �� �߰��� ������ �� �����ϴ�. 
�����ϱ� ���ؼ��� ���ؽ����̴� �ȿ��� �Ű����� _IN�� Ȱ���մϴ�. 
�� �ȿ��� ���ؽ��� POSITION������ ����ֽ��ϴ�. */
cbuffer CUTDATA : register(b1)
{
    float4 CUTVECTOR;
}

/* �ִϸ��̼��� �����Ӹ��� �ϳ��� �޽��� �����̴� ���Դϴ�. 
�� ���� ���ؽ��� ��ġ�� ��İ� �������鼭 �����Դϴ�. �� ��ġ�� ���� �����ϴ�. 
����� �⺻������ ������ �������� ������ ���ϴ�. ��(���)�� ���ؽ��� �������� ���Դϴ�. 
�ִϸ��̼��� ��ŸŸ�� ������ ���� �����̶�� ���� �մϴ�. 
�޽�->�ִϸ��̼� �������� � ������� �����͸� ������ ������ �����մϴ�. */

/* �ִϸ��̼� ������ ��(���)������ �ؽ��ķ� �־��־����ϴ�. */
Texture2D FrameAniTex : register(t0);

/* ����Դϴ�. */
float4x4 TexToMat(int _Idx)
{
    // ������ �����´�.
    // 17�� ���� ����������~ 
    // �׷�,  17 16�� ������ ������ � ������ �帱����.
    // 17�� ���� ����� �ʿ��� 
    // 134 * 4
    // 17�� ����� ��� �������� ���ΰ�? 
    // 17 * 4 �� ����� �� ����� ����������.
    
    
    //                         R G B A
    // ������ ���ϱ� + 0  16   1 0 0 0
    // ������ ���ϱ� + 1  16   0 1 0 0
    // ������ ���ϱ� + 2  16   0 0 1 0
    // ������ ���ϱ� + 3  16   0 0 0 1
    
    //              int   /  int   /  UINT
    //           Location / Offset / Status  
    //              UV        0        0
    
    /* Load�� ��� ����(����)���� �ؽ����� Index�� ���ؼ� ���� ������ �Լ��Դϴ�. ���÷��� ���� ������ �����ϴ�. �̹����� ���� �ʽ��ϴ�. 
    �ؽ����� ���� ������ �Լ��� DifTex.Sample(Smp, _In.Uv.xy);�Դϴ�.  */
    float4x4 BoneMat =
    {
        FrameAniTex.Load(int3((_Idx * 4) + 0, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 1, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 2, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 3, 0, 0)),
    };
    
    return BoneMat;
}

/* inout : ���̴� ���۷��������Դϴ�. �ܺο����� �����˴ϴ�. */
/* ��Ű�� �ؽ��� �Դϴ�. ����� ����ġ�� ���� �ջ�Ǽ� �������� ���ǵ˴ϴ�. */
void SkinningTex(inout float4 _Pos, inout float4 _Normal, inout float4 _Weight, inout int4 _Indices)
{
    float4 CalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 CalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float fWeight[4];

    fWeight[0] = _Weight.x;
    fWeight[1] = _Weight.y;
    fWeight[2] = _Weight.z;
    fWeight[3] = 1.f - _Weight.x - _Weight.y - _Weight.z; /* ����ġ�� ���� 1�Դϴ�. */
    
    // ����ġ�� �ǹ̴� ���ؽ��� ��ġ�� ����� ��, 
    // 0.5 0.5
    // 10  17
    // 10�����(��)�� 50%����ް�
    // 17�����(��)�� 50%�������
    // ���� ���� ���� ��ġ��.
    
    _Pos.w = 1.0f; /* ��ġ���ʹ� W = 1.0f �̵��� ����ȴ�   */
    _Normal.w = 0.0f; /* �������ʹ� W = 0.0f �̵��� ������� �ʴ´�.  */
    
    for (int i = 0; i < 4; ++i)
    {
        /* ���� ���� �ؽ��ľȿ��� ��ķ� ����˴ϴ�. */
        matrix matBone = TexToMat(_Indices[i]);
        CalPos += fWeight[i] * mul(_Pos, matBone);
        CalNormal += fWeight[i] * mul(_Normal, matBone);
    }
    
    _Pos = CalPos;
    _Normal = CalNormal;

    return;
}

VTX3DOUT VS_3DStaticTex(VTX3DIN _In)
{
    /* �ִϹ��̼��� ������ ��ġ�� �ٲ�� ���Դϴ�. �ִϸ��̼� ����� ���ؽ��鸶�� �����ָ�, �ִϸ��̼��� ���ϴ� ��ġ�� ���ϴ�.  */
    //SkinningTex(_In.Pos, _In.Normal, _In.Weight, _In.Index);
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, WVP);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    return Out;
}

struct STATICMASHOUT
{
    float4 ForwardColor : SV_Target0;
};

Texture2D Tex : register(t0);
//Texture2D NormalTexture : register(t1);
SamplerState Smp : register(s0);

STATICMASHOUT PS_3DStaticTex(VTX3DOUT _In)
{   
    STATICMASHOUT OUT = (STATICMASHOUT) 0;
    OUT.ForwardColor = Tex.Sample(Smp, _In.Uv.xy); /* �ؽ����� ���� ���ɴϴ�. */
    return OUT;
}