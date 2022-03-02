#include "Game_Device.h"
#include "Game_3D_Debug.h"
#include "HRenderTarget.h"

bool Game_Device::bDefResInit = false;

Game_Device::Game_Device() 
	: m_MC(0)
	, m_MQ(0)
	, m_Color(Game_Vector::BLUE)
	, m_pDevice(nullptr)
	, m_pContext(nullptr)
	, m_pSwapChain(nullptr)
{

}

Game_Device::~Game_Device() 
{
	// if (nullptr != m_DSV)	{		m_DSV->Release();			   }
	// if (nullptr != m_RTV)	{		m_RTV->Release();			   }
	// if (nullptr != m_DepthStencilTex)	{		m_DepthStencilTex->Release();  }
	// if (nullptr != m_BackBufferTex)	{		m_BackBufferTex->Release();	   }
	if (nullptr != m_pSwapChain)	{		m_pSwapChain->Release();	   }
	if (nullptr != m_pContext)	{		m_pContext->Release();		   }
	if (nullptr != m_pDevice)	{		m_pDevice->Release();		   }

}

void Game_Device::Init(const Game_Ptr<HGAMEWINDOW>& _Window) 
{
	// 지우는색
	// 디바이스 INIT으로 들어오게 되고.
	// 여기서 디바이스가 만들어지게 되면

	if (nullptr == _Window)
	{
		AMSG(L"존재하지 않는 윈도우에 디바이스를 장착시키려고 만드려고 했습니다");
	}

	m_WINDOW = _Window;

	int iFlag = 0;

#ifdef _DEBUG
	// 디바이스 만드는 모드가 디버그 모드 
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 디바이스 레벨
	D3D_FEATURE_LEVEL eLV = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

	if (S_OK != D3D11CreateDevice(
		nullptr, 
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		iFlag,
		nullptr, // 가능한 최대버전 알아내는 것
		0,
		D3D11_SDK_VERSION, // 현재 윈도우에 설치된 SDK 버전
		&m_pDevice,
		&eLV,
		&m_pContext))
	{
		AMSG(L"디바이스와 컨텍스트 생성에 실패했습니다.");
	}

	m_MC = 4; // MC
	// 
	if (S_OK != m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R32G32B32A32_UINT, m_MC, &m_MQ))
	{
		m_MC = 1; // MC
		if (S_OK != m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UINT, m_MC, &m_MQ))
		{
			AMSG(L"멀티샘플링 체크가 제대로 되지 않았습니다.");
		}
	}

	CreateSwapChain();
	CreateBackBufferTarget();

	// 하나의 디바이스도 만들어지지 않았다면
	if (false == bDefResInit)
	{
		MESHINIT();
		SHADERINIT();
		RASTERIZERINIT();
		DEPTHINIT();
		BLENDINIT();
		SAMPLERINIT();
		MATERIALINIT();
		Game_RenderTarget::Init();
		Game_3D_Debug::Init(false);
		bDefResInit = true;
	}
}


void Game_Device::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC SCDESC = {0,};

	SCDESC.BufferDesc.Width = m_WINDOW->Size().UIX();
	SCDESC.BufferDesc.Height = m_WINDOW->Size().UIY();

	// 여러분들 모티터 기기 정보를 얻어와서
	SCDESC.BufferDesc.RefreshRate.Numerator = 60;
	SCDESC.BufferDesc.RefreshRate.Denominator = 1;

	SCDESC.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SCDESC.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SCDESC.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 이걸 만드는 용도가 뭐냐?
	SCDESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	SCDESC.SampleDesc.Quality = 0;
	SCDESC.SampleDesc.Count = 1;
	SCDESC.OutputWindow = m_WINDOW->WINHWND();

	SCDESC.BufferCount = 2;

	SCDESC.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	SCDESC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// 전체화면
	// true면 안한다.
	SCDESC.Windowed = true;

	IDXGIDevice* pD = nullptr;
	IDXGIAdapter* pA = nullptr;
	IDXGIFactory* pF = nullptr;

	// 우리에게 제공하는 말단 API
	// 얻어오는 용도
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pD);
	if (nullptr == pD)
	{
		AMSG(L"if (nullptr == pD)");
	}

	pD->GetParent(__uuidof(IDXGIAdapter), (void**)&pA);
	if (nullptr == pA)
	{
		AMSG(L"if (nullptr == pA)");
	}

	pA->GetParent(__uuidof(IDXGIFactory), (void**)&pF);
	if (nullptr == pF)
	{
		AMSG(L"if (nullptr == pF)");
	}

	if (S_OK != pF->CreateSwapChain(m_pDevice, &SCDESC, &m_pSwapChain))
	{
		AMSG(L"스왑체인 생성에 실패했습니다.");
	}

	pF->Release();
	pA->Release();
	pD->Release();

}

void Game_Device::CreateBackBufferTarget() 
{
	ID3D11Texture2D* m_BackBufferTex;

	if (S_OK != m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_BackBufferTex))
	{
		AMSG(L"스왑체인에 텍스처가 존재하지 않습니다.");
	}

	m_BackBufferRenderTarget = new Game_RenderTarget();
	m_BackBufferRenderTarget->CreateTarget(m_BackBufferTex, Game_Vector::BLUE, true);
	m_BackBufferRenderTarget->CreateDepth(0);


	m_UserDataTarget = new Game_RenderTarget();
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);
	m_UserDataTarget->CreateTarget(m_BackBufferRenderTarget->Texture(0)->Size(), Game_Vector::ZERO);

}

void Game_Device::RenderStart() 
{
	m_BackBufferRenderTarget->Clear(true);
	m_BackBufferRenderTarget->Setting();
}

void Game_Device::RenderEnd() 
{
	m_pSwapChain->Present(0, 0);
}



void Game_Device::Reset() 
{
	Game_Device::MAINOBJ()->Context()->PSSetShader(nullptr, 0, 0);
	Game_Device::MAINOBJ()->Context()->GSSetShader(nullptr, 0, 0);
	Game_Device::MAINOBJ()->Context()->HSSetShader(nullptr, 0, 0);
	Game_Device::MAINOBJ()->Context()->DSSetShader(nullptr, 0, 0);
	Game_Device::MAINOBJ()->Context()->VSSetShader(nullptr, 0, 0);
}

Game_Ptr<Game_RenderTarget> Game_Device::BackBufferRenderTarget() 
{
	return m_BackBufferRenderTarget;
}

Game_Ptr<Game_RenderTarget> Game_Device::DataRenderTarget() 
{
	return m_UserDataTarget;
}