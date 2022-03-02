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
	size_t m_Size; // ������� ����
	SHADERTYPE m_ShaderType; // ���� ��� ���̴�
	D3D_SHADER_INPUT_TYPE m_DataType; // �ؽ�ó�� ������� 
	unsigned int m_Index;
};

// ���� ������ �ִ�.
// C++ 17����
class HRENDERPLAYER;
class HSHADER 
{
public:
	friend HRENDERPLAYER;

protected:
	// �������� �ƴϴ�.
	// char[]�ȿ� �װ� ��´�.
	// ���̴��� �ڵ� ����ü
	ID3DBlob* m_pBlob; // �����ϵ� �ڵ��� ���̳ʸ� ������
	// ������ ����ش�.
	ID3DBlob* m_pErrBlob; // �����ϵ� �ڵ��� ���̳ʸ� ������
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

