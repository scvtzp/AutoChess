#include "HGAMETIME.h"

Game_Time::Game_Timer Game_Time::MainTimer = Game_Time::Game_Timer();

Game_Time::Game_Timer::Game_Timer() 
{
	CountStart();
}

Game_Time::Game_Timer::~Game_Timer() 
{
}

// ����
// GameTimer NewTimer; // �� Ÿ�̸� ��ü�� ������ٸ�
// NewTimer.CountStart();
// ~~~~~~~~~~~~~~~ ������ �ð��� ��� �Լ��̴�.
// NewTimer.TimeCheck(); ->  ������ ���������� �Լ��� 
// ~~~~~~~~~~~~~~~ ������ �ð��� ��� �Լ��̴�.
// NewTimer.TimeCheck(); ->  ������ ���������� �Լ��� 

void Game_Time::Game_Timer::CountStart() 
{
	QueryPerformanceFrequency(&m_CountTime);
	// ������� �� �ð� ī���õ� �ð�
	QueryPerformanceCounter(&m_CurTime);
	// ����� �߿��� �����Ѵ�.
	QueryPerformanceCounter(&m_PrevTime);
}

void Game_Time::Game_Timer::TimeCheck() 
{
	QueryPerformanceCounter(&m_CurTime);
	// ��ŸŸ�� = CurTime - PrevTime / CountTime;
	// 

	// �ϴ� �ð��� �궧 DOUBLE�� ���� ����Ҷ��� �ִ���
	// �����ϰ� �ϱ� ���ؼ�
	// double�� ����Ѱ�.
	// double dDeltaTime; // 
	// float fDeltaTime; // ���߿� ����Ҷ� float���� ����ϴ� ��쵵 ���Ƽ�
	// double�� ���̴�.

	// ��Ģ���� ������ ���� 
	// ���� ������ ���� ���� �����µ�.
	// 1000�� ����ƴٰ� �ϸ�
	// 1�ʾȿ� 1000�� ����Ǹ�
	// 0.001������ �ȴ�.
	// �� ��ǻ�Ͱ� ������ ��������
	// �ð��� �궧���� �� �������� �� �������ۿ� ����.
	
	dDeltaTime = ((double)m_CurTime.QuadPart - (double)m_PrevTime.QuadPart) / (double)m_CountTime.QuadPart;
	// ����ð� Prev �� �Էµȴ�.
	m_PrevTime.QuadPart = m_CurTime.QuadPart;
	fDeltaTime = (float)dDeltaTime;
}

Game_Time::Game_Time()
{
}

Game_Time::~Game_Time()
{
}

void Game_Time::Update() 
{
	// ��� �������ش�.
	MainTimer.TimeCheck();
}

// �̳༮�� ���� �ѹ���
// Ȥ�� ����ο� ����� ���鶧
// 1���� �����ϸ� �Ǵµ�.
void Game_Time::TimeReset() 
{
	MainTimer.CountStart();
}