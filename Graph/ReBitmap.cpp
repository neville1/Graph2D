#include "StdAfx.h"
#include "ReBitmap.h"
#include "ReDraw.h"
#include "Shape.h"
#include "DIBSectionLite.h"

CReBitmap::CReBitmap() : m_pData(0), m_hBmp(0), m_nBmpPitch(0)
{}

CReBitmap::CReBitmap(int nWidth, int nHeight)
{
	m_pData = 0;
	m_hBmp = 0;
	m_szBmp.cx = nWidth;
	m_szBmp.cy = nHeight;
	m_nBmpPitch = m_szBmp.cx * 2;
	m_pData = new BYTE[nWidth * nHeight * 2];
}

CReBitmap::CReBitmap(int nWidth, int nHeight, BYTE *pData)
{
	m_hBmp = 0;
	m_szBmp.cx = nWidth;
	m_szBmp.cy = nHeight;
	m_nBmpPitch = m_szBmp.cx * 2;
	m_pData = pData;
}

CReBitmap::~CReBitmap()
{
	if (m_hBmp)
	{
		::DeleteObject(m_hBmp);
	}

	if (m_pData)
	{
		delete m_pData;
	}
}

void CReBitmap::ClearData()
{
	if (m_pData)
	{
		delete m_pData;
	}
	m_pData = 0;
}

void CReBitmap::SetData(int nWidth,int nHeight)
{
	if (m_pData)
	{
		delete m_pData;
	}
	m_szBmp.cx = nWidth;
	m_szBmp.cy = nHeight;
	m_nBmpPitch = m_szBmp.cx * 2;

	m_pData = new BYTE[nWidth * nHeight * 2];
}

bool CReBitmap::LoadBmp(HINSTANCE hInstance, LPCWSTR lpBitmapName)
{
	HBITMAP hBmp = ::LoadBitmap(hInstance, lpBitmapName);
	ASSERT(hBmp);

	return LoadBmp(hBmp);
}

bool CReBitmap::LoadBmp(LPCWSTR lpBitmapName)
{
	CDIBSectionLite dib;

	if ( ! dib.Load(lpBitmapName) )
		return false;

	HBITMAP hBmp = dib.GetSafeHandle();
	if (!hBmp)
	{
		return false;
	}
	return LoadBmp(hBmp);
}

bool CReBitmap::LoadBmp(HBITMAP hBmp)
{
	ASSERT(hBmp);

	if(m_pData) 
	{
		delete m_pData;
		m_pData = 0;
	}
	if (m_hBmp)
	{
		::DeleteObject(m_hBmp);
		m_hBmp = 0;
	}
	BITMAP  Bitmap;
	GetObject( hBmp, sizeof(BITMAP), &Bitmap);
	
	BITMAPINFO  dibInfo;
	dibInfo.bmiHeader.biBitCount = 24;
	dibInfo.bmiHeader.biClrImportant = 0;
	dibInfo.bmiHeader.biClrUsed = 0;
	dibInfo.bmiHeader.biCompression = BI_RGB;
	dibInfo.bmiHeader.biHeight = Bitmap.bmHeight;
	dibInfo.bmiHeader.biPlanes = 1;
	dibInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	dibInfo.bmiHeader.biSizeImage = Bitmap.bmWidth * Bitmap.bmHeight * 3;
	dibInfo.bmiHeader.biWidth = Bitmap.bmWidth;
	dibInfo.bmiHeader.biXPelsPerMeter = 0;
	dibInfo.bmiHeader.biYPelsPerMeter = 0;	
	dibInfo.bmiColors[0].rgbRed = 0;
	dibInfo.bmiColors[0].rgbGreen = 0;
	dibInfo.bmiColors[0].rgbBlue = 0;
	dibInfo.bmiColors[0].rgbReserved = 0;

	HDC hDC = ::GetDC(NULL);

	m_hBmp = CreateDIBSection(hDC, (const BITMAPINFO*)&dibInfo,
								   DIB_RGB_COLORS,
								   (void**)&m_pData,
								    NULL, 0);
	::ReleaseDC(NULL, hDC);

	HDC memDc = CreateCompatibleDC(NULL);
	if (!memDc) 
	{
		DeleteObject( hBmp );
		DeleteObject( m_hBmp );
		m_hBmp = 0;
		return false;
	}

	HDC targetDc = CreateCompatibleDC(NULL);
	if (!targetDc) 
	{
		DeleteDC( memDc );
		DeleteObject( hBmp );
		DeleteObject( m_hBmp );
		m_hBmp = 0;
		return false;
	}

	HBITMAP hOldBitmap1 = (HBITMAP)::SelectObject( memDc, hBmp);
	HBITMAP hOldBitmap2 = (HBITMAP)::SelectObject( targetDc, m_hBmp );

	::BitBlt( targetDc, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, memDc, 0, 0, SRCCOPY );

	m_szBmp.cx = Bitmap.bmWidth;
	m_szBmp.cy = Bitmap.bmHeight;
	int nBpp = 3;
	m_nBmpPitch = ((Bitmap.bmWidth * 3) + 3) & 0xfffc;

	::SelectObject(memDc, hOldBitmap1);
	::SelectObject(targetDc, hOldBitmap2);
	DeleteDC(memDc);
	DeleteDC(targetDc);
	DeleteObject(hBmp);

	BYTE *pByte = 0;
	UINT16 *pImage = new UINT16[m_szBmp.cx * m_szBmp.cy];


	for (int nY = 0; nY < m_szBmp.cy; ++nY)
	{
		for (int nX = 0; nX < m_szBmp.cx; ++nX)
		{
			pByte = (m_pData + nY * m_nBmpPitch + nX * nBpp);
			COLORREF col(RGB(pByte[2], pByte[1], pByte[0]));
			pImage[nY * Bitmap.bmWidth + nX] = GetViewColor565(col);
		}
	}

	m_pData = (BYTE *) pImage;
	m_nBmpPitch = m_szBmp.cx * CReDraw::GetBpp();

	if (m_szBmp.cx > 1000 || m_szBmp.cy > 1000)
	{
		AfxMessageBox(_T("Size Info Error"));
	}


	return true;
}

