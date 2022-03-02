#pragma once
#include "HRENDERER.h"
#include "HGAMEACTOR.h"
#include "HGAMESCENE.h"
#include "Game_Device.h"
#include "HVBBUFFER.h"
#include "HIBBUFFER.h"
#include "HCBUFFER.h"
#include "HDEPTHSTENCIL.h"
#include "HBLEND.h"
#include "HRASTERIZER.h"
#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "HSAMPLER.h"
#include "HGAMESPRITE.h"
#include "HCAM.h"
#include "HFBX.h"
#include "HFbxEx.h"
#include "HLight.h"

///////////////////////////////// ���� �÷��̾�

bool HRENDERPLAYER::IsTEXTURE(const Game_String& _Name)
{
	if (m_TEXTURE.end() == m_TEXTURE.find(_Name))
	{
		return false;
	}

	return true;
}
bool HRENDERPLAYER::IsSAMPLER(const Game_String& _Name)
{
	if (m_SAMPLER.end() == m_SAMPLER.find(_Name))
	{
		return false;
	}

	return true;
}
bool HRENDERPLAYER::IsCBUFFER(const Game_String& _Name)
{
	if (m_CBUFFER.end() == m_CBUFFER.find(_Name))
	{
		return false;
	}

	return true;
}

std::list<TEXTURESETTER*> HRENDERPLAYER::AllTextureSetter(const Game_String& _SetterName)
{
	std::list<TEXTURESETTER*> List;

	if (false == IsTEXTURE(_SetterName))
	{
		return List;
	}

	for (auto& _Setter : m_TEXTURE[_SetterName])
	{
		List.push_back(&_Setter);
	}

	return List;
}

void HRENDERPLAYER::Render()
{
	for (auto& _ResGroup : m_CBUFFER)
	{
		for (auto& _Res : _ResGroup.second)
		{
			if (_Res.m_Mode == CBUFFERMODE::CB_NONE)
			{
				AMSG(L"��������� ���� ������ ���� �ʾҽ��ϴ�. -> " + _Res.m_Data->Name);
				continue;
			}

			_Res.m_RES->DataChange(_Res.m_Value, (unsigned int)_Res.m_Data->m_Size);
			_Res.m_RES->Setting(_Res.m_Data->m_ShaderType, _Res.m_Data->m_Index);
		}
	}

	for (auto& _ResGroup : m_TEXTURE)
	{
		for (auto& _Res : _ResGroup.second)
		{
			if (nullptr == _Res.m_RES)
			{
				AMSG(L"�ؽ�ó�� ���������� �ʾҽ��ϴ�. -> " + _Res.m_Data->Name);
			}

			_Res.m_RES->Setting(_Res.m_Data->m_ShaderType, _Res.m_Data->m_Index);
		}
	}

	for (auto& _ResGroup : m_SAMPLER)
	{
		for (auto& _Res : _ResGroup.second)
		{
			if (nullptr == _Res.m_RES)
			{
				AMSG(L"���÷��� ���������� �ʾҽ��ϴ�. -> " + _Res.m_Data->Name);
			}

			_Res.m_RES->Setting(_Res.m_Data->m_ShaderType, _Res.m_Data->m_Index);
		}
	}

	m_Mat->Setting();
	m_Mesh->Render();
}

void HRENDERPLAYER::Reset()
{
	// ���� Ÿ���� ���ɼ��� ����?
	for (auto& _ResGroup : m_TEXTURE)
	{
		for (auto& _Res : _ResGroup.second)
		{
			if (nullptr == _Res.m_RES)
			{
				AMSG(L"�ؽ�ó�� ���������� �ʾҽ��ϴ�. -> " + _Res.m_Data->Name);
			}

			_Res.m_RES->Reset(_Res.m_Data->m_ShaderType, _Res.m_Data->m_Index);
		}
	}
}

Game_Ptr<HMESH> HRENDERPLAYER::MESH() {
	return m_Mesh;
}
Game_Ptr<Game_Material> HRENDERPLAYER::MATERIAL() {
	return m_Mat;
}

void HRENDERPLAYER::MESH(const Game_Ptr<HMESH>& _Mesh)
{
	if (nullptr == _Mesh)
	{
		AMSG(L"nullptr�� �Ž��� �������ټ��� �����ϴ�.");
	}

	m_Mesh = _Mesh;
}


void HRENDERPLAYER::MESH(const Game_String& _Name)
{
	m_Mesh = HMESH::Find(_Name);

	if (nullptr == m_Mesh)
	{
		AMSG(L"�������� �ʴ� �Ž��� �����Ϸ��� �߽��ϴ�.");
	}
}

