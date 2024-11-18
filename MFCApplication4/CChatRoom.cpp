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
}



BEGIN_MESSAGE_MAP(CChatRoom, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatRoom::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CChatRoom 메시지 처리기


BOOL CChatRoom::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
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
                    CString colData(Row[i]);  // 현재 열 데이터를 CString으로 변환
                    rowData += colData;       // 행 데이터를 결합
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

    return TRUE;  // 성공적으로 초기화되었으므로 TRUE 반환
}



void CChatRoom::OnBnClickedButtonSend()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString Query;
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString username = pMainFrame->m_strUserName;
    CString chatnameDlg = pMainFrame->chatname;
    CString text;
    GetDlgItemText(IDC_EDIT_SEND, text);
    Query.Format(
        _T("INSERT INTO `%s` (name, message) VALUES ('%s', '%s')"),
        chatnameDlg, username, text
    );





    CT2A asciiQuery(Query); // CString to ASCII
    const char* queryChar = asciiQuery;



    MYSQL Conn;
    mysql_init(&Conn);
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
    int Stat = mysql_query(ConnPtr, queryChar);
    if (Stat != 0) {
        MessageBox(NULL, _T("쿼리 오류"), MB_OK);
    }
}
