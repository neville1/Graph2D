#include "StdAfx.h"
#include "ReDraw.h"
#include "Line.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "Text.h"
#include "ReBitmap.h"

#include "DebugTool.h"
#include "DirectDrawMapping.h"
#include "BitmapMapping.h"

CReDraw * CReDraw::s_pThis = 0;
CVideoMapping *CReDraw::s_pVideoMapping = 0;

RECT CReDraw::s_rtDraw = { 0 };
RECT CReDraw::s_rtDrawPos = { 0 };
PanelInfo CReDraw::s_panelInfo = { 0 };
int CReDraw::s_nBpp = 2;
int CReDraw::s_nPitch = 0;


CRITICAL_SECTION CReDraw::s_cs; 


CReDraw * CReDraw::GetInstance()
{
	if (!s_pThis)
	{
		s_pThis = new CReDraw;
	}
	return s_pThis;
}

CReDraw::CReDraw()
{
#ifdef ROADEASY_PC
	ChangePixelSetting(16);
#endif

	m_pLine = 0;
	m_pRect = 0;
	m_pPoly = 0;
	m_pText = 0;
	m_bBmpMode = false;

	m_bInit = false;

	InitializeCriticalSection(&s_cs); 
}

CReDraw::~CReDraw()
{
	DeleteCriticalSection(&s_cs);
}


bool CReDraw::Initial(HWND hWnd, int nWidth, int nHeight)
{	
	if (m_bInit)
	{
		return true;
	}

	if (m_bBmpMode)
	{
		s_pVideoMapping->Initial(hWnd, nWidth, nHeight);
		return true;
	}

	s_pVideoMapping = new CDirectDrawMapping;
	bool bDirectIniial = s_pVideoMapping->Initial(hWnd, nWidth, nHeight);
#ifdef ROADEASY_PC
	bDirectIniial = false;
#endif

	if (bDirectIniial)
	{
		#ifdef ROADEASY_PC
		s_pVideoMapping->SetViewOffset(m_ptOffsetPC.x, m_ptOffsetPC.y);
		#endif
		
		m_bInit = true;
		return InitialTool();
	}
	s_pVideoMapping->Release();
	delete s_pVideoMapping;

	s_pVideoMapping = new CBitmapMapping;
	s_pVideoMapping->Initial(hWnd, nWidth, nHeight);
	m_bBmpMode = true;

	return InitialTool();
}

bool CReDraw::Flip()
{	
	return s_pVideoMapping->Flip();
}

char* CReDraw::GetVersion()
{
	return s_pVideoMapping->GetVersion();
}

void CReDraw::MoveScreen(int nOffsetX, int nOffsetY) {
	s_pVideoMapping->MoveScreen(nOffsetX, nOffsetY);
}

void CReDraw::CaptureFullScreen()
{
	s_pVideoMapping->CaptureFullScreen();
}

HDC CReDraw::GetDirectDC()
{
	return s_pVideoMapping->GetDirectDC();
}

void CReDraw::ReleaseDirectDC(HDC hDC)
{
	s_pVideoMapping->ReleaseDirectDC(hDC);
}

bool CReDraw::Flip(LPRECT rt)
{
	return s_pVideoMapping->Flip(rt);
}


void CReDraw::Release()
{
	s_pVideoMapping->Release();

	delete m_pLine;
	delete m_pRect;
	delete m_pPoly;
	
	m_pText->UnInitial();
	delete m_pText;
	delete s_pVideoMapping;

	delete s_pThis;
}

void CReDraw::SetDrawRect(LPRECT lpRect)
{
	s_rtDraw.left   =  max(0, lpRect->left);
	s_rtDraw.right  =  min((int) s_panelInfo.nWidth, lpRect->right);
	s_rtDraw.top    =  max(0, lpRect->top);
	s_rtDraw.bottom =  min((int) s_panelInfo.nHeight, lpRect->bottom);
}

bool CReDraw::InitialTool()
{
	if (m_pLine)
	{
		return true;
	}
	m_pLine = new CLine;
	m_pRect = new CRectangle;
	m_pPoly = new CPolygon;
	m_pText = new CText;	

	return true;
}

void CReDraw::DrawLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{
	m_pLine->DrawLine(nStartX, nStartY, nEndX, nEndY, col);
}

void CReDraw::DrawLineTrans(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{
	m_pLine->DrawLineTrans(nStartX, nStartY, nEndX, nEndY, col);
}

void CReDraw::DrawDotLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col)
{
	m_pLine->DrawDotLine(nStartX, nStartY, nEndX, nEndY, nStep, col);
}

void CReDraw::DrawAntiRoadBoundary(int nStartX, int nStartY, int nEndX, int nEndY, const vector<COLORREF>& vColBK, COLORREF col)
{
	m_pLine->DrawAntiBoundaryLine(nStartX, nStartY, nEndX, nEndY, vColBK, col);
}

void CReDraw::DrawAntiAliasLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{
	m_pLine->DrawAntiAliasLine(nStartX, nStartY, nEndX, nEndY, col);
}

void CReDraw::DrawAntiAliasEllipse(LPRECT lpRect, COLORREF col)
{
	m_pRect->DrawAntiAliasEllipse(lpRect->left, lpRect->right, lpRect->top, lpRect->bottom, col);
}

