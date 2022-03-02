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
	// 쉐도우 타겟은 쉐도우 만의 깊이버퍼를 따로 만들어야한다.
	ShadowDepthTarget->CreateDepth();

	// 이 범위안에 있는녀석을 
	// {4096, 4096} 에다가그리는 것.
	// 픽셀하나가 4096 / 10.24의 퀄리티가 된다.
	// 이 숫자가 크면 클수록 퀄리티가 늘어난다.
	// 작으면 퀄티기

	// 기본적으로 View는 텍스처 크기만큼 보게 되겠죠?

	// 4096 / 50
	// 숫자가 줄어들게 되면서
	// 퀄리티가 높아지고 그림자가 발생하는 크기는 작아지게 된다.

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