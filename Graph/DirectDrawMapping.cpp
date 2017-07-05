#include "StdAfx.h"
#include "ddraw.h"
#include "DirectDrawMapping.h"
#include "ReDraw.h"

#if (_WIN32_WCE >= 0x600)
#define DDLOCK_WAIT		DDLOCK_WAITNOTBUSY
#define DDBLT_WAIT		DDBLT_WAITNOTBUSY
#define DDSD_CAPS_1		DDSCAPS_OVERLAY
#else
#ifdef ROADEASY_PC
#define DDSD_CAPS_1		DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY //DDSCAPS_VIDEOMEMORY
#else
#define DDSD_CAPS_1		DDSCAPS_OFFSCREENPLAIN //| DDSCAPS_SYSTEMMEMORY //DDSCAPS_VIDEOMEMORY
#endif
#endif


LPDIRECTDRAWSURFACE CDirectDrawMapping::s_pScreen = 0;
LPDIRECTDRAWSURFACE CDirectDrawMapping::s_pBackScreen = 0;

BYTE * CDirectDrawMapping::s_pBuf = 0;
BYTE * CDirectDrawMapping::s_pCaptureBuf = 0;

CDirectDrawMapping::CDirectDrawMapping(void)
{
	m_hWnd = 0;
	m_pDraw = 0;
	s_pScreen = 0;
	m_ptViewOffset.x = 0;
	m_ptViewOffset.y = 0;

	m_nViewWidth = 0;
	m_nViewHeight = 0;
}

CDirectDrawMapping::~CDirectDrawMapping(void)
{

}

char* CDirectDrawMapping::GetVersion()
{
	return "DirectDraw";
}

bool CDirectDrawMapping::Initial(HWND hWnd, int nWidth, int nHeight)
{
	m_hWnd = hWnd;
	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;

	HRESULT hr = ::DirectDrawCreate(NULL, &m_pDraw, NULL);
	if (FAILED(hr))
	{
		TRACE(_T("Create DirectDraw Failed"));
		return false;
	}

	hr = m_pDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL);
	if (FAILED(hr))
	{
		TRACE(_T("DirectDraw SetCooperativeLevel Failed"));
		return false;
	}

	RECT rtView;
	::GetWindowRect(hWnd, &rtView);

	DDSURFACEDESC Ddsd;
	Ddsd.dwSize = sizeof(DDSURFACEDESC);
	Ddsd.dwFlags =  DDSD_CAPS;
	Ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    hr = m_pDraw->CreateSurface(&Ddsd, &s_pScreen, NULL); 
	if (FAILED(hr))
	{
		TRACE(_T("DirectDraw CreateSurface Failed"));
		return false;
	}

	memset(&m_SurfInfo, 0x00, sizeof(DDSURFACEDESC));
	m_SurfInfo.dwSize = sizeof(DDSURFACEDESC);	
	hr = s_pScreen->GetSurfaceDesc(&m_SurfInfo);
	if (FAILED(hr))
	{
		TRACE(_T("DirectDraw GetSurfaceDesc Failed"));
		return false;
	}

	DDSURFACEDESC Ddsd2;
	Ddsd2.dwSize = sizeof(DDSURFACEDESC);
	Ddsd2.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	Ddsd2.ddsCaps.dwCaps = DDSD_CAPS_1;
	Ddsd2.dwWidth = nWidth;
	Ddsd2.dwHeight = nHeight;

	
    hr = m_pDraw->CreateSurface(&Ddsd2, &s_pBackScreen, NULL); 
	if (FAILED(hr))
	{
		TRACE(_T("DirectDraw CreateSurface2 Failed"));
		return false;
	}

	CReDraw::s_panelInfo.nWidth = m_SurfInfo.dwWidth;
	CReDraw::s_panelInfo.nHeight = m_SurfInfo.dwHeight;

	CReDraw::s_nPitch = nWidth * 2;
	CReDraw::s_rtDraw.left = 0;
	CReDraw::s_rtDraw.top = 0;
	CReDraw::s_rtDraw.right = nWidth;
	CReDraw::s_rtDraw.bottom = nHeight;
 
	memset(&m_pixelFormat, 0x00, sizeof(DDPIXELFORMAT));
	m_pixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	hr = s_pScreen->GetPixelFormat(&m_pixelFormat);
	if (FAILED(hr))
	{
		TRACE(_T("DirectDraw GetPixelFormat Failed"));
		return false;
	}

	if (m_pixelFormat.dwRGBBitCount != 16)
	{
		TRACE(_T("Not support this PixelFormat"));
		return false;
	}

	DDSURFACEDESC desc;
	memset(&desc, 0x00, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	hr = s_pBackScreen->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0);
    if (FAILED(hr))
	{
		return false;
	}

	s_pBuf = (BYTE *) desc.lpSurface;

	::GetWindowRect(m_hWnd, &m_rtView);

	CPoint ptViewLT(m_rtView.left, m_rtView.top);
	CPoint ptViewRB(m_rtView.right, m_rtView.bottom);
	::ClientToScreen(m_hWnd, &ptViewLT);
	::ClientToScreen(m_hWnd, &ptViewRB);

	m_rtDest.left = ptViewLT.x; 
	m_rtDest.top  = ptViewLT.y;
	m_rtDest.right = ptViewRB.x;
	m_rtDest.bottom = ptViewRB.y;

	if(s_pCaptureBuf == NULL)
	{
		s_pCaptureBuf = new BYTE[m_nViewWidth * m_nViewHeight * 2];
		memset(s_pCaptureBuf, 0x00, sizeof(BYTE) * 2 * m_nViewWidth * m_nViewHeight);
	}

	return true;
}

