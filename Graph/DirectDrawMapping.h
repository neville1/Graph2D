#pragma once
#include "videomapping.h"



class CDirectDrawMapping : public CVideoMapping
{
public:
	CDirectDrawMapping(void);
	~CDirectDrawMapping(void);

	virtual char* GetVersion();

	virtual bool Initial(HWND hWnd, int nWidth, int nHeight);
	virtual bool Flip();
	virtual bool Flip(LPRECT rt);
	virtual void MoveScreen(int nOffsetX, int nOffsetY);
	virtual void CaptureFullScreen();
	virtual void BitBltCaptureScreen(int nDestLeft, int nDestTop, int nSrcLeft, int nSrcTop, int nSrcWidth, int nSrcHeight);
	virtual void Release();

	virtual void DrawPixel(int nX, int nY, UINT16 col16);
	virtual UINT16 GetPixel(int nX, int nY);
	virtual void DrawPixelTrans(int nX, int nY, UINT16 col16);


	virtual HDC GetDirectDC();
	virtual void ReleaseDirectDC(HDC hDC);

	#ifdef ROADEASY_PC
	virtual void SetViewOffset(const int nStartDrawX , const int nStartDrawY);
	#endif


private:
	static LPDIRECTDRAWSURFACE s_pScreen;
	static LPDIRECTDRAWSURFACE s_pBackScreen;

	static BYTE *s_pBuf;
	static BYTE * s_pCaptureBuf;

	LPDIRECTDRAW m_pDraw;
	DDPIXELFORMAT m_pixelFormat;
	DDSURFACEDESC m_SurfInfo;

	HWND m_hWnd;
	RECT m_rtView;
	RECT m_rtDest;
	POINT m_ptViewOffset;	

	int m_nViewWidth;
	int m_nViewHeight;
};
