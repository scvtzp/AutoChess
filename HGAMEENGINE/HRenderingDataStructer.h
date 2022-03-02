#pragma once
#include <HGAMEMATH.h>

///////////////////////////////////// �������� �����ִ� ����ü���� ��Ƴ��� ���
struct LightData
{
    float4 DifColor; // 
    float4 SpcColor; // 
    float4 AmbColor; // 
    float4 LightPower; // x dif, y spc, z ambi, w ��ü�� 
    float4 SpecPow; // x dif, y spc, z ambi, w ��ü�� 
    float4 LightDir;
    float4 LightDirInv; // �̰� L ���� ������
    float4 LightPos; // ����Ʈ ����Ʈ�϶� 
    float4 LightRange; // ����Ʈ ����Ʈ ����
    float4 ShadowOption; // �׸��ڸ� ���� �ɼ�
    float4 CamPos; // �̰� L ���� ������
    float4 OnOff; // �̰� L ���� ������
    float4 m_ShadowRange; // �׸��ڸ� ���� �ִ� ������� �����ϸ� �˴ϴ�.
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