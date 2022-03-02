#include "HGAMETIME.h"

Game_Time::Game_Timer Game_Time::MainTimer = Game_Time::Game_Timer();

Game_Time::Game_Timer::Game_Timer() 
{
	CountStart();
}

Game_Time::Game_Timer::~Game_Timer() 
{
}

// 사용법
// GameTimer NewTimer; // 이 타이머 객체를 만들었다면
// NewTimer.CountStart();
// ~~~~~~~~~~~~~~~ 사이의 시간을 재는 함수이다.
// NewTimer.TimeCheck(); ->  실행한 순간까지의 함수를 
// ~~~~~~~~~~~~~~~ 사이의 시간을 재는 함수이다.
// NewTimer.TimeCheck(); ->  실행한 순간까지의 함수를 

void Game_Time::Game_Timer::CountStart() 
{
	QueryPerformanceFrequency(&m_CountTime);
	// 현재까지 센 시간 카운팅된 시간
	QueryPerformanceCounter(&m_CurTime);
	// 디버깅 중에도 동작한다.
	QueryPerformanceCounter(&m_PrevTime);
}

void Game_Time::Game_Timer::TimeCheck() 
{
	QueryPerformanceCounter(&m_CurTime);
	// 델타타임 = CurTime - PrevTime / CountTime;
	// 

	// 일단 시간을 잴때 DOUBLE을 쓴건 계산할때는 최대한
	// 정밀하게 하기 위해서
	// double을 사용한것.
	// double dDeltaTime; // 
	// float fDeltaTime; // 나중에 사용할때 float으로 사용하는 경우도 많아서
	// double은 무겁다.

	// 사칙연산 순서에 따라서 
	// 보통 굉장히 작은 수가 나오는데.
	// 1000번 실행됐다고 하면
	// 1초안에 1000번 실행되면
	// 0.001나오게 된다.
	// 즉 컴퓨터가 빠르면 빠를수록
	// 시간을 잴때마다 그 간격으로 더 작을수밖에 없다.
	
	dDeltaTime = ((double)m_CurTime.QuadPart - (double)m_PrevTime.QuadPart) / (double)m_CountTime.QuadPart;
	// 현재시간 Prev 에 입력된다.
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
	// 계속 실행해준다.
	MainTimer.TimeCheck();
}

// 이녀석은 최초 한번만
// 혹은 어떤새로운 장면을 만들때
// 1번만 실행하면 되는데.
void Game_Time::TimeReset() 
{
	MainTimer.CountStart();
}