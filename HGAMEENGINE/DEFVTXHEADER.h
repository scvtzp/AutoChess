#pragma once

#include <HGAMEMATH.h>

class VTX2DIMGIN
{
public:
	Game_Vector Pos;
	Game_Vector Uv;
	Game_Vector Color;
};

class TARGETVTX
{
public:
	Game_Vector Pos;
	Game_Vector Uv;
};

// 

class VTX3DMESHIN
{
public:
	Game_Vector Pos;
	Game_Vector Uv;
	Game_Vector Color;
	Game_Vector Normal; // �̳༮�� Y��
	Game_Vector BiNormal; // �̳༮�� X��
	Game_Vector Tangent; // �̳༮�� Z�� // ���߿� �ٲܼ� �ִ�.
	Game_Vector Weight; // �󸶳� ������ �޾ƾ� �ϴ°�? 0.5 ������ �޴´�.
					// ��Ű�� ������ ���¾ָ� 1.0f
	int Index[4]; // 21��° ���� 60 �� ���ؽ��� 60��° ���̶� ���ϴ� �༮�̴�.
	//  Weight:        0.5    0.5
	//  Index:         60     59
};


