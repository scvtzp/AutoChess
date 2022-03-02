#pragma once
#include "Game_Device.h"

// 인덱스 버퍼도 버퍼로 치고
// 버텍스 버퍼도 버퍼로 보고
// 상수 버퍼도 버퍼로 본다.

class HDEVICEBUFFER
{
protected:
	// D3D11_MAPPED_SUBRESOURCE m_SubRes;
	D3D11_BUFFER_DESC m_BufferDesc;
	ID3D11Buffer* m_pBuffer;

public:
	HDEVICEBUFFER() : m_pBuffer(nullptr), m_BufferDesc({0,})
	{
		
	}
	~HDEVICEBUFFER() 
	{
		if (nullptr != m_pBuffer)
		{
			m_pBuffer->Release();
		}
	}


};

