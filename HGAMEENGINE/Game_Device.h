#pragma once
#include <HGAMEMATH.h>
#include <HGAMEDEBUG.h>

#include <HPTR.h>
#include <HMGR.h>
#include <HGAMEWINDOW.h>
#pragma comment(lib, "HGAMEBASE.lib")

// 어떤 윈도우에 3D화면을 띄울건데 그걸 위해서
// 그래픽카드를 쓸거니까 그 권한을 나한테 줘. 
// 그래픽 카드를 쓸수 있는 접근권한을 나한테 주라는 개념
class Game_RenderTarget;
class Game_Device : public HFOBJMGR<Game_Device>
{
public:
	static bool bDefResInit;

	static Game_Ptr<Game_Device> Create(const Game_String& _Window)
	{
		Game_Ptr<Game_Device> DEVICE = CreateToInsert(_Window);
		DEVICE->Init(HGAMEWINDOW::Find(_Window));
		return DEVICE;
	}

private:
	Game_Ptr<HGAMEWINDOW> m_WINDOW;

	// 옵션이 그 기술쓰겠다. = true
	UINT m_MC;
	UINT m_MQ;
	Game_Vector m_Color;					// 초기화 색깔. 
	ID3D11Device*		  m_pDevice;	// 그래픽카드의 메모리를 담당한다. 그래픽 카드의 메모리를 조작 및 할당 가능하다. 로딩
	ID3D11DeviceContext*  m_pContext;	// 그래픽카드의 연산을 담당한다. 랜더링

	// 스왑체인 다이렉트의 화면을 출력하는 시스템의 개념을
	// 기기가 특별해 지기 때문에. 
	IDXGISwapChain*			m_pSwapChain;	// 화면의 클리어와 출력을 담당한다.(ex) api 더블 버퍼링)
	Game_Ptr<Game_RenderTarget>		m_BackBufferRenderTarget;
	Game_Ptr<Game_RenderTarget>		m_UserDataTarget;

public:
	Game_Ptr<Game_RenderTarget> BackBufferRenderTarget();
	Game_Ptr<Game_RenderTarget> DataRenderTarget();



public:
	ID3D11Device* Device() {
		return m_pDevice;	
	}
	ID3D11DeviceContext* Context() {
		return m_pContext;
	}

public:
	Game_Device();
	~Game_Device();

public:
	void CreateSwapChain();
	void CreateBackBufferTarget();

public:
	void RenderStart();
	void RenderEnd();

	void MESHINIT();
	void MATERIALINIT();
	void SHADERINIT();
	void RASTERIZERINIT();
	void DEPTHINIT();
	void BLENDINIT();
	void SAMPLERINIT();

	static void Reset();

private:
	void Init(const Game_Ptr<HGAMEWINDOW>& _Window);

};
