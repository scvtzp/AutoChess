#pragma once
#include "HGAMERES.h"
#include "Game_Device.h"
#include <DirectXTex.h>
#include "HSHADER.h"
#include <fbxsdk.h>
#include "HVTXSHADER.h"
#include "DEFVTXHEADER.h"
// #include "HRENDERTARGET" // 3d의 핵심.

#pragma comment(lib, "zlib-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

class BoneFrameData
{
public:
	Game_Vector S; // 크
	Game_Vector Q; // 자
	Game_Vector T; // 이
	double Time;
	HMATRIX FrameMat;
};

class BoneFrame
{
public:
	int BoneIndex;

	// 내가 100 째 애니메이션을 한다면
	// 1434                      
	// 프레임별 본 정보가 들어있다.
	// 1434 그중에서 100와 101번째 프레임의 본의 정보를 얻어와야 한다.
	std::vector<BoneFrameData> m_Data;
};

class HFBXANIDATA
{
public:
	Game_String AniName;
	fbxsdk::FbxTime StartTime; // 0
	fbxsdk::FbxTime EndTime;   // 1434 / 30
	fbxsdk::FbxLongLong TimeStartCount; // 0
	fbxsdk::FbxLongLong TimeEndCount; // 1434
	fbxsdk::FbxLongLong FrameCount;
	fbxsdk::FbxTime::EMode TimeMode;
	int FbxModeCount;

	// 134 본의 개수
	// 본마다 존재하는 애니메이션 프레임
	// m_AniFrameData[0].size() == 1434
	// m_AniFrameData[1].size() == 1434
	// m_AniFrameData[2].size() == 1434
	// m_AniFrameData[3].size() == 1434
	// m_AniFrameData[4].size() == 1434
	// m_AniFrameData[5].size() == 1434
	// m_AniFrameData[6].size() == 1434
	// m_AniFrameData[7].size() == 1434
	// m_AniFrameData[8].size() == 1434
	// m_AniFrameData[9].size() == 1434
	// ....
	// m_AniFrameData[134].size() == 1434

	// HMAT ArrAniMat[134]
	// 30 0.0333333333333 1
	// 30 1.0000000000000 0
	// 30 0.0234881
	// 1434
	// 0      1                     2                3
	// 0      0.03333333333333333        0.06666666666666666666
	// 0      1                          2

	// ArrAniMat[0] = m_AniFrameData[0][0] 사이의 중간값을 m_AniFrameData[0][1]
	// ArrAniMat[1] = m_AniFrameData[1][0]
	// ArrAniMat[2] = m_AniFrameData[2][0]
	// ArrAniMat[3] = m_AniFrameData[3][0]
	// ArrAniMat[4] = m_AniFrameData[4][0]
	// 134개인데
	std::vector<BoneFrame> m_AniFrameData;

public:
	// 30프레임 애니메이션이고
	// 1434 일때
	// 30 == 1초

	// 600 이면 델타타임으로 얼마일까?
	float FrameTime(int _Frame)
	{
		fbxsdk::FbxTime tTime = {};
		tTime.SetFrame(_Frame, TimeMode);

		return (float)tTime.GetSecondDouble();
	}

public:
	int FrameModeCount()
	{
		switch (TimeMode)
		{
		case fbxsdk::FbxTime::eFrames120:
			return 120;
		case fbxsdk::FbxTime::eFrames100:
			return 100;
		case fbxsdk::FbxTime::eFrames60:
			return 60;
		case fbxsdk::FbxTime::eFrames50:
			return 50;
		case fbxsdk::FbxTime::eFrames48:
			return 48;
		case fbxsdk::FbxTime::eFrames30:
			return 30;
		case fbxsdk::FbxTime::eFrames24:
			return 24;
		default:
			AMSG(L"처리할수 없는 프레임 카운트 모드입니다.");
			break;
		}

		AMSG(L"처리할수 없는 프레임 카운트 모드입니다.");
		return -1;
	}

public:
	HFBXANIDATA() {}
	~HFBXANIDATA() {}
};

