#pragma once

class __declspec(dllexport) CShape
{
public:
	
	CShape() {};
	~CShape() {};

	static void SetRotate(bool bRotate);
	inline void PreLocate(int& nX, int& nY);
	inline UINT16 GetViewColor(COLORREF col);
    UINT16 GetAlphaColor(int nX, int nY, UINT16 col16, float fAlpha);
	inline UINT16 GetAntiAliasColor(UINT16 col, UINT16 colBK, float fDistance);

	inline UINT16 GetPixel(int nX, int nY);
	inline void   DrawPixel(int nX, int nY, UINT16 col16);
	void SmoothPixel(int nX, int nY);

protected:
	inline bool IsOutX(int nX);
	inline bool IsOutY(int nY);
	inline bool IsOutPoint(int nX, int nY);
	inline void InViewPos(int& nX, int& nY);

public:
	static bool s_bRotate;
};
