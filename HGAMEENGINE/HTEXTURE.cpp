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

	// Ȯ������ ��ҹ��� ���л������ ó���Ҽ� �ְ� �غ�����????
	// ��� �ϸ� �ɱ�?
	if (Ext == L"dds")
	{
		
	}
	else if(Ext == L"tga")
	{
		if (S_OK != DirectX::LoadFromTGAFile(GameFile.FullPath(), nullptr, m_Img))
		{
			AMSG(GameFile.FullPath() + L" : �ؽ�ó �ε��� �����߽��ϴ�.");
			return;
		}
	}
	else 
	{
		// �ε��� �Ѱ��� ������ ���� ���°� �ƴϴ�.
		if (S_OK != DirectX::LoadFromWICFile(GameFile.FullPath(), DirectX::WIC_FLAGS_NONE, nullptr, m_Img))
		{
			AMSG(GameFile.FullPath() + L" : �ؽ�ó �ε��� �����߽��ϴ�.");
			return;
		}
	}


	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (0 != (D3D11_BIND_SHADER_RESOURCE & Desc.BindFlags))
	{
		// m_SRV
		if (S_OK != DirectX::CreateShaderResourceView(Game_Device::MAINOBJ()->Device(), m_Img.GetImages(), m_Img.GetImageCount(), m_Img.GetMetadata(), &m_SRV))
		{
			AMSG(GameFile.FullPath() + L" : ���̴� ���ҽ� ������ �����߽��ϴ�.");
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

	//  cpu���� ���ٰ���?
	if (Desc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
	{
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		Desc.CPUAccessFlags = 0;
	}

	// �ؽ�ó�� ������ִ� ��.
	// m_pTex�� ���ɴϴ�.
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
		// 4����Ʈ �̹Ƿ�
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

	// ������۴� ��� �ִ� ���ҽ���?
	// �׷���ī�忡 �ִµ�
	// �װ� �����Ϸ���
	// �׷���ī��(Device And Context)���� ����� �þƾ� �Ѵ�.
	// �޸𸮿� ���õ� �Ŵϱ� Device�ϰ� ������ �ƴմϴ�.
	//Game_Device::MainContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_Sub);
	//memcpy_s(m_Sub.pData, m_BufferDesc.ByteWidth, _pData, m_BufferDesc.ByteWidth);
	// �ٽ����� �ٽ�ٰ� �˷��ִ°͵� �� �ؾ��Ѵ�.
	//Game_Device::MainContext()->Unmap(m_pBuffer, 0);

	// ������ ó���� �����.
	// ��κ� �Ұ����ϴ�.
	// �����ϴٰ� �ϴ��� ��������.
	// �� ������ ���ϱ� ���ؼ� ����.
	// => ��������.
	Game_Device::MAINOBJ()->Context()->Map(m_pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &MSUB);
	// MSUB�� �׷���ī��� ����� �����͸� �츮���� �ش�.
	// ���⿡ �ִ� �����͸� �����ϸ�
	// ���� �׷��� ī�忡 �ִ� �����Ͱ� �����Ǵ� ���̴�.
	// ���� ����ȵȴ�.
	// ���ؾ��Ѵٸ� ���Ƽ� �ض�.
	// 134 * 16 * 4 ��ŭ �־��ذ� ����Ʈ�� ����� ���ٸ�
	// 134 * �ȼ��ϳ� 16 * 4
	memcpy_s(MSUB.pData, Size, _PixelData, Size);
	Game_Device::MAINOBJ()->Context()->Unmap(m_pTex, 0);
}


void HTEXTURE::SettingData() 
{
	// ���� ���Ľ��̸�
// ���̴� ���ҽ��� �Ұ���
	if (0 != (D3D11_BIND_DEPTH_STENCIL & Desc.BindFlags))
	{
		if (S_OK != Game_Device::MAINOBJ()->Device()->CreateDepthStencilView(m_pTex, nullptr, &m_DSV))
		{
			AMSG(L"���̴� ���ҽ� ������ �����߽��ϴ�.");
		}
	}
	else 
	{
		// ���ε� �÷��װ�
		// Desc.BindFlags �ȿ� D3D11_BIND_SHADER_RESOURCE �����Ͱ� ����ִٴ� ��.
		// �̰� ��������� ������ �� �ؽ�ó�� ���̴��� ������ ���ټ��� ����.
		if (0 != (D3D11_BIND_SHADER_RESOURCE & Desc.BindFlags))
		{
			if (S_OK != Game_Device::MAINOBJ()->Device()->CreateShaderResourceView(m_pTex, nullptr, &m_SRV))
			{
				AMSG(L"���̴� ���ҽ� ������ �����߽��ϴ�.");
			}
		}

		if (0 != (D3D11_BIND_RENDER_TARGET & Desc.BindFlags))
		{
			if (S_OK != Game_Device::MAINOBJ()->Device()->CreateRenderTargetView(m_pTex, nullptr, &m_RTV))
			{
				AMSG(L"���� Ÿ�� ������ �����߽��ϴ�.");
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
