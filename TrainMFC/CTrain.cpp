﻿// CTrain.cpp: 구현 파일
//

#include "pch.h"
#include "TrainMFC.h"
#include "afxdialogex.h"
#include "CTrain.h"

#define TRAIN_TIMER 10
#define WAIT_TIMER 11
// CTrain 대화 상자

IMPLEMENT_DYNAMIC(CTrain, CDialog)

//열차 구역 내선
int railTopLeft[RAIL_NUM] = { 10,110,210,310,410,410,410,310,210,110,10,10,10 };
int railTopRight[RAIL_NUM] = { 110,210,310,410,510,510,510,410,310,210,110,110,110 };
int railBottomLeft[RAIL_NUM] = { 10,10,10,10,10,60,110,110,110,110,110,60,10 };
int railBottomRight[RAIL_NUM] = { 60,60,60,60,60,110,160,160,160,160,160,110,60 };
//열차 구역 외선
int smallRailTopLeft[3] = { 700, 510, 410 };
int smallRailTopRight[3] = { 900, 700, 510 };
int smallRailBottomLeft[3] = { 60, 60, 60 };
int smallRailBottomRight[3] = { 110, 110, 110 };
//열차 구역 반복
int subRailTopLeft[3] = { 850, 850, 850 };
int subRailTopRight[3] = { 900, 900, 900 };
int subRailBottomLeft[3] = { 310, 210, 110 };
int subRailBottomRight[3] = { 410, 310, 210 };
BOOL insCheck[RAIL_NUM] = { FALSE, };
BOOL smallInsCheck[3] = { FALSE, };
BOOL subInsCheck[3] = { FALSE, };
//겹치는 구역
BOOL rectResult;
CRect testRect;
//열차 출발/정지
BOOL trainFlag;
//열차 기본값
int trainAreaFlag;


CTrain::CTrain(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MAINTRAIN, pParent)
{
	startFlag = TRUE;

	trainCount = 0;
}

CTrain::~CTrain()
{
}

void CTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, testText1);
	DDX_Control(pDX, IDC_STATIC2, testText2);
	DDX_Control(pDX, IDC_EDIT_TRAIN, trainAreaNumber);
}


BEGIN_MESSAGE_MAP(CTrain, CDialog)
	ON_BN_CLICKED(IDCREATE, &CTrain::OnBnClickedCreate)
	ON_BN_CLICKED(IDSTART, &CTrain::OnBnClickedStart)
	ON_BN_CLICKED(IDSTOP, &CTrain::OnBnClickedStop)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//ON_WM_CTLCOLOR()

END_MESSAGE_MAP()


// CTrain 메시지 처리기
UINT ThreadMoveTrain(LPVOID param);

void CTrain::OnBnClickedCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	trainFlag = TRUE;

	arg1.hwnd = this->m_hWnd;
	arg1.type = 1;
	if (trainCount <= 4) {
		trainAreaFlag = GetDlgItemInt(IDC_EDIT_TRAIN);
		if (trainAreaFlag >= 3) {
			trainAreaFlag = 1;
		}
		m_thread_move[trainCount] = AfxBeginThread(ThreadMoveTrain, &arg1, THREAD_PRIORITY_NORMAL, 0, 0);
		trainCount++;
	}
}

void CTrain::OnBnClickedStart()
{
	trainFlag = TRUE;

	//SetTimer(TRAIN_TIMER, 50, NULL);
}


void CTrain::OnBnClickedStop()
{
	trainFlag = FALSE;


	/*
	KillTimer(TRAIN_TIMER);
	KillTimer(WAIT_TIMER);
	//가장 최근 스레드만 정지함
	for (int i = 0; i <= trainCount; i++) {
		m_thread_move[i]->SuspendThread();
	}
	*/
}

void CTrain::TimerFunction(int flag) {
	//열차 대기
	if (flag == 0) {
		KillTimer(TRAIN_TIMER);
		trainFlag = FALSE;
		SetTimer(WAIT_TIMER, 500, NULL);
	}
	//열차 출발
	else if (flag == 1) {
		KillTimer(WAIT_TIMER);
		trainFlag = TRUE;
		SetTimer(TRAIN_TIMER, 50, NULL);
	}

}

void CTrain::TrainMove(int left, int top, int right, int bottom) {
	//가상열차 위치 지정
	trainSize.left = left;
	trainSize.right = right;
	trainSize.top = top;
	trainSize.bottom = bottom;
}


