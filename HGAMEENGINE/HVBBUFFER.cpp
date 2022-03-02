#include "HVBBUFFER.h"

HVBBUFFER::HVBBUFFER() : m_Offset(0)
{

}

void HVBBUFFER::Create(size_t _Count, size_t _Size, const void* _Data, D3D11_USAGE _USAGE) 
{
	m_Size = (unsigned int)_Size;
	m_Count = (unsigned int)_Count;

	//                       4         32
	m_BufferDesc.ByteWidth = (unsigned int)(_Count * _Size);
	m_BufferDesc.Usage = _USAGE; // 어디에 저장해 달라. 보통 =그래픽카드에 저장

	// 이녀석은 cpu에서 수정할거다 안할거다.
	// D3D11_USAGE::D3D11_USAGE_DEFAULT 그래픽 카드에 저장되는데 수정 안한다.
	// D3D11_USAGE::D3D11_USAGE_DYNAMIC 그래픽 카드에 저장되는데 수정 한다.
	// 
	if (m_BufferDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
	{
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA DS = {};
	DS.pSysMem = _Data;

	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateBuffer(&m_BufferDesc, &DS, &m_pBuffer))
	{
		AMSG(L"버텍스 버퍼 생성에 실패했습니다");
	}
}

void HVBBUFFER::Setting() 
{
	// 어떤 자료형의 주소값을 수정할수 없다
	// 어떤 자료형의 주소값을 수정할수 없는 포인터의 주소값을 수정수 없는 포인터 주소값.
	// int* const* const* const Ptr = nullptr;

	// 슬롯이 16개 인데.
	// 0, 1 1개를 세팅하겠다.
	// m_pBuffer 이 버텍스 버퍼를
	// 그 버텍스 버퍼의 버텍스 1개의 크기는 m_Size
	// 그 버텍스의 버퍼의 0번부터 세팅하겠다.
	// 100
	// 50번째부터도 세팅 가능.
	Game_Device::MAINOBJ()->Context()->IASetVertexBuffers(0, 1, &m_pBuffer, &m_Size, &m_Offset);
}

