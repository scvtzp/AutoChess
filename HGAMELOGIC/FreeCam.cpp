#include "FreeCam.h"
#include <HGAMEACTOR.h>
#include <HCAM.h>
#include <HGAMEDEBUG.h> 
#include <HGAMEINPUT.h>
#include <Game_Trans.h>
#include <HGAMETIME.h>

FreeCam::FreeCam() 
{
	// -50                          50
	// 0    10 0    10 0     10
	// 50 40
	// % 10
	// float A = 10.0F;
	// A % 10.0f;
}

FreeCam::~FreeCam() 
{
}

void FreeCam::Init(float _Speed)
{
	m_Speed = _Speed;
	m_RotSpeed = 360.0f;

	if (nullptr == ACTOR()->FindComType<HCAM>())
	{
		AMSG(L"ķ�� �������� �ʴ� ���� ����ķ�� �־��ټ� �����ϴ�.");
	}

	m_Cam = ACTOR()->FindComType<HCAM>();

	if (false == HGAMEINPUT::IsKey(L"A"))
	{
		HGAMEINPUT::CreateKey(L"A", 'A');
		HGAMEINPUT::CreateKey(L"D", 'D');
		HGAMEINPUT::CreateKey(L"CAMUP", 'Q');
		HGAMEINPUT::CreateKey(L"CAMDOWN", 'E');
		HGAMEINPUT::CreateKey(L"W", 'W');
		HGAMEINPUT::CreateKey(L"S", 'S');

		HGAMEINPUT::CreateKey(L"Mouse_R", VK_RBUTTON);
	}
}

void FreeCam::Update() 
{
	// ���� �ܾƿ�(��ġ�� �ٲٸ� ���� �ȵȴ�. �����̳� ������� ����)
	// �ʱ�ȭ(���� ��ġ�� �ٽ� ������.)
	// �ν�Ʈ(�ӷ��� ���� �Ű�� ������)

	if (true == HGAMEINPUT::Press(L"A"))
	{
		TRANS()->LMOVE(TRANS()->WLEFT() * Game_Time::DeltaTime(m_Speed));
	}

	if(true == HGAMEINPUT::Press(L"D"))
	{
		TRANS()->LMOVE(TRANS()->WRIGHT() * Game_Time::DeltaTime(m_Speed));
	}

	if(true == HGAMEINPUT::Press(L"CAMUP"))
	{
		TRANS()->LMOVE(TRANS()->WUP() * Game_Time::DeltaTime(m_Speed));
	}

	if(true == HGAMEINPUT::Press(L"CAMDOWN"))
	{
		TRANS()->LMOVE(TRANS()->WDOWN() * Game_Time::DeltaTime(m_Speed));
	}

	if(true == HGAMEINPUT::Press(L"W"))
	{
		TRANS()->LMOVE(TRANS()->WFORWARD() * Game_Time::DeltaTime(m_Speed));
	}

	if(true == HGAMEINPUT::Press(L"S"))
	{
		TRANS()->LMOVE(TRANS()->WBACK() * Game_Time::DeltaTime(m_Speed));
	}

	if (true == HGAMEINPUT::Press(L"Mouse_R"))
	{
		TRANS()->WROTADDY(HGAMEINPUT::MouseDir3D().x * Game_Time::DeltaTime(m_RotSpeed));
		TRANS()->WROTADDX(-HGAMEINPUT::MouseDir3D().y * Game_Time::DeltaTime(m_RotSpeed));

		// TRANS()->LMOVE(TRANS()->WBACK() * Game_Time::DeltaTime(m_Speed));
	}
}