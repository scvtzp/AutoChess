#pragma once
#include "Game_Device.h"
#include <HMGR.h>

class HDEPTHSTENCIL : public HMGR<HDEPTHSTENCIL>
{
public:
	static void Create(const Game_String& _Name, const D3D11_DEPTH_STENCIL_DESC& Desc)
	{
		CreateToInsert(_Name, Desc);
	}

private:
	D3D11_DEPTH_STENCIL_DESC m_Desc;
	ID3D11DepthStencilState* m_State;

public:
	void Create();
	void Setting();

public:
	HDEPTHSTENCIL(const D3D11_DEPTH_STENCIL_DESC& _Desc);
	~HDEPTHSTENCIL();
};

