#include "HSHADER.h"
#include <HGAMEFILE.h>
#include "HVTXSHADER.h"
#include "HPIXSHADER.h"

void HSHADER::AutoShaderCompile(const Game_String& _FilePath)
{
	HGAMEFILE File = HGAMEFILE(_FilePath, L"rt");
	Game_String AllText = File.AllText();

	std::list<HSTRINGDATA> AllData = AllText.AllFindData(L"(");

	std::list<HSTRINGDATA> Name;
	std::list<HSTRINGDATA> Para;

	std::list<HSTRINGDATA>::iterator Start = AllData.begin();
	std::list<HSTRINGDATA>::iterator End = AllData.end();

	// fasdfasdfas (

	for (; Start != End; ++Start)
	{
		Name.push_back(AllText.RerversTrimToTrim((*Start).StartPos - 1));
		Para.push_back(AllText.TrimToTrim((*Start).EndPos));
	}

	int a = 0;

	{
		std::list<HSTRINGDATA>::iterator NameStart = Name.begin();
		std::list<HSTRINGDATA>::iterator NameEnd = Name.end();

		std::list<HSTRINGDATA>::iterator ParaStart = Para.begin();
		std::list<HSTRINGDATA>::iterator ParaEnd = Para.end();

		std::list<HSTRINGDATA> AllFunc = std::list<HSTRINGDATA>();

		for (; NameStart != NameEnd;)
		{
			if (L"register" == (*NameStart).Text || L"" == (*NameStart).Text)
			{
				NameStart = Name.erase(NameStart);
				ParaStart = Para.erase(ParaStart);
			}
			else 
			{
				std::list<HSTRINGDATA> FindParaList = AllText.RerversAllFindData((*ParaStart).Text, (*ParaStart).StartPos);

				if (0 == FindParaList.size())
				{
					NameStart = Name.erase(NameStart);
					ParaStart = Para.erase(ParaStart);
				}
				else if (0 != FindParaList.size())
				{
					HSTRINGDATA Data;
					for (auto& _ShaderFind : FindParaList)
					{
						Data = AllText.RerversTrimToTrim(_ShaderFind.StartPos - 1);
						if (L"struct" == Data.Text)
						{
							break;
						}
					}

					if (L"struct" == Data.Text)
					{
						++NameStart;
						++ParaStart;
					}
					else {
						NameStart = Name.erase(NameStart);
						ParaStart = Para.erase(ParaStart);
					}

				}
				else {
					++NameStart;
					++ParaStart;
				}
				int a = 0;
			}
		}
	}

	if (true == Name.empty())
	{
		AMSG(L"�Լ��� �ϳ��� ������� �ʽ��ϴ�.");
		return;
	}

	if (Name.size() != Para.size())
	{
		AMSG(L"�Լ��� �Ķ������ ������ �ٸ��ϴ�.");
		return;
	}

	a = 0;
	{
		std::list<HSTRINGDATA>::iterator NameStart = Name.begin();
		std::list<HSTRINGDATA>::iterator NameEnd = Name.end();

		std::list<HSTRINGDATA>::iterator ParaStart = Para.begin();
		std::list<HSTRINGDATA>::iterator ParaEnd = Para.end();

		// �̳༮�� ���� �Ǿ�� �Ѵ�.
		// ���ؽ� ���̴� �����ȼ����̴� ���� �����Ǿ�� �Ѵ�.
		for (; NameStart != NameEnd;++NameStart)
		{
			// ���⼭ �ݴ��ʿ� �ִ� ����� ������̿� �ִ� �༮��
			// �Լ��� ���� ���̴�.
			HSTRINGDATA ReturnData = AllText.RerversTrimToTrim(NameStart->StartPos - 1);

			// ����ü�� �ƴҶ��̴�.
			if (L"float4" == ReturnData.Text)
			{
				size_t FindStartPos = AllText.Find(L")", ReturnData.EndPos);
				FindStartPos = AllText.Find(L":", FindStartPos);

				HSTRINGDATA SementicData = AllText.TrimToTrim(FindStartPos + 1);

				if (0 == SementicData.Text.Find(L"SV_Position"))
				{
					HVTXSHADER::Load(_FilePath, NameStart->Text);
				}
				else if (0 == SementicData.Text.Find(L"SV_Target"))
				{
					HPIXSHADER::Load(_FilePath, NameStart->Text);
				}
				else 
				{
					AMSG(L"���̴� Ÿ���� ������ �� ���� ��Ȳ�Դϴ�");
				}
			}
			else 
			{
				std::list<HSTRINGDATA> FindReturnList = AllText.RerversAllFindData(ReturnData.Text, ReturnData.StartPos);

				if (0 == FindReturnList.size())
				{
					AMSG(L"�Լ��� ���ϰ��� �������� �ʽ��ϴ�. \n �������ϱ��� ��� ã�ƺ����ϴ� ��Ȳ�Դϴ�. Ȥ�� �Լ��̸� �м� �����Դϴ�.");
				}
				else if (0 != FindReturnList.size())
				{
					HSTRINGDATA FindData;
					bool Check = false;

					for (auto& _Shader : FindReturnList)
					{
						FindData = AllText.RerversTrimToTrim(_Shader.StartPos - 1);

						if (L"struct" == FindData.Text)
						{
							size_t LastPos = AllText.Find(L"}", FindData.EndPos);
							Game_String StructText = AllText.Cut(FindData.EndPos + 1, LastPos);

							if (std::wstring::npos != StructText.Find(L"SV_Position"))
							{
								HVTXSHADER::Load(_FilePath, NameStart->Text);
								Check = true;
							}
							else if (std::wstring::npos != StructText.Find(L"SV_Target"))
							{
								Game_Ptr<HPIXSHADER> PTR = HPIXSHADER::Load(_FilePath, NameStart->Text);

								Game_String Text = StructText.ToUpperReturn();

								if (std::wstring::npos != Text.Find(L"DEFFERD"))
								{
									PTR->RPType(RENDERPATHTYPE::RP_DEFFERD);
									int a = 0;
								}

								if (std::wstring::npos != Text.Find(L"FORWARD"))
								{
									PTR->RPType(RENDERPATHTYPE::RP_FORWARD);
									int a = 0;
								}

								Check = true;
							}
							else {
								AMSG(L"���̴� Ÿ���� ������ �� ���� ��Ȳ�Դϴ�");
							}
						}
						else {
							AMSG(L"���̴� Ÿ���� ������ �� ���� ��Ȳ�Դϴ�");
						}
					}
					if (Check == false)
					{
						AMSG(L"���̴� Ÿ���� ������ �� ���� ��Ȳ�Դϴ�");
					}
				}
			}
		}
	}
}

