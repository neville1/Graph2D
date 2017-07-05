#include "StdAfx.h"
#include "Text.h"
#include "ReDraw.h"
#include "FontDefine.h"
#include "LocalCode.h"
#include "BaseTool.h"
#include "SysInfo.h"

#pragma warning(disable: 4996)



#define FAMILYMAX   12 
 
typedef struct { 
    int nNumFonts; 
    TCHAR szFontFamily[LF_FACESIZE]; 
} FONTFAMSTRUCT; 
typedef FONTFAMSTRUCT *PFONTFAMSTRUCT;
int sFamilyCnt = 0; 
FONTFAMSTRUCT ffs[FAMILYMAX];

vector<CStringA> vFontTmp;

int CALLBACK FontFamilyCallback (CONST LOGFONT *lplf,    
                                 CONST TEXTMETRIC *,/*lpntm*/
                                 DWORD /*nFontType*/, LPARAM /*lParam*/) 
{   
    int rc = 1;   
   
    // Stop enumeration if array filled.   
    if (sFamilyCnt >= FAMILYMAX)   
        return 0;   
    // Copy face name of font.   
    lstrcpy (ffs[sFamilyCnt++].szFontFamily, lplf->lfFaceName);   
    TRACE(lplf->lfFaceName);
	TRACE(_T("\r\n"));

	CStringA m_FontTmp;
	m_FontTmp = lplf->lfFaceName;

	vFontTmp.push_back(m_FontTmp);
    return rc;   
}   

CText::CText() :  m_pCode(0)
{
	SetTextColor(RGB(0,0,0));
	SetBoundaryColor(RGB(255,255,255));
	m_bDebugMode = false;
}

CText::~CText()
{}

void CText::SetLocalCode(CLocalCode *pCode)
{
	m_pCode = pCode;
}

void CText::Initial()
{
 	HDC hDC = ::GetDC(NULL);  

	wchar_t chName[255] = {0};

	memset(m_chFontName, 0x00, sizeof(char)*40);

	CString strFontFile;
	strFontFile.Format(_T("%s\\kaiu.ttf"), CSysInfo::GetAppPath());
	wcscpy(chName, strFontFile.GetBuffer());

	TRACE(_T("===== 載入字型檔： %s\r\n"),chName);


	TRACE(_T("===== 加新字型前 =====\r\n"));
	int rc = 0;
	//DWORD dwError = ::GetLastError();
    rc = EnumFontFamilies ((HDC)hDC, (LPTSTR)NULL, FontFamilyCallback, 0);   	

	vector<CStringA> vFontOri;
	vFontOri = vFontTmp;
	vFontTmp.clear();

	int result = -999;
	DWORD dwError3 = ::GetLastError();
	result = ::AddFontResource(chName);
	DWORD dwError = ::GetLastError();
	::SendMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
	DWORD dwError2 = ::GetLastError();

	TRACE(_T("===== 加新字型後 =====\r\n"));
    rc = EnumFontFamilies ((HDC)hDC, (LPTSTR)NULL, FontFamilyCallback, 0);   	

	vector<CStringA> vFontNew;
	vFontNew = vFontTmp;

	if(vFontNew.size() <= vFontOri.size())
	{
		TRACE(_T("===== 加載字型失敗 Result = %d, Error Code = %d,%d,%d =====\r\n"), result, dwError,dwError2,dwError3);
	}
	else
	{
		for(int i=0;i< (int) vFontNew.size();i++)
		{
			for(int j=0;j< (int) vFontOri.size();j++)
			{
				if(vFontNew[i] == vFontOri[j])
				{
					vFontNew[i] = _T("");
				}
			}
		}

		for(int i=0;i< (int) vFontNew.size();i++)
		{
			if(vFontNew[i] != _T(""))
			{
				strcpy(m_chFontName, vFontNew[i]);
				TRACE(_T("===== 新字型名稱：%s ===== \r\n"),m_chFontName);
			}
		}
	}


	::ReleaseDC(NULL, hDC);


	int nFontSize[3] = { 0 };

	nFontSize[0] = m_pCode->GetFontSize(Font16).cx;
	nFontSize[1] = m_pCode->GetFontSize(Font20).cx;
	nFontSize[2] = m_pCode->GetFontSize(Font24).cx;


	if( strlen(m_chFontName) == 0 )
	{
		strcpy(m_chFontName, "微軟正黑體");
	}

	CBaseTool tool;
	CString m_FontTmp = tool.GetUnicodeString(m_chFontName);

	for (int i = 0; i < sizeof(nFontSize) / sizeof(int); ++i)
	{
		m_font[i] = ::CreateFont(nFontSize[i] + 4, 0, 0, 0,
					FW_MEDIUM, FALSE, FALSE, 0, 
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
					DEFAULT_PITCH | FF_SWISS, m_FontTmp);//DEFAULT_PITCH | FF_SWISS, _T("新細明體"));
	}

	for (int i = 0; i < sizeof(nFontSize) / sizeof(int); ++i)
	{
		m_bfont[i] = ::CreateFont(nFontSize[i] + 4, 0, 0, 0,                     
					FW_BOLD, FALSE, FALSE, 0, 
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
					DEFAULT_PITCH | FF_SWISS, m_FontTmp);//DEFAULT_PITCH | FF_SWISS, _T("新細明體"));
	}
}

