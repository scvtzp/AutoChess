#pragma once
#include <HGAMEMATH.h>
#include <HGAMEDEBUG.h>

#include <HPTR.h>
#include <HMGR.h>
#include <HGAMEWINDOW.h>
#pragma comment(lib, "HGAMEBASE.lib")

// � �����쿡 3Dȭ���� ���ǵ� �װ� ���ؼ�
// �׷���ī�带 ���Ŵϱ� �� ������ ������ ��. 
// �׷��� ī�带 ���� �ִ� ���ٱ����� ������ �ֶ�� ����
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

	// �ɼ��� �� ������ڴ�. = true
	UINT m_MC;
	UINT m_MQ;
	Game_Vector m_Color;					// �ʱ�ȭ ����. 
	ID3D11Device*		  m_pDevice;	// �׷���ī���� �޸𸮸� ����Ѵ�. �׷��� ī���� �޸𸮸� ���� �� �Ҵ� �����ϴ�. �ε�
	ID3D11DeviceContext*  m_pContext;	// �׷���ī���� ������ ����Ѵ�. ������

	// ����ü�� ���̷�Ʈ�� ȭ���� ����ϴ� �ý����� ������
	// ��Ⱑ Ư���� ���� ������. 
	IDXGISwapChain*			m_pSwapChain;	// ȭ���� Ŭ����� ����� ����Ѵ�.(ex) api ���� ���۸�)
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
