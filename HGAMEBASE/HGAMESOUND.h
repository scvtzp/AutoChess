#pragma once
#include <fmod.hpp> // C용 해서
#include "HGAMEIO.h"
#include <map>
#include "HMGR.h"
// : public HFILEMGR<HTEXTURE>
// 사실 
// fmod_vc.lib 릴리즈 모드때 사용
// fmodL_vc.lib 디버그 모드일때 사용
 #define TEST

#ifdef TEST 
#else
#endif

// 문제 없죠?
// 실행할때 적용되기 때문이다.
// 이건 실행되는 녀석이 아니기 때문에 지금은 없다고 해서 문제가 되지 않는다.
#ifdef _DEBUG // 디버그 모드라면
#pragma comment(lib, "fmodL_vc.lib")
#else // 릴리즈 모드라면
#pragma comment(lib, "fmod_vc.lib")
#endif

class Game_String;
class HGAMEFILE;
class HGAMESOUND : public HFILEMGR<HGAMESOUND>
{
private:
	static FMOD::System* m_pSoundSys;

	// 실제적으로 FMOD의 사운재생은
	// FMOD::Channel* 재생정보이다.
	// 재생정보가 필요하다.
	// 사운드 재생에 대해서 이것저것 참견하고 싶은 녀석은
	// 채널을 받아서 그 채널을 통해서 참견해야 한다.

	static std::map<Game_String, FMOD::Channel*> m_pAllPlaySound;

	class HGAMESTART
	{
	public:
		HGAMESTART();
		~HGAMESTART();
	};
	// 배웠던 문법의 응용이다.
	static HGAMESTART Starter;

public:
	static void Init();
	static void Update();

	static void Load(const HGAMEFILE& _File);



	// 한번 재생하고 


	// "BulletSound.mp3"
	static void Play(const Game_String& _FileName);
	// "BGM"이라는 이름으로 "BackgroundSound.mp3"
	static void Play(const Game_String& _PlayName, const Game_String& _FileName);
	// 재생되고 난 다음
	// "BGM" 멈춰
	static void Stop(const Game_String& _Name);
	// "BGM" 재생중이야?
	static bool IsPlay(const Game_String& _Name);

	// 맴버변수
private:
	FMOD::Sound* m_pSound;

private:
	void Load();

public:
	HGAMESOUND() : m_pSound(nullptr) 
	{

	}

};

