#pragma once

typedef struct PtListHeader {
   int Length;         /* # of points */
   POINT * PointPtr;   /* pointer to list of points */
}PtListHeader;

#include "Line.h"

class __declspec(dllexport) CPolygon : public CLine
{
public:
	CPolygon() {}
	~CPolygon() {}

	void DrawPolyLine(LPPOINT lpPoints, int nCount, COLORREF col);
	void DrawSwitchColPolyLine(LPPOINT lpPoints, int nCount, int nStep, COLORREF col1, COLORREF col2);
	void DrawPolygon(LPPOINT lpPoints, int nCount, COLORREF col);
	void FillAnyPolygon(LPPOINT lpPoints,int nCount ,COLORREF col);
	void FillConvexPolygon(LPPOINT lpPoints, int nCount, COLORREF col);

private:
	void BuildGET(PtListHeader * VertexList,struct EdgeState * NextFreeEdgeStruc);
	void MoveXSortedToAET(int YToMove);
	void ScanOutAET(int YToScan, UINT16 col16);
	void AdvanceAET();
	void XSortAET();

	void ScanEdge(int X1, int Y1, int X2, int Y2, int SetXStart,
                  int SkipFirst, struct HorLine **EdgePointPtr);
	void DrawHorizontalLineList(struct HorLineList * HLineListPtr, UINT16 col16);

};
