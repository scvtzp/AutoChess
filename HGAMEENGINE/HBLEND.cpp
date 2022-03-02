#include "HBLEND.h"

HBLEND::HBLEND(const D3D11_BLEND_DESC& _Desc) : m_Desc(_Desc)
{
	Create();
}

HBLEND::~HBLEND()
{
	if (nullptr != m_State)
	{
		m_State->Release();
	}
}


void HBLEND::Create()
{
	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateBlendState(&m_Desc, &m_State))
	{
		// L"레스터라이저 스테이트 생성에 실패했습니다"
		// const wchar_t* PTR;
		// PTR + 1;
		AMSG(L"레스터라이저 스테이트 생성에 실패했습니다.");
	}
}

void HBLEND::Setting()
{
	// 다이렉트 11이 버전이 4까지 나왔다. 
	Game_Device::MAINOBJ()->Context()->OMSetBlendState(m_State, nullptr, 0xffffffff);
}