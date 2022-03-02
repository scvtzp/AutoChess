#pragma once
#include "Game_Device.h"
#include <HMGR.h>

enum class SHADERTYPE
{
	SHADER_VS,
	SHADER_HS,
	SHADER_DS,
	SHADER_GS,
	SHADER_PS,
	SHADER_END,
};


class SHADERDATA 
{
public:
	Game_String Name;
	size_t m_Size; // 상수버퍼 전용
	SHADERTYPE m_ShaderType; // 내가 어느 쉐이더
	D3D_SHADER_INPUT_TYPE m_DataType; // 텍스처다 상수버퍼 
	unsigned int m_Index;
};

// 언어는 버전이 있다.
// C++ 17버전
class HRENDERPLAYER;
class HSHADER 
{
public:
	friend HRENDERPLAYER;

protected:
	// 사용권한이 아니다.
	// char[]안에 그걸 담는다.
	// 쉐이더의 코드 그자체
	ID3DBlob* m_pBlob; // 컴파일된 코드의 바이너리 포인터
	// 에러도 담아준다.
	ID3DBlob* m_pErrBlob; // 컴파일된 코드의 바이너리 포인터
	unsigned int m_VH;
	unsigned int m_VL;
	SHADERTYPE m_Type;

private:
	std::map<Game_String, SHADERDATA> m_ResData;

public:
	void ResCheck();


public:
	static void AutoShaderCompile(const Game_String& _File);
};

