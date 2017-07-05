#pragma once

#include "FontDefine.h"

struct PanelInfo
{
	UINT32 nWidth;
	UINT32 nHeight;
};

class CGAPI;
class CLine;
class CRectangle;
class CPolygon;
class CText;
class CReBitmap;
class CLocalCode;
class __declspec(dllexport) CReDraw
{
public:
	~CReDraw(void);

	static CReDraw * GetInstance();
	bool Initial(HWND hWnd); 
	bool Flip();
	void Release();

	void SetDrawRect(LPRECT lpRect);
	void SetRotate(bool bRotate);
	void DrawLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	void DrawDotLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col);
	void DrawAntiAliasLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	
	void DrawAntiAliasEllipse(LPRECT lpRect, COLORREF col);
	void DrawRect(LPRECT lpRect, COLORREF col);
	void FillRect(LPRECT lpRect, COLORREF col);
	void FillVerticalGradient(LPRECT lpRect, COLORREF colStart, COLORREF colEnd);
	void TransparentRect(LPRECT lpRect, COLORREF col, float fAlpha);

	void DrawPolyLine(LPPOINT lpPoints, int nCount, COLORREF col);
	void DrawPolygon(LPPOINT lpPoints, int nCount, COLORREF col);
	void FillAnyPolygon(LPPOINT lpPoints, int nCount, COLORREF col);
	void FillConvexPolygon(LPPOINT lpPoints, int nCount, COLORREF col);

	void SetLocalCode(CLocalCode *pCode);
	void SetTextColor(const COLORREF& colText);
	void SetBoundaryColor(const COLORREF& colBound);
	void DrawStr(const char *pText,Font_Type nFontType, int nX, int nY ,Text_Type tType = ONE_COLOR);
	void DrawRectStr(const char *pText,Font_Type nFontType, RECT& rStrRect,TextShowWay tShowWay,Text_Type tType = ONE_COLOR);
	void DrawScaleStr(const char *pText,Font_Type nFontType, int nX, int nY,int nFontSize, Text_Type tType = ONE_COLOR);
	void DrawRectHighStr(const char *pText,Font_Type nFontType, RECT& rStrRect,TextShowWay tShowWay,const char *pHighText,Text_Type tType = ONE_COLOR);
	
	bool IsText(const char *pStr);
	SIZE GetFontSize(Font_Type type);
	SIZE GetNumFontSize(Font_Type type);
	SIZE GetStrSize(const char *pText, int nFontSize);
	SIZE GetStrSize(const char *pText, Font_Type type);
	char * ReverseStr(const char *pStr);
	char * GetLowerCaseString(const char *pStr);
	int GetStrokeOfText(const char *pStr);
	int GetStrokeOfTextCount();
	int GetStrLength(const char *pStr);


	static RECT * GetDrawRect();
	static int GetPitch();
	static int GetBpp();
	static PanelInfo * GetPanelInfo();
protected:
	bool InitialTool();
private:
	CReDraw(void);
	

public:
	static CReDraw * s_pThis;
	static BYTE *s_pBuf;
	static PanelInfo s_panelInfo;

protected:
	CGAPI *m_pGAPI;
	static RECT s_rtDraw;
	static int s_nPitch;
	static int s_nBpp;
	CLine * m_pLine;
	CRectangle * m_pRect;
	CPolygon * m_pPoly;
	CText * m_pText;
};
