#pragma once

#include "gx.h"
class __declspec(dllexport) CGAPI
{
public:
	CGAPI();
	~CGAPI() {}

	HRESULT Create(HWND hWnd);
	HRESULT Release();

	HRESULT	Flip();
	BYTE * GetBuffer() { return m_pBuffer; }
	GXDisplayProperties& GetDisplayProperties() { return m_gxdp; }

protected:
	GXDisplayProperties m_gxdp;
	BYTE * m_pBuffer;
	UINT32 m_nVideoSize;
	HWND m_hWnd;
};
