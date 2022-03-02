#pragma once
#include "HSHADER.h"
#include <vector>

class HVTXSHADER : public HSHADER, public HFILEMGR<HVTXSHADER>
{
public:
	static Game_Ptr<HVTXSHADER> Load(const Game_String& _Path, const Game_String& _FuncName, unsigned int _VH = 5, unsigned int _VL = 0)
	{
		Game_Ptr<HVTXSHADER> Res = CreateToInsertToName(_Path, _FuncName);
		Res->Load(_FuncName, _VH, _VL);
		return Res;
	}

	// 내가 어떤 형태로 너에게 버텍스 버퍼를 넣어줬어.

public:
	unsigned int FmtSize(DXGI_FORMAT _Fmt);

public:
	ID3D11InputLayout* m_pLayOut;
	unsigned int m_Offset;
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_ArrInputList;

public:
	void AddLayout(
		const char* _SmtName, // "POSITIONT3"
		unsigned int _Index, // "POSITIONT0 ~ N"
		DXGI_FORMAT _Fmt, // r32g32
		unsigned int _InputSlot = 0, // 무시 인스턴싱 관련
		unsigned int _IDSR = 0, // 인스턴싱 관련
		D3D11_INPUT_CLASSIFICATION _InputClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA // 인스턴싱 관련
		);

	void CreateLayout();

	void LayOutCheck();


public:
	ID3D11VertexShader* m_ShaderPtr;


public:
	void Load(const Game_String& _FuncName, unsigned int _VH = 5, unsigned int _VL = 0);
	void Setting();
public:
	HVTXSHADER() : m_ShaderPtr(nullptr), m_pLayOut(nullptr)
	{
		m_Type = SHADERTYPE::SHADER_VS;
	}

	~HVTXSHADER()
	{
		if (nullptr != m_pLayOut)
		{
			m_pLayOut->Release();
		}
		m_ShaderPtr->Release();
	}

};