void CText::UnInitial()
{
	for(int i=0;i<3;i++)
	{
		if( m_font[i] != NULL)
		{
			if(::DeleteObject(m_font[i]) == 0)
				TRACE(_T("delete m_font object failed\r\n"));
			m_font[i] = NULL;
		}
		if( m_bfont[i] != NULL)
		{
			if(::DeleteObject(m_bfont[i]) == 0)
				TRACE(_T("delete m_bfont object failed\r\n"));

			m_bfont[i] = NULL;
		}
	}

	CString strFontFile;
	strFontFile.Format(_T("%s\\Font.ttf"), CSysInfo::GetAppPath());
	TRACE(_T("===== 移除字型檔： %s=====\r\n"),strFontFile);

	int rc = 0;
	HDC hDC = GetDC(NULL);
  	TRACE(_T("===== 移除字型前 =====\r\n"));
    rc = EnumFontFamilies ((HDC)hDC, (LPTSTR)NULL, FontFamilyCallback, 0);   	


	bool b_tag = false;
	if (0 == RemoveFontResource(strFontFile))
		TRACE(_T("remove font fail")); 
	::SendMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
	Sleep(1000);

	
	TRACE(_T("===== 移除字型後 =====\r\n"));
    rc = EnumFontFamilies ((HDC)hDC, (LPTSTR)NULL, FontFamilyCallback, 0);   	
	::ReleaseDC(NULL, hDC);
}

RECT CText::GetTextRect(const char *pText)
{
	RECT rtBoundary = {0, 0, 0, 0};
	if (strlen(pText) == 0)
	{
		return rtBoundary;
	}

	CBaseTool tool;
	CString strText = tool.GetUnicodeString(pText);

	HDC hDC = GetDirectDC();
	::DrawText(hDC, strText, strText.GetLength(), &rtBoundary, DT_CALCRECT);
	ReleaseDirectDC(hDC);

	return rtBoundary;
}

void CText::DrawStr(const char *pText,int nX, int nY, HFONT hFont)
{
	if (strlen(pText) == 0)
	{
		return;
	}

	CBaseTool tool;
	CString strText = tool.GetUnicodeString(pText);

	HDC hDC = GetDirectDC();
	if (hDC == NULL)
	{
		int a = 0;
	}
	::SetBkMode(hDC, TRANSPARENT);

	::SetTextColor(hDC, m_colText);
	HGDIOBJ hbj = ::SelectObject(hDC, hFont);

	RECT rtText = {0,0,0,0};
	::DrawText(hDC, strText, strText.GetLength(), &rtText, DT_CALCRECT);

	rtText.left += nX;
	rtText.top  += nY;
	rtText.right += nX;
	rtText.bottom += nY;

	::DrawText(hDC, strText, strText.GetLength(), &rtText, DT_NOCLIP | DT_VCENTER);
	::SelectObject(hDC, hbj);

	ReleaseDirectDC(hDC);
}


void CText::DrawStr(const char *pText,Font_Size1 nFontId, int nX, int nY, Text_Type /*tType*/, bool bAdjust)
{
	if (strlen(pText) == 0)
	{
		return;
	}

	CBaseTool tool;
	CString strText = tool.GetUnicodeString(pText);

	HDC hDC = GetDirectDC();
	HGDIOBJ hbj;

	if (hDC == NULL)
	{
		int a = 0;
	}
	::SetBkMode(hDC, TRANSPARENT);

	::SetTextColor(hDC, m_colText);
	hbj = ::SelectObject(hDC, m_font[nFontId]);

	RECT rtText = {0,0,0,0};
	::DrawText(hDC, strText, strText.GetLength(), &rtText, DT_CALCRECT);


#ifdef RES_800X480
	if(bAdjust)
	{
		CStringA strAText;
		strAText = pText;
		SIZE szText = GetStrSize(strAText.GetBuffer() , nFontId);
		int nOffsetX = (szText.cx - (rtText.right - rtText.left)) / 2;
		int nOffsetY = (szText.cy - (rtText.bottom - rtText.top)) / 2;
		rtText.left    += nOffsetX;
		rtText.right   += nOffsetX;
		rtText.top     += nOffsetY;
		rtText.bottom  += nOffsetY;
	}
#endif

	rtText.left += nX;
	rtText.top  += nY;
	rtText.right += nX;
	rtText.bottom += nY;

	::DrawText(hDC, strText, strText.GetLength(), &rtText, DT_SINGLELINE);

	::SelectObject(hDC, hbj);
	ReleaseDirectDC(hDC);

}

