// CRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CRegisterDlg.h"


// CRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REGISTER, pParent)
{

}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRegisterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRegisterDlg 메시지 처리기


void CRegisterDlg::OnBnClickedOk()
{
		CString m_StrRegi_name;
		CString m_StrRegi_id;
		CString m_StrRegi_pw;

		GetDlgItemText(IDC_EDIT_REGI_NAME, m_StrRegi_name);
		GetDlgItemText(IDC_EDIT_REGI_ID, m_StrRegi_id);
		GetDlgItemText(IDC_EDIT_REGI_PW, m_StrRegi_pw);

	
		
		
		CString Query;
		
		Query.Format(
			_T("INSERT INTO user (user_id, user_pw, user_name,user_friend) VALUES('%s', '%s', '%s','')"),m_StrRegi_name,
				m_StrRegi_id,
				m_StrRegi_pw
		
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
	
		Stat = mysql_query(ConnPtr, queryChar);
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.*/
	CDialogEx::OnOK();
}


