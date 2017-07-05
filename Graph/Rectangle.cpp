#include "StdAfx.h"
#include "Rectangle.h"
#include "ReDraw.h"
#include "math.h"

void CRectangle::DrawRect(LPRECT lpRect, COLORREF col)
{
	int nLeft   = min(lpRect->left, lpRect->right);
	int nTop    = min(lpRect->top , lpRect->bottom);
	int nRight  = max(lpRect->left, lpRect->right);
	int nBottom = max(lpRect->top , lpRect->bottom);

	nRight -= 1;
	nBottom -= 1;

	UINT16 col16 = GetViewColor(col);

	DrawHLine(nLeft, nRight, nTop, col16);
	DrawVLine(nTop, nBottom, nRight, col16);
	DrawHLine(nLeft, nRight, nBottom, col16);
	DrawVLine(nTop, nBottom, nLeft, col16);
}

void CRectangle::FillRect(LPRECT lpRect, COLORREF col)
{
	UINT16 col16 = GetViewColor(col);
	int nMinX = min(lpRect->left, lpRect->right);
	int nMaxX = max(lpRect->left, lpRect->right);
	int nMinY = min(lpRect->top, lpRect->bottom);
	int nMaxY = max(lpRect->top, lpRect->bottom);

	for (int nY = nMinY; nY < nMaxY; ++nY)
	{
		DrawHLine(nMinX, nMaxX, nY, col16);
	}
}

bool CRectangle::IsRoadLineBoundary(int nX, int nY, const vector<UINT16>& vColBK16, int nDir)
{
	for(int i = 0 ; i < (int)vColBK16.size() ; i++)
	{
		if( GetPixel(nX, nY) == vColBK16[i] )
		{
			return false;
		}
	}

	for(int i = 0 ; i < (int)vColBK16.size() ; i++)
	{
		if( nDir == Vertical )
		{
			if( GetPixel(nX-1, nY) == vColBK16[i] || GetPixel(nX+1, nY) == vColBK16[i] )
			{
				return true;
			}
		}
		else if( nDir == Horizontal )
		{
			if( GetPixel(nX, nY-1) == vColBK16[i] || GetPixel(nX, nY+1) == vColBK16[i] )
			{
				return true;
			}
		}
	}
	return false;
}

