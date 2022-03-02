#pragma once
#include "HSHADER.h"
#include <HMGR.h>
#include <HPTR.h>

#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class HTEXTURE : public HFILEMGR<HTEXTURE>
{
public:
	static Game_Ptr<HTEXTURE> Load(const HGAMEFILE& _File)
	{
		Game_Ptr<HTEXTURE> Res = CreateToInsertToFile(_File.FullPath());
		Res->Load();
		return Res;
	}

	static Game_Ptr<HTEXTURE> Load(const Game_String& _Path)
	{
		Game_Ptr<HTEXTURE> Res = CreateToInsertToFile(_Path);
		Res->Load();
		return Res;
	}

private:
	// 라이브러리에서 지원해주는 구조체
	DirectX::ScratchImage m_Img;
	D3D11_TEXTURE2D_DESC Desc; // 텍스처 정보
	ID3D11Resource* m_pTex; // 텍스처 메모리 권한

	ID3D11RenderTargetView* m_RTV; // 텍스처 랜더타겟 권한
	ID3D11DepthStencilView* m_DSV; // 텍스처 깊이타겟 권한

	// 이 권한을 이용해서 넣어줘야 한다.
	ID3D11ShaderResourceView* m_SRV; // 텍스처 쉐이더 세팅 권한

public:
	const D3D11_TEXTURE2D_DESC& TextDesc() 
	{
		return Desc;
	}

public:
	ID3D11DepthStencilView* DSV() {
		return m_DSV;
	}

	ID3D11RenderTargetView* RTV() {
		return m_RTV;
	}

public:
	Game_Vector Size();

public:
	HTEXTURE();
	~HTEXTURE();

public:
	// 커스텀한 텍스처를 만들수 있어야 한다.
	void Create(Game_Vector _Size, DXGI_FORMAT _Fmt, UINT _BindFlag, D3D11_USAGE _eUsage);
	void Create(D3D11_TEXTURE2D_DESC _Desc);
	void Create(ID3D11Texture2D* _BackBufferTex);

public:
	void Load();
	void Setting(SHADERTYPE _Type, unsigned int _Reg);
	void Reset(SHADERTYPE _Type, unsigned int _Reg);
	void SetPixel(void* _PixelData, int Size);
	float4 GetPixel(int2 _Pos);

	void SettingData();
};

