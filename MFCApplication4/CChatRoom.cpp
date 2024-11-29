// CChatRoom.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CChatRoom.h"
#include "MainFrm.h"
#include <windows.h>


// CChatRoom 대화 상자

IMPLEMENT_DYNAMIC(CChatRoom, CDialogEx)

CChatRoom::CChatRoom(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_CHATROOM, pParent)
    , m_strSend(_T(""))
{

}

CChatRoom::~CChatRoom()
{
}

void CChatRoom::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_SEND, m_sendbutton);
    DDX_Control(pDX, IDC_EDIT_SEND, m_edit_send);
    DDX_Control(pDX, IDC_LIST_CHATING, m_List_chating);
    DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
}



BEGIN_MESSAGE_MAP(CChatRoom, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatRoom::OnBnClickedButtonSend)
    ON_MESSAGE(UM_RECEIVE, OnReceive)
END_MESSAGE_MAP()


// CChatRoom 메시지 처리기


LPARAM CChatRoom::OnReceive(WPARAM wParam, LPARAM lParam) {

    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString chatnameDlg = pMainFrame->chatname;

    UpdateData(TRUE);
    char pTmp[256];
    CString strTmp;
    memset(pTmp, '\0', sizeof(pTmp));

    // 데이터를 소켓으로부터 수신
    m_socCom.Receive(pTmp, sizeof(pTmp));
    strTmp = CString(CA2T(pTmp, CP_UTF8));

    //case 구분
    //case 구분
    int firstPipe = strTmp.Find(_T("|")); // 첫 번째 '|' 위치
    int secondPipe = strTmp.Find(_T("|"), firstPipe + 1); // 두 번째 '|' 위치
    int thirdPipe = strTmp.Find(_T("|"), secondPipe + 1); // 세 번째 '|' 위치
    int fourthPipe = strTmp.Find(_T("|"), thirdPipe + 1); // 네 번째 '|' 위치
    int fifthPipe = strTmp.Find(_T("|"), fourthPipe + 1); // 네 번째 '|' 위치

    // 첫 번째 값
    CString firstValue = strTmp.Left(firstPipe).Trim();
    // 두 번째 값
    CString secondValue = strTmp.Mid(firstPipe + 1, secondPipe - firstPipe - 1).Trim();
    // 세 번째 값
    CString thirdValue = strTmp.Mid(secondPipe + 1, thirdPipe - secondPipe - 1).Trim();

    CString fourthValue = strTmp.Mid(thirdPipe + 1, fourthPipe - thirdPipe - 1).Trim();

    CString fifthValue = strTmp.Mid(fourthPipe + 1).Trim();

    // CString을 int로 변환
    int switchValue = _ttoi(firstValue); // CString -> 정수 변환

    switch (switchValue)
    {
    case 0:
        if (fifthValue == chatnameDlg)
            case1(secondValue, thirdValue, fourthValue);
        break;
    case 1:
        case1(secondValue, thirdValue, fourthValue);
        break;
    }


    UpdateData(FALSE);
    return TRUE;
}



BOOL CChatRoom::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_strIP = _T("192.168.213.117");
    UpdateData(TRUE);
    m_socCom.Create();
    m_socCom.Connect(m_strIP, 5000);
    m_socCom.Init(this->m_hWnd);

    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString username = pMainFrame->m_strUserName;
    CString chatnameDlg = pMainFrame->chatname;
    UpdateData(TRUE);
    char pTmp[256];
    CString strTmp;
    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));
    //채팅방 정보 추가
    strTmp.Format(_T("1|%s|%s|%s|%s"), chatnameDlg, NULL, NULL, NULL);
    // CString을 UTF-8로 변환
    CW2A strSendA(strTmp, CP_UTF8);
    strncpy_s(pTmp, sizeof(pTmp), strSendA, _TRUNCATE);
    // 데이터를 소켓을 통해 전송
    m_socCom.Send(pTmp, sizeof(pTmp));
    UpdateData(FALSE);
    return TRUE;  // 성공 시 TRUE 반환

}



void CChatRoom::OnBnClickedButtonSend()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString username = pMainFrame->m_strUserName;
    CString chatnameDlg = pMainFrame->chatname;
    UpdateData(TRUE);
    char pTmp[256];
    CString strTmp;
    CString text;
    GetDlgItemText(IDC_EDIT_SEND, text);
    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));
    //닉네임과 채팅방 정보 추가
    strTmp.Format(_T("0|%s|%s|%s|%s"), chatnameDlg, username, text, NULL);
    // CString을 UTF-8로 변환
    CW2A strSendA(strTmp, CP_UTF8);
    strncpy_s(pTmp, sizeof(pTmp), strSendA, _TRUNCATE);
    m_strSend.Empty();

    // 데이터를 소켓을 통해 전송
    m_socCom.Send(pTmp, sizeof(pTmp));
    UpdateData(FALSE);
}



void CChatRoom::case1(CString secondValue, CString thirdValue, CString fourthValue)
{
    // TODO: 여기에 구현 코드 추가.
    CString table;
    table.Format(_T("%s | %s | %s"), secondValue, thirdValue, fourthValue);
    int i = m_List_chating.GetCount();
    m_List_chating.InsertString(i, table);
}


void CChatRoom::case0(CString secondValue)
{
    // TODO: 여기에 구현 코드 추가.
    int i = m_List_chating.GetCount();
    m_List_chating.InsertString(i, secondValue);
}
