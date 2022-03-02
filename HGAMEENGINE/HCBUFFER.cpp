#include "HCBUFFER.h"
//
void HCBUFFER::Setting(SHADERTYPE _Type, unsigned int _Reg) 
{
	switch (_Type)
	{
	case SHADERTYPE::SHADER_VS:
		Game_Device::MAINOBJ()->Context()->VSSetConstantBuffers(_Reg, 1, &m_pBuffer);
		break;
	case SHADERTYPE::SHADER_HS:
		Game_Device::MAINOBJ()->Context()->HSSetConstantBuffers(_Reg, 1, &m_pBuffer);
		break;
	case SHADERTYPE::SHADER_DS:
		Game_Device::MAINOBJ()->Context()->DSSetConstantBuffers(_Reg, 1, &m_pBuffer);
		break;
	case SHADERTYPE::SHADER_GS:
		Game_Device::MAINOBJ()->Context()->GSSetConstantBuffers(_Reg, 1, &m_pBuffer);
		break;
	case SHADERTYPE::SHADER_PS:
		Game_Device::MAINOBJ()->Context()->PSSetConstantBuffers(_Reg, 1, &m_pBuffer);
		break;
	case SHADERTYPE::SHADER_END:
		break;
	default:
		break;
	}
}

void HCBUFFER::Create(unsigned int _Size)
{
	m_BufferDesc.ByteWidth = _Size;
	m_BufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC; // 어디에 저장해 달라. 보통 =그래픽카드에 저장
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateBuffer(&m_BufferDesc, nullptr, &m_pBuffer))
	{
		AMSG(L"상수버퍼 세팅에 실패했습니다");
	}
}

void HCBUFFER::DataChange(void* _Data, unsigned int _Size)
{
	if (m_BufferDesc.ByteWidth != _Size)
	{
		AMSG(NAME() + L" 상수버퍼의 크기가 다릅니다.");
	}
	// 데이터의 수정이니까 그게 될까요?
	// 그래픽카드야 내가 m_pBuffer버퍼의 내용을 수정할거야.
	// 그래픽카드는 안정성을 위해서
	// 멀티 쓰레드 환경일 경우를 대비해서
	// 이녀석을 누구도 사용하지 못하게 막는다.
	// 파일입출력
	Game_Device::MAINOBJ()->Context()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_SubData);
	memcpy_s(m_SubData.pData, m_BufferDesc.ByteWidth, _Data, m_BufferDesc.ByteWidth);
	// 사용권을 반환해준다.
	Game_Device::MAINOBJ()->Context()->Unmap(m_pBuffer, 0);

}