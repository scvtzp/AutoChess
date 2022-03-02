#pragma once
#include "Game_Trans.h"
#include "HMATERIAL.h"
#include "HMESH.h"
#include "HCBUFFER.h"
#include "HTEXTURE.h"
#include "HSAMPLER.h"
#include <set>


// 상수버퍼를 세팅해주는 기능
class HLight;
class CBUFFERSETTER
{
public:
	friend class HRENDERPLAYER;
	friend class Game_Renderer;
public:
	CBUFFERMODE m_Mode;
	const SHADERDATA* m_Data;
	Game_Ptr<HCBUFFER> m_RES;
	void* m_Value;

public:
	~CBUFFERSETTER() 
	{
		if (nullptr != m_Value && m_Mode == CBUFFERMODE::CB_NEW)
		{
			delete[] m_Value;
		}
	}
};

class TEXTURESETTER
{
public:
	friend class HRENDERPLAYER;
	friend class HRENDER;
public:
	const SHADERDATA* m_Data;
	Game_Ptr<HTEXTURE> m_RES;
	bool IsOption;
};

class SAMPLERSETTER
{
public:
	friend class HRENDERPLAYER;
	friend class HRENDER;
public:
	const SHADERDATA* m_Data;
	Game_Ptr<HSAMPLER> m_RES;
};

#define BOOLTOINT(NAME)  \
public: \
	bool NAME; \
private: \
	bool NAME##Temp##[3]; \

// 자동세팅이라 이렇게 바꾸면
class RenderPlayerOption 
{
public:
	BOOLTOINT(IsDifTexture);
	BOOLTOINT(IsNormalTexture);
	BOOLTOINT(IsShadow);
	BOOLTOINT(IsAni);
	BOOLTOINT(IsDummy0);
	BOOLTOINT(IsDummy1);
	BOOLTOINT(IsDummy2);
	BOOLTOINT(IsDummy3);

public:
	float4 BasicColor;

public:
	RenderPlayerOption() 
	{
		IsDifTexture = false;
		IsNormalTexture = false;
		IsShadow = false;
		IsAni = false;
		BasicColor = Game_Vector::WHITE;
	}
};

class HMESH;
class Game_Material;
class Game_Renderer;
class HRENDERPLAYER : public Game_Ref, public HOnOffBase
{
	friend Game_Renderer;

public:
	Game_Renderer* m_Parent;

	RenderPlayerOption RenderOption;

	Game_Ptr<HMESH> m_Mesh;
	Game_Ptr<Game_Material> m_Mat;

private:
	// 아 상수버퍼가 있구나 내가 그걸 세팅하는 클래스를 만들께.
	// 6칸
	//   어떤쉐이더에       이런이름의  이 타입의 리소스를 세팅해주는
	std::map<Game_String, std::vector<CBUFFERSETTER>> m_CBUFFER;
	std::map<Game_String, std::vector<TEXTURESETTER>> m_TEXTURE;
	std::map<Game_String, std::vector<SAMPLERSETTER>> m_SAMPLER;

public:
	std::list<TEXTURESETTER*> AllTextureSetter(const Game_String& _SetterName);


public:
	Game_Ptr<HMESH> MESH();
	Game_Ptr<Game_Material> MATERIAL();

	void MESH(const Game_Ptr<HMESH>& _Mesh);
	void MESH(const Game_String& _Name);
	void MATERIAL(const Game_String& _Name, bool _Push = true);

	Game_Ptr<HTEXTURE> GetTEXTURE(const Game_String& _Name);
	void TEXTURE(const Game_String& _Name, const Game_Ptr<HTEXTURE>& _Tex);
	void TEXTURE(const Game_String& _Name, const Game_String& _TexName);
	void SAMPLER(const Game_String& _Name, const Game_String& _SmpName);
	void CBUFFER(const Game_String& _Name, void* _Data, CBUFFERMODE _Mode);

	bool IsTEXTURE(const Game_String& _Name);
	bool IsSAMPLER(const Game_String& _Name);
	bool IsCBUFFER(const Game_String& _Name);

public:
	void Render();
	void Reset();
};


// 뭐든지 랜더할수 있는 기반을 다져줘야 한다.
// 애만으로 모든 랜더가 가능해야 한다.
// 여러분들이 불편하면 상속을 내리는 거다.
class HCAM;
class HMESH;
class Game_Material;
class Game_Renderer : public Game_Trans
{
private:
public:
	std::vector<Game_Ptr<HRENDERPLAYER>> m_PlayerList;

	std::vector<Game_Ptr<HRENDERPLAYER>> m_DefferdList;
	std::vector<Game_Ptr<HRENDERPLAYER>> m_ForwardList;
	std::vector<Game_Ptr<HRENDERPLAYER>> m_ShadowList;

public:
	// GetRenderPlayer();

public:
	void Init(int _Order = 0);
	void Init(const Game_String& _Mesh, const Game_String& _Mat, int _Order = 0);
	void Init(const Game_Ptr<HMESH>& _Mesh, const Game_String& _Mat, int _Order = 0);

	virtual void Render(Game_Ptr<HCAM> _Cam);
	virtual void DefferdRender(Game_Ptr<HCAM> _Cam);
	virtual void ForwardRender(Game_Ptr<HCAM> _Cam);
	virtual void ShadowRender(const LightData& _Data);


	Game_Ptr<HRENDERPLAYER> CreateRenderPlayer(const Game_Ptr<HMESH>& _Mesh, const Game_String& _Mat);
	Game_Ptr<HRENDERPLAYER> CreateRenderPlayer(const Game_String& _Mesh, const Game_String& _Mat);
	Game_Ptr<HRENDERPLAYER> CreateRenderPlayer(const Game_Ptr<HMESH>& _Mesh);

	Game_Ptr<HRENDERPLAYER> GetRenderPlayer(int _Index);

	void TEXTURE(const Game_String& _Name, const Game_Ptr<HTEXTURE>& _Res);
	void TEXTURE(const Game_String& _Name, const Game_String& _ResName);
	void CBUFFER(const Game_String& _Name, void* _Data, CBUFFERMODE _Mode = CBUFFERMODE::CB_LINK);
	void SAMPLER(const Game_String& _Name, const Game_String& _ResName);


	std::vector<Game_Ptr<HRENDERPLAYER>> CreateRenderPlayerToFbxEx(const Game_String& _FbxExName, const Game_String& _MatName, int DrawSet = 0);
	Game_Ptr<HRENDERPLAYER> CreateRenderPlayerToFbxPartEx(const Game_String& _FbxExName, const Game_String& _MatName, int DrawSet, int _SubIndex, int _MainIndex);

	std::vector<Game_Ptr<HRENDERPLAYER>> CreateRenderPlayerToFbx(const Game_String& _FbxName, const Game_String& _MatName, const Game_String& _SmpName);

	// 메테리얼만 세팅되지 않은
	// 여러분들이 메테리얼을 세팅해줘야 합니다.
	std::vector<Game_Ptr<HRENDERPLAYER>> CreateRenderPlayerToFbxNoneMat(const Game_String& _FbxName);

	void ShadowOn();

public:
	Game_Renderer();
	~Game_Renderer();
};
