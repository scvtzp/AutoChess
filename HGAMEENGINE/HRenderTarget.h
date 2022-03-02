#pragma once
#include "HGAMERES.h"
#include "Game_Device.h"
#include <vector>
#include "HRENDERER.h"
#include "HTEXTURE.h"

struct MergeOption
{
	float AlphaOption = 0;
	float Dummy00 = 0;
	float Dummy01 = 0;
	float Dummy02 = 0;
};

class HTEXTRUE;
class HRENDERPLAYER;
class Game_RenderTarget : public HMGR<Game_RenderTarget>
{
public:
	friend Game_Device;

private:
	static void Init();

private:
	static HRENDERPLAYER TargetMergePlayer;
	static HRENDERPLAYER TargetPlusPlayer;


private:
	std::vector<Game_Ptr<HTEXTURE>> m_AllTexture;
	std::vector<Game_Vector> m_ClearColor;
	std::vector<D3D11_VIEWPORT> m_VIEWPORT;
	std::vector<bool> m_Clear;
	// T * 
	// ID3D11RenderTargetView**
	// ID3D11RenderTargetView**
	std::vector<ID3D11RenderTargetView*> m_AllRTV;

	Game_Ptr<HTEXTURE> m_Depth = nullptr;

	MergeOption Option = {0, };

	void CreateViewPort(D3D11_TEXTURE2D_DESC _Desc);

public:
	Game_Ptr<HTEXTURE> Texture(int _index) 
	{
		return m_AllTexture[_index];
	}

	void Clear(bool _IsDepthClear); // 출력 대상이 되는것.
	void Setting(); // 출력 대상이 되는것.


	void Reset(); // 출력 대상이 되는것.


	void CreateDepth(unsigned int _TargetIndex = 0);

	void Setting(int _Index); // 출력 대상이 되는것.

	void Depth(Game_Ptr<HTEXTURE> _Depth);

	Game_Ptr<HTEXTURE> Depth();


public:
	// 순수하게 직접 만들때.
	void CreateTarget(Game_Vector _Size,  Game_Vector _ClearColor, bool _Clear = true, DXGI_FORMAT _Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, unsigned int _BindFlag = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	// 백버퍼가 사용.
	void CreateTarget(ID3D11Texture2D* _BackBufferTex, Game_Vector _ClearColor, bool _Clear);

	// 다른 타겟의 타겟을 내것으로 쓸때.
	void CreateTarget(Game_Ptr<HTEXTURE> _Target, Game_Vector _ClearColor, bool _Clear);

	void Merge(Game_Ptr<Game_RenderTarget> _Target, int _ThisIndex = 0, int _OtherIndex = 0, bool _AlphaOption = false);
	void Plus(Game_Ptr<Game_RenderTarget> _Target, int _ThisIndex = 0, int _OtherIndex = 0, bool _AlphaOption = false);

public:
	Game_RenderTarget();
	~Game_RenderTarget();
};

