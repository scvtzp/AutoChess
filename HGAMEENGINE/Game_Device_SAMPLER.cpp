#include "Game_Device.h"
#include <HGAMEIO.h>

#include "HSAMPLER.h"
#include "HTEXTURE.h"


void Game_Device::SAMPLERINIT()
{
	{
		D3D11_SAMPLER_DESC Desc = {};

		Desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;

		// 이미지 압축 방식 사용안함
		Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		Desc.MaxAnisotropy = 0;
		 
		// 내가 D3D11_COMPARISON_NEVER를 한순간 아래는 사실 큰 의미는 없다.
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Desc.MipLODBias = 1.0f;

		HSAMPLER::Create(L"LCSMP", Desc);
	}

	{
		D3D11_SAMPLER_DESC Desc = {};

		Desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

		// 이미지 압축 방식 사용안함
		Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		Desc.MaxAnisotropy = 0;

		// 내가 D3D11_COMPARISON_NEVER를 한순간 아래는 사실 큰 의미는 없다.
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Desc.MipLODBias = 1.0f;

		HSAMPLER::Create(L"LWSMP", Desc);
	}

	{
		D3D11_SAMPLER_DESC Desc = {};

		Desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;

		// 이미지 압축 방식 사용안함
		Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		Desc.MaxAnisotropy = 0;

		// 내가 D3D11_COMPARISON_NEVER를 한순간 아래는 사실 큰 의미는 없다.
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Desc.MipLODBias = 1.0f;

		HSAMPLER::Create(L"PCSMP", Desc);
	}

	{
		D3D11_SAMPLER_DESC Desc = {};

		Desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;

		// 이미지 압축 방식 사용안함
		Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		Desc.MaxAnisotropy = 0;

		// 내가 D3D11_COMPARISON_NEVER를 한순간 아래는 사실 큰 의미는 없다.
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Desc.MipLODBias = 1.0f;

		HSAMPLER::Create(L"PMSMP", Desc);
	}

	{
		D3D11_SAMPLER_DESC Desc = {};

		Desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

		// 이미지 압축 방식 사용안함
		Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		Desc.MaxAnisotropy = 0;

		// 내가 D3D11_COMPARISON_NEVER를 한순간 아래는 사실 큰 의미는 없다.
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Desc.MipLODBias = 1.0f;

		HSAMPLER::Create(L"PWSMP", Desc);
	}

	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");

		auto FileList = Dic.DirAllFile();

		for (auto& _File : FileList)
		{
			HTEXTURE::Load(_File);
		}
	}

}

