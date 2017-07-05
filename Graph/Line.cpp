#include "StdAfx.h"
#include "ReDraw.h"
#include "Line.h"
#include <vector>

inline void CLine::SetZBufBit(UINT8 *pZBuf,int nX, int nY)
{
	int	pos = (nX + nY * 480);
	int	bit = pos & 0x07;
	pos >>= 3;	//	/8
	
	*(pZBuf + pos) |= (0x01 << bit);
}

inline bool CLine::GetZBufBit(UINT8 *pZBuf,int nX, int nY)
{
	int	pos = (nX + nY * 480);
	int	bit = pos & 0x07;
	pos >>= 3;	//	/8
		
	return ((UINT8)(*(pZBuf + pos) & (UINT8)(0x01 << bit))) ? true : false;	
}

void CLine::DrawLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{
	DrawDotLine(nStartX, nStartY, nEndX, nEndY, INT_MAX, col);
}

void CLine::DrawLineZBuf(UINT8 *pZBuf,int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{	
	
	
	if (!SplitLine(nStartX, nStartY, nEndX, nEndY))
	{
		return;
	}

	UINT16 col16 = GetViewColor(col);
	int nXcr, nYcr;
	int nDx, nDy;
	if( nStartX > nEndX ) 
	{ 
		nXcr = -1; 
		nDx = nStartX - nEndX; 
	} 
	else 
	{ 
		nXcr = 1;
		nDx = nEndX - nStartX; 
	}	
	if( nStartY > nEndY ) 
	{ 
		nYcr = -1;
		nDy = nStartY - nEndY; 
	} 
	else 
	{ 
		nYcr = 1;
		nDy = nEndY - nStartY;
	}	

	int CurrentX = nStartX;
	int CurrentY = nStartY;
	if( nDx >= nDy )
	{           
		int dPr 	= nDy << 1;   						
		int dPru 	= dPr - (nDx << 1);				
		int P 		= dPr - nDx;		

		for ( ; nDx >= 0; nDx-- )							
		{
			if ( !GetZBufBit(pZBuf, CurrentX, CurrentY) )		
			{				
				DrawPixelTrans( CurrentX, CurrentY, col16);
				SetZBufBit(pZBuf, CurrentX, CurrentY);
			}
			
			if (P > 0)                              
			{ 
				CurrentX += nXcr;
				CurrentY += nYcr;
				P += dPru;							
			}
			else									
			{
				CurrentX += nXcr;					
				P += dPr;								
			}
		}		
	}
	else			
	{
		int dPr 	= nDx << 1;   						
		int dPru 	= dPr - ( nDy << 1 );    				
		int P 		= dPr - nDy;					

		for( ; nDy >= 0; nDy-- )							
		{
			if ( !GetZBufBit(pZBuf, CurrentX, CurrentY) )		
			{	
				DrawPixelTrans( CurrentX, CurrentY, col16);
				SetZBufBit(pZBuf, CurrentX, CurrentY);
			}
			
			if ( P > 0 )                              
			{ 
				CurrentX += nXcr;					
				CurrentY += nYcr;				
				P += dPru;							
			}
			else									
			{
				CurrentY += nYcr;					
				P += dPr;								
			}
		}		
	}
}

void CLine::DrawLineTrans(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{	
	//int nStep = INT_MAX;	
	if (!SplitLine(nStartX, nStartY, nEndX, nEndY))
	{
		return;
	}

	UINT16 col16 = GetViewColor(col);
	int nXcr, nYcr;
	int nDx, nDy;
	if( nStartX > nEndX ) 
	{ 
		nXcr = -1; 
		nDx = nStartX - nEndX; 
	} 
	else 
	{ 
		nXcr = 1;
		nDx = nEndX - nStartX; 
	}	
	if( nStartY > nEndY ) 
	{ 
		nYcr = -1;
		nDy = nStartY - nEndY; 
	} 
	else 
	{ 
		nYcr = 1;
		nDy = nEndY - nStartY;
	}	

	int CurrentX = nStartX;
	int CurrentY = nStartY;
	if( nDx >= nDy )
	{           
		int dPr 	= nDy << 1;   						
		int dPru 	= dPr - (nDx << 1);				
		int P 		= dPr - nDx;		

		for ( ; nDx >= 0; nDx-- )
		{
			DrawPixelTrans( CurrentX, CurrentY, col16);
			if (P > 0)
			{ 
				CurrentX += nXcr;
				CurrentY += nYcr;
				P += dPru;							
			}
			else									
			{
				CurrentX += nXcr;					
				P += dPr;								
			}
		}		
	}
	else			
	{
		int dPr 	= nDx << 1;   						
		int dPru 	= dPr - ( nDy << 1 );    				
		int P 		= dPr - nDy;					

		for( ; nDy >= 0; nDy-- )
		{
			DrawPixelTrans( CurrentX, CurrentY, col16);
			if ( P > 0 )
			{ 
				CurrentX += nXcr;					
				CurrentY += nYcr;				
				P += dPru;							
			}
			else									
			{
				CurrentY += nYcr;					
				P += dPr;								
			}
		}		
	}
}

void CLine::DrawDotLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col)
{	
	if (!SplitLine(nStartX, nStartY, nEndX, nEndY))
	{
		return;
	}

	UINT16 col16 = GetViewColor(col);
	int nXcr, nYcr;
	int nDx, nDy;
	if( nStartX > nEndX ) 
	{ 
		nXcr = -1; 
		nDx = nStartX - nEndX; 
	} 
	else 
	{ 
		nXcr = 1;
		nDx = nEndX - nStartX; 
	}	
	if( nStartY > nEndY ) 
	{ 
		nYcr = -1;
		nDy = nStartY - nEndY; 
	} 
	else 
	{ 
		nYcr = 1;
		nDy = nEndY - nStartY;
	}	

	int CurrentX = nStartX;
	int CurrentY = nStartY;
	if( nDx >= nDy )
	{           
		int dPr 	= nDy << 1;   						
		int dPru 	= dPr - (nDx << 1);				
		int P 		= dPr - nDx;		

		bool bDraw = true;
		int nOffset = 0;
		for ( ; nDx >= 0; nDx-- )							
		{
			if (bDraw)
			{
				DrawPixel( CurrentX, CurrentY, col16);
			}
			nOffset++;
			if (nOffset == nStep)
			{
				bDraw = !bDraw;
				nOffset = 0;
			}

			if (P > 0)                              
			{ 
				CurrentX += nXcr;
				CurrentY += nYcr;
				P += dPru;							
			}
			else									
			{
				CurrentX += nXcr;					
				P += dPr;								
			}
		}		
	}
	else			
	{
		int dPr 	= nDx << 1;   						
		int dPru 	= dPr - ( nDy << 1 );    				
		int P 		= dPr - nDy;					

		bool bDraw = true;
		int nOffset = 0;
		for( ; nDy >= 0; nDy-- )							
		{
			if (bDraw)
			{
				DrawPixel( CurrentX, CurrentY, col16);
			}
			nOffset++;
			if (nOffset == nStep)
			{
				bDraw = !bDraw;
				nOffset = 0;
			}

			if ( P > 0 )                              
			{ 
				CurrentX += nXcr;					
				CurrentY += nYcr;				
				P += dPru;							
			}
			else									
			{
				CurrentY += nYcr;					
				P += dPr;								
			}
		}		
	}
}

void CLine::DrawSwitchColLine(int nStartX, int nStartY, int nEndX, int nEndY, int nStep, COLORREF col1, COLORREF col2)
{	
	if (!SplitLine(nStartX, nStartY, nEndX, nEndY))
	{
		return;
	}

	bool bSwitch = false;

	UINT16 col1_16 = GetViewColor(col1);
	UINT16 col2_16 = GetViewColor(col2);

	int nXcr, nYcr;
	int nDx, nDy;
	if( nStartX > nEndX ) 
	{ 
		nXcr = -1; 
		nDx = nStartX - nEndX; 
	} 
	else 
	{ 
		nXcr = 1;
		nDx = nEndX - nStartX; 
	}	
	if( nStartY > nEndY ) 
	{ 
		nYcr = -1;
		nDy = nStartY - nEndY; 
	} 
	else 
	{ 
		nYcr = 1;
		nDy = nEndY - nStartY;
	}	

	int CurrentX = nStartX;
	int CurrentY = nStartY;
	if( nDx >= nDy )
	{           
		int dPr 	= nDy << 1;   						
		int dPru 	= dPr - (nDx << 1);				
		int P 		= dPr - nDx;		

		bool bDraw = true;
		int nOffset = 0;
		for ( ; nDx >= 0; nDx-- )							
		{
			if (bDraw)
			{
				if( bSwitch )
				{
					DrawPixel( CurrentX, CurrentY, col1_16);
				}
				else
				{
					DrawPixel( CurrentX, CurrentY, col2_16);
				}
			}
			nOffset++;
			if (nOffset == nStep)
			{
				bSwitch = !bSwitch;
				nOffset = 0;
			}

			if (P > 0)                              
			{ 
				CurrentX += nXcr;
				CurrentY += nYcr;
				P += dPru;							
			}
			else									
			{
				CurrentX += nXcr;					
				P += dPr;								
			}
		}		
	}
	else			
	{
		int dPr 	= nDx << 1;   						
		int dPru 	= dPr - ( nDy << 1 );    				
		int P 		= dPr - nDy;					

		bool bDraw = true;
		int nOffset = 0;
		for( ; nDy >= 0; nDy-- )							
		{
			if (bDraw)
			{
				if( bSwitch )
				{
					DrawPixel( CurrentX, CurrentY, col1_16);
				}
				else
				{
					DrawPixel( CurrentX, CurrentY, col2_16);
				}
			}
			nOffset++;
			if (nOffset == nStep)
			{
				bSwitch = !bSwitch;
				nOffset = 0;
			}

			if ( P > 0 )                              
			{ 
				CurrentX += nXcr;					
				CurrentY += nYcr;				
				P += dPru;							
			}
			else									
			{
				CurrentY += nYcr;					
				P += dPr;								
			}
		}		
	}
}

void CLine::DrawAntiBoundaryLine(int nStartX, int nStartY, int nEndX, int nEndY, const vector<COLORREF>& vColBK, COLORREF col)
{
	if (!SplitLine(nStartX, nStartY, nEndX, nEndY))
	{
		return;
	}

	
	UINT16 col16 = GetViewColor(col);

	int nColBK = (int)vColBK.size(); 
	vector<UINT16> vColBK16(nColBK);
	for(int i = 0 ; i < nColBK ; i++)
	{
		vColBK16[i] = GetViewColor(vColBK[i]);
	}

	bool bBoundary = false;

	int dx = (nEndX - nStartX);
	int dy = (nEndY - nStartY);
	float fK = 0;

	if (!IsOutPoint(nStartX, nStartY))
	{
		bBoundary = IsRoadLineBoundary(nStartX, nStartY, vColBK16, Non);
		if( bBoundary )
		{
			DrawPixel(nStartX, nStartY, col16);
		}
	}

	UINT16 colBack = 0;
	UINT16 nDestCol = 0;
	if (abs(dx) > abs(dy))
	{
		if (dx < 0)
		{
			swap(nStartX, nEndX);
			swap(nStartY, nEndY);
		}
		fK = (float) dy / (float) dx;		
		float fYt = (float) nStartY + fK;
		float fValue = 0;
		float fDistance = 0;
		for (int nX = nStartX + 1; nX < nEndX; ++nX)
		{	
			bBoundary = IsRoadLineBoundary(nX, (int)fYt, vColBK16, Horizontal);

			if( bBoundary )
			{
				fDistance = (float) (fYt - (int) (fYt));
				fValue = 1.0f - fDistance;

				if (!IsOutPoint(nX, (int) (fYt) + 1))
				{
					nDestCol = GetAntiAliasColor(col16, GetPixel(nX, (int) (fYt) + 1), fDistance);
					DrawPixel(nX, (int) fYt + 1, nDestCol);
				}
				if (!IsOutPoint(nX, (int) fYt))
				{
					nDestCol = GetAntiAliasColor(col16, GetPixel(nX, (int) fYt), fValue);
					DrawPixel(nX, (int) fYt, nDestCol);
				}	
			}
			fYt += fK;
		}
	}
	else
	{
		if (dy < 0)
		{
			swap(nStartX, nEndX);
			swap(nStartY, nEndY);
		}
		fK = (float) dx / (float) dy;
		float fXt = (float) nStartX + fK;
		float fDistance = 0;
		float fValue = 0;

		for (int nY = nStartY + 1; nY < nEndY; ++nY)
		{
			bBoundary = IsRoadLineBoundary((int)fXt, nY, vColBK16, Vertical);

			if( bBoundary )
			{
				fDistance = (float) (fXt - (int) (fXt));
				fValue = 1.0f - fDistance;

				if (!IsOutPoint((int) fXt + 1, nY))
				{
					nDestCol = GetAntiAliasColor(col16, GetPixel((int) fXt + 1, nY), fDistance);
					DrawPixel((int) fXt + 1, nY, nDestCol);
				}
				if (!IsOutPoint((int) fXt, nY))
				{
					nDestCol = GetAntiAliasColor(col16, colBack = GetPixel((int) fXt, nY), fValue);
					DrawPixel((int) fXt, nY, nDestCol);
				}
			}

			fXt += fK;
		}
	}

	if (!IsOutPoint(nEndX, nEndY))
	{
		bBoundary = IsRoadLineBoundary(nEndX, nEndY, vColBK16, Non);

		if( bBoundary )
		{
			DrawPixel( nEndX, nEndY, col16);
		}
	}
}

bool CLine::IsRoadLineBoundary(int nX, int nY, const vector<UINT16>& vColBK16, int nDir)//2009-8-31
{
	for(int i = 0 ; i < (int)vColBK16.size() ; i++)
	{
		if( nDir == Vertical )
		{
			if( GetPixel(nX-1, nY) == vColBK16[i] || GetPixel(nX+1, nY) == vColBK16[i] || 
				GetPixel(nX-2, nY) == vColBK16[i] || GetPixel(nX+2, nY) == vColBK16[i] )
			{
				return true;
			}
		}
		else if( nDir == Horizontal ) 
		{
			if( GetPixel(nX, nY-1) == vColBK16[i] || GetPixel(nX, nY+1) == vColBK16[i] ||
				GetPixel(nX, nY-2) == vColBK16[i] || GetPixel(nX, nY+2) == vColBK16[i] )
			{
				return true;
			}	
		}
		else if( nDir == Non )
		{
			if( GetPixel(nX-1, nY) == vColBK16[i] || GetPixel(nX+1, nY) == vColBK16[i] || 
				GetPixel(nX, nY-1) == vColBK16[i] || GetPixel(nX, nY+1) == vColBK16[i] ||
				GetPixel(nX-2, nY) == vColBK16[i] || GetPixel(nX+2, nY) == vColBK16[i] || 
				GetPixel(nX, nY-2) == vColBK16[i] || GetPixel(nX, nY+2) == vColBK16[i] )
			{
				return true;
			}
		}
	}
	return false;
}

void CLine::DrawAntiAliasLine(int nStartX, int nStartY, int nEndX, int nEndY, COLORREF col)
{

	if (!SplitLine(nStartX, nStartY, nEndX, nEndY))
	{
		return;
	}

	
	UINT16 col16 = GetViewColor(col);
	int dx = (nEndX - nStartX);
	int dy = (nEndY - nStartY);
	float fK = 0;

	if (!IsOutPoint(nStartX, nStartY))
	{
		DrawPixel(nStartX, nStartY, col16);
	}

	UINT16 colBack = 0;
	UINT16 nDestCol = 0;
	if (abs(dx) > abs(dy))
	{
		if (dx < 0)
		{
			swap(nStartX, nEndX);
			swap(nStartY, nEndY);
		}
		fK = (float) dy / (float) dx;		
		float fYt = (float) nStartY + fK;
		float fValue = 0;
		float fDistance = 0;
		for (int nX = nStartX + 1; nX < nEndX; ++nX)
		{	
			fDistance = (float) (fYt - (int) (fYt));
			fValue = 1.0f - fDistance;
			if (!IsOutPoint(nX, (int) (fYt) + 1))
			{
				nDestCol = GetAntiAliasColor(col16, GetPixel(nX, (int) (fYt) + 1), fDistance);
				DrawPixel(nX, (int) fYt + 1, nDestCol);
			}
			if (!IsOutPoint(nX, (int) fYt))
			{
				nDestCol = GetAntiAliasColor(col16, GetPixel(nX, (int) fYt), fValue);
				DrawPixel(nX, (int) fYt, nDestCol);
			}	
			fYt += fK;
		}
	}
	else
	{
		if (dy < 0)
		{
			swap(nStartX, nEndX);
			swap(nStartY, nEndY);
		}
		fK = (float) dx / (float) dy;
		float fXt = (float) nStartX + fK;
		float fDistance = 0;
		float fValue = 0;

		for (int nY = nStartY + 1; nY < nEndY; ++nY)
		{
			fDistance = (float) (fXt - (int) (fXt));
			fValue = 1.0f - fDistance;
			if (!IsOutPoint((int) fXt + 1, nY))
			{
				nDestCol = GetAntiAliasColor(col16, GetPixel((int) fXt + 1, nY), fDistance);
				DrawPixel((int) fXt + 1, nY, nDestCol);
			}
			if (!IsOutPoint((int) fXt, nY))
			{
				nDestCol = GetAntiAliasColor(col16, colBack = GetPixel((int) fXt, nY), fValue);
				DrawPixel((int) fXt, nY, nDestCol);
			}

			fXt += fK;
		}
	}

	if (!IsOutPoint(nEndX, nEndY))
	{
		DrawPixel(nEndX, nEndY, col16);
	}
}

void CLine::DrawHLine(int nStartX, int nEndX, int nY, UINT16 col16)
{
	RECT *pRect = CReDraw::GetDrawRect();
	if (nY < pRect->top || nY > pRect->bottom)
	{
		return;
	}
	if( nStartX > nEndX )
	{
		swap(nStartX, nEndX);
	}

	nStartX = max(nStartX, pRect->left);
	nEndX = min(nEndX, pRect->right);

	for (int nX = nStartX; nX < nEndX; ++nX)
	{
		DrawPixel(nX, nY, col16);
	}

}

void CLine::DrawVLine(int nStartY, int nEndY, int nX, UINT16 col16)
{
	RECT *pRect = CReDraw::GetDrawRect();
	if (nX < pRect->left || nX > pRect->right)
	{
		return;
	}
	if( nStartY > nEndY )
	{
		swap(nStartY, nEndY);
	}

	nStartY = max(nStartY, pRect->top);
	nEndY = min(nEndY, pRect->bottom);
	for (int nY = nStartY; nY < nEndY; ++nY)
	{
		DrawPixel(nX, nY, col16);
	}
}

void CLine::DrawHLine(int nStartX, int nEndX, int nY, COLORREF col, float fAlpha)
{
	RECT *pRect = CReDraw::GetDrawRect();
	if (nY < pRect->top || nY > pRect->bottom)
	{
		return;
	}
	if( nStartX > nEndX )
	{
		swap(nStartX, nEndX);
	}

	nStartX = max(nStartX, pRect->left);
	nEndX = min(nEndX, pRect->right);
	UINT16 colAlpha;
	UINT16 colBk = GetViewColor(col);
	for (int nX = nStartX; nX < nEndX; ++nX)
	{
		colAlpha = GetAlphaColor(nX, nY, colBk, fAlpha);
		DrawPixel(nX, nY, colAlpha);
	}
}

bool CLine::SplitLine(int& nStartX, int& nStartY, int& nEndX, int& nEndY)
{
	RECT *pRect = CReDraw::GetDrawRect();
	while (true)
	{
		int nCode1 = GetPosAreaCode(nStartX, nStartY);
		int nCode2 = GetPosAreaCode(nEndX, nEndY);
		if ((nCode1 + nCode2) == 0)
		{
			return true;
		}
		if ((nCode1 & nCode2) != 0)
		{
			return false;             
		}	
		if (nCode1 == 0)
		{
			swap(nStartX, nEndX);
			swap(nStartY, nEndY);
			swap(nCode1, nCode2);
		}

		if (nCode1 >> 3 == 1)
		{	
			float dSlope = ((float) (pRect->bottom - nStartY)) / (nEndY - nStartY);
			nStartX += (int) (((nEndX - nStartX) * dSlope));
			nStartY = pRect->bottom - 1;
		}
		if (((nCode1 >> 2) & 0x01) == 1)
		{
			 float dSlope = ((float)(pRect->top - nStartY)) / (nEndY - nStartY);
			 nStartY = pRect->top;
			 nStartX += (int) (((nEndX - nStartX) * dSlope));
		}
		if (((nCode1 >> 1) & 0x01) == 1)
		{
			float dSlope = ((float)(pRect->right - nStartX)) / (nEndX - nStartX);
			nStartX = pRect->right - 1;
			nStartY += (int) (((nEndY - nStartY) * dSlope));
		}
		if ((nCode1 & 0x01) == 1)
		{
			float dSlope = ((float) (pRect->left - nStartX))  / (nEndX - nStartX);
			nStartX = pRect->left;
			nStartY += (int) (((nEndY - nStartY) * dSlope));
		}
	}
}

inline int CLine::GetPosAreaCode(int nX, int nY)
{
	RECT *pRect = CReDraw::GetDrawRect();
	int code = 0;
	if (nX < pRect->left)
	{
		code = 1;
	}
	else if (nX > pRect->right - 1)
	{
		code = 2;
	}

	if (nY < pRect->top)
	{
		code += 4;
	}
	else if (nY > pRect->bottom - 1)
	{
		code += 8;
	}
	return code;
}
