#pragma once

class CReDraw;
class __declspec(dllexport) CShape
{
public:
	
	CShape() {};
	~CShape() {};

	inline UINT16 GetViewColor(COLORREF col);
	inline UINT16 GetAlphaColor(UINT16 col16, float fAlpha);
	inline UINT16 GetAlphaColor(int nX, int nY, COLORREF col, float fAlpha);
	inline UINT16 GetAlphaColor(int nX, int nY, UINT16 col16, float fAlpha);
	inline UINT16 GetAntiAliasColor(UINT16 col, UINT16 colBK, float fDistance);

	inline UINT16 GetPixel(int nX, int nY);
	inline void   DrawPixel(int nX, int nY, UINT16 col16);

	inline void DrawPixelTrans(int nX, int nY, UINT16 col16);

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
};