bool CReBitmap::BitBlt(int nX, int nY)
{
	if (!m_pData)
	{
		return false;
	}

	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < pRect->left || nY < pRect->top || 
		                    nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 *pSrcData = 0;
	UINT16 nTempData;
	for (int y = 0; y < m_szBmp.cy; ++y)
	{
		pSrcData  = (UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		
		for (int x = 0; x < m_szBmp.cx; ++x)
		{
			if (CShape::s_BitCount == 15)
			{
				nTempData = ((pSrcData[x] >> 6) << 5) | (pSrcData[x] & 0x1F);
				DrawPixel(nX + x, nY + y, nTempData);
			}
			else
			{
				DrawPixel(nX + x, nY + y, pSrcData[x]);
			}

		}
	}
	return true;
}

bool CReBitmap::MaskBitBlt(int nX, int nY, COLORREF col)
{
	if (!m_pData)
	{
		return false;
	}
	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < pRect->left || nY < pRect->top || 
		                    nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 colMask = GetViewColor(col);
	UINT16 *pSrcData = 0;
	UINT16 nTempData;
	for (int y = 0; y < m_szBmp.cy; ++y)
	{
		pSrcData =(UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		for (int x = 0; x < m_szBmp.cx; ++x)
		{
			if (CShape::s_BitCount == 15)
			{
				nTempData = ((pSrcData[x] >> 6) << 5) | (pSrcData[x] & 0x1F);
				if(nTempData != colMask)
				{
					DrawPixel(nX + x, nY + y, nTempData);
				}
			}else
			{
				if(pSrcData[x] != colMask)
				{
					DrawPixel(nX + x, nY + y, pSrcData[x]);
				}
			}
		}
	}
	return true;
}

bool CReBitmap::MaskBitBlt(int nX, int nY)
{
	if (!m_pData)
	{
		return false;
	}
	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < pRect->left || nY < pRect->top || 
		                    nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 colMask = static_cast<UINT16>(*m_pData);
	UINT16 *pSrcData = 0;
	for (int y = 0; y < m_szBmp.cy; ++y)
	{
		pSrcData =(UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		for (int x = 0; x < m_szBmp.cx; ++x)
		{
			if(pSrcData[x] != colMask)
			{
				DrawPixel(nX + x, nY + y, pSrcData[x]);
			}
		}
	}
	return true;
}

bool CReBitmap::SplitBitBlt(int nX, int nY, int nOffset, COLORREF col)
{
	if (!m_pData)
	{
		return false;
	}
	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < pRect->left || nY < pRect->top || 
		                    nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 colMask = GetViewColor(col);
	UINT16 *pSrcData = 0;
	UINT16 nTempData;
	for (int y = 0; y < m_szBmp.cy - nOffset; ++y)
	{
		pSrcData =(UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		for (int x = 0; x < m_szBmp.cx - nOffset; ++x)
		{
			if (CShape::s_BitCount == 15)
			{
				nTempData = ((pSrcData[x] >> 6) << 5) | (pSrcData[x] & 0x1F);
				if(nTempData != colMask)
				{
					DrawPixel(nX + x, nY + y, nTempData);
				}
			}else
			{
				if(pSrcData[x] != colMask)
				{
					DrawPixel(nX + x, nY + y, pSrcData[x]);
				}
			}
		}
	}
	return true;

}

bool CReBitmap::TransparentBitBlt(int nX, int nY, float fAlpha)
{
	if (!m_pData)
	{
		return false;
	}

	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < pRect->left || nY < pRect->top || 
		                    nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 *pSrcData = 0;
	UINT16 colAlpha = 0;
	UINT16 nTempData = 0;
	for (int y = 0; y < m_szBmp.cy; ++y)
	{
		pSrcData =(UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		for (int x = 0; x < m_szBmp.cx; ++x)
		{
			if (CShape::s_BitCount == 15)
			{
				nTempData = ((pSrcData[x] >> 6) << 5) | (pSrcData[x] & 0x1F);
				colAlpha = GetAlphaColor(nX + x, nY + y, nTempData, fAlpha);
			}

			colAlpha = GetAlphaColor(nX + x, nY + y, pSrcData[x], fAlpha);
			DrawPixel(nX + x, nY + y, colAlpha);
		}
	}
	return true;
}

bool CReBitmap::TransparentMaskBitBlt(int nX, int nY, COLORREF col, float fAlpha)
{
	if (!m_pData)
	{
		return false;
	}

	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < 0 || nY < 0 || nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 colMask = GetViewColor(col);
	UINT16 *pSrcData = 0;
	UINT16 colAlpha = 0;
	UINT16 nTempData = 0;
	for (int y = 0; y < m_szBmp.cy; ++y)
	{
		pSrcData =(UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		for (int x = 0; x < m_szBmp.cx; ++x)
		{
			if (CShape::s_BitCount == 15)
			{
				nTempData = ((pSrcData[x] >> 6) << 5) | (pSrcData[x] & 0x1F);
			}
			else
			{
				nTempData = pSrcData[x];
			}

			if (nTempData != colMask)
			{
				colAlpha = GetAlphaColor(nX + x, nY + y, nTempData, fAlpha);
				DrawPixel(nX + x, nY + y, colAlpha);				
			}
		}
	}
	return true;
}

bool CReBitmap::TransparentMask(int nX, int nY, COLORREF colMask, COLORREF col,  float fAlpha)
{
	if (!m_pData)
	{
		return false;
	}

	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < 0 || nY < 0 || nX + m_szBmp.cx > (int) pRect->right ||
							nY + m_szBmp.cy > (int) pRect->bottom)
	{
		return false;
	}

	UINT16 colMask16 = GetViewColor(colMask);
	UINT16 colBK = GetViewColor(col);
	UINT16 *pSrcData = 0;
	UINT16 colAlpha = 0;
	UINT16 nTempData = 0;
	for (int y = 0; y < m_szBmp.cy; ++y)
	{
		pSrcData =(UINT16 *) (m_pData + (m_szBmp.cy - y - 1) * m_nBmpPitch);
		for (int x = 0; x < m_szBmp.cx; ++x)
		{
			if (CShape::s_BitCount == 15)
			{
				nTempData = ((pSrcData[x] >> 6) << 5) | (pSrcData[x] & 0x1F);
			}
			else
			{
				nTempData = pSrcData[x];
			}
			
			if (nTempData != colMask16)
			{
				colAlpha = GetAlphaColor(nX + x, nY + y, colBK, fAlpha);
				DrawPixel(nX + x, nY + y, colAlpha);				
			}
		}
	}
	return true;
}
