#include "StdAfx.h"
#include "Shape.h"
#include "ReDraw.h"

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
	*((UINT16 *) (CReDraw::s_pBuf + nY * CReDraw::GetPitch() + nX * CReDraw::s_nBpp)) = col16;
}

inline void CShape::DrawPixelTrans(int nX, int nY, UINT16 col16)
{	
	UINT16 col;
	UINT16 *pBuf = ((UINT16 *) (CReDraw::s_pBuf + nY * CReDraw::GetPitch() + nX * CReDraw::s_nBpp));	
		
	col = (((*pBuf & 0xf800) >> 2) & 0xf800) | (((*pBuf & 0x07e0) >> 2) & 0x07e0) | (((*pBuf & 0x1f) >> 2) & 0x1f);

	*pBuf = col + (((((col16 & 0xf800) >> 1) + ((col16 & 0xf800) >> 2)) & 0xf800) | 
				  ((((col16 & 0x07e0) >> 1)  + ((col16 & 0x07e0) >> 2)) & 0x07e0) | 
				  ((((col16 & 0x1f)   >> 1)  + ((col16 & 0x1f)   >> 2)) & 0x1f));
}

inline UINT16 CShape::GetPixel(int nX, int nY)
{
	return *((UINT16 *) (CReDraw::s_pBuf + nY * CReDraw::GetPitch() + nX * CReDraw::s_nBpp));
}

inline UINT16 CShape::GetViewColor(COLORREF col)
{
	int nRed = GetRValue(col) >> 3;
	int nGreen = GetGValue(col) >> 2;
	int nBlue = GetBValue(col) >> 3;

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

UINT16 CShape::GetAlphaColor(UINT16 col16, float fAlpha)
{
	int nRed   = col16 >> 11;
	int nGreen = (col16 >> 5) & 0x3F;
	int nBlue  = col16 & 0x1F;

	nRed = (int)( (float)nRed * fAlpha);
	nGreen = (int)( (float)nGreen * fAlpha);
	nBlue = (int)( (float)nBlue * fAlpha);

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

UINT16 CShape::GetAlphaColor(int nX, int nY, COLORREF col, float fAlpha)
{
	UINT16 colBK = GetPixel(nX, nY);
	float fValue = 1 - fAlpha;

	int nRed, nGreen, nBlue;
	nRed = (int) ((colBK >> 11) * fValue);
	nGreen = (int) (((colBK >> 5) & 0x3F) * fValue);
	nBlue = (int) ((colBK & 0x1F) * fValue);

	nRed = (int) (GetRValue(col) * fAlpha + nRed);
	nGreen = (int) (GetGValue(col) * fAlpha + nGreen);
	nBlue = (int) (GetBValue(col) * fAlpha + nBlue);

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
	float fValue = 1.0f - fDistance;
	UINT32 nRed, nGreen, nBlue;
	nRed = (UINT32) (fValue * (colBK >> 11)) + (UINT32) (fDistance * (col >> 11));
	nGreen = (UINT32) (fValue * ((colBK >> 5) & 0x3F)) + (UINT32) (fDistance * ((col >> 5) & 0x3F));
	nBlue = (UINT32) (fValue * (colBK & 0x1F)) + (UINT32) (fDistance * (col & 0x1F));

	return (UINT16) ((nRed << 11) + (nGreen << 5) + nBlue);
}

void CShape::SmoothPixel(int nX, int nY, UINT16 *pCol, int nDest, bool bLeft)
{
	UINT16 nDestCol = 0;

	if( bLeft )
	{
		nDestCol = GetAntiAliasColor(GetPixel(nX, nY), GetPixel(nX+1, nY)/*pCol[nDest+1]*/, 0.7f);
		DrawPixel(nX, nY, nDestCol);

		nDestCol = GetAntiAliasColor(pCol[nDest+1], GetPixel(nX+1, nY), 0.5f);
		DrawPixel(nX+1, nY, nDestCol);
	}
	else
	{
		nDestCol = GetAntiAliasColor(pCol[nDest], GetPixel(nX, nY), 0.5f);
		DrawPixel(nX, nY, nDestCol);

		nDestCol = GetAntiAliasColor(GetPixel(nX, nY)/*pCol[nDest]*/, GetPixel(nX+1, nY), 0.7f);
		DrawPixel(nX+1, nY, nDestCol);
	}
}

HDC CShape::GetDirectDC()
{
	CReDraw::s_pBackScreen->Unlock(0);

	HDC hDC;
	HRESULT hr = CReDraw::s_pBackScreen->GetDC(&hDC);
	if (FAILED(hr))
	{
		return 0;
	}
	return hDC;
}

void CShape::ReleaseDirectDC(HDC hDC)
{
	HRESULT hr = CReDraw::s_pBackScreen->ReleaseDC(hDC);
	if (FAILED(hr))
	{
		ASSERT(0);
	}

	DDSURFACEDESC desc;
	memset(&desc, 0x00, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	hr = CReDraw::s_pBackScreen->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0);
    if (FAILED(hr))
	{
		return;
	}

	CReDraw::s_pBuf = (BYTE *) desc.lpSurface;
}