void CTrain::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC MemDC;

	// 화면 DC와 호환되는 메모리 DC를 생성
	MemDC.CreateCompatibleDC(&dc);
	// 비트맵 리소스 로딩
	CBitmap bmp;
	CBitmap* pOldBmp = NULL;

	//맵 만들기
	MemDC.SelectObject(pOldBmp);
	//역 테두리 설정
	CPen myPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* oldPen = dc.SelectObject(&myPen);
	int arraySize = (sizeof(railBottomLeft) / sizeof(*railBottomLeft));
	for (int i = 0; i < arraySize; i++) {
		//역 만들기
		dc.Rectangle(railTopLeft[i], railBottomLeft[i], railTopRight[i], railBottomRight[i]);
	}
	arraySize = (sizeof(smallRailBottomLeft) / sizeof(*smallRailBottomLeft));
	for (int i = 0; i < arraySize; i++) {
		//역 만들기
		dc.Rectangle(smallRailTopLeft[i], smallRailBottomLeft[i], smallRailTopRight[i], smallRailBottomRight[i]);
		dc.Rectangle(subRailTopLeft[i], subRailBottomLeft[i], subRailTopRight[i], subRailBottomRight[i]);
	}
	dc.SelectObject(oldPen);


	//출발 초록색 표시
	CBrush brush;
	CBrush* oldBrush = dc.SelectObject(&brush);
	brush.CreateSolidBrush(RGB(0, 255, 0));       // 초록
	oldBrush = dc.SelectObject(&brush);
	//	dc.Rectangle(railTopLeft[subStationCount], railBottomLeft[subStationCount], railTopRight[subStationCount], railBottomRight[subStationCount]);

		//dc.Rectangle(testRect.left, testRect.top, testRect.right, testRect.bottom);
	dc.SelectObject(oldBrush);    // 시스템 브러시 객체를 돌려줌

	MemDC.SelectObject(pOldBmp);


}

BOOL CTrain::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect;
	GetClientRect(rect);
	pDC->FillSolidRect(rect, RGB(0, 0, 0));
	return CDialog::OnEraseBkgnd(pDC);
}

