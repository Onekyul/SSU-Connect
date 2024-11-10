// CLogInDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CLogInDlg.h"
#include "CRegisterDlg.h"


// CLogInDlg 대화 상자

IMPLEMENT_DYNAMIC(CLogInDlg, CDialogEx)

CLogInDlg::CLogInDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{

}

CLogInDlg::~CLogInDlg()
{
}

void CLogInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogInDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLogInDlg::OnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CLogInDlg::OnClickedButtonRegister)
	ON_EN_CHANGE(IDC_EDIT_PW, &CLogInDlg::OnChangeEditPw)
END_MESSAGE_MAP()


// CLogInDlg 메시지 처리기


void CLogInDlg::OnClickedButtonLogin()
{
	
}


void CLogInDlg::OnClickedButtonRegister()
{
	CRegisterDlg Register;
	if (Register.DoModal() == IDOK) {
		//DB에 테이블 추가 및 ----
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CLogInDlg::OnChangeEditPw()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
