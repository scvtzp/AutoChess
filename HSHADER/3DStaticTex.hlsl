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

/* 상수버퍼로 보냅니다. 
다만, 상수버퍼는 세팅 훗 중간에 수정할 수 없습니다. 
수정하기 위해서는 버텍스쉐이더 안에서 매개변수 _IN을 활용합니다. 
그 안에는 버텍스의 POSITION정보가 들어있습니다. */
cbuffer CUTDATA : register(b1)
{
    float4 CUTVECTOR;
}

/* 애니메이션은 프레임마다 하나의 메쉬가 움직이는 것입니다. 
그 안의 버텍스의 위치가 행렬과 곱해지면서 움직입니다. 그 위치로 점을 보냅니다. 
행렬은 기본적으로 무엇과 곱해지는 식으로 씁니다. 본(행렬)은 버텍스와 곱해지는 것입니다. 
애니메이션은 델타타임 구조에 의해 보간이라는 것을 합니다. 
메쉬->애니메이션 구조에서 어떤 방식으로 데이터를 전달할 것인지 생각합니다. */

/* 애니메이션 프레임 본(행렬)정보를 텍스쳐로 넣어주었습니다. */
Texture2D FrameAniTex : register(t0);

/* 행렬입니다. */
float4x4 TexToMat(int _Idx)
{
    // 비율로 가져온다.
    // 17번 본을 가져오세요~ 
    // 그럼,  17 16번 사이의 보간된 어떤 값으로 드릴께요.
    // 17번 본의 행렬이 필요해 
    // 134 * 4
    // 17번 행렬은 어떻게 가져오는 것인가? 
    // 17 * 4 를 해줘야 그 행렬의 시작점이죠.
    
    
    //                         R G B A
    // 시작점 더하기 + 0  16   1 0 0 0
    // 시작점 더하기 + 1  16   0 1 0 0
    // 시작점 더하기 + 2  16   0 0 1 0
    // 시작점 더하기 + 3  16   0 0 0 1
    
    //              int   /  int   /  UINT
    //           Location / Offset / Status  
    //              UV        0        0
    
    /* Load는 어떠한 가감(보간)없이 텍스쳐의 Index를 통해서 값을 얻어오는 함수입니다. 샘플러로 인한 보간이 없습니다. 이미지로 쓰지 않습니다. 
    텍스쳐의 색을 얻어오는 함수는 DifTex.Sample(Smp, _In.Uv.xy);입니다.  */
    float4x4 BoneMat =
    {
        FrameAniTex.Load(int3((_Idx * 4) + 0, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 1, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 2, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 3, 0, 0)),
    };
    
    return BoneMat;
}

/* inout : 쉐이더 레퍼런스역할입니다. 외부에서도 수정됩니다. */
/* 스키닝 텍스쳐 입니다. 행렬이 가중치에 의해 합산되서 정점들이 정의됩니다. */
void SkinningTex(inout float4 _Pos, inout float4 _Normal, inout float4 _Weight, inout int4 _Indices)
{
    float4 CalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 CalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float fWeight[4];

    fWeight[0] = _Weight.x;
    fWeight[1] = _Weight.y;
    fWeight[2] = _Weight.z;
    fWeight[3] = 1.f - _Weight.x - _Weight.y - _Weight.z; /* 가중치의 합은 1입니다. */
    
    // 가중치의 의미는 버텍스의 위치를 계산할 때, 
    // 0.5 0.5
    // 10  17
    // 10번행렬(본)에 50%영향받고
    // 17번행렬(본)에 50%영향받은
    // 값이 너의 최종 위치다.
    
    _Pos.w = 1.0f; /* 위치벡터는 W = 1.0f 이동이 적용된다   */
    _Normal.w = 0.0f; /* 법선벡터는 W = 0.0f 이동이 적용되지 않는다.  */
    
    for (int i = 0; i < 4; ++i)
    {
        /* 들어온 값은 텍스쳐안에서 행렬로 저장됩니다. */
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
    /* 애니미이션은 정점의 위치가 바뀌는 것입니다. 애니메이션 행렬을 버텍스들마다 곱해주면, 애니메이션이 원하는 위치에 갑니다.  */
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
    OUT.ForwardColor = Tex.Sample(Smp, _In.Uv.xy); /* 텍스쳐의 색을 얻어옵니다. */
    return OUT;
}