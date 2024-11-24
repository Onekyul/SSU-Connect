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
        CString connectMessage = SOC_CLIENT_CONNECT + number;
        CStringA connectMessageA(CW2A(connectMessage, CP_UTF8));
        m_socCom[tmp]->Send(connectMessageA, 256);

        UpdateData(FALSE);
        return TRUE;
    }
    catch (CException* ex) {
        ex->ReportError();
        return FALSE;
    }
}



LPARAM CChatRoom::OnReceive(WPARAM wParam, LPARAM lParam) {
    CString Query;
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString username = pMainFrame->m_strUserName;
    CString chatnameDlg = pMainFrame->chatname;
    char pTmp[256];
    CString strTmp;
    memset(pTmp, '\0', sizeof(pTmp));

    // 데이터를 소켓으로부터 수신
    m_socCom[wParam]->Receive(pTmp, sizeof(pTmp));

    // 수신된 데이터를 UTF-8에서 유니코드로 변환
    strTmp = CA2W(pTmp, CP_UTF8);

    if (strTmp.Compare(SOC_CLIENT_DISCONNECT) == 0) {
        m_socServer.m_socCom[wParam].Close();
        m_socCom[wParam]->Close();
        m_socServer.m_index.push_back(wParam);
        m_using.erase(std::remove(m_using.begin(), m_using.end(), wParam), m_using.end());
    }
    else {
        CString id;
        id.Format(_T("%d"), wParam);

        // 변환된 데이터를 목록에 추가
        int i = m_List_chating.GetCount();
        CString displayStr;
        displayStr.Format(_T("사용자%s : %s"), id, strTmp);
        m_List_chating.InsertString(i, displayStr);

        for (int i : m_using) {
            if (i != wParam) {
                CString sendStr;
                sendStr.Format(_T("사용자%s : %s"), id, strTmp);
                CStringA sendStrA(CW2A(sendStr, CP_UTF8));
                m_socCom[i]->Send(sendStrA, 256);
                Query.Format(
                    _T("INSERT INTO `%s` (name, message) VALUES ('%s', '%s')"),
                    chatnameDlg, username, sendStrA
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
        }
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


    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UpdateData(TRUE);
    char pTmp[256];
    CString strTmp;

    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));

    // "관리자: " 메시지를 맨 앞에 추가
    strncpy_s(pTmp, sizeof(pTmp), "관리자 : ", _TRUNCATE);

    // CString -> UTF-8 -> char 배열 복사
    CW2A strSendA(m_strSend, CP_UTF8);
    strcat_s(pTmp, sizeof(pTmp), strSendA);

    m_strSend.Empty();

    // 데이터를 소켓을 통해 전송
    for (int i : m_using) {
        m_socCom[i]->Send(pTmp, sizeof(pTmp));
    }

    // 복사된 데이터를 UTF-8에서 유니코드로 변환하여 목록에 추가
    CStringW strConverted(CA2W(pTmp, CP_UTF8));
    int i = m_List_chating.GetCount();
    m_List_chating.InsertString(i, strConverted);

    UpdateData(FALSE);


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

