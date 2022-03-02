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
	// ����»�
	// ����̽� INIT���� ������ �ǰ�.
	// ���⼭ ����̽��� ��������� �Ǹ�

	if (nullptr == _Window)
	{
		AMSG(L"�������� �ʴ� �����쿡 ����̽��� ������Ű���� ������� �߽��ϴ�");
	}

	m_WINDOW = _Window;

	int iFlag = 0;

#ifdef _DEBUG
	// ����̽� ����� ��尡 ����� ��� 
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// ����̽� ����
	D3D_FEATURE_LEVEL eLV = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

	if (S_OK != D3D11CreateDevice(
		nullptr, 
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		iFlag,
		nullptr, // ������ �ִ���� �˾Ƴ��� ��
		0,
		D3D11_SDK_VERSION, // ���� �����쿡 ��ġ�� SDK ����
		&m_pDevice,
		&eLV,
		&m_pContext))
	{
		AMSG(L"����̽��� ���ؽ�Ʈ ������ �����߽��ϴ�.");
	}

	m_MC = 4; // MC
	// 
	if (S_OK != m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R32G32B32A32_UINT, m_MC, &m_MQ))
	{
		m_MC = 1; // MC
		if (S_OK != m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UINT, m_MC, &m_MQ))
		{
			AMSG(L"��Ƽ���ø� üũ�� ����� ���� �ʾҽ��ϴ�.");
		}
	}

	CreateSwapChain();
	CreateBackBufferTarget();

	// �ϳ��� ����̽��� ��������� �ʾҴٸ�
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

	// �����е� ��Ƽ�� ��� ������ ���ͼ�
	SCDESC.BufferDesc.RefreshRate.Numerator = 60;
	SCDESC.BufferDesc.RefreshRate.Denominator = 1;

	SCDESC.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SCDESC.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SCDESC.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// �̰� ����� �뵵�� ����?
	SCDESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	SCDESC.SampleDesc.Quality = 0;
	SCDESC.SampleDesc.Count = 1;
	SCDESC.OutputWindow = m_WINDOW->WINHWND();

	SCDESC.BufferCount = 2;

	SCDESC.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	SCDESC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// ��üȭ��
	// true�� ���Ѵ�.
	SCDESC.Windowed = true;

	IDXGIDevice* pD = nullptr;
	IDXGIAdapter* pA = nullptr;
	IDXGIFactory* pF = nullptr;

	// �츮���� �����ϴ� ���� API
	// ������ �뵵
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
		AMSG(L"����ü�� ������ �����߽��ϴ�.");
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
		AMSG(L"����ü�ο� �ؽ�ó�� �������� �ʽ��ϴ�.");
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