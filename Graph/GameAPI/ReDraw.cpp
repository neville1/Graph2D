#include "StdAfx.h"
#include "ReDraw.h"
#include "Line.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "Text.h"
#include "GAPI.h"
#include "ReBitmap.h"

CReDraw * CReDraw::s_pThis = 0;
BYTE * CReDraw::s_pBuf = 0;
int CReDraw::s_nPitch = 0;
int CReDraw::s_nBpp = 2;
RECT CReDraw::s_rtDraw = { 0 };
PanelInfo CReDraw::s_panelInfo = { 0 };


CReDraw * CReDraw::GetInstance()
{
	if (!s_pThis)
	{
		s_pThis = new CReDraw;
	}
	return s_pThis;
}

CReDraw::CReDraw(void)
{
	m_pGAPI = new CGAPI;
	m_pLine = 0;
	m_pRect = 0;
	m_pPoly = 0;
	m_pText = 0;
}

CReDraw::~CReDraw(void)
{

}

void CReDraw::SetRotate(bool bRotate)
{
	CShape::SetRotate(bRotate);
}

RECT * CReDraw::GetDrawRect()
{ 
	return &s_rtDraw;
}

int CReDraw::GetPitch() 
{ 
	return s_nPitch; 
}

int CReDraw::GetBpp()
{
	return s_nBpp;
}

PanelInfo * CReDraw::GetPanelInfo()
{
	return &s_panelInfo;
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

bool CReDraw::Initial(HWND hWnd)
{
	if (m_pGAPI->Create(hWnd) != S_OK)
	{
		return false;
	}

	GXDisplayProperties& gxdp = m_pGAPI->GetDisplayProperties();
	s_rtDraw.left = 0;
	s_rtDraw.top = 0;
	if (!CShape::s_bRotate)
	{
		s_rtDraw.right = gxdp.cxWidth;
		s_rtDraw.bottom = gxdp.cyHeight;
	}
	else
	{
		s_rtDraw.right = gxdp.cyHeight;
		s_rtDraw.bottom = gxdp.cxWidth;
	}

	s_nPitch = gxdp.cbyPitch;
	s_nBpp = 2;
	s_panelInfo.nWidth = gxdp.cxWidth;
	s_panelInfo.nHeight = gxdp.cyHeight;

	s_pBuf = m_pGAPI->GetBuffer();
	if (m_pLine)
	{
		return true;
	}

	return InitialTool();
}

bool CReDraw::Flip()
{
	return m_pGAPI->Flip() == S_OK;
}

void CReDraw::Release()
{
	m_pGAPI->Release();
	delete s_pBuf;
	if (m_pLine)
	{
		delete m_pLine;
	}
	if (m_pRect)
	{
		delete m_pRect;
	}
	if (m_pPoly)
	{
		delete m_pPoly;
	}
	if (m_pText)
	{
		delete m_pText;
	}
	if (s_pThis)
	{
		delete s_pThis;
	}
}

void CReDraw::SetDrawRect(LPRECT lpRect)
{
	GXDisplayProperties& gxdp = m_pGAPI->GetDisplayProperties();
	int nWidth = CShape::s_bRotate == false ? gxdp.cxWidth : gxdp.cyHeight;
	int nHeight = CShape::s_bRotate == false ? gxdp.cyHeight : gxdp.cxWidth;

	s_rtDraw.left   =  max(0, lpRect->left);
	s_rtDraw.right  =  min(nWidth, lpRect->right);
	s_rtDraw.top    =  max(0, lpRect->top);
	s_rtDraw.bottom =  min(nHeight, lpRect->bottom);
}

void CReDraw::DrawLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{
	m_pLine->DrawLine(nStartX, nStartY, nEndX, nEndY, col);
}

void CReDraw::DrawDotLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col)
{
	m_pLine->DrawDotLine(nStartX, nStartY, nEndX, nEndY, nStep, col);
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

SIZE CReDraw::GetStrSize(const char *pText, Font_Type type)
{
	return m_pText->GetStrSize(pText, type);
}

SIZE CReDraw::GetFontSize(Font_Type type)
{
	return m_pText->GetFontSize(type);
}

SIZE CReDraw::GetNumFontSize(Font_Type type)
{
	return m_pText->GetNumFontSize(type);
}

char * CReDraw::ReverseStr(const char *pStr)
{
	return m_pText->ReverseStr(pStr);
}
char * CReDraw::GetLowerCaseString(const char *pStr)
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

void CReDraw::DrawStr(const char *pText,Font_Type nFontType, int nX, int nY ,Text_Type tType)
{
	m_pText->DrawStr(pText, nFontType, nX, nY, tType);
}

void CReDraw::DrawRectStr(const char *pText,Font_Type nFontType, RECT& rStrRect,TextShowWay tShowWay,Text_Type tType)
{
	m_pText->DrawRectStr(pText, nFontType,rStrRect, tShowWay ,tType);
}

void CReDraw::DrawScaleStr(const char *pText,Font_Type nFontType, int nX, int nY,int nFontSize, Text_Type tType)
{
	m_pText->DrawScaleStr(pText, nFontType, nX, nY, nFontSize, tType);
}

void CReDraw::DrawRectHighStr(const char *pText,Font_Type nFontType, RECT& rStrRect,TextShowWay tShowWay,const char *pHighText,Text_Type tType)
{
	m_pText->DrawRectHighStr(pText,nFontType,rStrRect,tShowWay,pHighText,tType);
}
