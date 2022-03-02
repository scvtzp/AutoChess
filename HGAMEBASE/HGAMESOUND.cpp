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
	// m_pSoundSys�� �־��ָ� ���� ä���� �������ְڴٴ� �Ҹ��̴�.
	FMOD::System_Create(&m_pSoundSys);

	// ����ī�� ������ nullptr�� ���ü� �ִ�.
	if (nullptr == m_pSoundSys)
	{
		assert(true);
	}

	// 32 ä�ΰ��� ũ�� �Ű澵�ʿ� ���°�
	// 32 ä���̸� 8126�� �������
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

	// �Ͻ� ���� ���.
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

	// Ptr ����� ���� ����.

}

// �̳༮�� �����Դϴ�.
void HGAMESOUND::Load(const HGAMEFILE& _File)
{
	if (nullptr != Find(_File.FileName()))
	{
		assert(false);
	}
	
	HGAMESOUND* NewSound = CreateToInsertToFileObj(_File);
	NewSound->Load();
}
// �̳༮�� �ɹ�������
void HGAMESOUND::Load() 
{
	std::string Path = GameFile.FullPath();

	if (FMOD_OK != m_pSoundSys->createSound(Path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(true);
	}

}