void HSHADER::ResCheck() 
{
	ID3D11ShaderReflection* Reflector = nullptr;

	if (S_OK
		!=
		D3DReflect(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize()
			, IID_ID3D11ShaderReflection, (void**)&Reflector))
	{
	}

	D3D11_SHADER_DESC ShaderDesc;
	Reflector->GetDesc(&ShaderDesc);

	// ��� ���ҽ� ���� �˾ƿ���
	D3D11_SHADER_INPUT_BIND_DESC ResDesc;
	SHADERDATA SData;
	for (unsigned int i = 0; i < ShaderDesc.BoundResources; i++)
	{
		Reflector->GetResourceBindingDesc(i, &ResDesc);

		SData.Name = ResDesc.Name;
		SData.m_ShaderType = m_Type;
		SData.m_Index = ResDesc.BindPoint;
		SData.m_DataType = ResDesc.Type;
		
		switch (ResDesc.Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D11ShaderReflectionConstantBuffer* ConstBufferData = Reflector->GetConstantBufferByName(ResDesc.Name);
			D3D11_SHADER_BUFFER_DESC BufferDesc;
			ConstBufferData->GetDesc(&BufferDesc);
			SData.m_Size = BufferDesc.Size;
			break;
		}
		case D3D_SIT_TEXTURE:
			break;
		case D3D_SIT_SAMPLER:
			break;
		default:
			AMSG(L"���� ó���ϴ� ����� ������ ���� ���ҽ� �Դϴ�.");
			break;
		}

		if (m_ResData.end() != m_ResData.find(ResDesc.Name))
		{
			AMSG(L"�ߺ� ���ҽ��� �����մϴ�.");
		}

		m_ResData[ResDesc.Name] = SData;
	}

}