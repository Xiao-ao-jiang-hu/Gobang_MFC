
// GoBangDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "GoBang.h"
#include "GoBangDlg.h"
#include "afxdialogex.h"
#include"resource.h"
#include <fstream>



#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CGoBangDlg 对话框


CGoBangDlg::CGoBangDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GOBANG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGoBangDlg::DoDataExchange(CDataExchange* pDX)
{
	IsPlaying = false;
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			ChessBoard[i][j] = -1;
		}
	}//初始化棋盘
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGoBangDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CGoBangDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_QUIT, &CGoBangDlg::OnBnClickedQuit)
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENDGAME, &CGoBangDlg::OnBnClickedEndgame)
	ON_BN_CLICKED(IDC_REPENTANCE, &CGoBangDlg::OnBnClickedRepentance)
	ON_BN_CLICKED(IDC_SAVE, &CGoBangDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_OPEN, &CGoBangDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_BUTTON_AI, &CGoBangDlg::OnBnClickedButtonAi)
END_MESSAGE_MAP()


// CGoBangDlg 消息处理程序

BOOL CGoBangDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetBackgroundImage(IDB_BACKGROUNDIMAGE);
	CString filename = AfxGetApp()->m_lpCmdLine;
	if (filename == L"")
		return TRUE;
	filename.Remove('\"');
	if (filename.Mid(filename.ReverseFind('.')) == ".gob")
		OpenFile(filename);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGoBangDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{		
		CPaintDC dc(this);
		CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
		dc.SelectObject(pen);
		for (int i = 0; i < SIZE; i++)
		{
			dc.MoveTo(50, 50 + i * 50);
			dc.LineTo(750, 50 + i * 50);
		}//绘制棋盘横线
		for (int i = 0; i < SIZE; i++)
		{
			dc.MoveTo(50 + i * 50, 50);
			dc.LineTo(50 + i * 50, 750);
		}//绘制棋盘竖线
		for (int nx = 0; nx < SIZE; nx++)
		{
			for (int ny = 0; ny < SIZE; ny++)
			{

				int color = GetChessBoardColor(nx, ny);
				if (color == 0)//白棋
				{
					CBrush brush_w(RGB(255, 255, 255));
					const CPoint o(50 * nx + 50, 50 * ny + 50);//圆心
					dc.SelectObject(brush_w);
					dc.Ellipse(o.x - 15, o.y - 15, o.x + 15, o.y + 15);
				}
				else if (color == 1)//黑棋
				{
					CBrush brush_b(RGB(0, 0, 0));
					const CPoint o(50 * nx + 50, 50 * ny + 50);//圆心
					dc.SelectObject(brush_b);
					dc.Ellipse(o.x - 15, o.y - 15, o.x + 15, o.y + 15);
				}
			}
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGoBangDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGoBangDlg::OnBnClickedStart()
{
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(FALSE);
	if (IsPlaying && MessageBoxW(L"确定要重玩吗？", L"双人五子棋", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	GetDlgItem(IDC_START)->SetWindowTextW(L"重玩");
	if (IsPlaying) {
		GetDlgItem(IDC_BUTTON_AI)->EnableWindow(FALSE);
		IsPlaying = true;
		AIPlaying = false;
		NowColor = 1;//黑先
		index = -1;
		g.Reset();
	}
	else if (AIPlaying) {
		GetDlgItem(IDC_BUTTON_AI)->EnableWindow(FALSE);
		IsPlaying = false;
		AIPlaying = true;
		NowColor = 1;//黑先
		index = -1;
		g.Reset();
	}
	else {
		GetDlgItem(IDC_BUTTON_AI)->EnableWindow(FALSE);
		IsPlaying = true;
		AIPlaying = false;
		NowColor = 1;//黑先
		index = -1;
		g.Reset();
	}
	


	GetDlgItem(IDC_ENDGAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	CleanChessBoard();
}


void CGoBangDlg::OnBnClickedQuit()
{
	g.Reset();
	if (!IsPlaying || MessageBoxW(L"正在游戏中，确定要退出吗？", L"双人五子棋", MB_YESNO | MB_ICONQUESTION) == IDYES)
		EndDialog(0);
}

bool CGoBangDlg::AI_step() {
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(FALSE);
	int aiMove = ai.Search(&g);
	int y = aiMove % 15;
	int x = aiMove / 15;
	if (GetChessBoardColor(x, y) != -1)//如果已有棋子
	{
		return false;
	}
	SetChessBoardColor(x, y, NowColor);
	index++;
	order[index].x = x;
	order[index].y = y;
	g.PutChess(x * 15 + y);

	SendMessage(WM_SETCURSOR);
	int winner = GetWinner();
	if (winner != -1 || index == (SIZE * SIZE - 1))
	{
		if (winner == 0)
			MessageBoxW(L"AI胜利！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
		else if (winner == 1)
			MessageBoxW(L"你赢了！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
		else
			MessageBoxW(L"平局！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
		EndGame();
		return false;
	}
	NowColor = !NowColor;
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(index > -1);
	return true;
}

bool CGoBangDlg::Human_step(CPoint point) {
	int x = int(round(point.x / 50.0) - 1);
	int y = int(round(point.y / 50.0) - 1);
	//将鼠标坐标转为数组下标
	if (GetChessBoardColor(x, y) != -1)//如果已有棋子
		return false;
	SetChessBoardColor(x, y, NowColor);
	index++;
	order[index].x = x;
	order[index].y = y;
	g.PutChess(x * 15 + y);
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(index > -1);
	//放置棋子
	SendMessage(WM_SETCURSOR);
	int winner = GetWinner();
	if (winner != -1 || index == (SIZE * SIZE - 1))
	{
		if (winner == 0)
			MessageBoxW(L"白棋胜利！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
		else if (winner == 1)
			MessageBoxW(L"黑棋胜利！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
		else
			MessageBoxW(L"平局！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
		EndGame();
		return false;
	}
	NowColor = (!NowColor);
	return true;
}

void CGoBangDlg::OnLButtonUp(UINT nFlags, CPoint point)// 鼠标事件相应
{
	if (!(IsPlaying||AIPlaying) || point.x < 40 || point.x>760 || point.y < 40 || point.y>760)
		return;
	else if(IsPlaying){
		if (!Human_step(point))return;
	}
	else if(AIPlaying) {
		if (!Human_step(point))return;
		if (!AI_step())return;
		//return;
	}
	
}

int CGoBangDlg::GetChessBoardColor(int nx, int ny)
{
	return ChessBoard[ny][nx];
}

void CGoBangDlg::SetChessBoardColor(int nx, int ny, int color)
{
	ChessBoard[ny][nx] = color;
	CDC* dc = this->GetDC();
	CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
	dc->SelectObject(pen);
	if (color == 0)//白棋
	{
		CBrush brush_w(RGB(255, 255, 255));
		const CPoint o(50 * nx + 50, 50 * ny + 50);//圆心
		dc->SelectObject(brush_w);
		dc->Ellipse(o.x - 15, o.y - 15, o.x + 15, o.y + 15);
	}
	else if (color == 1)//黑棋
	{
		CBrush brush_b(RGB(0, 0, 0));
		const CPoint o(50 * nx + 50, 50 * ny + 50);//圆心
		dc->SelectObject(brush_b);
		dc->Ellipse(o.x - 15, o.y - 15, o.x + 15, o.y + 15);
	}
	else//清除该坐标棋子，需要重绘,用于悔棋
	{
		Invalidate();
	}
}

void CGoBangDlg::EndGame()
{
	CleanChessBoard();
	IsPlaying = false;
	AIPlaying = false;
	index = -1;
	GetDlgItem(IDC_START)->SetWindowTextW(L"双人对战");
	GetDlgItem(IDC_BUTTON_AI)->EnableWindow(TRUE);
	GetDlgItem(IDC_ENDGAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	
}

void CGoBangDlg::CleanChessBoard()
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			ChessBoard[i][j] = -1;
		}
	}
	Invalidate();
}

void CGoBangDlg::OpenFile(CString filename)
{
	std::ifstream infile;
	infile.open(CStringA(filename));
	if (!infile)
	{
		MessageBoxW(L"打开失败！", L"双人五子棋", MB_OK | MB_ICONERROR);
		return;
	}
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 15; x++)
		{
			int t;
			infile >> t;
			infile.seekg(infile.tellg().operator+(1));
			ChessBoard[y][x] = t;
		}
	}
	Invalidate();//绘制棋盘和棋子
	infile >> NowColor;
	infile.seekg(infile.tellg().operator+(1));
	infile >> index;
	for (int i = 0; i <= index; i++)
	{
		infile.seekg(infile.tellg().operator+(1));
		infile >> order[i].x;
		infile.seekg(infile.tellg().operator+(1));
		infile >> order[i].y;
	}
	infile.close();
	GetDlgItem(IDC_START)->SetWindowTextW(L"重玩");
	GetDlgItem(IDC_ENDGAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_REPENTANCE)->EnableWindow(index > 0);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	IsPlaying = true;
}



int CGoBangDlg::GetChessCount(int nx, int ny)
{
	int color = GetChessBoardColor(nx, ny);
	if (color == -1)
		return -1;
	int x = nx, y = ny;
	int m_max, count;
	while (--y >= 0 && GetChessBoardColor(x, y) == color);
	y++;
	for (count = 1; (++y < SIZE) && (GetChessBoardColor(x, y) == color); count++);
	m_max = count;
	x = nx, y = ny;
	while (--x >= 0 && GetChessBoardColor(x, y) == color);
	x++;
	for (count = 1; ++x < SIZE && GetChessBoardColor(x, y) == color; count++);
	if (m_max < count)
		m_max = count;
	x = nx, y = ny;
	while (x - 1 >= 0 && y - 1 >= 0 && GetChessBoardColor(x - 1, y - 1) == color)
		x--, y--;
	for (count = 1; x + 1 < SIZE && y + 1 < SIZE && GetChessBoardColor(x + 1, y + 1) == color; count++)
		x++, y++;
	if (m_max < count)
		m_max = count;
	x = nx, y = ny;
	while (x - 1 >= 0 && y + 1 < SIZE && GetChessBoardColor(x - 1, y + 1) == color)
		x--, y++;
	for (count = 1; x + 1 < SIZE && y - 1 >= 0 && GetChessBoardColor(x + 1, y - 1) == color; count++)
		x++, y--;
	if (m_max < count)
		m_max = count;
	return m_max;
}



int CGoBangDlg::GetWinner()
{
	if (GetChessCount(order[index].x, order[index].y) >= 5)
		return NowColor;
	return -1;
}


BOOL CGoBangDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (!(IsPlaying || AIPlaying) || point.x < 40 || point.x>760 || point.y < 40 || point.y>760)
		return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
	if (NowColor == 1)//黑棋
		SetCursor(LoadCursorW(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_BLACK)));
	else
		SetCursor(LoadCursorW(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_WHITE)));
	return TRUE;
}


void CGoBangDlg::OnClose()
{
	if (!IsPlaying || MessageBoxW(L"正在游戏中，确定要退出吗？", L"双人五子棋", MB_YESNO | MB_ICONQUESTION) == IDYES)
		CDialogEx::OnClose();

}


void CGoBangDlg::OnBnClickedEndgame()
{
	if (MessageBoxW(L"确定要结束本局吗？", L"双人五子棋", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		EndGame();
		GetDlgItem(IDC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AI)->EnableWindow(TRUE);
		g.Reset();
	}

}


void CGoBangDlg::OnBnClickedRepentance()
{
	if (IsPlaying) {
		g.Regret(1);
		SetChessBoardColor(order[index].x, order[index].y, -1);
		index--;
		GetDlgItem(IDC_REPENTANCE)->EnableWindow(index > -1);
		NowColor = (!NowColor);
	}
	if (AIPlaying) {
		g.Regret(2);
		SetChessBoardColor(order[index].x, order[index].y, -1);
		index--;
		GetDlgItem(IDC_REPENTANCE)->EnableWindow(index > -1);
		NowColor = (!NowColor);
		SetChessBoardColor(order[index].x, order[index].y, -1);
		index--;
		GetDlgItem(IDC_REPENTANCE)->EnableWindow(index > -1);
		NowColor = (!NowColor);
	}
}


void CGoBangDlg::OnBnClickedSave()
{
	CFileDialog filedlg(FALSE);
	filedlg.m_ofn.lpstrFilter = L"五子棋文件(*.gob)\0*.gob\0\0";
	if (filedlg.DoModal() != IDOK)
		return;
	CString filename = filedlg.GetPathName();
	if (filedlg.GetFileExt() == L"")
		filename += ".gob";
	std::ofstream outfile;
	outfile.open(CStringA(filename));
	if(!outfile)
	{
		MessageBoxW(L"保存失败！", L"双人五子棋", MB_OK | MB_ICONERROR);
		return;
	}
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 15; x++)
		{
			outfile << GetChessBoardColor(x, y) << '\0';
		}
		outfile << '\r';
	}
	//输出ChessBoard数组
	outfile <<NowColor<<'\r'<< index << '\r';
	for (int i = 0; i <= index; i++)
		outfile << order[i].x << '\0' << order[i].y << '\r';
	//输出order数组
	outfile.close();
	MessageBoxW(L"保存成功！", L"双人五子棋", MB_OK | MB_ICONINFORMATION);
}


void CGoBangDlg::OnBnClickedOpen()
{
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrFilter = L"五子棋文件(*.gob)\0*.gob\0\0";
	if (filedlg.DoModal() != IDOK)
		return;
	CString filename = filedlg.GetPathName();
	if (filedlg.GetFileExt() == L"")
		filename += ".gob";
	if ((IsPlaying||AIPlaying) && MessageBoxW(L"正在游戏中，本局将被结束。确定要打开棋局吗？", L"双人五子棋", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	OpenFile(filename);
	g.Reset();
	if (MessageBoxW(L"是否进行AI对战？", L"双人五子棋", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		for (int i = 0; i <= index; ++i) {
			g.PutChess(order[i].x * 15 + order[i].y);
		}
		if (!(index % 2)) {
			AI_step();
		}
		IsPlaying = false;
		AIPlaying = true;
	}
	else {
		IsPlaying = true;
		AIPlaying = false;
	}
		
}



void CGoBangDlg::OnBnClickedButtonAi()
{
	//GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->SetWindowTextW(L"重玩");
	GetDlgItem(IDC_ENDGAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_AI)->EnableWindow(FALSE);
	IsPlaying = false;
	AIPlaying = true;
	NowColor = 1;//黑先
	index = -1;
	g.Reset();
	srand((unsigned)time(NULL));

	CleanChessBoard();
}