void CRectangle::DrawAntiAliasEllipse(int nX1, int nX2, int nY1, int nY2, COLORREF col)
{
	UINT16 col16 = GetViewColor(col);
	int nCenterX = (nX1 + nX2) / 2;
	int nCenterY = (nY1 + nY2) / 2;
	int nDx = (nX2 - nX1);
	int nDy = (nY2 - nY1);
	int nRadiusX = (int) (fabs((double) nDx) / 2.0f);
	int nRadiusY = (int) (fabs((double) nDy) / 2.0f);
	float fDistance = 0;
	float fValue = 0;
	if (nX2 < nX1)
	{
		swap(nX1, nX2);
		swap(nY1, nY2);
	}

	float fYt = 0;
	UINT16 nDestCol = 0;
	for (int nX = -nRadiusX; nX <= nRadiusX; ++nX)
	{
		fYt = (float) (nRadiusY * sqrt(1.0f - (float) (nX * nX) / (float) (nRadiusX * nRadiusX)));
		fDistance = (float) (fYt - (int) fYt);
		fValue = 1.0f - fDistance;
		if (!IsOutPoint(nCenterX + nX, nCenterY + (int) fYt))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX + nX, nCenterY + (int) fYt), fValue);
			DrawPixel(nCenterX + nX, nCenterY + (int) fYt, nDestCol);
		}

		if (!IsOutPoint(nCenterX + nX, nCenterY + (int) fYt + 1))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX + nX, nCenterY + (int) fYt + 1), fDistance);
			DrawPixel(nCenterX + nX, nCenterY + (int) fYt + 1, nDestCol);
		}

		if (!IsOutPoint(nCenterX + nX, nCenterY - (int) fYt))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX + nX, nCenterY - (int) fYt), fValue);
			DrawPixel(nCenterX + nX, nCenterY - (int) fYt, nDestCol);
		}

		if (!IsOutPoint(nCenterX + nX, nCenterY - (int) fYt - 1))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX + nX, nCenterY - (int) fYt - 1), fDistance);
			DrawPixel(nCenterX + nX, nCenterY - (int) fYt - 1, nDestCol);
		}
	}

	float fXt = 0;
	for (int nY = -nRadiusY; nY <= nRadiusY; ++nY)
	{
		fXt = (float) (nRadiusX * sqrt(1.0f - (float) (nY * nY) / (float) (nRadiusY * nRadiusY)));
		fDistance = (float) (fXt - (int) fXt);
		fValue = 1.0f - fDistance;

		if (!IsOutPoint(nCenterX + (int) fXt, nCenterY + nY))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX + (int) fXt, nCenterY + nY), fValue);
			DrawPixel(nCenterX + (int) fXt, nCenterY + nY, nDestCol);
		}

		if (!IsOutPoint(nCenterX + (int) fXt + 1, nCenterY + nY))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX + (int) fXt + 1, nCenterY + nY), fDistance);
			DrawPixel(nCenterX + (int) fXt + 1, nCenterY + nY, nDestCol);
		}

		if (!IsOutPoint(nCenterX - (int) fXt, nCenterY + nY))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX - (int) fXt, nCenterY + nY), fValue);
			DrawPixel(nCenterX - (int) fXt, nCenterY + nY, nDestCol);
		}

		if (!IsOutPoint(nCenterX - (int) fXt - 1, nCenterY + nY))
		{
			nDestCol = GetAntiAliasColor(col16, GetPixel(nCenterX - (int) fXt, nCenterY + nY), fDistance);
			DrawPixel(nCenterX - (int) fXt - 1, nCenterY + nY, nDestCol);
		}
	}
}

void CRectangle::FillVerticalGradient(LPRECT lpRect, COLORREF ColStart, COLORREF ColEnd)
{
	int nMinX = min(lpRect->left, lpRect->right);
	int nMaxX = max(lpRect->left, lpRect->right);
	int nMinY = min(lpRect->top, lpRect->bottom);
	int nMaxY = max(lpRect->top, lpRect->bottom);
	int nHeight = nMaxY - nMinY;
	float fPercent;
	BYTE byRed, byGreen, byBlue;
	for (int i = 0; i < nHeight; ++i)
	{
		fPercent = 1 - (float) i / (float) (nHeight << 1);

		byRed =   (BYTE) (GetRValue(ColStart) * fPercent) + (BYTE) (GetRValue(ColEnd) * (1 - fPercent));
		byGreen = (BYTE) (GetGValue(ColStart) * fPercent) + (BYTE) (GetGValue(ColEnd) * (1 - fPercent));
		byBlue =   (BYTE) (GetBValue(ColStart)* fPercent) + (BYTE) (GetBValue(ColEnd) * (1 - fPercent));

		UINT16 col16 = GetViewColor(RGB(byRed, byGreen, byBlue));
		DrawHLine(nMinX, nMaxX, nMinY + i, col16);
	}
}

void CRectangle::TransparentRect(LPRECT lpRect, COLORREF col, float fAlpha)
{
	UINT16 col16 = GetViewColor(col);
	int nMinX = min(lpRect->left, lpRect->right);
	int nMaxX = max(lpRect->left, lpRect->right);
	int nMinY = min(lpRect->top, lpRect->bottom);
	int nMaxY = max(lpRect->top, lpRect->bottom);
	RECT *pRect = CReDraw::GetDrawRect();

	nMinX = max(nMinX, pRect->left);
	nMinY = max(nMinY, pRect->top);
	nMaxX = min(nMaxX, pRect->right);
	nMaxY = min(nMaxY, pRect->bottom);

	for (int nY = nMinY; nY < nMaxY; ++nY)
	{
		for (int nX = nMinX; nX < nMaxX; ++nX)
		{
			DrawPixel(nX, nY, GetAlphaColor(nX, nY, col16, fAlpha));
		}
	}

}
