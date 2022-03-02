// 그냥 고정시켜 놓겠습니다.

struct TransDataStruct
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
};


struct RenderOptionStruct
{
    int IsDifTexture;
    int IsNormalTexture;
    int IsShadow;
    int IsAni;
    int IsDummy0;
    int IsDummy1;
    int IsDummy2;
    int IsDummy3;
    float4 BasicColor;
};



//cbuffer HTRANSDATA : register(b0)
//{
//    TransDataStruct MatrixData;
//}


//cbuffer RenderOption : register(b7)
//{
//    RenderOptionStruct RenderOptionData;
//}
