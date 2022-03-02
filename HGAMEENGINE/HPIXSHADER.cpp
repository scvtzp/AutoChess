#include "HPIXSHADER.h"


void HPIXSHADER::Load(const Game_String& _FuncName, unsigned int _VH, unsigned int _VL)
{
	m_VH = _VH;
	m_VL = _VL;

	unsigned int Flag = 0;

#ifdef _DEBUG
	// ����� ���� ���̴� �⵿
	Flag = D3D10_SHADER_DEBUG;
#endif
	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	char Arr[200];

	sprintf_s(Arr, "ps_%d_%d", m_VH, m_VL);

	std::string FuncName = _FuncName;

	if (S_OK != D3DCompileFromFile(
		GameFile.FullPath(),
		nullptr,  // ��ũ�ε� �׳� ���̴� �ȿ� �ִ°�
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // ��ũ��带 �׳� �˾Ƽ� �ϰڴ�.
		FuncName.c_str(), // �������ϰ� ���� �Լ��̸�
		Arr, // ����
		Flag,
		0,// �⺻ �ɼ����� ������
		&m_pBlob,
		&m_pErrBlob)
		)
	{
		Game_String ErrorText = (char*)m_pErrBlob->GetBufferPointer();
		AMSG(ErrorText + L"���̴� ������ ����");
		return;
	}

	if (S_OK != Game_Device::MAINOBJ()->Device()->CreatePixelShader(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		nullptr,
		&m_ShaderPtr))
	{
		AMSG(L"���ؽ� ���̴� �������̽� ������ �����߽��ϴ�");
	}

	ResCheck();
}

void HPIXSHADER::Setting() 
{
	// m_ShaderPtr
	Game_Device::MAINOBJ()->Context()->PSSetShader(m_ShaderPtr, 0, 0);
}