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
    m_rCount = 0;
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
    ON_MESSAGE(UM_ACCEPT, &CChatRoom::OnAccept)
    ON_MESSAGE(UM_RECEIVE, OnReceive)
END_MESSAGE_MAP()


// CChatRoom 메시지 처리기

LPARAM CChatRoom::OnAccept(WPARAM wParam, LPARAM lParam) {
    try {
        int tmp = m_socServer.m_index.front();

        CString number;
        number.Format(_T("%d"), tmp);

        m_socCom[tmp] = new CSocCom();
        m_socCom[tmp] = m_socServer.GetAcceptSocCom();

        m_socServer.m_index.pop_front();
        m_using.push_back(tmp);

        m_socCom[tmp]->m_index = tmp;
        m_socCom[tmp]->Init(this->m_hWnd);

        // CString을 UTF-8로 변환 후 전송
        //CString connectMessage = SOC_CLIENT_CONNECT + number;
        //CStringA connectMessageA(CW2A(connectMessage, CP_UTF8));
        //m_socCom[tmp]->Send(connectMessageA, 256);

        UpdateData(FALSE);
        return TRUE;
    }
    catch (CException* ex) {
        ex->ReportError();
        return FALSE;
    }
}



LPARAM CChatRoom::OnReceive(WPARAM wParam, LPARAM lParam) {

    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString chatnameDlg = pMainFrame->chatname;

    char pTmp[256];
    CString strTmp;
    memset(pTmp, '\0', sizeof(pTmp));

    // 데이터를 소켓으로부터 수신
    m_socCom[wParam]->Receive(pTmp, sizeof(pTmp));

    // 수신된 데이터를 UTF-8에서 유니코드로 변환
    strTmp = CString(CA2T(pTmp, CP_UTF8));

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

    int switchValue = _ttoi(firstValue); // CString -> 정수 변환

    switch (switchValue)
    {
    case 0:
        case0(secondValue, thirdValue, fourthValue);
        break;
    case 1:
        case1(secondValue, wParam);
        break;
    }

    return TRUE;

}




BOOL CChatRoom::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        m_socServer.m_index.push_back(i);
    }
    CString ipAddress = _T("10.21.30.220"); // 원하는 IP 주소로 변경
    UINT port = 5000; // 원하는 포트 번호로 설정

    m_socServer.Create(port);
    m_socServer.Bind(port, ipAddress);
    m_socServer.Listen();
    m_socServer.Init(this->m_hWnd);

    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString chatnameDlg = pMainFrame->chatname;
    CString query;
    query.Format(_T("SELECT * FROM `%s`"), chatnameDlg);

    //CT2A asciiQuery(Query); // CString to ASCII
    CW2A utfQuery(query, CP_UTF8);
    char* queryChar = utfQuery;

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
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString username = pMainFrame->m_strUserName;
    CString chatnameDlg = pMainFrame->chatname;
    CString text;
    GetDlgItemText(IDC_EDIT_SEND, text);
    char pTmp[256];
    UpdateData(TRUE);
    //수신한 내용을 DB에 INSERT
    CString Query;
    Query.Format(
        _T("INSERT INTO `%s` (name, message) VALUES ('%s', '%s')"),
        chatnameDlg, username, text);
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;
    MYSQL Conn;
    mysql_init(&Conn);

    // MySQL 서버 연결
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }

    // SQL 쿼리 실행
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    //SELECT 쿼리 
    Query.Format(_T("SELECT * FROM `%s` ORDER BY chat_time DESC LIMIT 1"), chatnameDlg);
    CW2A utfQuery2(Query, CP_UTF8);
    char* queryChar2 = utfQuery2;

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }

    // SQL 쿼리 실행
    Stat = mysql_query(ConnPtr, queryChar2);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
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
            CW2A utfrowData(rowData, CP_UTF8);
            int i = m_List_chating.GetCount();
            m_List_chating.InsertString(i, rowData);
            strncpy_s(pTmp, sizeof(pTmp), utfrowData, _TRUNCATE);
            //
            CString text;
            text.Format(_T("0|%s|%s"), rowData, chatnameDlg);
            CW2A utftext(text, CP_UTF8);
            strncpy_s(pTmp, sizeof(pTmp), utftext, _TRUNCATE);


            //클라이언트 전체에 전송
            for each (int j in m_using) {
                m_socCom[j]->Send(pTmp, sizeof(pTmp));
            }
        }
        m_strSend.Empty();
        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 결과가 없을 경우 FALSE 반환
    }
    UpdateData(FALSE);
    // 연결 닫기
    mysql_close(ConnPtr);
}






void CChatRoom::case0(CString secondValue, CString thirdValue, CString fourthValue)
{
    // TODO: 여기에 구현 코드 추가
     // 변수에 값 저장
    CString chatname = secondValue;  // 첫 번째 값
    CString name = thirdValue;   // 두 번째 값
    CString strTmp = fourthValue; // 세 번째 값
    char pTmp[256];
    memset(pTmp, '\0', sizeof(pTmp));

    // MySQL 서버 연결
    MYSQL Conn;
    mysql_init(&Conn);
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }

    //수신한 내용을 DB에 INSERT
    CString Query;
    Query.Format(
        _T("INSERT INTO `%s` (name, message) VALUES ('%s', '%s')"),
        chatname, name, strTmp);
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    //SELECT 쿼리 
    Query.Format(_T("SELECT * FROM `%s` ORDER BY chat_time DESC LIMIT 1"), chatname);
    CW2A utfQuery2(Query, CP_UTF8);
    char* queryChar2 = utfQuery2;
    Stat = mysql_query(ConnPtr, queryChar2);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
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
            CW2A utfrowData(rowData, CP_UTF8);
            int i = m_List_chating.GetCount();
            m_List_chating.InsertString(i, rowData);

            CString text;
            text.Format(_T("0|%s|%s"), rowData, chatname);
            CW2A utftext(text, CP_UTF8);
            strncpy_s(pTmp, sizeof(pTmp), utftext, _TRUNCATE);


            //클라이언트 전체에 전송
            for each (int j in m_using) {
                m_socCom[j]->Send(pTmp, sizeof(pTmp));
            }
        }
        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 결과가 없을 경우 FALSE 반환
    }
    // 연결 닫기
    mysql_close(ConnPtr);
}


void CChatRoom::case1(CString secondValue, WPARAM wParam)
{
    // TODO: 여기에 구현 코드 추가.
    char pTmp[256];
    memset(pTmp, '\0', sizeof(pTmp));
    // MySQL 서버 연결
    MYSQL Conn;
    mysql_init(&Conn);
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }


    // SQL 쿼리 실행

    CString Query;
    Query.Format(_T("SELECT * FROM `%s`"), secondValue);
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
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
            rowData += _T("1|");
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

            CString Id;
            Id.Format(_T("%d"), wParam);

            CString edit;
            edit.Format(_T("%s|nulldata"), rowData);
            CW2A utfrowData(edit, CP_UTF8);
            strncpy_s(pTmp, sizeof(pTmp), utfrowData, _TRUNCATE);


            //해당 클라이언트에 전송
            for each (int j in m_using) {
                if (j == _ttoi(Id))
                    m_socCom[j]->Send(pTmp, sizeof(pTmp));
            }
        }

        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 결과가 없을 경우 FALSE 반환
    }

    // 연결 닫기
    mysql_close(ConnPtr);

}
