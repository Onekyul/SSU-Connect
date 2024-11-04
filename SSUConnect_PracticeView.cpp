
// SSUConnect_PracticeView.cpp: CSSUConnectPracticeView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "SSUConnect_Practice.h"
#endif

#include "SSUConnect_PracticeDoc.h"
#include "SSUConnect_PracticeView.h"
#include "CLoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSSUConnectPracticeView

IMPLEMENT_DYNCREATE(CSSUConnectPracticeView, CView)

BEGIN_MESSAGE_MAP(CSSUConnectPracticeView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSSUConnectPracticeView 생성/소멸

CSSUConnectPracticeView::CSSUConnectPracticeView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	CLoginDlg* dlg1 = new CLoginDlg;
	dlg1->DoModal();
}

CSSUConnectPracticeView::~CSSUConnectPracticeView() {
	delete pButton;
}

BOOL CSSUConnectPracticeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CSSUConnectPracticeView 그리기

void CSSUConnectPracticeView::OnDraw(CDC* pDC)
{
	CSSUConnectPracticeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	pButton->ShowWindow(SW_SHOW);
}


// CSSUConnectPracticeView 인쇄

BOOL CSSUConnectPracticeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CSSUConnectPracticeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CSSUConnectPracticeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CSSUConnectPracticeView 진단

#ifdef _DEBUG
void CSSUConnectPracticeView::AssertValid() const
{
	CView::AssertValid();
}

void CSSUConnectPracticeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSSUConnectPracticeDoc* CSSUConnectPracticeView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSSUConnectPracticeDoc)));
	return (CSSUConnectPracticeDoc*)m_pDocument;
}
#endif //_DEBUG


// CSSUConnectPracticeView 메시지 처리기


void CSSUConnectPracticeView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	pButton = new CButton();
	pButton->Create(_T("Test"), BS_DEFPUSHBUTTON, CRect(0, 0, 200, 50), this, 100);
}