bool CDirectDrawMapping::Flip()
{
	s_pBackScreen->Unlock(0);

	CRect rtOffset(m_rtView);
	rtOffset.left += m_ptViewOffset.x;
	rtOffset.top += m_ptViewOffset.y;
	rtOffset.right += m_ptViewOffset.x;
	rtOffset.bottom += m_ptViewOffset.y;

	HRESULT hr = s_pScreen->Blt(&rtOffset, s_pBackScreen, &m_rtView, DDBLT_WAIT,NULL);

	if (FAILED(hr))
	{
		ASSERT(0);
		return false;
	}

	DDSURFACEDESC desc;
	memset(&desc, 0x00, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	hr = s_pBackScreen->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0);
    if (FAILED(hr))
	{
		return false;
	}
	s_pBuf = (BYTE *) desc.lpSurface;

	return true;
}

bool CDirectDrawMapping::Flip(LPRECT rt)
{
	s_pBackScreen->Unlock(0);

	CRect rtOffset(rt);
	rtOffset.left += m_ptViewOffset.x;
	rtOffset.top += m_ptViewOffset.y;
	rtOffset.right += m_ptViewOffset.x;
	rtOffset.bottom += m_ptViewOffset.y;

	HRESULT hr = s_pScreen->Blt(&rtOffset, s_pBackScreen, rt, DDBLT_WAIT,NULL);
	if (FAILED(hr))
	{
		ASSERT(0);
		return false;
	}

	DDSURFACEDESC desc;
	memset(&desc, 0x00, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	hr = s_pBackScreen->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0);
    if (FAILED(hr))
	{
		return false;
	}

	s_pBuf = (BYTE *) desc.lpSurface;
	return true;
}

void CDirectDrawMapping::MoveScreen(int nOffsetX, int nOffsetY)
{
	int nDestLeft = 0, nDestTop = 0, nSrcLeft = 0, nSrcTop = 0, nSrcWidth = 0, nSrcHeight = 0;

	if(nOffsetX > 0 && nOffsetY > 0) // +,+
	{
		nDestLeft = nOffsetX;
		nDestTop = nOffsetY;

		nSrcLeft = 0;
		nSrcTop = 0;

		nSrcWidth = m_nViewWidth - nOffsetX;
		nSrcHeight = m_nViewHeight - nOffsetY;
	}
	else
	if(nOffsetX > 0 && nOffsetY <= 0)//  +,-
	{
		nOffsetY = abs(nOffsetY);

		nDestLeft = nOffsetX;
		nDestTop = 0;

		nSrcLeft = 0;
		nSrcTop = nOffsetY;

		nSrcWidth = m_nViewWidth - nOffsetX;
		nSrcHeight = m_nViewHeight;
	}
	else
	if(nOffsetX <= 0 && nOffsetY > 0)// -,+
	{
		nOffsetX = abs(nOffsetX);

		nDestLeft = 0;
		nDestTop = nOffsetY;

		nSrcLeft = nOffsetX;
		nSrcTop = 0;

		nSrcWidth = m_nViewWidth;
		nSrcHeight = m_nViewHeight - nOffsetY;
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

		nSrcWidth = m_nViewWidth;
		nSrcHeight = m_nViewHeight;
	}

	BitBltCaptureScreen(nDestLeft, nDestTop, nSrcLeft, nSrcTop, nSrcWidth, nSrcHeight);
}

