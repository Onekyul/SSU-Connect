// CCheckDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CCheckDlg.h"
#include "MFCApplication4Doc.h"
#include "MainFrm.h"

// CCheckDlg 대화 상자

IMPLEMENT_DYNAMIC(CCheckDlg, CDialogEx)

CCheckDlg::CCheckDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CKPW, pParent)
{

}

CCheckDlg::~CCheckDlg()
{
}

void CCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCheckDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCheckDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCheckDlg 메시지 처리기


void CCheckDlg::OnBnClickedOk()

{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CString m_strId = pMainFrame->m_strUserId;
	CString m_strPw = pMainFrame->m_strUserPw;

	CString m_strCkpw;
	GetDlgItemText(IDC_EDIT_CKPW, m_strCkpw);
	
	if (m_strCkpw == m_strPw) {
		if (AfxMessageBox(_T("정말 회원탈퇴하시겠습니까?"), MB_YESNO) == IDYES) {
			CString query;
			query.Format(_T("DELETE FROM user WHERE user_id='%s' AND user_pw='%s'"), pMainFrame->m_strUserId, pMainFrame->m_strUserPw);

		}
		EndDialog(IDYES);
	}
	else {
		MessageBox(_T("잘못된 비밀번호 입력입니다."));
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CDialogEx::OnOK();
}
