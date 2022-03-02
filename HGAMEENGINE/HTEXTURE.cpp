#include "HTEXTURE.h"

HTEXTURE::HTEXTURE() : m_SRV(nullptr), m_RTV(nullptr), m_DSV(nullptr), m_pTex(nullptr)
{
}
HTEXTURE::~HTEXTURE() 
{
	if (nullptr != m_SRV) { 
		m_SRV->Release();
	};
	if(nullptr != m_RTV	) { 
		m_RTV->Release();
	};
	if(nullptr != m_DSV	) { 
		m_DSV->Release();
	};
	if(nullptr != m_pTex) {
		m_pTex->Release();
	};

}

void HTEXTURE::Load() 
{
	Game_String Ext = GameFile.Ext();

	// 확장자의 대소문자 구분상관없이 처리할수 있게 해보세요????
	// 어떻게 하면 될까?
	if (Ext == L"dds")
	{
		
	}
	else if(Ext == L"tga")
	{
		if (S_OK != DirectX::LoadFromTGAFile(GameFile.FullPath(), nullptr, m_Img))
		{
			AMSG(GameFile.FullPath() + L" : 텍스처 로딩에 실패했습니다.");
			return;
		}
	}
	else 
	{
		// 로딩만 한거지 권한은 아직 얻어온게 아니다.
		if (S_OK != DirectX::LoadFromWICFile(GameFile.FullPath(), DirectX::WIC_FLAGS_NONE, nullptr, m_Img))
		{
			AMSG(GameFile.FullPath() + L" : 텍스처 로딩에 실패했습니다.");
			return;
		}
	}


	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (0 != (D3D11_BIND_SHADER_RESOURCE & Desc.BindFlags))
	{
		// m_SRV
		if (S_OK != DirectX::CreateShaderResourceView(Game_Device::MAINOBJ()->Device(), m_Img.GetImages(), m_Img.GetImageCount(), m_Img.GetMetadata(), &m_SRV))
		{
			AMSG(GameFile.FullPath() + L" : 쉐이더 리소스 생성에 실패했습니다.");
			return;
		}
	}

	Desc.Width = (unsigned int)m_Img.GetMetadata().width;
	Desc.Height = (unsigned int)m_Img.GetMetadata().height;

}

Game_Vector HTEXTURE::Size() 
{
	return Game_Vector((float)Desc.Width, (float)Desc.Height, 1.0F );
}

void HTEXTURE::Setting(SHADERTYPE _Type, unsigned int _Reg)
{
	switch (_Type)
	{
	case SHADERTYPE::SHADER_VS:
		Game_Device::MAINOBJ()->Context()->VSSetShaderResources(_Reg, 1, &m_SRV);
		break;
	case SHADERTYPE::SHADER_HS:
		Game_Device::MAINOBJ()->Context()->HSSetShaderResources(_Reg, 1, &m_SRV);
		break;
	case SHADERTYPE::SHADER_DS:
		Game_Device::MAINOBJ()->Context()->DSSetShaderResources(_Reg, 1, &m_SRV);
		break;
	case SHADERTYPE::SHADER_GS:
		Game_Device::MAINOBJ()->Context()->GSSetShaderResources(_Reg, 1, &m_SRV);
		break;
	case SHADERTYPE::SHADER_PS:
		Game_Device::MAINOBJ()->Context()->PSSetShaderResources(_Reg, 1, &m_SRV);
		break;
	case SHADERTYPE::SHADER_END:
		break;
	default:
		break;
	}
}

void HTEXTURE::Reset(SHADERTYPE _Type, unsigned int _Reg) 
{
	ID3D11ShaderResourceView* NULLPTR = nullptr;

	switch (_Type)
	{
	case SHADERTYPE::SHADER_VS:
		Game_Device::MAINOBJ()->Context()->VSSetShaderResources(_Reg, 1, &NULLPTR);
		break;
	case SHADERTYPE::SHADER_HS:
		Game_Device::MAINOBJ()->Context()->HSSetShaderResources(_Reg, 1, &NULLPTR);
		break;
	case SHADERTYPE::SHADER_DS:
		Game_Device::MAINOBJ()->Context()->DSSetShaderResources(_Reg, 1, &NULLPTR);
		break;
	case SHADERTYPE::SHADER_GS:
		Game_Device::MAINOBJ()->Context()->GSSetShaderResources(_Reg, 1, &NULLPTR);
		break;
	case SHADERTYPE::SHADER_PS:
		Game_Device::MAINOBJ()->Context()->PSSetShaderResources(_Reg, 1, &NULLPTR);
		break;
	case SHADERTYPE::SHADER_END:
		break;
	default:
		break;
	}
}

void HTEXTURE::Create(Game_Vector _Size, DXGI_FORMAT _Fmt, UINT _BindFlag, D3D11_USAGE _eUsage) 
{
	D3D11_TEXTURE2D_DESC _NewDesc = {0,};
	_NewDesc.Width = (unsigned)_Size.IX();
	_NewDesc.Height = (unsigned)_Size.IY();
	_NewDesc.ArraySize = 1; //
	_NewDesc.Usage = _eUsage;
	_NewDesc.Format = _Fmt;

	_NewDesc.SampleDesc.Count = 1;
	_NewDesc.SampleDesc.Quality = 0;
	_NewDesc.MipLevels = 1;
	_NewDesc.BindFlags = _BindFlag;

	Create(_NewDesc);
}