void CDirectDrawMapping::BitBltCaptureScreen(int nDestLeft, int nDestTop, int nSrcLeft, int nSrcTop, int nSrcWidth, int nSrcHeight)
{
	for(int nSrcY=nSrcTop, nDestY=nDestTop ; nSrcY<nSrcHeight ; nSrcY++, nDestY++)
	{
		for(int nSrcX=nSrcLeft, nDestX=nDestLeft ; nSrcX<nSrcWidth; nSrcX++, nDestX++)
		{
			UINT16 *pPixel = (UINT16 *) (s_pCaptureBuf + nSrcY * CReDraw::s_nPitch + nSrcX * CReDraw::s_nBpp);
			DrawPixel(nDestX, nDestY, *pPixel);
		}
	}
}

void CDirectDrawMapping::CaptureFullScreen()
{
	memcpy(s_pCaptureBuf, s_pBuf, sizeof(BYTE) * 2 * m_nViewWidth * m_nViewHeight);
}

void CDirectDrawMapping::Release()
{
	if (s_pBackScreen)
	{
		s_pBackScreen->Release();
	}
	if (s_pScreen)
	{
		s_pScreen->Release();
	}
	if (m_pDraw)
	{
		m_pDraw->Release();
	}

	if(s_pCaptureBuf)
	{
		delete [] s_pCaptureBuf;
		s_pCaptureBuf = 0;
	}
}

#ifdef ROADEASY_PC
void CDirectDrawMapping::SetViewOffset(const int nOffsetX , const int nOffsetY )
{
	m_ptViewOffset.x = nOffsetX;
	m_ptViewOffset.y = nOffsetY;
}
#endif



void CDirectDrawMapping::DrawPixel(int nX, int nY, UINT16 col16)
{
	*((UINT16 *) (s_pBuf + nY * CReDraw::s_nPitch + nX * CReDraw::s_nBpp)) = col16;
}

UINT16 CDirectDrawMapping::GetPixel(int nX, int nY)
{
	return *((UINT16 *) (s_pBuf + nY * CReDraw::s_nPitch + nX * CReDraw::s_nBpp));
}

void CDirectDrawMapping::DrawPixelTrans(int nX, int nY, UINT16 col16)
{	
	UINT16 col;
	UINT16 *pBuf = ((UINT16 *) (s_pBuf + nY * CReDraw::s_nPitch + nX * CReDraw::s_nBpp));	
		
	col = (((*pBuf & 0xf800) >> 2) & 0xf800) | (((*pBuf & 0x07e0) >> 2) & 0x07e0) | (((*pBuf & 0x1f) >> 2) & 0x1f);

	*pBuf = col + (((((col16 & 0xf800) >> 1) + ((col16 & 0xf800) >> 2)) & 0xf800) | 
				  ((((col16 & 0x07e0) >> 1)  + ((col16 & 0x07e0) >> 2)) & 0x07e0) | 
				  ((((col16 & 0x1f)   >> 1)  + ((col16 & 0x1f)   >> 2)) & 0x1f));
}

HDC CDirectDrawMapping::GetDirectDC()
{
	s_pBackScreen->Unlock(0);

	HDC hDC;
	HRESULT hr = s_pBackScreen->GetDC(&hDC);
	if (FAILED(hr))
	{
		return 0;
	}
	return hDC;
}

void CDirectDrawMapping::ReleaseDirectDC(HDC hDC)
{
	HRESULT hr = s_pBackScreen->ReleaseDC(hDC);
	if (FAILED(hr))
	{
		ASSERT(0);
	}

	DDSURFACEDESC desc;
	memset(&desc, 0x00, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	hr = s_pBackScreen->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0);
    if (FAILED(hr))
	{
		return;
	}

	s_pBuf = (BYTE *) desc.lpSurface;
}
