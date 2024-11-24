// CChatRoom.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CChatRoom.h"
#include "MainFrm.h"


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
    UpdateData(TRUE);
    char pTmp[256];
    CString strTmp;
    memset(pTmp, '\0', sizeof(pTmp));

    // 데이터를 소켓으로부터 수신
    m_socCom.Receive(pTmp, sizeof(pTmp));

    // 수신된 데이터를 UTF-8에서 유니코드로 변환
    strTmp = CA2W(pTmp, CP_UTF8);

    if (strTmp.Find(SOC_CLIENT_CONNECT) == 0) {
        m_strUserID.Format(_T("사용자 : %s"), strTmp.Right(1));
    }
    else {
        // 변환된 데이터를 목록에 추가
        int i = m_List_chating.GetCount();
        m_List_chating.InsertString(i, strTmp);
    }

    UpdateData(FALSE);
    return TRUE;
}



BOOL CChatRoom::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_strIP = _T("10.21.30.220");
    UpdateData(TRUE);
    m_socCom.Create();
    m_socCom.Connect(m_strIP, 5000);
    m_socCom.Init(this->m_hWnd);

    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString chatnameDlg = pMainFrame->chatname;
    CString query;
    query.Format(_T("SELECT * FROM `%s`"), chatnameDlg);

    CT2A asciiQuery(query); // CString to ASCII
    const char* queryChar = asciiQuery;

    MYSQL Conn;
    mysql_init(&Conn);

    // MySQL 서버 연결
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return FALSE;  // 연결 실패 시 FALSE 반환
    }

    // SQL 쿼리 실행
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return FALSE;  // 쿼리 실패 시 FALSE 반환
    }

    // 결과를 저장
    MYSQL_RES* Result = mysql_store_result(ConnPtr);

    if (Result != NULL) {
        // 결과의 필드 수 가져오기
        int num_fields = mysql_num_fields(Result);
        MYSQL_ROW Row;

        // 결과 행을 반복하여 가져오기
        while ((Row = mysql_fetch_row(Result))) {
            CString rowData;  // 한 행을 저장할 변수
            for (int i = 0; i < num_fields; i++) {
                if (Row[i] != NULL) {
                    // UTF-8 문자열로 변환
                    CString colData = CString(CA2T(Row[i], CP_UTF8));
                    rowData += colData;  // 행 데이터를 결합
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");  // 열 사이에 구분자 추가
                    }
                }
                else {
                    rowData += _T("NULL");
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");
                    }
                }
            }
            // 한 행의 데이터를 리스트에 추가
            m_List_chating.AddString(rowData);
        }

        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return FALSE;  // 결과가 없을 경우 FALSE 반환
    }

    // 연결 닫기
    mysql_close(ConnPtr);

    return TRUE;  // 성공 시 TRUE 반환

}



void CChatRoom::OnBnClickedButtonSend()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UpdateData(TRUE);
    char pTmp[256];
    CString strTmp;

    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));

    // CString을 UTF-8로 변환
    CW2A strSendA(m_strSend, CP_UTF8);
    strncpy_s(pTmp, sizeof(pTmp), strSendA, _TRUNCATE);
    m_strSend.Empty();

    // 데이터를 소켓을 통해 전송
    m_socCom.Send(pTmp, sizeof(pTmp));

    // 복사된 데이터를 UTF-8에서 유니코드로 변환하여 목록에 추가
    CStringW strConverted(CA2W(pTmp, CP_UTF8));
    int i = m_List_chating.GetCount();
    CString combinedStr;
    combinedStr.Format(_T("사용자%d: %s"), myUserId, strConverted);
    m_List_chating.InsertString(i, combinedStr);

    UpdateData(FALSE);
}

