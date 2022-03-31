#pragma once
#include "GAMELOGIC.h"
#include "Chess_Base.h"
#include "Game_2DAnimation.h"
#include <HCOL.h>

struct ChessPiece_Supply
{
	vector<Chess_Base> Lv1_Supply;
	vector<Chess_Base> Lv2_Supply;
	vector<Chess_Base> Lv3_Supply;
	vector<Chess_Base> Lv4_Supply;
	vector<Chess_Base> Lv5_Supply;

	//�⹰ ����.
	//1��-45 2��-30 3��-25 4��-15 5��-10
};

enum class Chess_Team
{
	Ally,
	Enemy,
	End
};

class Chess_player : public HACTORCOM
{
public:
	Chess_player();
	~Chess_player();

private:
	int Hp = 100;
	int Level = 1;
	int Exp = 0;

	map<Chess_Race, Chess_Base> Race_synergy; //���� ���� ��Ȳ?(Ư��)
	map<Chess_Class, Chess_Base> Class_synergy; //���� ���� ��Ȳ(Ư��) //���� ���۽ø��� ���ư�.

	//���콺
	Game_Ptr<Game_Collision> Mouse_Col;
	Game_Ptr<Game_Renderer> Mouse_ColRenderer;

	//��ư
	Game_Ptr<Game_Collision> PauseButton_Col;
	Game_Ptr<Game_Renderer> PauseButton_ColRenderer;

	//����
	vector<Chess_Base> Chess_Store = std::vector<Chess_Base>(5);
	Game_Ptr<Game_Actor> Store_Actor[5];
	Game_Ptr<Game_Renderer> Store_Renderer[5];
	Game_Ptr<Game_2DAnimation> Store_Ani[5];
	Game_Ptr<Game_Collision> Store_Col[5];
	Game_Ptr<Game_Renderer> Store_ColRenderer[5];

	//�ʵ�
	vector<Game_Ptr<Chess_Base>> Piece_Banch; //��ġ����
	void Set_Banch_Sequence();
	
	//�ʵ� �̵�
	static bool PickCheck;
	static Game_Vector PickPos;

	//UI
	Game_Ptr<Game_Renderer> Ui_Black;

	//Test
	template<typename T>
	void Spawn_Unit(int _X, int _Y, Chess_Team _Team = Chess_Team::Ally); //_Team 0�̶�� �Ʊ� 1�̶�� ����


public:
	static ChessPiece_Supply ChessPiece_Supply; //����ó
	static list<Game_Ptr<Chess_Base>> Piece_Board; //�� ���� ���� ��� (��ū ����)
	static list<Game_Ptr<Chess_Base>> Piece_Enemy_ChessBoard; //���� �������� ��� (��ū ����)

	static bool Round;

	void Init() override;
	void Update() override;

	//Store ����
	template <class T>
	void Supply_Input(int Level);
	void ReRoll();

	void Set_ShopAni();
	void Set_ShopTex(Game_String Name, int ShopNum, float X, float Y, float Z, int AniStart = -1, int AniEnd = -1);
	void Store_Unit_Col(Game_Collision* _This, Game_Collision* _Other); //Ŭ��������	
	void Make_Piece(Game_String _Name, int _Star = 1); //�̸� �־��ָ� ���� ��ġ�� ����.
	template <class T>
	void Make_Piece_T(int _Star);

	//TEst
	void Test();
	void Ui_Switch();

	//init ���� ����.
	void UI_Setting();
	void Round_Col(Game_Collision* _This, Game_Collision* _Other); //���� ����/���� ��ư
	void Board_Col(Game_Collision* _This, Game_Collision* _Other); 

	//��Ÿ
	void Check_StarUP(Game_String _Name, int _Star = 1); //3�� ���̸� �������ִ°� üũ.
	bool Find_Piece(int x, int y);
};


//�ڵ� �̴� ����
//1. ������ ���� %�� ���� 5���� ��� ����.
//2. �� ����� ������ �ϳ� ����. x5swa3e2qw2