void HRENDERPLAYER::MATERIAL(const Game_String& _Name, bool _Push/* = true*/)
{
	m_Mat = Game_Material::Find(_Name);
	if (nullptr == m_Mat)
	{
		AMSG(L"�������� �ʴ� ���׸����� �����Ϸ��� �߽��ϴ�.");
	}

	for (auto _Shader : m_Mat->m_ALLSHADER)
	{
		if (nullptr == _Shader)
		{
			continue;
		}

		for (auto& _Res : _Shader->m_ResData)
		{
			switch (_Res.second.m_DataType)
			{
			case D3D_SIT_CBUFFER:
			{
				m_CBUFFER[_Res.second.Name].resize(m_CBUFFER[_Res.second.Name].size() + 1);
				CBUFFERSETTER& Setter = m_CBUFFER[_Res.second.Name][m_CBUFFER[_Res.second.Name].size() - 1];
				Setter.m_Data = &(_Res.second);
				Setter.m_RES = new HCBUFFER();
				Setter.m_RES->Create((int)Setter.m_Data->m_Size);
				Setter.m_Mode = CBUFFERMODE::CB_NONE;

				// �̳༮ ��ŭ�� �츮 �����ӿ�ũ���� ����� ����.
				// HTRANSDATA��� �̸��̸� �̳༮��
				// Ʈ�������� �ִ� TRANSDATA�����͸� ���̴��� �־��ֱ�� ����Ѵ�.
				if (L"HTRANSDATA" == _Res.second.Name)
				{
					CBUFFER(L"HTRANSDATA", m_Parent->PTRANSDATA(), CBUFFERMODE::CB_LINK);
				}
				if (L"TRANSDATA" == _Res.second.Name)
				{
					CBUFFER(L"TRANSDATA", m_Parent->PTRANSDATA(), CBUFFERMODE::CB_LINK);
				}
				if (L"RenderOption" == _Res.second.Name)
				{
					CBUFFER(L"RenderOption", &RenderOption, CBUFFERMODE::CB_LINK);
				}

				break;
			}
			case D3D_SIT_TEXTURE:
			{
				m_TEXTURE[_Res.second.Name].resize(m_TEXTURE[_Res.second.Name].size() + 1);
				TEXTURESETTER& Setter = m_TEXTURE[_Res.second.Name][m_TEXTURE[_Res.second.Name].size() - 1];
				Setter.m_Data = &(_Res.second);
				Setter.m_RES = HTEXTURE::Find(L"ERROR.png");
				Setter.IsOption = false;
				break;
			}
			case D3D_SIT_SAMPLER:
			{
				m_SAMPLER[_Res.second.Name].resize(m_SAMPLER[_Res.second.Name].size() + 1);
				SAMPLERSETTER& Setter = m_SAMPLER[_Res.second.Name][m_SAMPLER[_Res.second.Name].size() - 1];
				Setter.m_Data = &(_Res.second);
				Setter.m_RES = HSAMPLER::Find(L"LWSMP");
				break;
			}
			default:
				AMSG(L"���� ó���ϴ� ����� ������ ���� ���ҽ� �Դϴ�.");
				break;
			}
		}
	}

	if (nullptr == m_Parent)
	{
		return;
	}

	if (true == _Push)
	{
		switch (m_Mat->PathType())
		{
		case RENDERPATHTYPE::RP_DEFFERD:
			m_Parent->m_DefferdList.push_back(this);
			break;
		case RENDERPATHTYPE::RP_FORWARD:
			m_Parent->m_ForwardList.push_back(this);
			break;
		default:
			break;
		}
	}
}

Game_Ptr<HTEXTURE> HRENDERPLAYER::GetTEXTURE(const Game_String& _Name)
{
	for (auto& _Res : m_TEXTURE[_Name])
	{
		if (nullptr != _Res.m_RES)
		{
			return _Res.m_RES;
		}
	}

	return nullptr;
}

void HRENDERPLAYER::TEXTURE(const Game_String& _Name, const Game_Ptr<HTEXTURE>& _Tex)
{
	if (false == IsTEXTURE(_Name))
	{
		AMSG(L"�������� �ʴ� �ؽ�ó �����Դϴ�." + _Name);
	}

	if (nullptr == _Tex)
	{
		AMSG(L"�������� �ʴ� �ؽ�ó �� �����Ϸ��� �߽��ϴ�." + _Name);
		return;
	}

	for (auto& _Res : m_TEXTURE[_Name])
	{
		_Res.m_RES = _Tex;
	}
}

