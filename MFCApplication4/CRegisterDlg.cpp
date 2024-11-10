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
END_MESSAGE_MAP()


// CRegisterDlg 메시지 처리기
