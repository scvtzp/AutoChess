#include "HRASTERIZER.h"

HRASTERIZER::HRASTERIZER(const D3D11_RASTERIZER_DESC& _Desc) : m_Desc(_Desc)
{
	Create();
}

HRASTERIZER::~HRASTERIZER() 
{
	if (nullptr != m_State)
	{
		m_State->Release();
	}
}


void HRASTERIZER::Create()
{
	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateRasterizerState(&m_Desc, &m_State))
	{
		// L"�����Ͷ����� ������Ʈ ������ �����߽��ϴ�"
		// const wchar_t* PTR;
		// PTR + 1;
		AMSG(L"�����Ͷ����� ������Ʈ ������ �����߽��ϴ�.");
	}
}

void HRASTERIZER::Setting() 
{
	// ���̷�Ʈ 11�� ������ 4���� ���Դ�. 
	Game_Device::MAINOBJ()->Context()->RSSetState(m_State);
}