void HRENDERPLAYER::TEXTURE(const Game_String& _Name, const Game_String& _TexName)
{
	if (false == IsTEXTURE(_Name))
	{
		AMSG(L"�������� �ʴ� �ؽ�ó �����Դϴ�." + _Name);
	}

	Game_Ptr<HTEXTURE> Tex = HTEXTURE::Find(_TexName);

	if (nullptr == Tex)
	{
		AMSG(L"�������� �ʴ� �ؽ�ó �� �����Ϸ��� �߽��ϴ�." + _Name);
		return;
	}

	for (auto& _Res : m_TEXTURE[_Name])
	{
		_Res.m_RES = Tex;
	}
}
void HRENDERPLAYER::SAMPLER(const Game_String& _Name, const Game_String& _SmpName)
{
	if (false == IsSAMPLER(_Name))
	{
		AMSG(L"�������� �ʴ� ���÷� �����Դϴ�." + _Name);
	}

	Game_Ptr<HSAMPLER> Smp = HSAMPLER::Find(_SmpName);

	if (nullptr == Smp)
	{
		AMSG(L"�������� �ʴ� �ؽ�ó �� �����Ϸ��� �߽��ϴ�." + _Name);
		return;
	}


	for (auto& _Res : m_SAMPLER[_Name])
	{
		_Res.m_RES = Smp;
	}
}
void HRENDERPLAYER::CBUFFER(const Game_String& _Name, void* _Data, CBUFFERMODE _Mode)
{
	if (false == IsCBUFFER(_Name))
	{
		AMSG(L"�������� �ʴ� ������� �����Դϴ�." + _Name);
	}

	for (auto& _Res : m_CBUFFER[_Name])
	{
		_Res.m_Mode = _Mode;

		switch (_Mode)
		{
		case CBUFFERMODE::CB_LINK:
			_Res.m_Value = _Data;
			break;
		case CBUFFERMODE::CB_NEW:
			_Res.m_Value = new char[_Res.m_Data->m_Size];
			memcpy_s(_Res.m_Value, _Res.m_Data->m_Size, _Data, _Res.m_Data->m_Size);
			break;
		default:
			break;
		}
	}
}



///////////////////////////////// ������.

Game_Renderer::Game_Renderer()
{
}

Game_Renderer::~Game_Renderer()
{
}

void Game_Renderer::Init(int _Order /*= 0*/)
{
	PARENT(ACTOR()->TRANS());
	Order(_Order);
	ACTOR()->SCENE()->PushRender(this);
}

void Game_Renderer::Init(const Game_String& _Mesh, const Game_String& _Mat, int _Order/*= 0*/)
{
	Init(_Order);
	CreateRenderPlayer(_Mesh, _Mat);
}

void Game_Renderer::Init(const Game_Ptr<HMESH>& _Mesh, const Game_String& _Mat, int _Order/*= 0*/)
{
	Init(_Order);
	CreateRenderPlayer(_Mesh, _Mat);
}

Game_Ptr<HRENDERPLAYER> Game_Renderer::CreateRenderPlayer(const Game_String& _Mesh, const Game_String& _Mat)
{
	HRENDERPLAYER* NewPlayer = new HRENDERPLAYER();

	NewPlayer->m_Parent = this;
	NewPlayer->MESH(_Mesh);
	NewPlayer->MATERIAL(_Mat);
	m_PlayerList.push_back(NewPlayer);

	return NewPlayer;
}

Game_Ptr<HRENDERPLAYER> Game_Renderer::CreateRenderPlayer(const Game_Ptr<HMESH>& _Mesh, const Game_String& _Mat)
{
	HRENDERPLAYER* NewPlayer = new HRENDERPLAYER();
	NewPlayer->m_Parent = this;
	NewPlayer->MESH(_Mesh);
	NewPlayer->MATERIAL(_Mat);
	m_PlayerList.push_back(NewPlayer);


	return NewPlayer;
}

Game_Ptr<HRENDERPLAYER> Game_Renderer::CreateRenderPlayer(const Game_Ptr<HMESH>& _Mesh)
{
	HRENDERPLAYER* NewPlayer = new HRENDERPLAYER();
	NewPlayer->m_Parent = this;
	NewPlayer->MESH(_Mesh);
	m_PlayerList.push_back(NewPlayer);
	return NewPlayer;
}

Game_Ptr<HRENDERPLAYER> Game_Renderer::GetRenderPlayer(int _Index) 
{
	return m_PlayerList[_Index];
}


