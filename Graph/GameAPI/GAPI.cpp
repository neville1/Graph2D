#include "StdAfx.h"
#include "GAPI.h"


CGAPI::CGAPI() : m_hWnd(0), m_nVideoSize(0), m_pBuffer(0)
{}

HRESULT CGAPI::Create(HWND hWnd)
{
	if(!GXOpenDisplay(hWnd, GX_FULLSCREEN))
	{
		return ::GetLastError();
	}

	m_gxdp = GXGetDisplayProperties();
	m_nVideoSize = m_gxdp.cxWidth * m_gxdp.cyHeight * m_gxdp.cBPP / 8;

	m_hWnd = hWnd;
	if(m_pBuffer)
	{
		return S_OK;
	}
	m_pBuffer = (BYTE *) malloc(m_nVideoSize);

	if (!m_pBuffer)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}

HRESULT CGAPI::Release()
{
	if(!GXCloseDisplay())
	{
		return ::GetLastError();
	}

	if(m_pBuffer) 
	{
		free(m_pBuffer);
	}
	return S_OK;
}

HRESULT	CGAPI::Flip()
{
	BYTE *pDisplayMemory = (BYTE *) GXBeginDraw();
	if(!pDisplayMemory)
	{
		return ::GetLastError();
	}

	memcpy(pDisplayMemory, m_pBuffer, m_nVideoSize);

	if(!GXEndDraw())
	{
		return ::GetLastError();
	}
	return S_OK;
}