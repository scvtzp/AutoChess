#pragma once
#include <HGAMEMATH.h>

///////////////////////////////////// 랜더링과 관련있는 구조체들을 모아놓는 헤더
struct LightData
{
    float4 DifColor; // 
    float4 SpcColor; // 
    float4 AmbColor; // 
    float4 LightPower; // x dif, y spc, z ambi, w 전체다 
    float4 SpecPow; // x dif, y spc, z ambi, w 전체다 
    float4 LightDir;
    float4 LightDirInv; // 이게 L 빛의 역방향
    float4 LightPos; // 포인트 라이트일때 
    float4 LightRange; // 포인트 라이트 범위
    float4 ShadowOption; // 그림자를 위한 옵션
    float4 CamPos; // 이게 L 빛의 역방향
    float4 OnOff; // 이게 L 빛의 역방향
    float4 m_ShadowRange; // 그림자를 볼수 있는 범위라고 생각하면 됩니다.
    HMATRIX m_View;
    HMATRIX m_Proj;
    HMATRIX m_ViewProj;
    HMATRIX m_CamViewInverse;

    LightData() : LightPower(1.0f, 1.0f, 1.0f, 1.0f), SpecPow(30.0f, 0.0f, 0.0f, 0.0f)
    {
        AmbColor = Game_Vector::ZERO;
    }
};

struct ObjectLightData
{
    int LightCount[4]; // x 3
    LightData ArrLightData[128];
};

struct DEBUGOPTION
{
    // float IsTexture;
    bool IsTexture;
private:
    bool IsTexture1;
    bool IsTexture2;
    bool IsTexture3;
public:
    float Dumy1;
    float Dumy2;
    float Dumy3;
    float4 DebugDrawColor;

public:
    DEBUGOPTION() 
    {
        memset(this, 0, sizeof(DEBUGOPTION));
    }
};