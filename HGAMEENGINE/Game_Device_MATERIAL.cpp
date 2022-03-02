#include "Game_Device.h"
#include <HGAMEIO.h>

#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "DEFVTXHEADER.h"

#include "Game_Device.h"
#include "HMATERIAL.h"

#pragma warning( push )
#pragma warning( disable : 26444 )

void Game_Device::MATERIALINIT()
{
	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"Shadow");
		MAT->VTXSHADER(L"VS_Shadow");
		MAT->PIXSHADER(L"PS_Shadow");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"ShadowBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"HeightMap");
		MAT->VTXSHADER(L"VS_MapDefferdTexture");
		MAT->PIXSHADER(L"PS_MapDefferdTexture");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"EmissionBloomCheck");
		MAT->VTXSHADER(L"VS_EmissionBloomCheck");
		MAT->PIXSHADER(L"PS_EmissionBloomCheck");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSZERODEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"EmissionBloom");
		MAT->VTXSHADER(L"VS_EmissionBloom");
		MAT->PIXSHADER(L"PS_EmissionBloom");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSZERODEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"OUTLINEDRAW");
		MAT->VTXSHADER(L"VS_OUTLINEDRAW");
		MAT->PIXSHADER(L"PS_OUTLINEDRAW");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSZERODEPTH");
		MAT->BLEND(L"AlphaBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"Emission");
		MAT->VTXSHADER(L"VS_Emission");
		MAT->PIXSHADER(L"PS_Emission");
		MAT->RASTERIZER(L"BACK");
		// MAT->DEPTHSTENCIL(L"EDEPTH");
		
		MAT->DEPTHSTENCIL(L"EDEPTH");
		// MAT->DEPTHSTENCIL(L"EQUALDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"OUTLINE");
		MAT->VTXSHADER(L"VS_OUTLINE");
		MAT->PIXSHADER(L"PS_OUTLINE");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSZERODEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"SKY");
		MAT->VTXSHADER(L"VS_SKY");
		MAT->PIXSHADER(L"PS_SKY");
		MAT->RASTERIZER(L"FRONT");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"DebugMesh");
		MAT->VTXSHADER(L"VS_DebugMesh");
		MAT->PIXSHADER(L"PS_DebugMesh");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}




	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"TARGETDEBUGBACK");
		MAT->VTXSHADER(L"VS_TARGETDEBUGBACK");
		MAT->PIXSHADER(L"PS_TARGETDEBUGBACK");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"TARGETDEBUG");
		MAT->VTXSHADER(L"VS_TARGETDEBUG");
		MAT->PIXSHADER(L"PS_TARGETDEBUG");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"DefferdMerge");
		MAT->VTXSHADER(L"VS_DEFFERDMERGE");
		MAT->PIXSHADER(L"PS_DEFFERDMERGE");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"DefferdLight");
		MAT->VTXSHADER(L"VS_DEFFERDLIGHT");
		MAT->PIXSHADER(L"PS_DEFFERDLIGHT");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"OneBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"Defferd");
		MAT->VTXSHADER(L"VS_Defferd");
		MAT->PIXSHADER(L"PS_Defferd");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"Foward");
		MAT->VTXSHADER(L"VS_Foward");
		MAT->PIXSHADER(L"PS_Foward");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"GRID");
		MAT->VTXSHADER(L"VS_GRID");
		MAT->PIXSHADER(L"PS_GRID");
		MAT->RASTERIZER(L"NONE");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"2DIMAGE");
		MAT->VTXSHADER(L"VS_2DIMG");
		MAT->PIXSHADER(L"PS_2DIMG");
		MAT->RASTERIZER(L"BACK");
		//MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"TargetPlus");
		MAT->VTXSHADER(L"VS_TargetMerge");
		MAT->PIXSHADER(L"PS_TargetMerge");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"OneBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"TargetMerge");
		MAT->VTXSHADER(L"VS_TargetMerge");
		MAT->PIXSHADER(L"PS_TargetMerge");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"ALWAYSDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"3DANIDefferdTexture");
		MAT->VTXSHADER(L"VS_3DAniDefferdTexture");
		MAT->PIXSHADER(L"PS_3DAniDefferdTexture");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"3DStaticDefferdTexture");
		MAT->VTXSHADER(L"VS_3DStaticDefferdTexture");
		MAT->PIXSHADER(L"PS_3DStaticDefferdTexture");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}


	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"3DStaticDefferdColor");
		MAT->VTXSHADER(L"VS_3DStaticDefferdColor");
		MAT->PIXSHADER(L"PS_3DStaticDefferdColor");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}

	{
		Game_Ptr<Game_Material> MAT = Game_Material::Create(L"3DStaticTex");
		MAT->VTXSHADER(L"VS_3DStaticTex");
		MAT->PIXSHADER(L"PS_3DStaticTex");
		MAT->RASTERIZER(L"BACK");
		MAT->DEPTHSTENCIL(L"DEFDEPTH");
		MAT->BLEND(L"AlphaBlend");
	}
}

#pragma warning( pop )