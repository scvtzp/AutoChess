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
	Game_Vector Normal; // 이녀석이 Y축
	Game_Vector BiNormal; // 이녀석이 X축
	Game_Vector Tangent; // 이녀석이 Z축 // 나중에 바꿀수 있다.
	Game_Vector Weight; // 얼마나 영향을 받아야 하는가? 0.5 영향을 받는다.
					// 스키닝 정보가 없는애면 1.0f
	int Index[4]; // 21번째 본에 60 이 버텍스는 60번째 본이랑 곱하는 녀석이다.
	//  Weight:        0.5    0.5
	//  Index:         60     59
};