void CReDraw::DrawRect(LPRECT lpRect, COLORREF col)
{
	m_pRect->DrawRect(lpRect, col);
}

void CReDraw::FillRect(LPRECT lpRect, COLORREF col)
{
	m_pRect->FillRect(lpRect, col);
}

void CReDraw::FillVerticalGradient(LPRECT lpRect, COLORREF colStart, COLORREF colEnd)
{
	m_pRect->FillVerticalGradient(lpRect, colStart, colEnd);
}

void CReDraw::TransparentRect(LPRECT lpRect, COLORREF col, float fAlpha)
{
	m_pRect->TransparentRect(lpRect, col, fAlpha);
}

void CReDraw::DrawPolyLine(LPPOINT lpPoints, int nCount, COLORREF col)
{
	m_pPoly->DrawPolyLine(lpPoints, nCount, col);
}

void CReDraw::DrawSwitchColPolyLine(LPPOINT lpPoints, int nCount, int nStep, COLORREF col1, COLORREF col2)//2009-9-1
{
	m_pPoly->DrawSwitchColPolyLine(lpPoints, nCount, nStep, col1, col2);
}

void CReDraw::DrawPolygon(LPPOINT lpPoints, int nCount, COLORREF col)
{
	m_pPoly->DrawPolygon(lpPoints, nCount, col);
}

void CReDraw::FillAnyPolygon(LPPOINT lpPoints, int nCount, COLORREF col)
{
	m_pPoly->FillAnyPolygon(lpPoints, nCount, col);
}

void CReDraw::FillConvexPolygon(LPPOINT lpPoints, int nCount, COLORREF col)
{
	m_pPoly->FillConvexPolygon(lpPoints, nCount, col);
}

void CReDraw::TextInitial(CLocalCode *pCode)
{
	m_pText->SetLocalCode(pCode);
	m_pText->Initial();
}

void CReDraw::SetLocalCode(CLocalCode *pCode)
{
	m_pText->SetLocalCode(pCode);
}

void CReDraw::SetTextColor(const COLORREF& colText)
{
	m_pText->SetTextColor(colText);
}

void CReDraw::SetBoundaryColor(const COLORREF& colBound)
{
	m_pText->SetBoundaryColor(colBound);
}

bool CReDraw::IsText(const char *pStr)
{
	return m_pText->IsText(pStr);
}

bool CReDraw::IsText(const char *pStr,const int nIndex)
{
	return m_pText->IsText(pStr,nIndex);
}

SIZE CReDraw::GetStrSize(const char *pText, Font_Size1 type)
{
	return m_pText->GetStrSize(pText, type);
}

SIZE CReDraw::GetFontSize(Font_Size1 type)
{
	return m_pText->GetFontSize(type);
}

SIZE CReDraw::GetNumFontSize(Font_Size1 type)
{
	return m_pText->GetNumFontSize(type);
}

char * CReDraw::ReverseStr(const char *pStr)
{
	return m_pText->ReverseStr(pStr);
}
char *  CReDraw::GetLowerCaseString(const char *pStr)
{
	return m_pText->GetLowerCaseString(pStr);
}
int CReDraw::GetStrokeOfText(const char *pStr)
{
	return m_pText->GetStrokeOfText(pStr);
}
int CReDraw::GetStrokeOfTextCount()
{
	return m_pText->GetStrokeOfTextCount();
}
int CReDraw::GetStrLength(const char *pStr)
{
	return m_pText->GetStrLength(pStr);
}

RECT CReDraw::GetTextRect(const char *pText)
{
	return m_pText->GetTextRect(pText);
}

void CReDraw::DrawStr(const char *pText, int nX, int nY, HFONT hFont)
{
	m_pText->DrawStr(pText, nX, nY, hFont);
}

void CReDraw::DrawStr(const char *pText,Font_Size1 nFontType, int nX, int nY ,Text_Type tType, bool bAdjust)
{
	m_pText->DrawStr(pText, nFontType, nX, nY, tType, bAdjust);
}

void CReDraw::DrawRectStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay,Font_Weight nFontWeight, Text_Type tType)
{
	m_pText->DrawRectStr(pText, nFontType,rStrRect, tShowWay ,nFontWeight, tType);
}

void CReDraw::DrawRectHighStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay,const char *pHighText,COLORREF colHigh,Text_Type tType)
{
	m_pText->DrawRectHighStr(pText,nFontType,rStrRect,tShowWay,pHighText,colHigh,tType);
}

PanelInfo * CReDraw::GetPanelInfo()
{
	return &s_panelInfo;
}

RECT * CReDraw::GetDrawRect()
{ 
	return &s_rtDraw;
}
RECT *  CReDraw::GetStartDrawPos( )
{
	return &s_rtDrawPos;
}
int CReDraw::GetPitch() 
{ 
	return s_nPitch; 
}

int CReDraw::GetBpp()
{
	return s_nBpp;
}

void CReDraw::DumpMemory()
{
	MEMORYSTATUS mem;
	GlobalMemoryStatus(&mem);

	CString strAvail;
	strAvail.Format(_T("VM = %dMB\r\n"), mem.dwAvailVirtual);

	AfxMessageBox(strAvail);
}

void CReDraw::SetDebugMode(const bool bDebug)
{
	m_pText->SetDebugMode(bDebug);
}
