#include "HRenderTarget.h"
#include "HTEXTURE.h"

HRENDERPLAYER Game_RenderTarget::TargetMergePlayer;
HRENDERPLAYER Game_RenderTarget::TargetPlusPlayer;

void Game_RenderTarget::Init()
{
	TargetMergePlayer.MESH(L"TargetFullRect");
	TargetMergePlayer.MATERIAL(L"TargetMerge");
	TargetMergePlayer.SAMPLER(L"SAMPLER", L"LWSMP");

	TargetPlusPlayer.MESH(L"TargetFullRect");
	TargetPlusPlayer.MATERIAL(L"TargetPlus");
	TargetPlusPlayer.SAMPLER(L"SAMPLER", L"LWSMP");

	
}

Game_RenderTarget::Game_RenderTarget() : m_Depth(nullptr)
{

}
Game_RenderTarget::~Game_RenderTarget()
{

}

void Game_RenderTarget::CreateViewPort(D3D11_TEXTURE2D_DESC _Desc)
{
	D3D11_VIEWPORT ViewPort;
	ViewPort = { 0, };
	// 윈도우 좌표계 기반이다.
	ViewPort.TopLeftX = 0.0f;
	ViewPort.TopLeftY = 0.0f;
	ViewPort.Width = (float)_Desc.Width;
	ViewPort.Height = (float)_Desc.Height;
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;
	m_VIEWPORT.push_back(ViewPort);
}

