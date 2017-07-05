#pragma once

#include "Shape.h"
#include <algorithm>
#include <vector>

using namespace std;

class __declspec(dllexport) CLine : public CShape
{
public:
	CLine() {};
	~CLine() {};

	void DrawLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	void DrawDotLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col);
	void DrawSwitchColLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col1, COLORREF col2);
	void DrawLineTrans(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);
	void DrawAntiBoundaryLine(int nStartX, int nStartY, int nEndX, int nEndY, const vector<COLORREF>& vColBK, COLORREF col);
	void DrawAntiAliasLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);

	void DrawLineZBuf(UINT8 *pZBuf,int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col);

	void DrawVLine(int nStartY, int nEndY, int nX, UINT16 col16);
	void DrawHLine(int nStartX, int nEndX, int nY, UINT16 col16);
	void DrawHLine(int nStartX, int nEndX, int nY, COLORREF col, float fAlpha);

private:
	bool SplitLine(int& nStartX, int& nStartY, int& nEndX, int& nEndY);
	inline int GetPosAreaCode(int nX, int nY);
	
	inline void SetZBufBit(UINT8 *pZBuf,int nX, int nY);
	inline bool GetZBufBit(UINT8 *pZBuf,int nX, int nY);

	bool IsRoadLineBoundary(int nX, int nY, const vector<UINT16>& vColBK16, int nDir);
};