UINT DrawObject(LPVOID param)
{
	CTrain* pMain = (CTrain*)param;
	HwndArg* pArg = (HwndArg*)param;
	CDC dc;
	HDC hdc = ::GetDC(pArg->hwnd);
	CRect rect;
	int trainSpeed = 1;  //열차 속도
	int trainX;  //x기억
	int trainY;  //y기억
	BOOL lineWhile;  //반복구간 플래그용 변수

	int lineSelect;
	lineSelect = trainAreaFlag;
	if (lineSelect == 0) {
		trainSpeed = 850;  //열차 속도
		rect = CRect(trainSpeed, 20, trainSpeed + 30, 50);
	}
	else if (lineSelect == 1) {
		trainSpeed = 0;  //열차 속도
		rect = CRect(trainSpeed, 20, trainSpeed + 30, 50);
	}
	else if (lineSelect == 2) {
		trainSpeed = 380;  //열차 속도
		rect = CRect(860, trainSpeed, 860 + 30, trainSpeed + 30);
		lineWhile = TRUE;
	}
	CRect MainRectTest;
	CRect tmpRect;

	dc.Attach(hdc);

	int stationCount = 0; //현재 도착역
	int subStationCount = 0; //정차이후 출발역
	while (1) {
		if (trainFlag == TRUE) {
			Sleep(100);
			//2호선 외선
			
			if (lineSelect == 0) {
				trainSpeed -= 10;
				rectResult = testRect.IntersectRect(&rect, &CRect(smallRailTopLeft[stationCount], smallRailBottomLeft[stationCount], smallRailTopRight[stationCount], smallRailBottomRight[stationCount]));
				if (stationCount <= 2) {
					//왼
					rect = CRect(trainSpeed, 70, trainSpeed + 30, 100);
					InvalidateRect(pArg->hwnd, rect, TRUE);
					trainX = trainSpeed + 30;
				}
				CBrush brush;
				CBrush* oldBrush = dc.SelectObject(&brush);

				if (IntersectRect(tmpRect, rect, CRect(smallRailTopLeft[stationCount], smallRailBottomLeft[stationCount], smallRailTopRight[stationCount], smallRailBottomRight[stationCount])) && stationCount >= 0)
				{
					smallInsCheck[stationCount] = TRUE;
				}
				dc.SelectObject(oldBrush);

				dc.Rectangle(rect);

				MainRectTest = rect;
				if (IntersectRect(tmpRect, MainRectTest, CRect(smallRailTopLeft[stationCount], smallRailBottomLeft[stationCount], smallRailTopRight[stationCount], smallRailBottomRight[stationCount])) && stationCount >= 0)
				{
					//색칠 + 무효화
					brush.CreateSolidBrush(RGB(0, 255, 0));       // 초록
					oldBrush = dc.SelectObject(&brush);
					dc.Rectangle(smallRailTopLeft[stationCount], smallRailBottomLeft[stationCount], smallRailTopRight[stationCount], smallRailBottomRight[stationCount]);
					OutputDebugString(_T("test\n"));
					//이전 부분과 충돌이 있을경우에만 무효화 해주기
					if (IntersectRect(tmpRect, MainRectTest, CRect(smallRailTopLeft[subStationCount], smallRailBottomLeft[subStationCount], smallRailTopRight[subStationCount], smallRailBottomRight[subStationCount])) && !smallInsCheck[subStationCount]) {
						InvalidateRect(pArg->hwnd, CRect(smallRailTopLeft[stationCount - 1], smallRailBottomLeft[stationCount - 1], smallRailTopRight[stationCount - 1], smallRailBottomRight[stationCount - 1]), TRUE);
					}

				}
				else if (IntersectRect(tmpRect, MainRectTest, CRect(smallRailTopLeft[subStationCount], smallRailBottomLeft[subStationCount], smallRailTopRight[subStationCount], smallRailBottomRight[subStationCount])) && subStationCount >= 0)
				{
					//색만 칠하기
					brush.CreateSolidBrush(RGB(255, 0, 0));       // 빨강
					oldBrush = dc.SelectObject(&brush);
					dc.Rectangle(smallRailTopLeft[subStationCount], smallRailBottomLeft[subStationCount], smallRailTopRight[subStationCount], smallRailBottomRight[subStationCount]);
					smallInsCheck[subStationCount] = FALSE;
					OutputDebugString(_T("test\n"));

				}

				//정차, 방향 조정
				if (stationCount <= 2 && ((100 - (stationCount * 20)) + trainSpeed) == smallRailTopRight[stationCount]) {
					subStationCount = stationCount;
					Sleep(500);
					stationCount++;
					if (stationCount == 3) {
						trainSpeed = 80;
						lineSelect = 1;
						trainX = 490;
						subStationCount = 5;
						stationCount = 6;
					}
				}
			}
			//2호선 내선
			else if (lineSelect == 1) {
				if (stationCount <= 6) {
					trainSpeed += 10;
				}
				else {
					trainSpeed -= 10;
				}
				rectResult = testRect.IntersectRect(&rect, &CRect(railTopLeft[stationCount], railBottomLeft[stationCount], railTopRight[stationCount], railBottomRight[stationCount]));
				//열차 이동
				if (stationCount <= 4) {
					//오른
					rect = CRect(trainSpeed, 20, trainSpeed + 30, 50);
					InvalidateRect(pArg->hwnd, rect, TRUE);
					trainX = trainSpeed + 30;
				}
				else if (stationCount <= 6) {
					//아래
					if (stationCount == 6) {
						CString testX;
						testX.Format(L"%d", trainSpeed);
						OutputDebugString(testX);
					}
					rect = CRect(trainX - 30, trainSpeed, trainX, 30 + trainSpeed);
					InvalidateRect(pArg->hwnd, rect, TRUE);
					trainY = 30 + trainSpeed;
				}
				else if (stationCount <= 10) {
					//왼
					rect = CRect(trainSpeed, trainY - 30, trainSpeed + 30, trainY);
					InvalidateRect(pArg->hwnd, rect, TRUE);
					trainX = trainSpeed + 30;

				}
				else if (stationCount <= 12) {
					//위
					rect = CRect(trainX - 30, trainSpeed, trainX, 30 + trainSpeed);
					InvalidateRect(pArg->hwnd, rect, TRUE);
				}

				CBrush brush;
				CBrush* oldBrush = dc.SelectObject(&brush);

				if (IntersectRect(tmpRect, rect, CRect(railTopLeft[stationCount], railBottomLeft[stationCount], railTopRight[stationCount], railBottomRight[stationCount])) && stationCount >= 0)
				{
					insCheck[stationCount] = TRUE;
				}

				dc.SelectObject(oldBrush);
				dc.Rectangle(rect);
				MainRectTest = rect;

				if (IntersectRect(tmpRect, MainRectTest, CRect(railTopLeft[stationCount], railBottomLeft[stationCount], railTopRight[stationCount], railBottomRight[stationCount])) && stationCount >= 0)
				{
					//색칠 + 무효화
					brush.CreateSolidBrush(RGB(0, 255, 0));       // 초록
					oldBrush = dc.SelectObject(&brush);
					dc.Rectangle(railTopLeft[stationCount], railBottomLeft[stationCount], railTopRight[stationCount], railBottomRight[stationCount]);
					//OutputDebugString(_T("test\n"));
					//이전 부분과 충돌이 있을경우에만 무효화 해주기
					if (IntersectRect(tmpRect, MainRectTest, CRect(railTopLeft[subStationCount], railBottomLeft[subStationCount], railTopRight[subStationCount], railBottomRight[subStationCount])) && !insCheck[subStationCount]) {
						InvalidateRect(pArg->hwnd, CRect(railTopLeft[stationCount - 1], railBottomLeft[stationCount - 1], railTopRight[stationCount - 1], railBottomRight[stationCount - 1]), TRUE);
					}

				}
				else if (IntersectRect(tmpRect, MainRectTest, CRect(railTopLeft[subStationCount], railBottomLeft[subStationCount], railTopRight[subStationCount], railBottomRight[subStationCount])) && subStationCount >= 0)
				{
					//색만 칠하기
					brush.CreateSolidBrush(RGB(255, 0, 0));       // 빨강
					oldBrush = dc.SelectObject(&brush);
					dc.Rectangle(railTopLeft[subStationCount], railBottomLeft[subStationCount], railTopRight[subStationCount], railBottomRight[subStationCount]);
					insCheck[subStationCount] = FALSE;
				}

				//정차, 방향 조정
				if (stationCount <= 4 && (50 + trainSpeed) == railTopRight[stationCount]) {
					subStationCount = stationCount;
					Sleep(1000);
					stationCount++;
					if (stationCount == 5) {
						trainSpeed = 20;
					}
				}
				else if ((stationCount >= 5 && stationCount <= 6 && (30 + trainSpeed) == railBottomRight[stationCount] - 10)) {

					subStationCount = stationCount;
					Sleep(1000);
					stationCount++;
					if (stationCount == 7) {
						trainSpeed = trainX - 30;
					}
				}
				else if (stationCount >= 5 && stationCount <= 10 && (70 + trainSpeed) == railTopRight[stationCount]) {
					subStationCount = stationCount;
					Sleep(1000);
					stationCount++;
					if (stationCount == 11) {
						trainSpeed = trainY - 30;
					}
				}
				else if ((stationCount >= 11 && stationCount <= 12 && (30 + trainSpeed) == railBottomRight[stationCount] - 10)) {
					subStationCount = stationCount;
					Sleep(1000);
					stationCount++;
					if (stationCount == 13) {
						stationCount = 1;
						trainSpeed = 50;
						trainX = 0;
						trainY = 0;
					}
				}
			}

			if (lineSelect == 2) {
				if (lineWhile == TRUE) {
					trainSpeed -= 10;
				}
				else {
					trainSpeed += 10;
				}
				rectResult = testRect.IntersectRect(&rect, &CRect(subRailTopLeft[stationCount], subRailBottomLeft[stationCount], subRailTopRight[stationCount], subRailBottomRight[stationCount]));
				if (stationCount <= 3) {
					//위
					rect = CRect(860, trainSpeed, 860 + 30, trainSpeed + 30);
					InvalidateRect(pArg->hwnd, rect, TRUE);
				}
				CBrush brush;
				CBrush* oldBrush = dc.SelectObject(&brush);

				if (IntersectRect(tmpRect, rect, CRect(subRailTopLeft[stationCount], subRailBottomLeft[stationCount], subRailTopRight[stationCount], subRailBottomRight[stationCount])) && stationCount >= 0)
				{
					subInsCheck[stationCount] = TRUE;
				}
				dc.SelectObject(oldBrush);

				dc.Rectangle(rect);

				MainRectTest = rect;
				if (lineWhile == TRUE) {
					if (IntersectRect(tmpRect, MainRectTest, CRect(subRailTopLeft[stationCount], subRailBottomLeft[stationCount], subRailTopRight[stationCount], subRailBottomRight[stationCount])) && stationCount >= 0)
					{

						//색칠 + 무효화
						brush.CreateSolidBrush(RGB(0, 255, 0));       // 초록
						oldBrush = dc.SelectObject(&brush);
						dc.Rectangle(subRailTopLeft[stationCount], subRailBottomLeft[stationCount], subRailTopRight[stationCount], subRailBottomRight[stationCount]);
						//이전 부분과 충돌이 있을경우에만 무효화 해주기
						if (IntersectRect(tmpRect, MainRectTest, CRect(subRailTopLeft[subStationCount], subRailBottomLeft[subStationCount], subRailTopRight[subStationCount], subRailBottomRight[subStationCount])) && !subInsCheck[subStationCount]) {

							InvalidateRect(pArg->hwnd, CRect(subRailTopLeft[stationCount - 1], subRailBottomLeft[stationCount - 1], subRailTopRight[stationCount - 1], subRailBottomRight[stationCount - 1]), TRUE);
						}

					}
					else if (IntersectRect(tmpRect, MainRectTest, CRect(subRailTopLeft[subStationCount], subRailBottomLeft[subStationCount], subRailTopRight[subStationCount], subRailBottomRight[subStationCount])) && subStationCount >= 0)
					{
						//색만 칠하기
						brush.CreateSolidBrush(RGB(255, 0, 0));       // 빨강
						oldBrush = dc.SelectObject(&brush);
						dc.Rectangle(subRailTopLeft[subStationCount], subRailBottomLeft[subStationCount], subRailTopRight[subStationCount], subRailBottomRight[subStationCount]);
						subInsCheck[subStationCount] = FALSE;
					}
				}
				else if (lineWhile == FALSE) {
					if (IntersectRect(tmpRect, MainRectTest, CRect(subRailTopLeft[stationCount], subRailBottomLeft[stationCount], subRailTopRight[stationCount], subRailBottomRight[stationCount])) && stationCount >= 0)
					{
						//색칠 + 무효화
						brush.CreateSolidBrush(RGB(0, 255, 0));       // 초록
						oldBrush = dc.SelectObject(&brush);
						dc.Rectangle(subRailTopLeft[stationCount], subRailBottomLeft[stationCount], subRailTopRight[stationCount], subRailBottomRight[stationCount]);
						//이전 부분과 충돌이 있을경우에만 무효화 해주기
						if (IntersectRect(tmpRect, MainRectTest, CRect(subRailTopLeft[subStationCount], subRailBottomLeft[subStationCount], subRailTopRight[subStationCount], subRailBottomRight[subStationCount])) && !subInsCheck[subStationCount]) {

							InvalidateRect(pArg->hwnd, CRect(subRailTopLeft[stationCount + 1], subRailBottomLeft[stationCount + 1], subRailTopRight[stationCount + 1], subRailBottomRight[stationCount + 1]), TRUE);
						}

					}
					else if (IntersectRect(tmpRect, MainRectTest, CRect(subRailTopLeft[subStationCount], subRailBottomLeft[subStationCount], subRailTopRight[subStationCount], subRailBottomRight[subStationCount])) && subStationCount >= 0)
					{
						//색만 칠하기
						brush.CreateSolidBrush(RGB(255, 0, 0));       // 빨강
						oldBrush = dc.SelectObject(&brush);
						dc.Rectangle(subRailTopLeft[subStationCount], subRailBottomLeft[subStationCount], subRailTopRight[subStationCount], subRailBottomRight[subStationCount]);
						subInsCheck[subStationCount] = FALSE;
					}
				}

				//정차, 방향 조정
				if (lineWhile == TRUE && (trainSpeed + 70) == subRailBottomRight[stationCount]) {
					subStationCount = stationCount;
					Sleep(500);
					stationCount++;
					if (stationCount == 3) {
						subStationCount = 2;
						stationCount = 1;
						lineWhile = FALSE;
					}
				}
				else if (lineWhile == FALSE && (trainSpeed + 70) == subRailBottomRight[stationCount]) {
					subStationCount = stationCount;
					Sleep(500);
					stationCount--;
					if (stationCount == -1) {
						lineWhile = TRUE;
						subStationCount = 0;
						stationCount = 1;

					}
				}
			}
		}
	}

	return 1;
}

UINT ThreadMoveTrain(LPVOID param)
{
	CTrain* pMain = (CTrain*)param;
	ThreadArg* pArg = (ThreadArg*)param;
	CDC dc;
	HDC hdc = ::GetDC(pArg->hwnd);
	CBrush brush = RGB(255, 255, 255);
	dc.Attach(hdc);

	switch (pArg->type)
	{
	case 1:
		dc.SelectObject(&brush);
		while (0 != DrawObject(pMain))
		{
			DrawObject(pMain);
		}
		break;
	case 2:

		break;

	default:
		break;
	}

	dc.Detach();
	::ReleaseDC(pArg->hwnd, hdc);

	return 0;
}


/*
CString test;
			test.Format(L"%d", bmpInfo.bmHeight + trainSpeed);
			subStationCountext1.SetWindowTextW(test);
			test.Format(L"%d", trainCount);
			subStationCountext2.SetWindowTextW(test);
*/

/*
HBRUSH CTrain::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr;
	if (nCtlColor == CTLCOLOR_STATIC)	//STATIC 컨트롤만 배경색 변경
		pDC->SetBkColor(RGB(0, 0, 0));	//배경색 변경
	hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	return hbr;
}
*/