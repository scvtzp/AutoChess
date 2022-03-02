#include "Game_Device.h"
#include <HGAMEIO.h>

#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "DEFVTXHEADER.h"

#include "Game_Device.h"
#include "HDEPTHSTENCIL.h"

#pragma warning( push )
#pragma warning( disable : 26444 )

void Game_Device::DEPTHINIT()
{
	{
		D3D11_DEPTH_STENCIL_DESC Desc = { 0, };
		Desc.DepthEnable = true;
		Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		Desc.StencilEnable = false;

		HDEPTHSTENCIL::Create(L"DEFDEPTH", Desc);
	}

	{
		D3D11_DEPTH_STENCIL_DESC Desc = { 0, };
		Desc.DepthEnable = true;
		Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		Desc.StencilEnable = false;
		HDEPTHSTENCIL::Create(L"EDEPTH", Desc);
	}

	{
		D3D11_DEPTH_STENCIL_DESC Desc = { 0, };

		//if (���� �׸�����z > ���� �׷����¾�z)
		//{
		//	�׷�����
		//}
		//else {
		//	�׷����� �ʴ´�.
		//}
		Desc.DepthEnable = true;
		Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		// üũ
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		Desc.StencilEnable = false;

		HDEPTHSTENCIL::Create(L"ALWAYSDEPTH", Desc);
	}

	{
		D3D11_DEPTH_STENCIL_DESC Desc = { 0, };

		//if (���� �׸�����z > ���� �׷����¾�z)
		//{
		//	�׷�����
		//}
		//else {
		//	�׷����� �ʴ´�.
		//}
		Desc.DepthEnable = true;
		Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		// üũ
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		Desc.StencilEnable = false;

		HDEPTHSTENCIL::Create(L"ALWAYSZERODEPTH", Desc);
	}


}

#pragma warning( pop )