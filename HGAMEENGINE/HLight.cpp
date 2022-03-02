#include "HLight.h"
#include <HGAMEDEBUG.h>
#include <HGAMEWINDOW.h>
#include "HGAMEACTOR.h"
#include "Game_Trans.h"
#include "HGAMESCENE.h"
#include "HRenderTarget.h"


void HLight::InitSetting(int _Order)
{
	Order(_Order);
	ACTOR()->SCENE()->PushLight(this);

	LightPower.x = 1.0f;
	LightPower.y = 1.0f;
	LightPower.z = 1.0f;
	LightPower.w = 1.0f;

	ShadowDepthTarget = new Game_RenderTarget();
	ShadowDepthTarget->CreateTarget({4096, 4096}, Game_Vector::NONE, true, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);
	// ������ Ÿ���� ������ ���� ���̹��۸� ���� �������Ѵ�.
	ShadowDepthTarget->CreateDepth();

	// �� �����ȿ� �ִ³༮�� 
	// {4096, 4096} ���ٰ��׸��� ��.
	// �ȼ��ϳ��� 4096 / 10.24�� ����Ƽ�� �ȴ�.
	// �� ���ڰ� ũ�� Ŭ���� ����Ƽ�� �þ��.
	// ������ ��Ƽ��

	// �⺻������ View�� �ؽ�ó ũ�⸸ŭ ���� �ǰ���?

	// 4096 / 50
	// ���ڰ� �پ��� �Ǹ鼭
	// ����Ƽ�� �������� �׸��ڰ� �߻��ϴ� ũ��� �۾����� �ȴ�.

	m_Near = 0.1f;
	m_Far = 10.0f;

	ViewSize = ShadowDepthTarget->Texture(0)->Size();

	Qulity = 400.0f;
}

void HLight::LightUpdate()
{
	ShadowDepthTarget->Clear(true);

	for (LightData* Data : DataList)
	{
		Data->LightDir = TRANS()->WFORWARD();
		Data->LightDirInv = TRANS()->WBACK();
		Data->LightDir.w = Data->LightDirInv.w = 0.0f;
		Data->LightPower = LightPower;
		// Data->AmbColor = Game_Vector::ONE * 0.2f;
		Data->OnOff.x = IsUpdate() == true ? 1.0f : 0.0f;
		Data->m_View.VIEWLH(TRANS()->WPOS(), TRANS()->WFORWARD(), TRANS()->WUP());
		Data->m_Proj.ORTHLH(ViewSize / Qulity, m_Near, m_Far);
		Data->m_ViewProj = Data->m_View * Data->m_Proj;
	}
}