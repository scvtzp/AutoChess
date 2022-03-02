#pragma once
#include <Windows.h>

class Game_Time
{
// A 컴퓨터와 B컴퓨터가 있다.
// A: 1초에 1번 게임의 데드타임 함수가 돌아간다.
// B: 1초에 2번 게임의 데드타임 함수가 돌아간다.

// 똑같이 캐릭터의 이동속력이 1이라면
// A는 초당 1을 이동하고
// B는 초당 2를 이동하게 된다
// => 불합리

// 둘다 1초 움직였을때.
// 동일하게 1을 이용하게 하려면

// A 의 이동량 = 1 * 1 = 1;
// B 의 이동량 = 1 * 0.5F + 1 * 0.5F = 1;

// 1이 델타타임
// 0.5F 가 델타타임

// 시간을 구해야 하는데.
// 공식부터 말을 한다면
// CPU에서는 기본적으로 타이머 장착되어있고
// 이건 심지어 프로그램 처리와는 무관하다.
// CPU의 기본기능이다.
// 지속적으로 시간을 체크하고 있다.
// 그걸 불러올수 있는데.
// 이때 2가지 시간을 얻어올수 있다.

// 1번 1초당 샐수 있는 시간
	// 이시간이 1초 단위로 샌다.
	// 어떤 컴퓨터가 1초에 100을 셀수 있다고 말한다면
	// 이건 픽스다.

// 2번 1초당 샐수 있는 시간으로 여태까지 샌시간을 알수 있다.
	// 1초당 100을 샐수 있다면
	// 1초동안 샜다면 100
	// 2초동안 숫자를 샜다면 200

	//////////////////////////////   
// 우리가 알아야할 함수는 2개이다.
	// 1초당 샐수있는 시간과
	// 현재까지 샌시간을 모두 계산해주는 함수를 알아야 한다.
// CountTime = 1초당 샐수 있는시간		100
// CurTime = 현재까지 샌시간            0
// PrevTime = 현재까지 샌시간			0
// ~~~~~~~~~~~ 1초가 지났다.(정확하게 1초가 지났다고 가정해봅시다.)
// CurTime = 현재까지 샌시간            100
// PrevTime = 
//            100        0       / 100
// 델타타임 = CurTime - PrevTime / CountTime;
// 끝나면 하는일 PrevTime = CurTime;


// ~~~~~~~~~~~ 0.5초가 지났다.(정확하게 1초가 지났다고 가정해봅시다.)
// CurTime = 현재까지 샌시간            150
// PrevTime =                           100
//            150        100       / 100
// 델타타임 = CurTime - PrevTime / CountTime;
// 0.5F
// 끝나면 하는일 PrevTime = CurTime;
// 델타타임 1.0F는 1초를 의미하게 된다.

// 즉 실제 시간을 실수로 변환한것이 델타타임이다. 

public:
	class Game_Timer 
	{
		friend Game_Time;
	private:
		//  1초당 샐수 있는시간
		LARGE_INTEGER m_CountTime; 
		LARGE_INTEGER m_CurTime;
		LARGE_INTEGER m_PrevTime;
		double dDeltaTime; // 8 바이트 실수로더큰 실수를 쓸수 있다. 연산이 그만큼 
		float fDeltaTime; // 4 바이트 실수

	public:
		void CountStart();
		void TimeCheck();

	public:
		Game_Timer();
		~Game_Timer();
	};

private:
	// static 함수와 맴버변수가 아니라면
	// 굉장히 귀찮은 사용방법을 가지므로
	// static을 사용한다.
	// HGAMETIME은 시계를 하나 두고 
	// 그걸 메인시간으로 삼아서 모든 게임에 관련된 객체나 코드들이 그것을 이용한다.
	static Game_Timer MainTimer;

public:
	static float DeltaTime(float _Speed = 1.0f) {
		return MainTimer.fDeltaTime * _Speed;
	}

public:
	static void TimeReset();
	static void Update();

public:
	Game_Time();
	~Game_Time();
};
