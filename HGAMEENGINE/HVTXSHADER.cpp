#include "HVTXSHADER.h"


unsigned int HVTXSHADER::FmtSize(DXGI_FORMAT _Fmt)
{
	switch (_Fmt)
	{
	case DXGI_FORMAT_UNKNOWN:
		AMSG(L"포맷을 알수가 없습니다");
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 16;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 12;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:

	case DXGI_FORMAT_BC2_TYPELESS:

	case DXGI_FORMAT_BC2_UNORM:

	case DXGI_FORMAT_BC2_UNORM_SRGB:

	case DXGI_FORMAT_BC3_TYPELESS:

	case DXGI_FORMAT_BC3_UNORM:

	case DXGI_FORMAT_BC3_UNORM_SRGB:

	case DXGI_FORMAT_BC4_TYPELESS:

	case DXGI_FORMAT_BC4_UNORM:

	case DXGI_FORMAT_BC4_SNORM:

	case DXGI_FORMAT_BC5_TYPELESS:

	case DXGI_FORMAT_BC5_UNORM:

	case DXGI_FORMAT_BC5_SNORM:

	case DXGI_FORMAT_B5G6R5_UNORM:

	case DXGI_FORMAT_B5G5R5A1_UNORM:

	case DXGI_FORMAT_B8G8R8A8_UNORM:

	case DXGI_FORMAT_B8G8R8X8_UNORM:

	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:

	case DXGI_FORMAT_B8G8R8A8_TYPELESS:

	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:

	case DXGI_FORMAT_B8G8R8X8_TYPELESS:

	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:

	case DXGI_FORMAT_BC6H_TYPELESS:

	case DXGI_FORMAT_BC6H_UF16:

	case DXGI_FORMAT_BC6H_SF16:

	case DXGI_FORMAT_BC7_TYPELESS:

	case DXGI_FORMAT_BC7_UNORM:

	case DXGI_FORMAT_BC7_UNORM_SRGB:

	case DXGI_FORMAT_AYUV:

	case DXGI_FORMAT_Y410:

	case DXGI_FORMAT_Y416:

	case DXGI_FORMAT_NV12:

	case DXGI_FORMAT_P010:

	case DXGI_FORMAT_P016:

	case DXGI_FORMAT_420_OPAQUE:

	case DXGI_FORMAT_YUY2:

	case DXGI_FORMAT_Y210:

	case DXGI_FORMAT_Y216:

	case DXGI_FORMAT_NV11:

	case DXGI_FORMAT_AI44:

	case DXGI_FORMAT_IA44:

	case DXGI_FORMAT_P8:

	case DXGI_FORMAT_A8P8:

	case DXGI_FORMAT_B4G4R4A4_UNORM:

	case DXGI_FORMAT_P208:

	case DXGI_FORMAT_V208:

	case DXGI_FORMAT_V408:

	case DXGI_FORMAT_FORCE_UINT:

	default:
		break;
	}

	return 0;
}

void HVTXSHADER::LayOutCheck() 
{
	// 버텍스 버퍼가 어떻게.
	//	VTX->AddLayout("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//	VTX->AddLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//	VTX->AddLayout("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//	VTX->CreateLayout();

	// HSTRINGDATA DATA = "312321" + 1;
	ID3D11ShaderReflection* Reflector = nullptr;

	if (S_OK 
		!= 
		D3DReflect(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize()
			, IID_ID3D11ShaderReflection, (void**)&Reflector))
	{
		AMSG(L"쉐이더 리플렉션에 실패했습니다.");
	}

	D3D11_SHADER_DESC ShaderDesc;
	Reflector->GetDesc(&ShaderDesc);

	for (unsigned int i = 0; i < ShaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC InputDesc = {};
		Reflector->GetInputParameterDesc(i, &InputDesc);
		DXGI_FORMAT Fmt = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		// 0001

		switch (InputDesc.Mask)
		{
		case 1:
			switch (InputDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
				break;
			default:
				AMSG(L"레이아웃 해석 실패");
				break;
			}
			break;
		case 3:
			switch (InputDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
				break;
			default:
				AMSG(L"레이아웃 해석 실패");
				break;
			}
			break;
		case 7:
			switch (InputDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			default:
				AMSG(L"레이아웃 해석 실패");
				break;
			}
			break;
		case 15:
			switch (InputDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				Fmt = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			default:
				AMSG(L"레이아웃 해석 실패");
				break;
			}
			break;
		default:
			break;
		}

		if (Fmt == DXGI_FORMAT::DXGI_FORMAT_UNKNOWN)
		{
			AMSG(L"인풋 레이아웃 포맷 판단에 실패했습니다.");
		}

		AddLayout(InputDesc.SemanticName, 0, Fmt, 0);
	}

	CreateLayout();
}

