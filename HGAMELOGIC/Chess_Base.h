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
	//1��
	Unicorn, //������
	Tusk_Champion, //������
	The_Source, //�� �ҽ�
	God_of_War,
	Egersis_Ranger,

	//2��
	SwordMaster,
	Shining_Dragon, //������ ��

	//3��
	WereWolf,
	Poisonous_Worm, //����
	Thunder_Spirit, //������
	WindRanger,
	Shadow_Davil, //�׸��� �Ǹ�
	Flame_Wizard, //�ҹ���

	//4��
	Doom_Arbiter,
	Pirate_Captain,
	Tortola_Elder,
	Siren,

	//5��
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
	//Chess_Name Name; //����
	Game_String Name;

	Chess_Race RaceName1 = Chess_Race::End;
	Chess_Race RaceName2 = Chess_Race::End;

	Chess_Class ClassName = Chess_Class::End;

	bool MyUnit = true; //�� �⹰�ΰ���? (���� �����̸� false)
	bool Banch = false; //��ġ�� true

	int Cost = 0; //����
	int Star = 1; //���

	float Hp = 0; //ü��
	float MaxHp = 0;//�ִ�ü��
	int	Mana = 0; //����
	int	MaxMana = 100; //����
	bool Play_Skill = false; //��ų�������ΰ���
	float SkileTime = 0.f; //��ų ���ʳ� �����ֳ���

	int Atk = 0; //���ݷ�
	float Atk_Rate = 0; //����
	float Range = 0; //��Ÿ�
	float Hp_Regeneration = 0; //ü��
	int Armor = 0; //����
	int Magic_Resistance = 0; //����
	
	bool Token = false; //��ū�ΰ�? o/x

	int Damage_To_Player = 0; //��� �÷��̾�� ������.

	float Position_X = 0.f; //��ǥ (0x0 ~ 7x7) - ������ �������� ���� ��ġ.
	float Position_Y = 0.f; //��ǥ
	int Move_X = 0.f; //�̵� ��ǥ ��ǥ
	int Move_Y = 0.f; //�̵� ��ǥ ��ǥ
	float Real_X = 0.f; //��¥ ��ǥ
	float Real_Y = 0.f;

	float Rot = 0.f;

	bool Death = false; //���� (player boardlist���� �����ϱ� ���ؼ�.)
	Chess_State State = Chess_State::End; //�ִϸ��̼� ����

};

class AStar_Struct
{
public:
	int X;
	int Y;	
	int FScore = 999;
	int GScore = 999;
	int HScore = 999;
	int Origin_X = 999; //�ִ� �Ÿ��� ���� ���. (���� �θ�)
	int Origin_Y = 999;
	bool Block = false; //���� �� ���� �޴� ��. true = ����
};

class HACTORCOM;
class Chess_Base : public HACTORCOM
{
public:
	ChessPiece_Info Info;
	Game_String Get_Name() { return Info.Name; }

	//���
	Game_Ptr<Game_Actor> MeshActor;

	//ü�¹�
	Game_Ptr<Game_Renderer> HP_Bar;
	Game_Ptr<Game_Renderer> HP_BlackBar;

	//��¿�
	//Game_Ptr<Game_Actor> MeshActor;
	Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr;

	void AStar();
	bool Board_Array[8][8]; //���� �� Ž����. true = ����

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

	//�ڽĵ��
	void IsBanch();
	void Check_Death(bool Check = false);
	void Death_Check();
	virtual void Skill_Init();
	virtual void Skill_Update();

	//BaseUpdate ����
	void Base_Update();
	void Set_Ani();
};