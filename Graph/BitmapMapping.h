#pragma once
#include "videomapping.h"

class CBitmapMapping : public CVideoMapping
{
public:
	CBitmapMapping(void);
	~CBitmapMapping(void);

	virtual bool Initial(HWND hWnd, int nWidth, int nHeight);
	virtual bool Flip();
	virtual bool Flip(LPRECT rt);
	virtual void MoveScreen(int nOffsetX, int nOffsetY);
	virtual void CaptureFullScreen();
	virtual void BitBltCaptureScreen(int, int, int, int, int, int);
	virtual void Release();

	virtual void DrawPixel(int nX, int nY, UINT16 col16);
	virtual UINT16 GetPixel(int nX, int nY);
	virtual void DrawPixelTrans(int nX, int nY, UINT16 col16);

	virtual char* GetVersion();

	virtual HDC GetDirectDC();
	virtual void ReleaseDirectDC(HDC hDC);

	#ifdef ROADEASY_PC
	virtual void SetViewOffset(const int nStartDrawX , const int nStartDrawY);
	#endif

private:
	void Reset();
	void SetDrawRect(LPRECT lpRect);

private:
	static SIZE s_szBmp;
	static BYTE * s_pBuf;
	static BYTE * s_pCaptureBuf;

	HWND m_hWnd;
	HDC hDCWnd;
	HBITMAP m_hBmp;
	HDC m_hMemDC;
	HGDIOBJ m_hOldBmp;
};
