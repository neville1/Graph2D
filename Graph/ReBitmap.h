#pragma once

#include "Shape.h"


class CReDraw;
class __declspec(dllexport) CReBitmap : public CShape
{
public:
	CReBitmap();
	CReBitmap(int nWidth, int nHeight);
	CReBitmap(int nWidth, int nHeight, BYTE *pData);
	virtual ~CReBitmap();

	void SetData(int nWidth,int nHeight);

	bool LoadBmp(LPCWSTR lpBitmapName);
	bool LoadBmp(HBITMAP hBmp);
	bool LoadBmp(HINSTANCE hInstance, LPCWSTR lpBitmapName);

	void ClearData();

	bool TransparentMask(int nX, int nY, COLORREF colMask, COLORREF col,  float fAlpha);

	bool BitBlt(int nX, int nY); 
	bool MaskBitBlt(int nX, int nY);	
	bool MaskBitBlt(int nX, int nY, COLORREF col);
	bool TransparentBitBlt(int nX, int nY, float fAlpha);
	bool TransparentMaskBitBlt(int nX, int nY, COLORREF col, float fAlpha);
	bool SplitBitBlt(int nX, int nY, int nOffset, COLORREF col);
	SIZE GetBmpSize() {return m_szBmp;}

	BYTE * GetBmpData() { return m_pData; };
	void SetBmpData(BYTE *pData) { m_pData = pData ;};

private:
	HBITMAP m_hBmp;
	SIZE   m_szBmp;
	int    m_nBmpPitch;
	BYTE  *m_pData;
};