void HTEXTURE::Create(D3D11_TEXTURE2D_DESC _Desc) 
{
	Desc = _Desc;

	//  cpu에서 접근가능?
	if (Desc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
	{
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		Desc.CPUAccessFlags = 0;
	}

	// 텍스처를 만들어주는 것.
	// m_pTex을 얻어옵니다.
	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateTexture2D(&Desc, nullptr, (ID3D11Texture2D**)&m_pTex))
	{
		assert(false);
	}

	SettingData();
}

float4 HTEXTURE::GetPixel(int2 _Pos) 
{
	if (Size().x <= _Pos.x)
	{
		_Pos.x = 0;
	}

	if (Size().y <= _Pos.y)
	{
		_Pos.y = 0;
	}

	if (0 > _Pos.y)
	{
		_Pos.y = 0;
	}

	if (0 > _Pos.x)
	{
		_Pos.x = 0;
	}

	uint8_t* Ptr = m_Img.GetPixels();
	Game_Vector Color = Game_Vector::ZERO;
	DXGI_FORMAT For = m_Img.GetMetadata().format;
	switch (For)
	{
	case DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM:
		// 4바이트 이므로
		Ptr = Ptr + (((_Pos.y * (int)Size().x) + _Pos.x) * 4);
		Color.x = Ptr[0] / 255.0f; // R;
		Color.y = Ptr[1] / 255.0f; // G;
		Color.z = Ptr[2] / 255.0f; // B;
		Color.w = Ptr[3] / 255.0f; // A;
		break;
	default:
		break;
	}
	return Color;
}

void HTEXTURE::SetPixel(void* _PixelData, int Size)
{
	D3D11_MAPPED_SUBRESOURCE MSUB;

	// 상수버퍼는 어디에 있는 리소스죠?
	// 그래픽카드에 있는데
	// 그걸 수정하려면
	// 그래픽카드(Device And Context)에게 허락을 맡아야 한다.
	// 메모리와 관련된 거니까 Device일거 같은데 아닙니다.
	//Game_Device::MainContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_Sub);
	//memcpy_s(m_Sub.pData, m_BufferDesc.ByteWidth, _pData, m_BufferDesc.ByteWidth);
	// 다썼으면 다썼다고 알려주는것도 꼭 해야한다.
	//Game_Device::MainContext()->Unmap(m_pBuffer, 0);

	// 랜더링 처리가 멈춘다.
	// 대부분 불가능하다.
	// 가능하다고 하더라도 위험하죠.
	// 그 위험을 피하기 위해서 멈춰.
	// => 느려진다.
	Game_Device::MAINOBJ()->Context()->Map(m_pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &MSUB);
	// MSUB에 그래픽카드와 연결된 포인터를 우리에게 준다.
	// 여기에 있는 포인터를 수정하면
	// 실제 그래픽 카드에 있는 데이터가 수정되는 것이다.
	// 많이 쓰면안된다.
	// 꼭해야한다면 몰아서 해라.
	// 134 * 16 * 4 만큼 넣어준것 바이트로 계산해 본다면
	// 134 * 픽셀하나 16 * 4
	memcpy_s(MSUB.pData, Size, _PixelData, Size);
	Game_Device::MAINOBJ()->Context()->Unmap(m_pTex, 0);
}


void HTEXTURE::SettingData() 
{
	// 뎁스 스탠실이면
// 쉐이더 리소스가 불가능
	if (0 != (D3D11_BIND_DEPTH_STENCIL & Desc.BindFlags))
	{
		if (S_OK != Game_Device::MAINOBJ()->Device()->CreateDepthStencilView(m_pTex, nullptr, &m_DSV))
		{
			AMSG(L"쉐이더 리소스 생성에 실패했습니다.");
		}
	}
	else 
	{
		// 바인드 플래그가
		// Desc.BindFlags 안에 D3D11_BIND_SHADER_RESOURCE 데이터가 들어있다는 것.
		// 이걸 만들어주지 않으면 이 텍스처를 쉐이더에 세팅을 해줄수가 없다.
		if (0 != (D3D11_BIND_SHADER_RESOURCE & Desc.BindFlags))
		{
			if (S_OK != Game_Device::MAINOBJ()->Device()->CreateShaderResourceView(m_pTex, nullptr, &m_SRV))
			{
				AMSG(L"쉐이더 리소스 생성에 실패했습니다.");
			}
		}

		if (0 != (D3D11_BIND_RENDER_TARGET & Desc.BindFlags))
		{
			if (S_OK != Game_Device::MAINOBJ()->Device()->CreateRenderTargetView(m_pTex, nullptr, &m_RTV))
			{
				AMSG(L"랜더 타겟 생성에 실패했습니다.");
			}
		}
	}
}

void HTEXTURE::Create(ID3D11Texture2D* _BackBufferTex) 
{
	m_pTex = _BackBufferTex;
	_BackBufferTex->GetDesc(&Desc);
	SettingData();
}
