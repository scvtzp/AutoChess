#include "HGAMESOUND.h"
#include <assert.h>
#include "HGAMESTRING.h"


HGAMESOUND::HGAMESTART::HGAMESTART() 
{
	Init();
}
HGAMESOUND::HGAMESTART::~HGAMESTART() 
{

}


std::map<Game_String, FMOD::Channel*> HGAMESOUND::m_pAllPlaySound;
FMOD::System* HGAMESOUND::m_pSoundSys = nullptr;
HGAMESOUND::HGAMESTART HGAMESOUND::Starter;

void HGAMESOUND::Init() 
{
	// m_pSoundSys를 넣어주면 값을 채워서 리턴해주겠다는 소리이다.
	FMOD::System_Create(&m_pSoundSys);

	// 사운드카드 없으면 nullptr이 나올수 있다.
	if (nullptr == m_pSoundSys)
	{
		assert(true);
	}

	// 32 채널개수 크게 신경쓸필요 없는게
	// 32 채널이면 8126개 동시재생
	if (FMOD_OK != m_pSoundSys->init(32, FMOD_DEFAULT, nullptr))
	{
		assert(true);
	}
}

void HGAMESOUND::Update() 
{
	m_pSoundSys->update();
}

void HGAMESOUND::Play(const Game_String& _Name) 
{
	Game_Ptr<HGAMESOUND> Sound = Find(_Name);

	if (nullptr == Sound)
	{
		assert(false);
	}

	m_pSoundSys->playSound(Sound->m_pSound, nullptr, false, nullptr);
}

bool HGAMESOUND::IsPlay(const Game_String& _Name) 
{
	std::map<Game_String, FMOD::Channel*>::iterator Find = m_pAllPlaySound.find(_Name);

	if (Find == m_pAllPlaySound.end())
	{
		return false;
	}

	bool Playing = false;

	Find->second->isPlaying(&Playing);

	return Playing;
}

void HGAMESOUND::Stop(const Game_String& _Name) {
	std::map<Game_String, FMOD::Channel*>::iterator Find = m_pAllPlaySound.find(_Name);

	if (Find == m_pAllPlaySound.end())
	{
		return;
	}

	Find->second->stop();
}

void HGAMESOUND::Play(const Game_String& _PlayName, const Game_String& _FileName) 
{
	Game_Ptr<HGAMESOUND> Sound = Find(_FileName);

	if (nullptr == Sound)
	{
		assert(false);
	}

	std::map<Game_String, FMOD::Channel*>::iterator Find = m_pAllPlaySound.find(_PlayName);

	if (Find == m_pAllPlaySound.end())
	{
		FMOD::Channel* Ptr = nullptr;
		m_pSoundSys->playSound(Sound->m_pSound, nullptr, false, &Ptr);

		if (nullptr == Ptr)
		{
			assert(false);
		}

		m_pAllPlaySound.insert(std::map<Game_String, FMOD::Channel*>::value_type(_PlayName, Ptr));

		return;
	}

	// 일시 정지 기능.
	// Find->second->setPaused(true)

	Find->second->stop();

	FMOD::Channel* Ptr = nullptr;
	m_pSoundSys->playSound(Sound->m_pSound, nullptr, false, &Ptr);

	if (nullptr == Ptr)
	{
		assert(false);
	}

	Find->second = Ptr;

	// Ptr->setVolume()
	// Ptr->stop()

	// Ptr 재생에 대한 권한.

}

// 이녀석은 전역입니다.
void HGAMESOUND::Load(const HGAMEFILE& _File)
{
	if (nullptr != Find(_File.FileName()))
	{
		assert(false);
	}
	
	HGAMESOUND* NewSound = CreateToInsertToFileObj(_File);
	NewSound->Load();
}
// 이녀석은 맴버변수고
void HGAMESOUND::Load() 
{
	std::string Path = GameFile.FullPath();

	if (FMOD_OK != m_pSoundSys->createSound(Path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(true);
	}

}