void CText::DrawScaleStr(HDC hDC, const char *pText,Font_Size1 nFontId, RECT& rtRect)
{
	CBaseTool tool;
	CString strText = tool.GetUnicodeString(pText);
	HGDIOBJ hbj;

	::SetTextColor(hDC, m_colText);
	hbj = ::SelectObject(hDC, m_font[nFontId]);

	::DrawText(hDC, strText, strText.GetLength(), &rtRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	::SelectObject(hDC, hbj);
}

bool CText::GetCopyPos(const char *pStr,const char *pSubStr,int & nStartPos,int & nEndPos)
{

	char* pStartPos = strstr((char *)pStr,(char *)pSubStr);
	if(pStartPos==NULL)
	{
		return false;
	}
	nStartPos = (int) (pStartPos - pStr)/(int) sizeof(char);
	int i=0;
	while(pSubStr[i]!='\0')
	{
		i++;
	}
	nEndPos = nStartPos+i;
	return true;
}

void CText::DrawScaleHighStr(HDC hDC, const char *pText,Font_Size1 nFontType, RECT& rtRect, const char *pHighText,COLORREF colHigh)
{
	int nStartPos;
	int nEndPos;
	bool bGetHighPos = GetCopyPos(pText,pHighText,nStartPos,nEndPos);

	SetTextColor(RGB(255,255,255));
	
	int nStrLen  = (int) strlen(pText);
	int nTextCount = 0;
	SIZE szFont;
	if(m_bDebugMode)
	{
		nTextCount = nStrLen;
		szFont.cx = 32;
		szFont.cy = 32;
	}
	else
	{
		nTextCount = m_pCode->GetStrLen(pText);
		szFont = m_pCode->GetFontSize(nFontType);
	}

	vector<RECT> vRtText(nTextCount);

	int iOneTextW= szFont.cx / 2;
	int iOneTextH= szFont.cy;

	int nTextIndex = 0;
	for (int i = 0; i < nStrLen; ++i)
	{
		int iLeftTopX=rtRect.left+i*iOneTextW;

		if(!m_bDebugMode)
		{
			if (m_pCode->IsText(pText + i))
			{
				i++;
			}		
		}
		int iRightDownX=rtRect.left+(i+1)*iOneTextW;
		vRtText[nTextIndex].left  = iLeftTopX;
		vRtText[nTextIndex].top   = rtRect.top;
		vRtText[nTextIndex].right = iRightDownX;
		vRtText[nTextIndex].bottom = rtRect.bottom;	
		nTextIndex++;
	}

	char chText[3];
	nTextIndex = 0;
	for (int i = 0 ; i < nStrLen; ++i)
	{
		if(bGetHighPos)
		{
			if(i>=nStartPos && i<nEndPos)
			{
				SetTextColor(colHigh);
			}
			else
			{
				SetTextColor(RGB(255,255,255));
			}
		}
		if(!m_bDebugMode)
		{
			if (m_pCode->IsText(pText + i))
			{
				chText[0] = pText[i];
				chText[1] = pText[i+1];
				chText[2] = '\0';
				i++;
			}
			else
			{
				chText[0] = pText[i];
				chText[1] = '\0';
			}
		}
		DrawScaleStr(hDC, chText, nFontType, vRtText[nTextIndex]);
		nTextIndex++;
	}
	SetTextColor(RGB(255,255,255));
}

void CText::DrawRectStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay, Font_Weight nFontWeight, Text_Type /*tType*/)
{
	if (strlen(pText) == 0)
	{
		return;
	}

	CStringA sNewText;
	if(m_bDebugMode)
	{
		sNewText = pText;
	}
	else
	{
		sNewText = InFixedLengthStr(pText, nFontType, (rStrRect.right - rStrRect.left));
	}
	

	CBaseTool tool;
	CString strText = tool.GetUnicodeString(sNewText.GetBuffer()/*pText*/);

	HDC hDC = GetDirectDC();
	HGDIOBJ hbj;

	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, m_colText);

	switch( nFontWeight )
	{
	case Font_Thin :
		hbj = ::SelectObject(hDC, m_font[nFontType]);
		break;
	case Font_Bold :
		hbj = ::SelectObject(hDC, m_bfont[nFontType]);
		break;
	default:
		break;
	}

	UINT nFormat = 0;
	switch(tShowWay)
	{
		case LEFT_SHOW:nFormat = DT_LEFT; break;
		case RIGHT_SHOW:nFormat = DT_RIGHT; break;
		case CENTER_SHOW: nFormat = DT_CENTER; break;
	}

	::DrawText(hDC, strText, strText.GetLength(), &rStrRect, DT_SINGLELINE | DT_VCENTER | nFormat);

	::SelectObject(hDC, hbj);
	ReleaseDirectDC(hDC);
    
}

