#pragma once
#include "GAMELOGIC.h"
#include <set>
#include <HGAMESOUND.h>

#pragma comment(lib, "HGAMEBASE.lib")
#pragma comment(lib, "HGAMEENGINE.lib")

enum class Chess_Race
{
	Egersis,
	Dragon,
	Undead,
	Demon,
	God,
	Glacier,
	Elf,
	Goblin,
	Spirit,
	Marine,
	Dwarf,
	Beast,
	Human,
	Orc,
	Kira,
	Insectoid,
	
	End
};

enum class Chess_Class
{
	Mage,


	End
};

enum class Chess_Name
{
	//1성
	Unicorn, //유니콘
	Tusk_Champion, //바코펀
	The_Source, //더 소스
	God_of_War,
	Egersis_Ranger,

	//2성
	SwordMaster,
	Shining_Dragon, //빛나는 용

	//3성
	WereWolf,
	Poisonous_Worm, //독뱀
	Thunder_Spirit, //레이저
	WindRanger,
	Shadow_Davil, //그림자 악마
	Flame_Wizard, //불법사

	//4성
	Doom_Arbiter,
	Pirate_Captain,
	Tortola_Elder,
	Siren,

	//5성
	Tsunami_Stalker,
	Strange_Egg,
	God_Of_Thunder,

	End
};

enum class Chess_State
{
	Attack_1,
	Attack_2,
	Born,
	Death,
	Dizzy,
	Idle,
	Jump,
	Run,
	Skill01,
	Skill02,
	Victory,
	End
};

struct ChessPiece_Info
{
	//Chess_Name Name; //수정
	Game_String Name;

	Chess_Race RaceName1 = Chess_Race::End;
	Chess_Race RaceName2 = Chess_Race::End;

	Chess_Class ClassName = Chess_Class::End;

	bool MyUnit = true; //내 기물인가요? (상대방 유닛이면 false)
	bool Banch = false; //벤치면 true

	int Cost = 0; //가격
	int Star = 1; //등급

	float Hp = 0; //체력
	float MaxHp = 0;//최대체력
	int	Mana = 0; //마나
	int	MaxMana = 100; //마나
	bool Play_Skill = false; //스킬쓰는중인가요
	float SkileTime = 0.f; //스킬 몇초나 쓰고있나요

	int Atk = 0; //공격력
	float Atk_Rate = 0; //공속
	float Range = 0; //사거리
	float Hp_Regeneration = 0; //체젠
	int Armor = 0; //방어력
	int Magic_Resistance = 0; //마방
	
	bool Token = false; //토큰인가? o/x

	int Damage_To_Player = 0; //상대 플레이어에게 데미지.

	float Position_X = 0.f; //좌표 (0x0 ~ 7x7) - 남에게 보여지는 지금 위치.
	float Position_Y = 0.f; //좌표
	int Move_X = 0.f; //이동 목표 좌표
	int Move_Y = 0.f; //이동 목표 좌표
	float Real_X = 0.f; //진짜 좌표
	float Real_Y = 0.f;

	float Rot = 0.f;

	bool Death = false; //죽음 (player boardlist에서 제거하기 위해서.)
	Chess_State State = Chess_State::End; //애니메이션 상태

};

class AStar_Struct
{
public:
	int X;
	int Y;	
	int FScore = 999;
	int GScore = 999;
	int HScore = 999;
	int Origin_X = 999; //최단 거리의 이전 노드. (유사 부모)
	int Origin_Y = 999;
	bool Block = false; //막힌 길 판정 받는 곳. true = 막힘
};

class HACTORCOM;
class Chess_Base : public HACTORCOM
{
public:
	ChessPiece_Info Info;
	Game_String Get_Name() { return Info.Name; }

	//출력
	Game_Ptr<Game_Actor> MeshActor;

	//체력바
	Game_Ptr<Game_Renderer> HP_Bar;
	Game_Ptr<Game_Renderer> HP_BlackBar;

	//출력용
	//Game_Ptr<Game_Actor> MeshActor;
	Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr;

	void AStar();
	bool Board_Array[8][8]; //막힌 곳 탐지용. true = 막힘

	list<AStar_Struct*> OpenList;
	list<AStar_Struct*> CloseList;
	bool IsMove = false;
	bool IsAtk = false;
	float AtkTime = 0.f;
	void Move();
	float Speed = 2.f;


	float Me_CloseEnemy_distance();
	int Get_HScore(int x1, int y1, int x2, int y2);
	bool IsBlock(int x, int y);
	AStar_Struct* Find_Close_Astar(int x, int y);
	AStar_Struct* Find_Open_Astar(int x, int y);
	AStar_Struct Find_Enemy();
	void Find_Route(int Start_X, int Start_Y, int Route_X, int Route_Y);

	void Set_Rot(int _x, int _y);

	//자식들용
	void IsBanch();
	void Check_Death(bool Check = false);
	void Death_Check();
	virtual void Skill_Init();
	virtual void Skill_Update();

	//BaseUpdate 내부
	void Base_Update();
	void Set_Ani();
};