// HLeftView.cpp: 구현 파일
//

#include "pch.h"
#include "HTOOL.h"
#include "HLeftView.h"
#include <HGAMEWINDOW.h>
#include <Game_Device.h>
#include <GAMELOGIC.h>

#pragma comment(lib, "HGAMEBASE.lib")
#pragma comment(lib, "HGAMEENGINE.lib")
#pragma comment(lib, "HGAMELOGIC.lib")


// HLeftView

IMPLEMENT_DYNCREATE(HLeftView, CFormView)

HLeftView::HLeftView()
	: CFormView(IDD_HLeftView)
{

}

HLeftView::~HLeftView()
{
}

void HLeftView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(HLeftView, CFormView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// HLeftView 진단

#ifdef _DEBUG
void HLeftView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void HLeftView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// HLeftView 메시지 처리기


int HLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;


	HGAMEWINDOW::Create(L"DEFWINDOW", m_hWnd);
	HGAMEWINDOW::MAINOBJ()->Size({ 1280, 720 });
	Game_Device::Create(L"DEFWINDOW");


	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