void CText::DrawRectHighStr(const char *pText,Font_Size1 nFontType, RECT& rStrRect,TextShowWay tShowWay,const char *pHighText,COLORREF colHigh,Text_Type /*tType*/ )
{
	if (!pText || !pHighText)
	{
		return;
	}
	if (strlen(pText) == 0 || strlen(pHighText) == 0)
	{
		return;
	}

	if (strlen(pText) == 0)
	{
		return;
	}

	RECT rtRect = rStrRect;

	CStringA sNewText;
	SIZE sSize;
	if(m_bDebugMode)
	{
		sNewText = pText;
		sSize.cx = 0;
		sSize.cy = 0;
	}
	else
	{
		sNewText = InFixedLengthStr(pText, nFontType, (rtRect.right - rtRect.left));
		sSize = GetStrSize(sNewText, nFontType);
	}

	int nWidth = rtRect.right - rtRect.left;

	switch(tShowWay)
	{
	case LEFT_SHOW:
		{
			rtRect.left += rtRect.left;
		}
		break;
	case RIGHT_SHOW:
		{
			rtRect.left = rtRect.left + (nWidth - sSize.cx);
		}
		break;
	case CENTER_SHOW: 
		{
			rtRect.left = rtRect.left + ((nWidth - sSize.cx)/2);
		}
		break;
	}

	HDC hDC = GetDirectDC();

	::SetBkMode(hDC, TRANSPARENT);
	
	DrawScaleHighStr(hDC, sNewText,nFontType, rtRect, pHighText,colHigh);

	ReleaseDirectDC(hDC);
}


CStringA CText::InFixedLengthStr(const char * pText ,Font_Size1 nFontType , int nLength)
{
	SIZE sz = m_pCode->GetStrSize(pText, nFontType);
	int nStrLen = CStringA(pText).GetLength();
	if( sz.cx > nLength)
	{
		float fRatio =  (float)nLength / sz.cx;
		int nLength = (int)(nStrLen*fRatio*0.65);
		CStringA sNewStr;
		int nNewWordIndex = 0;
		while( sNewStr.GetLength() < nLength)
		{
			if(m_pCode->IsText(pText + nNewWordIndex))
			{
				sNewStr += CStringA(pText).Mid(nNewWordIndex,2);
				nNewWordIndex+=2;
			}
			else
			{
				sNewStr += CStringA(pText).Mid(nNewWordIndex,1);
				nNewWordIndex+=1;
			}
		}
		return sNewStr+"...";
	}
	return CStringA(pText);

}

bool CText::IsText(const char *pStr)
{
	return m_pCode->IsText(pStr);
}

bool CText::IsText(const char *pStr,const int nIndex)
{
	return m_pCode->IsText(pStr,nIndex);
}

SIZE CText::GetStrSize(const char *pStr, Font_Size1 type)
{
	return m_pCode->GetStrSize(pStr, type);
}

int CText::GetStrLength(const char *pStr)
{
	return m_pCode->GetStrLen(pStr);
}

char * CText::ReverseStr(const char *pStr)
{
	return m_pCode->ReverseStr(pStr);
}
char * CText::GetLowerCaseString(const char *pStr)
{
	return m_pCode->GetLowerCaseString(pStr);
}
SIZE CText::GetFontSize(Font_Size1 type)
{
	return m_pCode->GetFontSize(type);
}

SIZE CText::GetNumFontSize(Font_Size1 type)
{
	return m_pCode->GetNumFontSize(type);
}

int CText::GetStrokeOfText(const char *pStr)
{
	return m_pCode->GetStrokeOfText(pStr);
}

int CText::GetStrokeOfTextCount()
{
	return m_pCode->GetStrokeOfTextCount();
}
