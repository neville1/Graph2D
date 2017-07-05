#pragma once

class __declspec(dllexport) CShape
{
public:
	
	CShape() {};
	virtual ~CShape() {};

	UINT16 GetViewColor(COLORREF col);
	UINT16 GetViewColor565(COLORREF col);
	UINT16 GetAlphaColor(UINT16 col16, float fAlpha);
    UINT16 GetAlphaColor(int nX, int nY, COLORREF col, float fAlpha);
    UINT16 GetAlphaColor(int nX, int nY, UINT16 col16, float fAlpha);
	inline UINT16 GetAntiAliasColor(UINT16 col, UINT16 colBK, float fDistance);

	UINT16 GetPixel(int nX, int nY);
	void   DrawPixel(int nX, int nY, UINT16 col16);
	void	DrawPixelTrans(int nX, int nY, UINT16 col16);
	void SmoothPixel(int nX, int nY, UINT16 *pCol, int nDest, bool bLeft);
        
    HDC GetDirectDC();
	void ReleaseDirectDC(HDC hDC);

protected:
	inline bool IsOutX(int nX);
	inline bool IsOutY(int nY);
	inline bool IsOutPoint(int nX, int nY);
	inline void InViewPos(int& nX, int& nY);
protected:
	enum DirType { Non = 0, Vertical = 1, Horizontal = 2 };

public:
	static int s_BitCount;
	static int s_RShift;
	static int s_GShift;
	static int s_BShift;
	static int s_RShift2;
	static int s_GShift2;
	static int s_GMark;
	static int s_BMark;


};
