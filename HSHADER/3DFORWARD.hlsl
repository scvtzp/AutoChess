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



cbuffer CUTDATA : register(b1)
{
    float4 CUTVECTOR;
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
    
    // 10  20  40  0
    // 0.1 0.8 0.1 1.0
    
    // ����ġ�� �ǹ̴�
    // index 134
    
    // 0.5 0.5
    // 10  17
    // 10�����(��)�� 50%����ް�
    // 17�����(��)�� 50%�������
    // ���� ���� ���� ��ġ��.
    
    
    // _Indices    [0]    [1]    [0]   [0] 
    // fWeight     [0.5]  [0.5]  []    []
    
    _Pos.w = 1.0f;
    _Normal.w = 0.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        matrix matBone = TexToMat(_Indices[i]);
        CalPos += fWeight[i] * mul(_Pos, matBone);
        CalNormal += fWeight[i] * mul(_Normal, matBone);
    }
    
    _Pos = CalPos;
    _Normal = CalNormal;

    return;
}

VTX3DOUT VS_3DFORWARD(VTX3DIN _In)
{
    // �ִϸ��̼� �ȴٴ°�
    // ������ �������� �ٲ�� ���̴�.
    // _In.Pos�� �ٲ� ���ü� �ִ�.
    SkinningTex(_In.Pos, _In.Normal, _In.Weight, _In.Index);
    
    VTX3DOUT Out = (VTX3DOUT) 0;
    // _In.Pos * ANIMAT[_In.Index[0]]
    // mul(_In.Pos, ANIMAT[_In.Index[0]]);
    Out.Pos = mul(_In.Pos, WVP); 
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    return Out;
}

struct FORWARDOUT
{
    float4 ForwardColor : SV_Target0;
};

// �ؽ�ó�� ����ϰڴٰ� �Ѱ�.
Texture2D DifTex : register(t0);
SamplerState Smp : register(s0);
FORWARDOUT PS_3DFORWARD(VTX3DOUT _In) 
{
    FORWARDOUT Out = (FORWARDOUT) 0;
    // Out.ForwardColor = _In.Color;
    Out.ForwardColor = DifTex.Sample(Smp, _In.Uv.xy);
    return Out;
}