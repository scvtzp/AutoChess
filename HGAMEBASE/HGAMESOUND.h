#pragma once
#include <fmod.hpp> // C�� �ؼ�
#include "HGAMEIO.h"
#include <map>
#include "HMGR.h"
// : public HFILEMGR<HTEXTURE>
// ��� 
// fmod_vc.lib ������ ��嶧 ���
// fmodL_vc.lib ����� ����϶� ���
 #define TEST

#ifdef TEST 
#else
#endif

// ���� ����?
// �����Ҷ� ����Ǳ� �����̴�.
// �̰� ����Ǵ� �༮�� �ƴϱ� ������ ������ ���ٰ� �ؼ� ������ ���� �ʴ´�.
#ifdef _DEBUG // ����� �����
#pragma comment(lib, "fmodL_vc.lib")
#else // ������ �����
#pragma comment(lib, "fmod_vc.lib")
#endif

class Game_String;
class HGAMEFILE;
class HGAMESOUND : public HFILEMGR<HGAMESOUND>
{
private:
	static FMOD::System* m_pSoundSys;

	// ���������� FMOD�� ��������
	// FMOD::Channel* ��������̴�.
	// ��������� �ʿ��ϴ�.
	// ���� ����� ���ؼ� �̰����� �����ϰ� ���� �༮��
	// ä���� �޾Ƽ� �� ä���� ���ؼ� �����ؾ� �Ѵ�.

	static std::map<Game_String, FMOD::Channel*> m_pAllPlaySound;

	class HGAMESTART
	{
	public:
		HGAMESTART();
		~HGAMESTART();
	};
	// ����� ������ �����̴�.
	static HGAMESTART Starter;

public:
	static void Init();
	static void Update();

	static void Load(const HGAMEFILE& _File);



	// �ѹ� ����ϰ� 


	// "BulletSound.mp3"
	static void Play(const Game_String& _FileName);
	// "BGM"�̶�� �̸����� "BackgroundSound.mp3"
	static void Play(const Game_String& _PlayName, const Game_String& _FileName);
	// ����ǰ� �� ����
	// "BGM" ����
	static void Stop(const Game_String& _Name);
	// "BGM" ������̾�?
	static bool IsPlay(const Game_String& _Name);

	// �ɹ�����
private:
	FMOD::Sound* m_pSound;

private:
	void Load();

public:
	HGAMESOUND() : m_pSound(nullptr) 
	{

	}

};

