#include "StdAfx.h"
#include "Shape.h"
#include "ReDraw.h"



bool CShape::s_bRotate = false;

void CShape::SetRotate(bool bRotate)
{
	s_bRotate = bRotate;
}

inline bool CShape::IsOutPoint(int nX, int nY)
{
	RECT *pRect = CReDraw::GetDrawRect();
	return nX >  pRect->right - 1 || nY > pRect->bottom - 1 || nX < pRect->left ||  nY < pRect->top;
}

inline void CShape::InViewPos(int& nX, int& nY)
{
	RECT *pRect = CReDraw::GetDrawRect();
	nX = max(nX, pRect->left);
	nX = min(nX, (int) pRect->right);
	nY = max(nY, pRect->top);
	nY = min(nY, (int) pRect->bottom);
}

inline void CShape::DrawPixel(int nX, int nY, UINT16 col16)
{	
	PreLocate(nX, nY);

	*((UINT16 *) (CReDraw::s_pBuf + nY * CReDraw::GetPitch() + nX * CReDraw::GetBpp())) = col16;
}

inline UINT16 CShape::GetPixel(int nX, int nY)
{
	PreLocate(nX, nY);

	return *((UINT16 *) (CReDraw::s_pBuf + nY * CReDraw::GetPitch() + nX * CReDraw::GetBpp()));
}

void CShape::SmoothPixel(int nX, int nY)
{
	PreLocate(nX, nY);

	PanelInfo *pPanelInfo = CReDraw::GetPanelInfo();
	int nPitch = pPanelInfo->nWidth * CReDraw::GetBpp();
	POINT ptOffset[] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};
	UINT16 nPixel = 0;
	int nRed = 0;
	int nGreen = 0;
	int nBlue = 0;
	for (int i = 0; i < 4; ++i)
	{
		nPixel = *(UINT16 *)(CReDraw::s_pBuf + (nY + ptOffset[i].y)  * nPitch + (nX + ptOffset[i].x) * 2);
		nRed += (nPixel >> 11);
		nGreen += (nPixel >> 5) & 0x3F;
		nBlue += (nPixel & 0x1F);
	}
	nRed = nRed >> 2;
	nGreen = nGreen >> 2;
	nBlue = nBlue >> 2;

	*(UINT16 *)(CReDraw::s_pBuf + (nY * nPitch + (nX * 2))) = (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

inline UINT16 CShape::GetViewColor(COLORREF col)
{
	int nRed = GetRValue(col) >> 3;
	int nGreen = GetGValue(col) >> 2;
	int nBlue = GetBValue(col) >> 3;

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

UINT16 CShape::GetAlphaColor(int nX, int nY, UINT16 col16, float fAlpha)
{
	UINT16 colBK = GetPixel(nX, nY);
	float fValue = 1 - fAlpha;

	int nRed, nGreen, nBlue;
	nRed = (int) ((colBK >> 11) * fValue);
	nGreen = (int) (((colBK >> 5) & 0x3F) * fValue);
	nBlue = (int) ((colBK & 0x1F) * fValue);

	nRed = (int) ((col16 >> 11) * fAlpha + nRed);
	nGreen = (int) (((col16 >> 5) & 0x3F) * fAlpha + nGreen);
	nBlue = (int) ((col16 & 0x1F) * fAlpha + nBlue);

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

inline UINT16 CShape::GetAntiAliasColor(UINT16 col, UINT16 colBK, float fDistance)
{
	float fValue = 1 - fDistance;
	UINT32 nRed, nGreen, nBlue;
	nRed = (UINT32) (fValue * (colBK >> 11)) + (UINT32) (fDistance * (col >> 11));
	nGreen = (UINT32) (fValue * ((colBK >> 5) & 0x3F)) + (UINT32) (fDistance * ((col >> 5) & 0x3F));
	nBlue = (UINT32) (fValue * (colBK & 0x1F)) + (UINT32) (fDistance * (col & 0x1F));

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

inline void CShape::PreLocate(int& nX, int& nY)
{
	if (!s_bRotate)
	{
		return;
	}
	int nTemp = nX;
	nX = CReDraw::s_panelInfo.nWidth - nY - 1;
	nY = nTemp;
}