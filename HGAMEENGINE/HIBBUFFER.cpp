#include "HIBBUFFER.h"

HIBBUFFER::HIBBUFFER() : m_Offset(0)
{

}
HIBBUFFER::~HIBBUFFER() 
{

}


void HIBBUFFER::Create(size_t _Count, size_t _Size, const void* _Data, DXGI_FORMAT _Fmt, D3D11_USAGE _USAGE)
{
	m_Fmt = _Fmt;
	m_Size = (unsigned int)_Size;
	m_Count = (unsigned int)_Count;

	//                       4         32
	m_BufferDesc.ByteWidth = (unsigned int)(_Count * _Size);
	m_BufferDesc.Usage = _USAGE; // ��� ������ �޶�. ���� =�׷���ī�忡 ����

	// �̳༮�� cpu���� �����ҰŴ� ���ҰŴ�.
	// D3D11_USAGE::D3D11_USAGE_DEFAULT �׷��� ī�忡 ����Ǵµ� ���� ���Ѵ�.
	// D3D11_USAGE::D3D11_USAGE_DYNAMIC �׷��� ī�忡 ����Ǵµ� ���� �Ѵ�.
	// 
	if (m_BufferDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
	{
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA DS = {};
	DS.pSysMem = _Data;

	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateBuffer(&m_BufferDesc, &DS, &m_pBuffer))
	{
		AMSG(L"���ؽ� ���� ������ �����߽��ϴ�");
	}
}

void HIBBUFFER::Setting() 
{
	Game_Device::MAINOBJ()->Context()->IASetIndexBuffer(m_pBuffer, m_Fmt, m_Offset);
}