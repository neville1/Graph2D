#pragma once

#include "Line.h"

class __declspec(dllexport) CRectangle : public CLine
{
public:
	CRectangle() {};
	~CRectangle() {};

	void DrawRect(LPRECT lpRect, COLORREF col);
	void FillRect(LPRECT lpRect, COLORREF col);
	void FillVerticalGradient(LPRECT lpRect, COLORREF ColStart, COLORREF ColEnd);
	void TransparentRect(LPRECT lpRect, COLORREF col, float fAlpha);

	void DrawAntiAliasEllipse(int nX1, int nX2, int nY1, int nY2, COLORREF col);
private:
	bool IsRoadLineBoundary(int nX, int nY, const vector<UINT16>& vColBK16, int nDir);
};
