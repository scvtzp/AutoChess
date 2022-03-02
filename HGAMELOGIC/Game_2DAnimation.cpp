#pragma once
#include "Game_2DAnimation.h"
#include <HRENDERER.h>
#include <HGAMEACTOR.h>
#include <HGAMETIME.h>
#include <HGAMESPRITE.h>
#include <HRENDERER.h>
#pragma once

void Game_2DAnimation::Init(Game_Ptr<Game_Renderer> _Render)
{
	if (nullptr != _Render)
		m_Render123 = _Render;

	if (nullptr == _Render)
	{
		AMSG(L"������ �ִϸ��̼��� �������� �־�� ����� �ֽ��ϴ�.");
	}
}

void Game_2DAnimation::Update()
{
	// float Time = Game_Time::DeltaTime();
	if (nullptr == m_CurAni)
	{
		AMSG(L"�ִϸ��̼��� �������� �ʾҽ��ϴ�.");
	}


	m_CurAni->m_CurFrameTime -= Game_Time::DeltaTime();

	if (0 >= m_CurAni->m_CurFrameTime)
	{
		//m_CurAni->CheckEndFunc(m_CurAni->m_CurFrame);
		++m_CurAni->m_CurFrame;
		//m_CurAni->CheckStartFunc(m_CurAni->m_CurFrame);
		//// 10 ���������� ���ߴ�.
		//// 10 �������� ����
		//// START


		//// �䶧 ���ָ� �ȴ�.

		m_CurAni->m_CurFrameTime = m_CurAni->m_FrameTime;

		unsigned int Frame = m_CurAni->m_CurFrame;

		if (m_CurAni->m_End < m_CurAni->m_CurFrame)
		{
			if (true == m_CurAni->m_Loop)
			{
				m_CurAni->m_CurFrame = m_CurAni->m_Start;
			}
			else {
				--m_CurAni->m_CurFrame;
			}
		}
	}

	unsigned int Check = m_CurAni->m_CurFrame;

	//��������Ʈ ������ ����.
	//m_SpriteRender->SPRITE(m_CurAni->m_SpriteName, m_CurAni->m_CurFrame);
	Game_Vector aa = Game_Sprite::Find(m_CurAni->m_SpriteName)->SpriteData(m_CurAni->m_CurFrame);
	m_Render123->CBUFFER(L"CUTDATA", &aa, CBUFFERMODE::CB_NEW);
}

void Game_2DAnimation::CreateAni(
	const Game_String& _AniName,
	const Game_String& _SpriteName,
	unsigned int _Start,
	unsigned int _End,
	float _FrameTime,
	bool _Loop)
{
	if (m_Ani.end() != m_Ani.find(_AniName))
	{
		AMSG(_AniName + L"�̹� ������� �ִϸ��̼� �̸��Դϴ�");
	}

	if (nullptr == Game_Sprite::Find(_SpriteName))
	{
		AMSG(_AniName + L"��������Ʈ�� �������� �ʾ� �ִϸ��̼��� ����� �����ϴ�.");
	}

	// �ٷ� Ű�� �Բ� ���������.
	m_Ani[_AniName] = new Game_2DFrameAnimation();
	m_Ani[_AniName]->m_AniName = _AniName;
	m_Ani[_AniName]->m_SpriteName = _SpriteName;
	m_Ani[_AniName]->m_Start = _Start;
	m_Ani[_AniName]->m_End = _End;
	m_Ani[_AniName]->m_FrameTime = _FrameTime;
	m_Ani[_AniName]->m_Loop = _Loop;
	m_Ani[_AniName]->Actor = ACTOR();
}

void Game_2DAnimation::ChangeAni(const Game_String& _AniName)
{
	if (m_Ani.end() == m_Ani.find(_AniName))
	{
		AMSG(_AniName + L"�ִϸ��̼��� �������� �ʽ��ϴ�");
	}

	//�ٲܷ��� ���°� ������ �����ϸ� ���� X
	if (m_CurAni != NULL && m_CurAni->m_AniName == _AniName)
	{
		return;
	}

	m_CurAni = m_Ani[_AniName];
	m_CurAni->Reset();
	//m_CurAni->CheckStartFunc(m_CurAni->m_Start);
}


//Game_Ptr<Game_2DAnimation::Game_2DFrameAnimation> Game_2DAnimation::FindAni(const Game_String& _AniName)
//{
//	if (m_Ani.end() == m_Ani.find(_AniName))
//	{
//		return nullptr;
//	}
//
//	return m_Ani[_AniName];
//}