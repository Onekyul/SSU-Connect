// MainFrm.cpp: CMainFrame 클래스의 구현

#include "pch.h"
#include "framework.h"
#include "MFCApplication4.h"
#include "Resource.h"
#include "MainFrm.h"
#include "CLogInDlg.h"
#include "CCheckDlg.h"
#include "CChangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CMainFrame::OnLogoutClicked)
    ON_BN_CLICKED(IDC_BUTTON_FRIENDS, &CMainFrame::OnFriendsClicked)
    ON_BN_CLICKED(IDC_BUTTON_CHATROOMS, &CMainFrame::OnChatRoomsClicked)
    ON_BN_CLICKED(IDC_BUTTON_SETTINGS, &CMainFrame::OnSettingsClicked)
    ON_BN_CLICKED(IDC_BUTTON_EDIT, &CMainFrame::OnEditProfileClicked)
    ON_BN_CLICKED(IDC_BUTTON_WITHDRAW,&CMainFrame::OnDeleteAccountClicked)
    ON_WM_PAINT()
    ON_WM_SHOWWINDOW()
    ON_WM_SIZE()
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
    if (!m_profileName.Create(_T("이름"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(10, 10, 100, 30), this, IDC_STATIC_PROFILE))
    {   
        TRACE0("프로필 이름 컨트롤을 생성하지 못했습니다.\n");
        return -1;
    }
        

    // 로그아웃 버튼 생성
    if (!m_logoutButton.Create(_T("로그아웃"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(280, 10, 370, 30), this, IDC_BUTTON_LOGOUT))
    {
        TRACE0("로그아웃 버튼을 생성하지 못했습니다.\n");
        m_logoutButton.MoveWindow(280, 5, 100, 20);
        return -1;
    }

    // 하단 네비게이션 버튼들 생성
    if (!m_friendsButton.Create(_T("친구"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 500, 125, 550), this, IDC_BUTTON_FRIENDS))
    {
        TRACE0("친구 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    if (!m_chatRoomsButton.Create(_T("채팅방"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(125, 500, 255, 550), this, IDC_BUTTON_CHATROOMS))
    {
        TRACE0("채팅방 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    if (!m_settingsButton.Create(_T("설정"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(255, 500, 380, 550), this, IDC_BUTTON_SETTINGS))
    {
        TRACE0("설정 버튼을 생성하지 못했습니다.\n");
        return -1;
    }

    SetWindowPos(NULL, 100, 100, 400, 600, SWP_NOZORDER | SWP_NOACTIVATE);

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
    DestroyWindow();
}

void CMainFrame::OnFriendsClicked()
{
    m_currentScreen = SCREEN_FRIENDS;
    RemoveViews();








    Invalidate();  // 화면 갱신
}

void CMainFrame::OnChatRoomsClicked()
{
    m_currentScreen = SCREEN_CHATROOMS;  // 채팅방 화면으로 설정
    RemoveViews();
    Invalidate();  // 화면 갱신
}

void CMainFrame::OnSettingsClicked()
{
    m_currentScreen = SCREEN_SETTINGS;
    RemoveViews();
    Invalidate();

}
void CMainFrame::OnEditProfileClicked() {
    CChangeDlg ChangePW;

    if (ChangePW.DoModal() == IDOK) {

    }
    
}

void CMainFrame::OnDeleteAccountClicked() {
    CCheckDlg CheckPW;

    if (CheckPW.DoModal() == IDOK) {

    }

}

// OnPaint 함수에서 화면 상태에 맞는 내용을 그립니다.
void CMainFrame::OnPaint()
{


    CPaintDC dc(this); // 그리기 장치를 위한 DC

    dc.MoveTo(0, 30);
    dc.LineTo(400, 30);

    dc.MoveTo(0, 500);
    dc.LineTo(400, 500);
    CString Profile_name;
    Profile_name.Format(_T("%s"), m_strUserName);
    dc.TextOutW(110, 10, Profile_name);

    switch (m_currentScreen)
    {
    case SCREEN_FRIENDS:
        dc.TextOutW(100, 100, _T("친구 목록 화면"));
        break;

    case SCREEN_CHATROOMS:
        dc.TextOutW(100, 100, _T("채팅방 화면"));
        break;

    case SCREEN_SETTINGS:
        dc.TextOutW(120, 150, _T("프로필 사진"));
        dc.TextOutW(120, 300, _T("사용자 이름"));
        

        // 회원정보 변경 버튼 생성
        if (!m_editProfileButton.GetSafeHwnd())
        {
            m_editProfileButton.Create(_T("회원정보 변경"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(120, 350, 240, 380), this, IDC_BUTTON_EDIT);
        }

        // 로그아웃 버튼 생성
        if (m_logoutButton.GetSafeHwnd())
        {
            m_logoutButton.MoveWindow(120, 385, 120, 30);
        }

        // 회원탈퇴 버튼 생성
        if (!m_deleteAccountButton.GetSafeHwnd())
        {
            m_deleteAccountButton.Create(_T("회원탈퇴"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(120, 420, 240, 450), this, IDC_BUTTON_WITHDRAW);
        }
        break;

    default:
        dc.TextOutW(100, 100, _T("기본 화면"));
        break;
    }
}




void CMainFrame::RemoveViews()
{
    if (m_currentScreen != SCREEN_SETTINGS) {
        m_logoutButton.MoveWindow(280, 5, 100, 20);
    }
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

    MYSQL Conn;
    MYSQL* ConnPtr = NULL;
    
    int Stat;

  
    //ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    //mysql_query(ConnPtr, "set session character_set_connection=euckr;");
    //mysql_query(ConnPtr, "set session character_set_results=euckr;");
    //mysql_query(ConnPtr, "set session character_set_client=euckr;");

   
    

    if (bShow)  // 창이 처음 보일 때만 실행
    {
        static bool isDialogShown = false;
        if (!isDialogShown)
        {
            isDialogShown = true;
            CLogInDlg login;
            if (login.DoModal() == IDOK && m_bLogIn)
            {
                SetUserInfo(login.m_strId, login.m_strPw, login.m_strName);
                //DB확인 if문

            }
        }   
    }
}   


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    if (cx != 400 || cy != 600)  // 설정한 고정 크기
    {
        SetWindowPos(NULL, 0, 0, 400, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CMainFrame::SetUserInfo(CString Id, CString Pw, CString Name)
{
    m_strUserId = Id;
    m_strUserPw = Pw;
    m_strUserName = Name;
    // TODO: 여기에 구현 코드 추가.
}