void Game_Renderer::Render(Game_Ptr<HCAM> _Cam)
{
	CamUpdate(_Cam);

	for (auto& RenderPlayer : m_PlayerList)
	{
		if (false == RenderPlayer->IsUpdate())
		{
			continue;
		}

		if (true == RenderPlayer->IsCBUFFER(L"LIGHTDATA"))
		{
			ObjectLightData* Data = SCENE()->GetLightData(Order());
			RenderPlayer->CBUFFER(L"LIGHTDATA", Data, CBUFFERMODE::CB_LINK);
			// int a = 0;
		};

		RenderPlayer->Render();
	}
}

void Game_Renderer::DefferdRender(Game_Ptr<HCAM> _Cam) 
{
	if (0 == m_DefferdList.size())
	{
		return;
	}

	CamUpdate(_Cam);

	for (auto& RenderPlayer : m_DefferdList)
	{
		if (false == RenderPlayer->IsUpdate())
		{
			continue;
		}

		if (true == RenderPlayer->IsCBUFFER(L"LIGHTDATA"))
		{
			ObjectLightData* Data = SCENE()->GetLightData(Order());
			RenderPlayer->CBUFFER(L"LIGHTDATA", Data, CBUFFERMODE::CB_LINK);
		};

		RenderPlayer->Render();
	}

}
void Game_Renderer::ForwardRender(Game_Ptr<HCAM> _Cam) 
{
	if (0 == m_ForwardList.size())
	{
		return;
	}

	CamUpdate(_Cam);

	for (auto& RenderPlayer : m_ForwardList)
	{
		if (false == RenderPlayer->IsUpdate())
		{
			continue;
		}

		if (true == RenderPlayer->IsCBUFFER(L"LIGHTDATA"))
		{
			ObjectLightData* Data = SCENE()->GetLightData(Order());
			RenderPlayer->CBUFFER(L"LIGHTDATA", Data, CBUFFERMODE::CB_LINK);
			// int a = 0;
		};

		RenderPlayer->Render();
	}
}

// �����忡 �ִ� �༮�� �׸��� �ְڳ׿�.

void Game_Renderer::ShadowRender(const LightData& _Light) 
{
	LightUpdate(_Light);

	for (auto& RenderPlayer : m_ShadowList)
	{
		if (false == RenderPlayer->IsUpdate())
		{
			continue;
		}

		RenderPlayer->Render();
	}


	// ������ ������ 
}

void Game_Renderer::TEXTURE(const Game_String& _Name, const Game_Ptr<HTEXTURE>& _Res)
{
	for (auto& RenderPlayer : m_PlayerList)
	{
		RenderPlayer->TEXTURE(_Name, _Res);
	}
}

void Game_Renderer::TEXTURE(const Game_String& _Name, const Game_String& _ResName)
{
	for (auto& RenderPlayer : m_PlayerList)
	{
		RenderPlayer->TEXTURE(_Name, _ResName);
	}
}
void Game_Renderer::CBUFFER(const Game_String& _Name, void* _Data, CBUFFERMODE _Mode)
{
	for (auto& RenderPlayer : m_PlayerList)
	{
		RenderPlayer->CBUFFER(_Name, _Data, _Mode);
	}
}
void Game_Renderer::SAMPLER(const Game_String& _Name, const Game_String& _ResName)
{
	for (auto& RenderPlayer : m_PlayerList)
	{
		RenderPlayer->SAMPLER(_Name, _ResName);
	}
}

std::vector<Game_Ptr<HRENDERPLAYER>> Game_Renderer::CreateRenderPlayerToFbxEx(const Game_String& _FbxExName, const Game_String& _MatName, int _DrawSet)
{
	std::vector<Game_Ptr<HRENDERPLAYER>> NewList;
	Game_Ptr<Game_Fbx_Ex> Fbx = Game_Fbx_Ex::Find(_FbxExName);

	DRAWSET* Ptr = Fbx->GetDrawSet(_DrawSet);

	for (size_t i = 0; i < Ptr->m_Mesh.size(); i++)
	{
		for (size_t j = 0; j < Ptr->m_Mesh[i].size(); j++)
		{
			// �����÷��̾� �����
			Game_Ptr<HRENDERPLAYER> RP = CreateRenderPlayer(Ptr->m_Mesh[i][j], _MatName);
			NewList.push_back(RP);
		}
	}
	return NewList;
}


