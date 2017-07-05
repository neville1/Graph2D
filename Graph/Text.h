#pragma once

#include "Shape.h"
#include "FontDefine.h"
#include <vector>

using namespace std;

class CLocalCode;
class __declspec(dllexport) CText : public CShape
{
public:
	CText();
	virtual ~CText();

	void Initial();
	void UnInitial();

	void SetLocalCode(CLocalCode *pCode);

	RECT GetTextRect(const char *pText);
	void DrawStr(const char *pText,int nX, int nY, HFONT hFont);
	void DrawStr(const char *pText,Font_Size1 nFontId, int nX, int nY, Text_Type tType = ONE_COLOR, bool bAdjust = false);
	void DrawScaleStr(HDC hDC, const char *pText,Font_Size1 nFontId, RECT&);
	void DrawRectStr(const char *pText, Font_Size1 nFontType, RECT& rStrRect, TextShowWay tShowWay, Font_Weight = Font_Thin, Text_Type tType = ONE_COLOR);
	void DrawRectHighStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay,const char *pHighText, COLORREF colHigh = RGB(200, 0, 0), Text_Type tType = ONE_COLOR);
	
	void SetTextColor(COLORREF colText) {m_colText = colText;}
	void SetBoundaryColor(COLORREF colBound) { m_colBound = colBound; }
	COLORREF GetTextColor() {return m_colText;}
	COLORREF GetBoundaryColor() {return m_colBound;}

	bool IsText(const char *pStr,const int nIndex);
	bool IsText(const char *pStr);
	SIZE GetStrSize(const char *pStr, Font_Size1 type);
	char * ReverseStr(const char *pStr);
	char * GetLowerCaseString(const char *pStr);
	int GetStrLength(const char *pStr);
	SIZE GetFontSize(Font_Size1 type);
	SIZE GetNumFontSize(Font_Size1 type);
	int GetStrokeOfText(const char *pStr);
	int GetStrokeOfTextCount();

	void SetDebugMode(const bool bDebug){ m_bDebugMode = bDebug;}
private:
	void DrawScaleHighStr(HDC hDC, const char *pText,Font_Size1 nFontType, RECT& rtRect, const char *pHighText,COLORREF colHigh);
	bool GetCopyPos(const char *pStr,const char *pSubStr,int & nStartPos,int & nEndPos);
	CStringA InFixedLengthStr(const char * pText ,Font_Size1 nFontType , int nLength);

private:
	CLocalCode *m_pCode;
	COLORREF m_colText;
	COLORREF m_colBound;
	HFONT m_font[3];
	HFONT m_bfont[3];
	char m_chFontName[40];
	bool m_bDebugMode;
};
