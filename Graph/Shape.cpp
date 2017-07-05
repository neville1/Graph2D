#include "StdAfx.h"
#include "Shape.h"
#include "ReDraw.h"

int CShape::s_BitCount = 16;
int CShape::s_RShift = 3;
int CShape::s_GShift = 2;
int CShape::s_BShift = 3;
int CShape::s_RShift2 = 11;
int CShape::s_GShift2 = 5;
int CShape::s_GMark = 0x3F;
int CShape::s_BMark = 0x1F;


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

void CShape::DrawPixel(int nX, int nY, UINT16 col16)
{
	CReDraw::s_pVideoMapping->DrawPixel(nX, nY, col16);
}

UINT16 CShape::GetPixel(int nX, int nY)
{
	return CReDraw::s_pVideoMapping->GetPixel(nX, nY);
}

UINT16 CShape::GetViewColor565(COLORREF col)
{
	int nRed = GetRValue(col) >> 3;
	int nGreen = GetGValue(col) >> 2;
	int nBlue = GetBValue(col) >> 3;

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

UINT16 CShape::GetViewColor(COLORREF col)
{
	int nRed = GetRValue(col) >> s_RShift;
	int nGreen = GetGValue(col) >> s_GShift;
	int nBlue = GetBValue(col) >> s_BShift;

	return (UINT16) ((nRed << s_RShift2) + (nGreen << s_GShift2) + nBlue);
}

UINT16 CShape::GetAlphaColor(UINT16 col16, float fAlpha)
{
	return 0;
	int nRed   = col16 >> s_RShift2;
	int nGreen = (col16 >> s_GShift2) & s_GMark;
	int nBlue  = col16 & s_BMark;

	nRed = (int)( (float)nRed * fAlpha);
	nGreen = (int)( (float)nGreen * fAlpha);
	nBlue = (int)( (float)nBlue * fAlpha);

	return (UINT16) ((nRed << s_RShift2) + (nGreen << s_GShift2) + nBlue);
}

UINT16 CShape::GetAlphaColor(int nX, int nY, COLORREF col, float fAlpha)
{
	UINT16 colBK = GetPixel(nX, nY);
	float fValue = 1 - fAlpha;

	int nRed, nGreen, nBlue;
	nRed = (int) ((colBK >> s_RShift2) * fValue);
	nGreen = (int) (((colBK >> s_GShift2) & s_GMark) * fValue);
	nBlue = (int) ((colBK & s_BMark) * fValue);

	nRed = (int) (GetRValue(col) * fAlpha + nRed);
	nGreen = (int) (GetGValue(col) * fAlpha + nGreen);
	nBlue = (int) (GetBValue(col) * fAlpha + nBlue);

	return (UINT16) ((nRed << s_RShift2) + (nGreen << s_GShift2) + nBlue);
}

UINT16 CShape::GetAlphaColor(int nX, int nY, UINT16 col16, float fAlpha)
{
	UINT16 colBK = GetPixel(nX, nY);
	float fValue = 1 - fAlpha;

	int nRed, nGreen, nBlue;
	nRed = (int) ((colBK >> s_RShift2) * fValue);
	nGreen = (int) (((colBK >> s_GShift2) & s_GMark) * fValue);
	nBlue = (int) ((colBK & s_BMark) * fValue);

	nRed = (int) ((col16 >> s_RShift2) * fAlpha + nRed);
	nGreen = (int) (((col16 >> s_GShift2) & s_GMark) * fAlpha + nGreen);
	nBlue = (int) ((col16 & s_BMark) * fAlpha + nBlue);

	return (UINT16) ((nRed << s_RShift2) + (nGreen << s_GShift2) + nBlue);
}

void CShape::DrawPixelTrans(int nX, int nY, UINT16 col16)
{
	UINT16 col = GetPixel(nX, nY);

	int nRed, nGreen, nBlue;
	nRed = (int) (col >> s_RShift2) >> 2;
	nGreen = (int) ((col >> s_GShift2) & s_GMark) >> 2;
	nBlue = (int) (col & s_BMark) >> 2;

	int nRed16, nGreen16, nBlue16;
	nRed16 = ((int) (col16 >> s_RShift2) >> 2) * 3;
	nGreen16 = ((int) ((col16 >> s_GShift2) & s_GMark) >> 2) * 3;
	nBlue16 = ((int) (col16 & s_BMark) >> 2) * 3;
	UINT16 colDest = (UINT16) (((nRed + nRed16)<< s_RShift2) + ((nGreen + nGreen16) << s_GShift2) + (nBlue16 + nBlue));

	DrawPixel(nX, nY, colDest);
}

inline UINT16 CShape::GetAntiAliasColor(UINT16 col, UINT16 colBK, float fDistance)
{
	float fValue = 1.0f - fDistance;
	UINT32 nRed, nGreen, nBlue;
	nRed = (UINT32) (fValue * (colBK >> s_RShift2)) + (UINT32) (fDistance * (col >> s_RShift2));
	nGreen = (UINT32) (fValue * ((colBK >> s_GShift2) & s_GMark)) + (UINT32) (fDistance * ((col >> s_GShift2) & s_GMark));
	nBlue = (UINT32) (fValue * (colBK & s_BMark)) + (UINT32) (fDistance * (col & s_BMark));

	return (UINT16) ((nRed << s_RShift2) + (nGreen << s_GShift2) + nBlue);
}

void CShape::SmoothPixel(int nX, int nY, UINT16 *pCol, int nDest, bool bLeft)
{
	UINT16 nDestCol = 0;

	if( bLeft )
	{
		nDestCol = GetAntiAliasColor(GetPixel(nX, nY), GetPixel(nX+1, nY), 0.7f);
		DrawPixel(nX, nY, nDestCol);

		nDestCol = GetAntiAliasColor(pCol[nDest+1], GetPixel(nX+1, nY), 0.5f);
		DrawPixel(nX+1, nY, nDestCol);
	}
	else
	{
		nDestCol = GetAntiAliasColor(pCol[nDest], GetPixel(nX, nY), 0.5f);
		DrawPixel(nX, nY, nDestCol);

		nDestCol = GetAntiAliasColor(GetPixel(nX, nY), GetPixel(nX+1, nY), 0.7f);
		DrawPixel(nX+1, nY, nDestCol);
	}
}

HDC CShape::GetDirectDC()
{
	return CReDraw::s_pVideoMapping->GetDirectDC();
}

void CShape::ReleaseDirectDC(HDC hDC)
{
	CReDraw::s_pVideoMapping->ReleaseDirectDC(hDC);
}