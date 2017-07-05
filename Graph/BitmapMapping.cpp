#include "StdAfx.h"
#include "BitmapMapping.h"
#include "ReDraw.h"
#include "Shape.h"

SIZE CBitmapMapping::s_szBmp = { 0 };

BYTE * CBitmapMapping::s_pBuf = 0;
BYTE * CBitmapMapping::s_pCaptureBuf = 0;//20100406 (William)

CBitmapMapping::CBitmapMapping(void)
{
}

CBitmapMapping::~CBitmapMapping(void)
{
}

char* CBitmapMapping::GetVersion()	//20091130 otwen adds for 版本多寫bitmap or ddraw
{
	return "Bitmap";
}

bool CBitmapMapping::Initial(HWND hWnd, int nWidth, int nHeight)
{
	Reset();

	m_hWnd = hWnd;

	hDCWnd = ::GetDC(m_hWnd);

	s_szBmp.cx = nWidth;
	s_szBmp.cy = nHeight;
	CReDraw::s_nPitch = ((s_szBmp.cx * 2 ) + 3) & 0xfffc;
	m_hMemDC = ::CreateCompatibleDC(hDCWnd);

	//HBITMAP hTempBmp = ::CreateCompatibleBitmap(m_hMemDC, 10, 10);
	//m_hOldBmp = ::SelectObject(m_hMemDC, hTempBmp);
	//HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(255,255,255));
	//HGDIOBJ hOldPen = ::SelectObject(m_hMemDC, hPen);
	//::Rectangle(m_hMemDC,0,0,10,10);

	//COLORREF colTest = ::GetPixel(m_hMemDC, 0, 0);

	//int R = GetRValue(colTest);
	//int G = GetGValue(colTest);
	//int B = GetBValue(colTest);




	BITMAPINFO  dibInfo;
	dibInfo.bmiHeader.biBitCount = 16;
	dibInfo.bmiHeader.biClrImportant = 0;
	dibInfo.bmiHeader.biClrUsed = 0;
	dibInfo.bmiHeader.biCompression = 0;
	dibInfo.bmiHeader.biHeight = s_szBmp.cy;
	dibInfo.bmiHeader.biPlanes = 1;
	dibInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	dibInfo.bmiHeader.biSizeImage = s_szBmp.cx * s_szBmp.cy * CReDraw::s_nBpp;
	dibInfo.bmiHeader.biWidth = s_szBmp.cx;
	dibInfo.bmiHeader.biXPelsPerMeter = 0;
	dibInfo.bmiHeader.biYPelsPerMeter = 0;
	dibInfo.bmiColors[0].rgbBlue = 0;
	dibInfo.bmiColors[0].rgbGreen = 0;
	dibInfo.bmiColors[0].rgbRed = 0;
	dibInfo.bmiColors[0].rgbReserved = 0;

	m_hBmp = CreateDIBSection(hDCWnd,
							  (const BITMAPINFO*)&dibInfo,
							  DIB_RGB_COLORS,
							  (void**)&s_pBuf,
							  NULL, 0);
	
	m_hOldBmp = ::SelectObject(m_hMemDC, m_hBmp);

	HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(255,255,255));
	HGDIOBJ hOldPen = ::SelectObject(m_hMemDC, hPen);
	::Rectangle(m_hMemDC,0,0,10,10);
	UINT16 nPixel = GetPixel(0,0);
	::SelectObject(m_hMemDC, hOldPen);
	::DeleteObject(hPen);

	CShape::s_BitCount = (nPixel == 0xFFFF) ? 16 : 15;
	if (CShape::s_BitCount == 15)
	{
		CShape::s_RShift = 3;
		CShape::s_GShift = 3;
		CShape::s_BShift = 3;
		CShape::s_RShift2 = 10;
		CShape::s_GShift2 = 5;
	    CShape::s_GMark = 0x1F;
		CShape::s_BMark = 0x1F;
	}

	RECT rt;
	rt.left = 0;
	rt.top  = 0;
	rt.right  = s_szBmp.cx;
	rt.bottom = s_szBmp.cy;
	
	CReDraw::s_panelInfo.nWidth = s_szBmp.cx;
	CReDraw::s_panelInfo.nHeight = s_szBmp.cy;
	SetDrawRect(&rt);

	if(s_pCaptureBuf == NULL)//20100406 (William)
	{
		s_pCaptureBuf = new BYTE[s_szBmp.cx * s_szBmp.cy * 2];
		memset(s_pCaptureBuf, 0x00, sizeof(BYTE) * 2 * s_szBmp.cx * s_szBmp.cy);
	}

	return true;
}

bool CBitmapMapping::Flip()
{
	return ::BitBlt(hDCWnd, 0, 0, s_szBmp.cx, s_szBmp.cy, m_hMemDC, 0, 0, SRCCOPY) ? true : false;
}

bool CBitmapMapping::Flip(LPRECT rt)
{
//#ifdef WINCE //20100408 (William)
	UINT32 nWidth = rt->right - rt->left;
	UINT32 nHeight = rt->bottom - rt->top;
	::BitBlt(hDCWnd, rt->left, rt->top, nWidth, nHeight,  m_hMemDC, rt->left, rt->top, SRCCOPY);

	return true;
//#else
//	return Flip();
//#endif
}

