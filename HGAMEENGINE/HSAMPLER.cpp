#include "HSAMPLER.h"


HSAMPLER::HSAMPLER(const D3D11_SAMPLER_DESC& _Desc) : m_Desc(_Desc)
{
	Create();
}

HSAMPLER::~HSAMPLER()
{
	if (nullptr != m_State)
	{
		m_State->Release();
	}
}


void HSAMPLER::Create()
{
	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateSamplerState(&m_Desc, &m_State))
	{
		// L"레스터라이저 스테이트 생성에 실패했습니다"
		// const wchar_t* PTR;
		// PTR + 1;
		AMSG(L"레스터라이저 스테이트 생성에 실패했습니다.");
	}
}

void HSAMPLER::Setting(SHADERTYPE _Type, unsigned int _Reg)
{
	switch (_Type)
	{
	case SHADERTYPE::SHADER_VS:
		Game_Device::MAINOBJ()->Context()->VSSetSamplers(_Reg, 1, &m_State);
		break;
	case SHADERTYPE::SHADER_HS:
		Game_Device::MAINOBJ()->Context()->HSSetSamplers(_Reg, 1, &m_State);
		break;
	case SHADERTYPE::SHADER_DS:
		Game_Device::MAINOBJ()->Context()->DSSetSamplers(_Reg, 1, &m_State);
		break;
	case SHADERTYPE::SHADER_GS:
		Game_Device::MAINOBJ()->Context()->GSSetSamplers(_Reg, 1, &m_State);
		break;
	case SHADERTYPE::SHADER_PS:
		Game_Device::MAINOBJ()->Context()->PSSetSamplers(_Reg, 1, &m_State);
		break;
	case SHADERTYPE::SHADER_END:
		break;
	default:
		break;
	}
}