class HFBXBONEDATA
{
public:
	Game_String Name;
	int			iDepth; // 루트면 0 -> 좌우로 
	int			iIndex; // 임의 선생님이 만들었다.
	int			iParentIndex; // 내부모가 49번째 애다.
	HMATRIX		Offset;
	HMATRIX		Bone;

public:
	HFBXBONEDATA() {}
	~HFBXBONEDATA() {}
};

class HFBXMATDATA
{
public:
	// 픽셀쉐이더에서 그냥 최종색상에 곱해주면 되는 색상.
	Game_Vector DifColor;
	Game_Vector AmbColor; 
	Game_Vector SpcColor; 
	Game_Vector EmvColor; 
	float SpecularPower;		
	float TransparencyFactor;	
	float Shininess;			
	Game_String DifTexture;	// 디퓨즈 
	// DifTexture 이 녀석이 여러분들이 일반적으로 알고 있는 색깔이라는 것을 표현해주는 디퓨즈 텍스처이다.
	Game_String BmpTexture; // 범프 후일 설명
	Game_String SpcTexture; // 스펙큘러 후일 설명

public:
	HFBXMATDATA() {}
	~HFBXMATDATA() {}
};

class HFBXIW
{
public:
	int Index;
	double Weight;
};

// 134 
// 40 40 40 14 

class HMESH;
class HVBBUFFER;
class HIBBUFFER;
class HRENDERPIPELINE;
class HFBX : public HFILEMGR<HFBX>
{
public:
	static Game_Ptr<HFBX> Load(const HGAMEFILE& _File)
	{
		Game_Ptr<HFBX> Res = CreateToInsertToFile(_File.FullPath());
		Res->Load();
		return Res;
	}


	// 
	static void LoadToUserFormat(const HGAMEFILE& _File, const Game_String& _UserPath)
	{
		Game_Ptr<HFBX> Res = CreateToInsertToFile(_File.FullPath());
		Res->LoadUserFormat(_UserPath);
		return;
	}

private:
	// 1000
	// 0010
	// 0100
	// 0001

	fbxsdk::FbxAMatrix ReMat;
	fbxsdk::FbxManager* m_Mgr; // 가장 기본이 되는 fbx 시스템의 권한.
	fbxsdk::FbxIOSettings* m_Ios; // fbx 파일의 io시스템을 정의하는 항목 fbx다 멀티 플랫폼이기 때문에 아마 이렇게 지원할거라 예상한다.
	fbxsdk::FbxScene* m_Scene; // 3d맥스 편집에서 가장 기본적인 씬을 정의하는 녀석.
	fbxsdk::FbxImporter* m_Imp; // 이녀석은 fbx를 로드하는 실질적인 객체
	fbxsdk::FbxAxisSystem eLocalSpaceType; // 기본 기저백터의 기본적인 방향을 알아낸다.
	// 1 0 0 0
	// 0 1 0 0
	// 0 0 -1 0
	// 0 0 0 1

	// 스탠다드 라이브러리죠?
	// std::string

public:

	// STATIC MESH
	// 보통 그런경우는 없다. 그런데 있다면.
	// 정점그룹의 모임    
	// 이녀석이 여러개일때도 있고
	std::vector<       std::vector<VTX3DMESHIN>          > m_FbxVtx; // 버텍스

	// 이걸로 나눠놨다.
	// std::vector<std::vector<UINT>>
	// 그리고 인덱스 버퍼가 4개라면 재질 정보도 4개이다.
	std::vector<       std::vector<std::vector<UINT>>    > m_FbxIdx; // 인덱스

	// 이 데이터들은 우리 구조로 변환시켜야 하기는 하지만.

	// 우리구조로 변환시킨 녀석들
	std::vector<Game_Ptr<HVBBUFFER>> m_VB;
	std::vector<std::vector<Game_Ptr<HIBBUFFER>>> m_IB;

	// 메테리얼
	std::vector<HFBXMATDATA> m_UserMatData;
	// STATIC MESH END