void CBitmapMapping::MoveScreen(int nOffsetX, int nOffsetY)//20100406 (William) : 移動的象現相關資料設定
{
	int nDestLeft = 0, nDestTop = 0, nSrcLeft = 0, nSrcTop = 0, nSrcWidth = 0, nSrcHeight = 0;

	if(nOffsetX > 0 && nOffsetY > 0) // +,+
	{
		nDestLeft = nOffsetX;
		nDestTop = nOffsetY;

		nSrcLeft = 0;
		nSrcTop = 0;

		nSrcWidth = s_szBmp.cx - nOffsetX;
		nSrcHeight = s_szBmp.cy - nOffsetY;
	}
	else
	if(nOffsetX > 0 && nOffsetY <= 0)//  +,-
	{
		nOffsetY = abs(nOffsetY);

		nDestLeft = nOffsetX;
		nDestTop = 0;

		nSrcLeft = 0;
		nSrcTop = nOffsetY;

		nSrcWidth = s_szBmp.cx - nOffsetX;
		nSrcHeight = s_szBmp.cy;
	}
	else
	if(nOffsetX <= 0 && nOffsetY > 0)// -,+
	{
		nOffsetX = abs(nOffsetX);

		nDestLeft = 0;
		nDestTop = nOffsetY;

		nSrcLeft = nOffsetX;
		nSrcTop = 0;

		nSrcWidth = s_szBmp.cx;
		nSrcHeight = s_szBmp.cy - nOffsetY;
	}
	else
	if(nOffsetX <= 0 && nOffsetY <= 0)// -,-
	{
		nOffsetX = abs(nOffsetX);
		nOffsetY = abs(nOffsetY);

		nDestLeft = 0;
		nDestTop = 0;

		nSrcLeft = nOffsetX;
		nSrcTop = nOffsetY;

		nSrcWidth = s_szBmp.cx;
		nSrcHeight = s_szBmp.cy;
	}

	BitBltCaptureScreen(nDestLeft, nDestTop, nSrcLeft, nSrcTop, nSrcWidth, nSrcHeight);
}

void CBitmapMapping::BitBltCaptureScreen(int nDestLeft, int nDestTop, int nSrcLeft, int nSrcTop, int nSrcWidth, int nSrcHeight)//20100407 (William)
{
	for(int nSrcY=nSrcTop, nDestY=nDestTop ; nSrcY<nSrcHeight ; nSrcY++, nDestY++)
	{
		for(int nSrcX=nSrcLeft, nDestX=nDestLeft ; nSrcX<nSrcWidth; nSrcX++, nDestX++)
		{
			UINT16 *pPixel = (UINT16 *) (s_pCaptureBuf + (s_szBmp.cy - nSrcY - 1) * CReDraw::s_nPitch + nSrcX * CReDraw::s_nBpp);
			DrawPixel(nDestX, nDestY, *pPixel);
		}
	}
}

void CBitmapMapping::CaptureFullScreen()//20100406 (William)
{
	memcpy(s_pCaptureBuf, s_pBuf, sizeof(BYTE) * 2 * s_szBmp.cx * s_szBmp.cy);
}

void CBitmapMapping::Release()
{
	::DeleteObject(m_hBmp);
	::DeleteDC(m_hMemDC);

	if(s_pCaptureBuf)
	{
		delete [] s_pCaptureBuf;
		s_pCaptureBuf = 0;
	}
}

void CBitmapMapping::DrawPixel(int nX, int nY, UINT16 col16)
{
	*((UINT16 *) (s_pBuf + (s_szBmp.cy - nY - 1) * CReDraw::s_nPitch  + nX * CReDraw::s_nBpp)) = col16;
}

UINT16 CBitmapMapping::GetPixel(int nX, int nY)
{
	return *((UINT16 *) (s_pBuf + (s_szBmp.cy - nY - 1) * CReDraw::s_nPitch + nX * CReDraw::s_nBpp));
}

void CBitmapMapping::DrawPixelTrans(int nX, int nY, UINT16 col16)
{
	UINT16 col;
	UINT16 *pBuf = ((UINT16 *) (s_pBuf + (s_szBmp.cy - nY  - 1) * CReDraw::GetPitch() + nX * CReDraw::GetBpp()));	

	col = (((*pBuf & 0xf800) >> 2) & 0xf800) | (((*pBuf & 0x07e0) >> 2) & 0x07e0) | (((*pBuf & 0x1f) >> 2) & 0x1f);

	*pBuf = col + (((((col16 & 0xf800) >> 1)  + ((col16 & 0xf800) >> 2)) & 0xf800) | 
				   ((((col16 & 0x07e0) >> 1)  + ((col16 & 0x07e0) >> 2)) & 0x07e0) | 
				   ((((col16 & 0x1f)   >> 1)  + ((col16 & 0x1f)   >> 2)) & 0x1f));
}


HDC CBitmapMapping::GetDirectDC()
{
	return m_hMemDC;
}

void CBitmapMapping::ReleaseDirectDC(HDC hDC)
{
}

#ifdef ROADEASY_PC
void CBitmapMapping::SetViewOffset(const int nStartDrawX , const int nStartDrawY)
{
}
#endif

void CBitmapMapping::Reset()
{
	if (m_hBmp)
	{
		::SelectObject(m_hMemDC, m_hOldBmp);
		::DeleteObject(m_hBmp);
		m_hBmp = 0;
	}

	if (m_hMemDC)
	{
		::ReleaseDC(m_hWnd, m_hMemDC);
		m_hMemDC = 0;
	}

	if (hDCWnd)
	{
		::ReleaseDC(NULL, hDCWnd);
		hDCWnd = 0;
	}

	if(s_pCaptureBuf)
	{
		delete [] s_pCaptureBuf;
		s_pCaptureBuf = 0;
	}
}

void CBitmapMapping::SetDrawRect(LPRECT lpRect)
{
	CReDraw::s_rtDraw.left   =  max(0, lpRect->left);
	CReDraw::s_rtDraw.right  =  min(s_szBmp.cx, lpRect->right);
	CReDraw::s_rtDraw.top    =  max(0, lpRect->top);
	CReDraw::s_rtDraw.bottom =  min(s_szBmp.cx, lpRect->bottom);
}
