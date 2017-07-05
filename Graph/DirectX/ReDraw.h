#pragma once

#include "ddraw.h"
#include "FontDefine.h"
#include <vector>

using namespace std;

struct PanelInfo
{
	UINT32 nWidth;
	UINT32 nHeight;
};

class CLine;
class CRectangle;
class CPolygon;
class CText;
class CReBitmap;
class CLocalCode;
class __declspec(dllexport) CReDraw
{
public:
	static CReDraw * GetInstance();
	~CReDraw();

	bool Initial(HWND hWnd, int nWidth, int nHeight);
	bool Flip();
	bool Flip(LPRECT rt);
	void Release();

	void SetDrawRect(LPRECT lpRect);
	void DrawLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	void DrawLineTrans(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	void DrawDotLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col);
	void DrawAntiAliasLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	void DrawAntiRoadBoundary(int nStartX, int nStartY, int nEndX, int nEndY, const vector<COLORREF>& vColBK, COLORREF col);

	void DrawAntiAliasEllipse(LPRECT lpRect, COLORREF col);
	void DrawRect(LPRECT lpRect, COLORREF col);
	void FillRect(LPRECT lpRect, COLORREF col);
	void FillVerticalGradient(LPRECT lpRect, COLORREF colStart, COLORREF colEnd);
	void TransparentRect(LPRECT lpRect, COLORREF col, float fAlpha);

	void DrawPolyLine(LPPOINT lpPoints, int nCount, COLORREF col);
	void DrawSwitchColPolyLine(LPPOINT lpPoints, int nCount, int nStep, COLORREF col1, COLORREF col2);
	void DrawPolygon(LPPOINT lpPoints, int nCount, COLORREF col);
	void FillAnyPolygon(LPPOINT lpPoints, int nCount, COLORREF col);
	void FillConvexPolygon(LPPOINT lpPoints, int nCount, COLORREF col);

	void TextInitial(CLocalCode *pCode);
	void SetLocalCode(CLocalCode *pCode);
	void SetTextColor(const COLORREF& colText);
	void SetBoundaryColor(const COLORREF& colBound);
	RECT GetTextRect(const char *pText);
	void DrawStr(const char *pText,int nX, int nY, HFONT hFont);
	void DrawStr(const char *pText,Font_Size1 nFontType, int nX, int nY ,Text_Type tType = ONE_COLOR, bool bAdjust = false);
	void DrawRectStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay, Font_Weight nFontWeight = Font_Thin, Text_Type tType = ONE_COLOR);
	void DrawRectHighStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay,const char *pHighText,Text_Type tType = ONE_COLOR);

	
	bool IsText(const char *pStr);
	bool IsText(const char *pStr,const int nIndex);

	SIZE GetFontSize(Font_Size1 type);
	SIZE GetNumFontSize(Font_Size1 type);
	SIZE GetStrSize(const char *pText, int nFontSize);
	SIZE GetStrSize(const char *pText, Font_Size1 type);

	char * ReverseStr(const char *pStr);
	char * GetLowerCaseString(const char *pStr);
	int GetStrokeOfText(const char *pStr);
	int GetStrokeOfTextCount();
	int GetStrLength(const char *pStr);
	

	static PanelInfo * GetPanelInfo();
	static RECT * GetDrawRect();
	static RECT * GetStartDrawPos();
	static int GetPitch();
	static int GetBpp();

	int  GetStrLen(const char *pText,int nNumFontW);


	void DumpMemory();

public:
	static BYTE *s_pBuf;

private:
	CReDraw();
	bool InitialTool();
	void ResetData();
	

public:
	static CReDraw * s_pThis;
	static PanelInfo s_panelInfo;
	static int s_nBpp;
	static int s_nPitch;
	static RECT s_rtDraw;
	static RECT s_rtDrawPos;
	static LPDIRECTDRAWSURFACE s_pScreen;
	static LPDIRECTDRAWSURFACE s_pBackScreen;

protected:
	static CRITICAL_SECTION s_cs; 
	POINT m_ptViewOffset;	
	bool m_bInit;
	HWND m_hWnd;
	LPDIRECTDRAW m_pDraw;
	DDPIXELFORMAT m_pixelFormat;
	CLine * m_pLine;
	CRectangle * m_pRect;
	CPolygon * m_pPoly;
	CText * m_pText;
	DDSURFACEDESC m_SurfInfo;
	RECT m_rtView;
	RECT m_rtDest;
};