void Game_RenderTarget::Clear(bool _IsDepthClear)
{
	if (true == _IsDepthClear && nullptr != m_Depth)
	{
		Game_Device::MAINOBJ()->Context()->ClearDepthStencilView(m_Depth->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	for (size_t i = 0; i < m_AllTexture.size(); i++)
	{
		if (false == m_Clear[i])
		{
			continue;
		}

		Game_Device::MAINOBJ()->Context()->ClearRenderTargetView(m_AllTexture[i]->RTV(), m_ClearColor[i].Arr);
	}

}

void Game_RenderTarget::Setting(int _Index)
{
	Game_Device::MAINOBJ()->Context()->RSSetViewports((unsigned int)1, &m_VIEWPORT[_Index]);

	if (nullptr != m_Depth)
	{
		Game_Device::MAINOBJ()->Context()->OMSetRenderTargets(1, &m_AllRTV[_Index], m_Depth->DSV());

	}
	else
	{
		Game_Device::MAINOBJ()->Context()->OMSetRenderTargets(1, &m_AllRTV[_Index], nullptr);
	}
}

void Game_RenderTarget::Setting()
{
	
	Game_Device::MAINOBJ()->Context()->RSSetViewports((unsigned int)m_VIEWPORT.size(), &m_VIEWPORT[0]);

	if (nullptr != m_Depth)
	{
		Game_Device::MAINOBJ()->Context()->OMSetRenderTargets((unsigned int)m_AllRTV.size(), &m_AllRTV[0], m_Depth->DSV());

	}
	else 
	{
		Game_Device::MAINOBJ()->Context()->OMSetRenderTargets((unsigned int)m_AllRTV.size(), &m_AllRTV[0], nullptr);
	}
}

void Game_RenderTarget::Reset()
{
	ID3D11RenderTargetView* ArrNullptr[8] = {nullptr,};
	Game_Device::MAINOBJ()->Context()->OMSetRenderTargets((unsigned int)m_AllRTV.size(), ArrNullptr, nullptr);
}


void Game_RenderTarget::CreateTarget(Game_Vector _Size, Game_Vector _ClearColor, bool _Clear, DXGI_FORMAT _Fmt, unsigned int _BindFlag)
{
	HTEXTURE* NewRenderTargetTexture = new HTEXTURE();
	// D3D11_USAGE::D3D11_USAGE_DEFAULT 그래픽카드에 메모리 할당해라. 그리고 안건들겠다.
	// D3D11_USAGE::D3D11_USAGE_DYNAMIC 그래픽카드에 메모리를 할당해라. 그리고 건들겠다.
	// D3D11_USAGE::D3D11_USAGE_ 그래픽카드에 메모리를 할당해라. 그리고 건들겠다.
	NewRenderTargetTexture->Create(_Size, _Fmt, _BindFlag, D3D11_USAGE::D3D11_USAGE_DEFAULT);

	m_AllTexture.push_back(NewRenderTargetTexture);
	if (nullptr == NewRenderTargetTexture->RTV())
	{
		AMSG(L"if (nullptr == NewRenderTargetTexture->RTV())");
	}

	m_ClearColor.push_back(_ClearColor);

	CreateViewPort(NewRenderTargetTexture->TextDesc());
	m_AllRTV.push_back(NewRenderTargetTexture->RTV());
	m_Clear.push_back(_Clear);
}

void Game_RenderTarget::CreateTarget(ID3D11Texture2D* _BackBufferTex, Game_Vector _ClearColor /*= Game_Vector::BLUE*/, bool _Clear)
{
	HTEXTURE* NewRenderTargetTexture = new HTEXTURE();
	NewRenderTargetTexture->Create(_BackBufferTex);

	CreateTarget(NewRenderTargetTexture, _ClearColor, _Clear);

}

void Game_RenderTarget::CreateTarget(Game_Ptr<HTEXTURE> _TargetTex, Game_Vector _ClearColor, bool _Clear)
{
	m_AllTexture.push_back(_TargetTex);
	if (nullptr == _TargetTex->RTV())
	{
		AMSG(L"if (nullptr == NewRenderTargetTexture->RTV())");
	}

	m_ClearColor.push_back(_ClearColor);
	m_Clear.push_back(_Clear);

	CreateViewPort(_TargetTex->TextDesc());
	m_AllRTV.push_back(_TargetTex->RTV());
}

void Game_RenderTarget::CreateDepth(unsigned int _TargetIndex /*= 0*/)
{
	if (_TargetIndex >= m_AllTexture.size())
	{
		AMSG(L"if (_TargetIndex >= m_AllTexture.size())");
	}

	m_Depth = new HTEXTURE();

	// 깊이 
	// 내부의 맴버변수가 모두 public일때만 된다.
	// 그리고 아무런 생성자도 없어야 한다.
	D3D11_TEXTURE2D_DESC TD = { 0, };

	TD.ArraySize = 1;

	TD.Width = m_AllTexture[0]->TextDesc().Width;
	TD.Height = m_AllTexture[0]->TextDesc().Height;
	TD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TD.SampleDesc.Count = 1;
	TD.SampleDesc.Quality = 0;
	TD.MipLevels = 1;
	// 무조건 그래픽 카드에 
	TD.Usage = D3D11_USAGE_DEFAULT;
	TD.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	m_Depth->Create(TD);

	/*m_pDevice->CreateTexture2D(&TD, 0, &m_DepthStencilTex);
	if (nullptr == m_DepthStencilTex)
	{
		AMSG(L"깊이 버퍼 텍스처를 만들지 못했습니다.");
	}

	m_pDevice->CreateDepthStencilView(m_DepthStencilTex, 0, &m_DSV);
	if (nullptr == m_DSV)
	{
		AMSG(L"깊이 버퍼 사용권한(DSV)을 만들지 못했습니다.");
	}*/
}

void Game_RenderTarget::Depth(Game_Ptr<HTEXTURE> _Depth)
{
	if (nullptr == _Depth)
	{
		AMSG(L"if (nullptr == _Depth)");
	}


	m_Depth = _Depth;
}

Game_Ptr<HTEXTURE> Game_RenderTarget::Depth()
{
	return m_Depth;
}

void Game_RenderTarget::Merge(Game_Ptr<Game_RenderTarget> _Target, int _ThisIndex, int _OtherIndex, bool _AlphaOption)
{
	Setting(_ThisIndex);

	Option.AlphaOption = _AlphaOption;

	TargetMergePlayer.CBUFFER(L"MergeOption", &Option, CBUFFERMODE::CB_LINK);
	TargetMergePlayer.TEXTURE(L"TargetTex", _Target->Texture(_OtherIndex));
	TargetMergePlayer.Render();
	TargetMergePlayer.Reset();
}

void Game_RenderTarget::Plus(Game_Ptr<Game_RenderTarget> _Target, int _ThisIndex/* = 0*/, int _OtherIndex/* = 0*/, bool _AlphaOption /*= false*/)
{
	// 백버퍼 랜더타겟
	Setting(_ThisIndex);

	Option.AlphaOption = _AlphaOption;

	TargetPlusPlayer.CBUFFER(L"MergeOption", &Option, CBUFFERMODE::CB_LINK);
	TargetPlusPlayer.TEXTURE(L"TargetTex", _Target->Texture(_OtherIndex));
	TargetPlusPlayer.Render();
	TargetPlusPlayer.Reset();

}