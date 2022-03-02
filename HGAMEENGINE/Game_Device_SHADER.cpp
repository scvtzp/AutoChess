#include "Game_Device.h"
#include <HGAMEIO.h>

#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "DEFVTXHEADER.h"

#include "Game_Device.h"

#pragma warning( push )
#pragma warning( disable : 26444 )

void Game_Device::SHADERINIT() 
{
	HGAMEDIRECTORY m_Dir;
	m_Dir.MoveParent(L"AutoChess");
	m_Dir.Move(L"HSHADER");

	std::list<HGAMEFILE> File = m_Dir.DirAllFile(L"hlsl");

	for (auto& _Value : File)
	{
		// 쉐이더 파일 내부에
		// 몇개의 함수가 들어있는지 알아야 하고
		// + 그게 함수가 확실하다면
		// 픽셀쉐이더인지 버텍스 쉐이더인지를 구분할수 있어야 한다.

		HSHADER::AutoShaderCompile(_Value.FullPath());
	}

	//{
	//	Game_Ptr<HVTXSHADER> VTX = HVTXSHADER::Load(m_Dir.PlusFileName(L"2DIMG.hlsl"), L"VS_2DIMG");
	//	VTX->AddLayout("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//	VTX->AddLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//	VTX->AddLayout("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//	VTX->CreateLayout();
	//	HPIXSHADER::Load(m_Dir.PlusFileName(L"2DIMG.hlsl"), L"PS_2DIMG");
	//}
}

#pragma warning( pop )