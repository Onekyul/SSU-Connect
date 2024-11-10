// MainFrm.cpp: CMainFrame 클래스의 구현

#include "pch.h"
#include "framework.h"
#include "MFCApplication4.h"
#include "Resource.h"
#include "MainFrm.h"
#include "CLogInDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CMainFrame::OnLogoutClicked)
    ON_BN_CLICKED(IDC_BUTTON_FRIENDS, &CMainFrame::OnFriendsClicked)
    ON_BN_CLICKED(IDC_BUTTON_CHATROOMS, &CMainFrame::OnChatRoomsClicked)
    ON_BN_CLICKED(IDC_BUTTON_SETTINGS, &CMainFrame::OnSettingsClicked)
    ON_WM_PAINT()
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // 상태 줄 표시기
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
    : m_currentScreen(SCREEN_NONE)  // 초기 화면 상태 설정
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    SetMenu(NULL);

    // 상단 프로필 이름 생성
    if (!m_profileName.Create(_T("닉네임"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(10, 10, 100, 30), this, IDC_STATIC_PROFILE))
    {
        TRACE0("프로필 이름 컨트롤을 생성하지 못했습니다.\n");
        return -1;
    }

    // 로그아웃 버튼 생성
    if (!m_logoutButton.Create(_T("로그아웃"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(250, 10, 320, 30), this, IDC_BUTTON_LOGOUT))
    {
        TRACE0("로그아웃 버튼을 생성하지 못했습니다.\n");
        return -1;
    }

    // 하단 네비게이션 버튼들 생성
    if (!m_friendsButton.Create(_T("친구"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(10, 400, 80, 440), this, IDC_BUTTON_FRIENDS))
    {
        TRACE0("친구 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    if (!m_chatRoomsButton.Create(_T("채팅방"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(100, 400, 180, 440), this, IDC_BUTTON_CHATROOMS))
    {
        TRACE0("채팅방 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    if (!m_settingsButton.Create(_T("설정"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(200, 400, 280, 440), this, IDC_BUTTON_SETTINGS))
    {
        TRACE0("설정 버튼을 생성하지 못했습니다.\n");
        return -1;
    }

    return 0;
}

// PreCreateWindow 함수 정의
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}

// 각 버튼 클릭 이벤트 핸들러
void CMainFrame::OnLogoutClicked()
{
    AfxMessageBox(_T("로그아웃 되었습니다."));
}

void CMainFrame::OnFriendsClicked()
{
    RemoveViews();
    m_currentScreen = SCREEN_FRIENDS;  // 친구 목록 화면으로 설정
    Invalidate();  // 화면 갱신
}

void CMainFrame::OnChatRoomsClicked()
{
    RemoveViews();
    m_currentScreen = SCREEN_CHATROOMS;  // 채팅방 화면으로 설정
    Invalidate();  // 화면 갱신
}

void CMainFrame::OnSettingsClicked()
{
    RemoveViews();
    m_currentScreen = SCREEN_SETTINGS;  // 설정 화면으로 설정
    Invalidate();  // 화면 갱신
}

// OnPaint 함수에서 화면 상태에 맞는 내용을 그립니다.
void CMainFrame::OnPaint()
{
    CPaintDC dc(this); // 그리기 장치를 위한 DC

    switch (m_currentScreen)
    {
    case SCREEN_FRIENDS:
        dc.TextOutW(100, 100, _T("친구 목록 화면"));
        break;

    case SCREEN_CHATROOMS:
        dc.TextOutW(100, 100, _T("채팅방 화면"));
        break;

    case SCREEN_SETTINGS:
        dc.TextOutW(100, 100, _T("설정 화면"));
        if (!m_editProfileButton.GetSafeHwnd())
        {
            m_editProfileButton.Create(_T("프로필 설정"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(100, 150, 200, 180), this, 1001);
            m_deleteAccountButton.Create(_T("회원 탈퇴"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(100, 200, 200, 230), this, 1002);
        }
        break;

    default:
        dc.TextOutW(100, 100, _T("기본 화면"));
        break;
    }
}




void CMainFrame::RemoveViews()
{
    // TODO: 여기에 구현 코드 추가.
    if (m_editProfileButton.GetSafeHwnd())
    {
        m_editProfileButton.DestroyWindow(); // 첫 번째 버튼 제거
    }
    if (m_deleteAccountButton.GetSafeHwnd())
    {
        m_deleteAccountButton.DestroyWindow(); // 두 번째 버튼 제거
    }
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CFrameWnd::OnShowWindow(bShow, nStatus);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    if (bShow)  // 창이 처음 보일 때만 실행
    {
        static bool isDialogShown = false;
        if (!isDialogShown)
        {
            isDialogShown = true;
            CLogInDlg login;
            if (login.DoModal() == IDOK && m_bLogIn)
            {

                //DB확인 if문

            }
        }
    }
}
