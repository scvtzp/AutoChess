#include "Game_Device.h"
#include <HGAMEIO.h>

#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "DEFVTXHEADER.h"

#include "Game_Device.h"
#include "HRASTERIZER.h"

#pragma warning( push )
#pragma warning( disable : 26444 )

void Game_Device::RASTERIZERINIT()
{

	{
		D3D11_RASTERIZER_DESC Desc = { D3D11_FILL_MODE::D3D11_FILL_SOLID , D3D11_CULL_MODE::D3D11_CULL_BACK , 0};
		Desc.MultisampleEnable = true;
		Desc.AntialiasedLineEnable = true;

		HRASTERIZER::Create(L"BACK", Desc);
	}

	{
		D3D11_RASTERIZER_DESC Desc =
		{ D3D11_FILL_MODE::D3D11_FILL_WIREFRAME , D3D11_CULL_MODE::D3D11_CULL_BACK , 0 };
		Desc.MultisampleEnable = true;
		Desc.AntialiasedLineEnable = true;
		HRASTERIZER::Create(L"WBACK", Desc);
	}

	{
		D3D11_RASTERIZER_DESC Desc = { D3D11_FILL_MODE::D3D11_FILL_SOLID , D3D11_CULL_MODE::D3D11_CULL_FRONT , 0 };
		Desc.MultisampleEnable = true;
		Desc.AntialiasedLineEnable = true;

		HRASTERIZER::Create(L"FRONT", Desc);
	}

	{
		D3D11_RASTERIZER_DESC Desc =
		{ D3D11_FILL_MODE::D3D11_FILL_WIREFRAME , D3D11_CULL_MODE::D3D11_CULL_FRONT , 0 };
		Desc.MultisampleEnable = true;
		Desc.AntialiasedLineEnable = true;
		HRASTERIZER::Create(L"WFRONT", Desc);
	}

	{
		D3D11_RASTERIZER_DESC Desc = { D3D11_FILL_MODE::D3D11_FILL_SOLID , D3D11_CULL_MODE::D3D11_CULL_NONE , 0 };
		Desc.MultisampleEnable = true;
		Desc.AntialiasedLineEnable = true;

		HRASTERIZER::Create(L"NONE", Desc);
	}

	{
		D3D11_RASTERIZER_DESC Desc =
		{ D3D11_FILL_MODE::D3D11_FILL_WIREFRAME , D3D11_CULL_MODE::D3D11_CULL_NONE , 0 };
		Desc.MultisampleEnable = true;
		Desc.AntialiasedLineEnable = true;
		HRASTERIZER::Create(L"WNONE", Desc);
	}

}

#pragma warning( pop )