	// 랜더 파이프라인이 된다. 랜더파이프라인에 들어가는 텍스처 샘플러 컬러
	// 어떠한 정점에 곱해진다.

	std::map<Game_String, HFBXANIDATA*> m_UserAniDataMap; // 애니메이션 

	std::map<Game_String, HFBXBONEDATA*> m_MapBoneData;



	std::vector<Game_Ptr<HMESH>> m_Mesh;
	std::vector<HFBXANIDATA> m_UserAniData; // 애니메이션 
	std::vector<HFBXBONEDATA> m_UserBoneData; // 본



	// 애니메이션 가중치 및 인덱스 정리용 자료구조.
	// 잠깐쓰고 안쓴다.
	// 이녀석이 매쉬가 여러개 있을때를 대비한 것인데. Root가 여러개 있을때를 대비한 것인데.
	// 
	std::vector<std::map<int, std::vector<HFBXIW>>> m_MapWI;

public:
	size_t UserAniDataSize()
	{
		return m_UserAniData.size();
	}

	Game_String UserAniDataName(size_t _Index)
	{
		return m_UserAniData[_Index].AniName;
	}


	// 요것들이 계산해서 나온 결과물 들이다.
	// 다시 계산안하려면?
	// 클래스를 그냥저장할수는 없다.

	// std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx; // 버텍스
	// std::vector<std::vector<std::vector<UINT>>> m_FbxIdx; // 인덱스
	// std::vector<HFBXBONEDATA> m_UserBoneData; // 본
	// std::vector<HFBXANIDATA> m_UserAniData; // 애니메이션 
	// std::vector<HFBXMATDATA> m_UserMatData;

public:
	HFBX();
	~HFBX();

public:
	void Load();
	void LoadUserFormat(const Game_String& _Path);
	void SaveUserFormat(const Game_String& _Path);

public:
	HFBXANIDATA* FindAniData(const Game_String& _Name)
	{
		if (m_UserAniDataMap.end() == m_UserAniDataMap.find(_Name))
		{
			return nullptr;
		}

		return m_UserAniDataMap[_Name];
	}

private:

	void AniCheck();
	void BoneCheck();
	void Trangulate(fbxsdk::FbxNode* _Node, int& _MatCount);
	// 랜더파이프라인을 만들때 필요한 정보이다.
	// 보통 메테리얼이라고 한다.
	// 여기서 얻어지는 메테리얼의 개수는 보통
	// 정점집합이 나뉘어져있는 서브셋개수와 동일하다.


	// 본 관련함수
	void LoadBoneCount(fbxsdk::FbxNode* _Node, int& _Count);
	void LoadBone(fbxsdk::FbxNode* _Node, int& _iIndex, int _iDepth = 0, int _iParent = -1);

	// 정점 관련 함수.
	void MeshVtxData(fbxsdk::FbxNode* _Node);
	void VtxData(fbxsdk::FbxMesh* _Mesh);

	int MeshCount;
	void AnimationClipCheck(fbxsdk::FbxNode* _Node);
	void CalAnimationClip(fbxsdk::FbxMesh* _Mesh);
	void CalWeightAndIndex();


	void LoadNormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadTangent(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadBinormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);


	void LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx);
	void LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone);
	void LoadTimeTransForm(fbxsdk::FbxNode* _Node, fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone);


	// std::map<Game_String, HFBXBONEDATA*> m_MapBoneData;

	HFBXBONEDATA* FindBone(const Game_String& _Name);

	// 메테리얼 관련 함수.
	void MaterialCheck(fbxsdk::FbxNode* _Node, int& _Count);
	Game_Vector MaterialColor(FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName);
	float MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName);
	Game_String MaterialTex(FbxSurfaceMaterial* pMtrl, const char* _FactorName);


	static HMATRIX FbxMatToHMAT(const fbxsdk::FbxAMatrix& _BaseTrans);
	static Game_Vector FbxVecToHVEC(const fbxsdk::FbxVector4& _BaseVector);
	static Game_Vector FbxQToHVEC(const fbxsdk::FbxQuaternion& _BaseQ);


public:
	void CreateMesh();
};