Game_Ptr<HRENDERPLAYER> Game_Renderer::CreateRenderPlayerToFbxPartEx(const Game_String& _FbxExName, const Game_String& _MatName, int _DrawSet, int _SubIndex, int _MainIndex)
{
	Game_Ptr<Game_Fbx_Ex> Fbx = Game_Fbx_Ex::Find(_FbxExName);

	DRAWSET* Ptr = Fbx->GetDrawSet(_DrawSet);

	Game_Ptr<HRENDERPLAYER> RP = CreateRenderPlayer(Ptr->m_Mesh[_SubIndex][_MainIndex], _MatName);
	return RP;
}

std::vector<Game_Ptr<HRENDERPLAYER>> Game_Renderer::CreateRenderPlayerToFbx(const Game_String& _FbxName, const Game_String& _MatName, const Game_String& _SmpName)
{
	std::vector<Game_Ptr<HRENDERPLAYER>> NewList;

	Game_Ptr<HFBX> Fbx = HFBX::Find(_FbxName);

	// �Ž��� 4���� ���� ���������� 4���̴�.
	// Fbx->m_UserMatData size == 4
	// Fbx->m_Mesh size == 4
	// RenderPlayer.resize(Fbx->m_Mesh.size());
	for (size_t i = 0; i < Fbx->m_Mesh.size(); i++)
	{
		// �����÷��̾� �����
		Game_Ptr<HRENDERPLAYER> RP = CreateRenderPlayer(Fbx->m_Mesh[i], _MatName);

		// DifTexture zofl
		HGAMEFILE NewFile = (Fbx->GameFile.DirPath() + Fbx->m_UserMatData[i].DifTexture);
		Game_String DifTexName = NewFile.FileName();
		Game_Ptr<HTEXTURE> DifTex = HTEXTURE::Find(DifTexName);
		if (nullptr == DifTex)
		{
			HTEXTURE::Load(Fbx->GameFile.DirPath() + Fbx->m_UserMatData[i].DifTexture);
			DifTex = HTEXTURE::Find(DifTexName);
		}
		RP->TEXTURE(L"DifTex", DifTexName);
		RP->SAMPLER(L"Smp", _SmpName);
		NewList.push_back(RP);

		if (0 >= Fbx->m_UserAniData.size())
		{
			// �� ������ ó���ϸ�
			// �ִϸ��̼��� ��Ű�µ� �ʿ��ϴ�.
			// �������� ���� �ؽ�ó
			std::list<TEXTURESETTER*> List = RP->AllTextureSetter(L"FrameAniTex");
			for (auto Setter : List)
			{
				// �̰� �װ� ������ִ� ��.
				// �ȼ����ص� ��Ʈ������.
				// �� �̷��ָ� ���������?
				// FBX�� �ε��Ҷ� �� �ִϸ��̼��� �ִ� �༮�� �ε��ϴ°��� �ƴϴ�.
				Setter->IsOption = true;
			}
		}
	}
	return NewList;
}


std::vector<Game_Ptr<HRENDERPLAYER>> Game_Renderer::CreateRenderPlayerToFbxNoneMat(const Game_String& _FbxName) {
	std::vector<Game_Ptr<HRENDERPLAYER>> NewList;

	Game_Ptr<HFBX> Fbx = HFBX::Find(_FbxName);

	for (size_t i = 0; i < Fbx->m_Mesh.size(); i++)
	{
		// �����÷��̾� �����
		Game_Ptr<HRENDERPLAYER> RP = CreateRenderPlayer(Fbx->m_Mesh[i]);
		NewList.push_back(RP);
	}
	return NewList;
}

void Game_Renderer::ShadowOn() 
{
	// �ٵ��°� �ƴϰ� ������ ���ϱ�?
	for (auto& RenderPlayer : m_DefferdList)
	{
		HRENDERPLAYER* NewPlayer = new HRENDERPLAYER();
		NewPlayer->m_Parent = this;
		NewPlayer->MESH(RenderPlayer->MESH());
		NewPlayer->MATERIAL(L"Shadow", false);

		if (true == RenderPlayer->IsTEXTURE(L"FrameAniTex"))
		{
			Game_Ptr<HTEXTURE> Tex = RenderPlayer->GetTEXTURE(L"FrameAniTex");
			Game_String TexName = Tex->NAME();
			if (nullptr != Tex && L"ERROR.png" != TexName)
			{
				NewPlayer->RenderOption.IsAni = true;
				NewPlayer->TEXTURE(L"FrameAniTex", Tex);
			}
			// �ִϸ��̼ǿ� ��Ű�� �ؽ�ó�� �ִٴ� �̾߱��̴�.
		}

		m_ShadowList.push_back(NewPlayer);


		//m_ShadowList.push_back();

		// ���� ���׸����� �����ϴ� �༮�̴�.
		// RenderPlayer->RenderOption.IsShadow = true;
	}
}