void HVTXSHADER::Load(const Game_String& _FuncName, unsigned int _VH, unsigned int _VL)
{
	m_VH = _VH;
	m_VL = _VL;

	unsigned int Flag = 0;

#ifdef _DEBUG
	// 디버그 모드로 쉐이더 기동
	Flag = D3D10_SHADER_DEBUG;
#endif

	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	char Arr[200];

	sprintf_s(Arr, "vs_%d_%d", m_VH, m_VL);
	// vs_5_0

	// D3D11_INPUT_ELEMENT_DESC

	std::string FuncName = _FuncName;

	if (S_OK != D3DCompileFromFile(
		GameFile.FullPath(),
		nullptr,  // 매크로도 그냥 쉐이더 안에 있는걸
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // 인크루드를 그냥 알아서 하겠다.
		FuncName.c_str(), // 컴파일하고 싶은 함수이름
		Arr, // 버전
		Flag,
		0,// 기본 옵션으로 컴파일
		&m_pBlob,
		&m_pErrBlob)
		)
	{
		Game_String ErrorText = (char*)m_pErrBlob->GetBufferPointer();

		AMSG(ErrorText + L"쉐이더 컴파일 에러");
		return;
	}

	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateVertexShader(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		nullptr,
		&m_ShaderPtr))
	{
		AMSG(L"버텍스 쉐이더 인터페이스 생성에 실패했습니다");
	}

	LayOutCheck();


	ResCheck();
}


void HVTXSHADER::Setting()
{
	// m_ShaderPtr
	Game_Device::MAINOBJ()->Context()->IASetInputLayout(m_pLayOut);
	Game_Device::MAINOBJ()->Context()->VSSetShader(m_ShaderPtr, 0, 0);
}

void HVTXSHADER::AddLayout(
	const char* _SmtName, // "POSITIONT3"
	unsigned int _Index, // "POSITIONT0 ~ N"
	DXGI_FORMAT _Fmt, // r32g32
	unsigned int _InputSlot, // 인스턴싱 관련
	unsigned int _IDSR, // 인스턴싱 관련
	D3D11_INPUT_CLASSIFICATION _InputClass // 인스턴싱 관련
)
{
	D3D11_INPUT_ELEMENT_DESC IED = { 0 };

	IED.SemanticName = _SmtName;
	IED.SemanticIndex = _Index;
	IED.Format = _Fmt;

	IED.InputSlot = _InputSlot;
	IED.InstanceDataStepRate = _IDSR;
	IED.InputSlotClass = _InputClass;
	IED.AlignedByteOffset = m_Offset;
	m_Offset += FmtSize(_Fmt);
	m_ArrInputList.push_back(IED);
}

void HVTXSHADER::CreateLayout()
{
	if (S_OK != Game_Device::MAINOBJ()->Device()->CreateInputLayout(
		&m_ArrInputList[0],
		(unsigned int)m_ArrInputList.size(),
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		&m_pLayOut))
	{
		AMSG(L"인풋 레이아웃 생성에 실패했습니다.");
	}
	
}