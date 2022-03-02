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
		// L"�����Ͷ����� ������Ʈ ������ �����߽��ϴ�"
		// const wchar_t* PTR;
		// PTR + 1;
		AMSG(L"�����Ͷ����� ������Ʈ ������ �����߽��ϴ�.");
	}
}

void HBLEND::Setting()
{
	// ���̷�Ʈ 11�� ������ 4���� ���Դ�. 
	Game_Device::MAINOBJ()->Context()->OMSetBlendState(m_State, nullptr, 0xffffffff);
}