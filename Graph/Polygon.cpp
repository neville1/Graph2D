#include "StdAfx.h"
#include "Polygon.h"
#include "ReDraw.h"
#include "Sutherlandhodgman.h"

struct HorLine {
   int XStart; /* X coordinate of leftmost pixel in line */
   int XEnd;   /* X coordinate of rightmost pixel in line */
};

struct HorLineList {
   int Length;                /* # of horizontal lines */
   int YStart;                /* Y coordinate of topmost line */
   struct HorLine * HLinePtr;   /* pointer to list of horz lines */
};

struct EdgeState {
   struct EdgeState *NextEdge;
   int X;
   int StartY;
   int WholePixelXMove;
   int XDirection;
   int ErrorTerm;
   int ErrorTermAdjUp;
   int ErrorTermAdjDown;
   int Count;
};

#define INDEX_FORWARD(Index) \
   Index = (Index + 1) % VertexList->Length;

/* Advances the index by one vertex backward through the vertex list,
   wrapping at the start of the list */
#define INDEX_BACKWARD(Index) \
   Index = (Index - 1 + VertexList->Length) % VertexList->Length;

/* Advances the index by one vertex either forward or backward through
   the vertex list, wrapping at either end of the list */
#define INDEX_MOVE(Index,Direction)                                  \
   if (Direction > 0)                                                \
      Index = (Index + 1) % VertexList->Length;                      \
   else                                                              \
      Index = (Index - 1 + VertexList->Length) % VertexList->Length;

static struct EdgeState *GETPtr, *AETPtr;

void CPolygon::DrawPolyLine(LPPOINT lpPoints, int nCount, COLORREF col)
{
	for (int i = 0; i < nCount - 1; ++i)
	{
		DrawLine(lpPoints[i].x, lpPoints[i].y, lpPoints[i + 1].x, lpPoints[i + 1].y, col);
	}
}

void CPolygon::DrawSwitchColPolyLine(LPPOINT lpPoints, int nCount,  int nStep, COLORREF col1, COLORREF col2)//2009-9-1
{
	for (int i = 0; i < nCount - 1; ++i)
	{
		DrawSwitchColLine(lpPoints[i].x, lpPoints[i].y, lpPoints[i + 1].x, lpPoints[i + 1].y, nStep, col1, col2);
	}
}

void CPolygon::DrawPolygon(LPPOINT lpPoints, int nCount, COLORREF col)
{
	DrawPolyLine(lpPoints, nCount, col);
	DrawLine(lpPoints[nCount - 1].x, lpPoints[nCount - 1].y, lpPoints[0].x, lpPoints[0].y, col);
}

void CPolygon::FillAnyPolygon(LPPOINT lpPoints,int nCount ,COLORREF col)
{
	if (nCount < 3)
	{
		return;
	}

	bool bOut = false;
	for (int i = 0; i < nCount; ++i)
	{
		if (IsOutPoint(lpPoints[i].x, lpPoints[i].y))
		{
			bOut = true;
			break;
		}
	}

	PtListHeader Header;
	Header.PointPtr = lpPoints;
	Header.Length   = nCount;
	vector<POINT> vPos;	
	if (bOut)
	{
		RECT *pRect = CReDraw::GetDrawRect();
		RectF rt((REAL) pRect->left, (REAL) pRect->top, 
			     (REAL) pRect->right - pRect->left, (REAL) pRect->bottom - pRect->top);

		pointVector input;
		input.resize(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			input[i].X = (REAL) lpPoints[i].x;
			input[i].Y = (REAL) lpPoints[i].y;
		}

		pointVector output;
		SutherlandHodgman sh(rt);
		sh.Clip(input, output);
		if (output.empty())
		{
			return;
		}

		vPos.resize((int) output.size());
		for (int i = 0; i < (int) vPos.size(); ++i)
		{
			vPos[i].x = (int) output[i].X;
			vPos[i].y = (int) output[i].Y;
		}
		Header.PointPtr = (LPPOINT) &vPos[0];
		Header.Length = (int) vPos.size();
	}

	PtListHeader * VertexList = &Header;
    struct EdgeState *EdgeTableBuffer;
    int CurrentY = 0;
	/* It takes a minimum of 3 vertices to cause any pixels to be
	  drawn; reject polygons that are guaranteed to be invisible */
	//if (VertexList->Length < 3)
	//{
	//  return;
	//}
	/* Get enough memory to store the entire edge table */
	if ((EdgeTableBuffer =
		 (struct EdgeState *) (malloc(sizeof(struct EdgeState) *
		 VertexList->Length))) == NULL)
	{
	  return;  /* couldn't get memory for the edge table */
	}

	UINT16 col16 = GetViewColor(col);
	/* Build the global edge table */
	BuildGET(VertexList, EdgeTableBuffer);
	/* Scan down through the polygon edges, one scan line at a time,
	  so long as at least one edge remains in either the GET or AET */
	AETPtr = NULL;    /* initialize the active edge table to empty */
	if( GETPtr != NULL ) CurrentY = GETPtr->StartY; /* start at the top polygon vertex */
	while ((GETPtr != NULL) || (AETPtr != NULL)) 
	{
	  MoveXSortedToAET(CurrentY);  /* update AET for this scan line */
	  ScanOutAET(CurrentY, col16); /* draw this scan line from AET */
	  AdvanceAET();                /* advance AET edges 1 scan line */
	  XSortAET();                  /* resort on X */
	  CurrentY++;                  /* advance to the next scan line */
	}
	/* Release the memory we've allocated and we're done */
	free(EdgeTableBuffer);
}

void CPolygon::MoveXSortedToAET(int YToMove) 
{
   struct EdgeState *AETEdge, **AETEdgePtr, *TempEdge;
   int CurrentX;

   /* The GET is Y sorted. Any edges that start at the desired Y
      coordinate will be first in the GET, so we'll move edges from
      the GET to AET until the first edge left in the GET is no longer
      at the desired Y coordinate. Also, the GET is X sorted within
      each Y coordinate, so each successive edge we add to the AET is
      guaranteed to belong later in the AET than the one just added */
   AETEdgePtr = &AETPtr;
   while ((GETPtr != NULL) && (GETPtr->StartY == YToMove)) {
      CurrentX = GETPtr->X;
      /* Link the new edge into the AET so that the AET is still
         sorted by X coordinate */
      for (;;) {
         AETEdge = *AETEdgePtr;
         if ((AETEdge == NULL) || (AETEdge->X >= CurrentX)) {
            TempEdge = GETPtr->NextEdge;
            *AETEdgePtr = GETPtr;  /* link the edge into the AET */
            GETPtr->NextEdge = AETEdge;
            AETEdgePtr = &GETPtr->NextEdge;
            GETPtr = TempEdge;   /* unlink the edge from the GET */
            break;
         } else {
            AETEdgePtr = &AETEdge->NextEdge;
         }
      }
   }
}

/* Fills the scan line described by the current AET at the specified Y
   coordinate in the specified color, using the odd/even fill rule */
void CPolygon::ScanOutAET(int YToScan, UINT16 col16) 
{
   int LeftX;
   struct EdgeState *CurrentEdge;

   /* Scan through the AET, drawing line segments as each pair of edge
      crossings is encountered. The nearest pixel on or to the right
      of left edges is drawn, and the nearest pixel to the left of but
      not on right edges is drawn */
   CurrentEdge = AETPtr;
   while (CurrentEdge != NULL) 
   {
      LeftX = CurrentEdge->X;
      CurrentEdge = CurrentEdge->NextEdge;
	  if( CurrentEdge == NULL ) break;
	  DrawHLine(LeftX, CurrentEdge->X, YToScan, col16);
      CurrentEdge = CurrentEdge->NextEdge;
   }
}

/* Advances each edge in the AET by one scan line.
   Removes edges that have been fully scanned. */
void CPolygon::AdvanceAET() 
{
   struct EdgeState *CurrentEdge, **CurrentEdgePtr;

   /* Count down and remove or advance each edge in the AET */
   CurrentEdgePtr = &AETPtr;
   while ((CurrentEdge = *CurrentEdgePtr) != NULL) {
      /* Count off one scan line for this edge */
      if ((--(CurrentEdge->Count)) == 0) {
         /* This edge is finished, so remove it from the AET */
         *CurrentEdgePtr = CurrentEdge->NextEdge;
      } else {
         /* Advance the edge's X coordinate by minimum move */
         CurrentEdge->X += CurrentEdge->WholePixelXMove;
         /* Determine whether it's time for X to advance one extra */
         if ((CurrentEdge->ErrorTerm +=
               CurrentEdge->ErrorTermAdjUp) > 0) {
            CurrentEdge->X += CurrentEdge->XDirection;
            CurrentEdge->ErrorTerm -= CurrentEdge->ErrorTermAdjDown;
         }
         CurrentEdgePtr = &CurrentEdge->NextEdge;
      }
   }
}

void CPolygon::XSortAET() 
{
   struct EdgeState *CurrentEdge, **CurrentEdgePtr, *TempEdge;
   int SwapOccurred;

   /* Scan through the AET and swap any adjacent edges for which the
      second edge is at a lower current X coord than the first edge.
      Repeat until no further swapping is needed */
   if (AETPtr != NULL) {
      do {
         SwapOccurred = 0;
         CurrentEdgePtr = &AETPtr;
         while ((CurrentEdge = *CurrentEdgePtr)->NextEdge != NULL) {
            if (CurrentEdge->X > CurrentEdge->NextEdge->X) {
               /* The second edge has a lower X than the first;
                  swap them in the AET */
               TempEdge = CurrentEdge->NextEdge->NextEdge;
               *CurrentEdgePtr = CurrentEdge->NextEdge;
               CurrentEdge->NextEdge->NextEdge = CurrentEdge;
               CurrentEdge->NextEdge = TempEdge;
               SwapOccurred = 1;
            }
            CurrentEdgePtr = &(*CurrentEdgePtr)->NextEdge;
         }
      } while (SwapOccurred != 0);
   }
}


void CPolygon::FillConvexPolygon(LPPOINT lpPoints, int nCount, COLORREF col)
{
	if (nCount < 3)
	{
		return;
	}

	bool bOut = false;
	for (int i = 0; i < nCount; ++i)
	{
		if (IsOutPoint(lpPoints[i].x, lpPoints[i].y))
		{
			bOut = true;
			break;
		}
	}

	PtListHeader Header;
	Header.PointPtr = lpPoints;
	Header.Length   = nCount;
	vector<POINT> vPos;
	if (bOut)
	{
		RECT *pRect = CReDraw::GetDrawRect();
		RectF rt((REAL) pRect->left, (REAL) pRect->top, 
			     (REAL) pRect->right - pRect->left, (REAL) pRect->bottom - pRect->top);

		pointVector input;
		input.resize(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			input[i].X = (REAL) lpPoints[i].x;
			input[i].Y = (REAL) lpPoints[i].y;
		}

		pointVector output;
		SutherlandHodgman sh(rt);
		sh.Clip(input, output);
		if (output.empty())
		{
			return;
		}

		vPos.resize((int) output.size());
		for (int i = 0; i < (int) vPos.size(); ++i)
		{
			vPos[i].x = (int) output[i].X;
			vPos[i].y = (int) output[i].Y;
		}
		Header.PointPtr = (LPPOINT) &vPos[0];
		Header.Length = (int) vPos.size();
	}
	

   PtListHeader * VertexList = &Header;
   int i, MinIndexL, MaxIndex, MinIndexR, SkipFirst, Temp;
   int MinPoint_Y, MaxPoint_Y, TopIsFlat, LeftEdgeDir;
   int NextIndex, CurrentIndex, PreviousIndex;
   int DeltaXN, DeltaYN, DeltaXP, DeltaYP;
   struct HorLineList WorkingHLineList;
   struct HorLine *EdgePointPtr;
   POINT *VertexPtr;

   /* Point to the vertex list */
   VertexPtr = VertexList->PointPtr;

   /* Scan the list to find the top and bottom of the polygon */
   MaxPoint_Y = MinPoint_Y = VertexPtr[MinIndexL = MaxIndex = 0].y;
   for (i = 1; i < VertexList->Length; i++) {
      if (VertexPtr[i].y < MinPoint_Y)
         MinPoint_Y = VertexPtr[MinIndexL = i].y; /* new top */
      else if (VertexPtr[i].y > MaxPoint_Y)
         MaxPoint_Y = VertexPtr[MaxIndex = i].y; /* new bottom */
   }
   if (MinPoint_Y == MaxPoint_Y)
   {
      return;  /* polygon is 0-height; avoid infinite loop below */
   }

   UINT16 col16 = GetViewColor(col);
   /* Scan in ascending order to find the last top-edge point */
   MinIndexR = MinIndexL;
   while (VertexPtr[MinIndexR].y == MinPoint_Y)
      INDEX_FORWARD(MinIndexR);
   INDEX_BACKWARD(MinIndexR); /* back up to last top-edge point */

   /* Now scan in descending order to find the first top-edge point */
   while (VertexPtr[MinIndexL].y == MinPoint_Y)
      INDEX_BACKWARD(MinIndexL);
   INDEX_FORWARD(MinIndexL); /* back up to first top-edge point */

   /* Figure out which direction through the vertex list from the top
      vertex is the left edge and which is the right */
   LeftEdgeDir = -1; /* assume left edge runs down thru vertex list */
   if ((TopIsFlat = (VertexPtr[MinIndexL].x != VertexPtr[MinIndexR].x) ? 1 : 0) == 1) {
      /* If the top is flat, just see which of the ends is leftmost */
      if (VertexPtr[MinIndexL].x > VertexPtr[MinIndexR].x) {
         LeftEdgeDir = 1;  /* left edge runs up through vertex list */
         Temp = MinIndexL;       /* swap the indices so MinIndexL   */
         MinIndexL = MinIndexR;  /* points to the start of the left */
         MinIndexR = Temp;       /* edge, similarly for MinIndexR   */
      }
   } else {
      /* Point to the downward end of the first line of each of the
         two edges down from the top */
      NextIndex = MinIndexR;
      INDEX_FORWARD(NextIndex);
      PreviousIndex = MinIndexL;
      INDEX_BACKWARD(PreviousIndex);
      /* Calculate X and Y lengths from the top vertex to the end of
         the first line down each edge; use those to compare slopes
         and see which line is leftmost */
      DeltaXN = VertexPtr[NextIndex].x - VertexPtr[MinIndexL].x;
      DeltaYN = VertexPtr[NextIndex].y - VertexPtr[MinIndexL].y;
      DeltaXP = VertexPtr[PreviousIndex].x - VertexPtr[MinIndexL].x;
      DeltaYP = VertexPtr[PreviousIndex].y - VertexPtr[MinIndexL].y;
      if (((long)DeltaXN * DeltaYP - (long)DeltaYN * DeltaXP) < 0L) {
         LeftEdgeDir = 1;  /* left edge runs up through vertex list */
         Temp = MinIndexL;       /* swap the indices so MinIndexL   */
         MinIndexL = MinIndexR;  /* points to the start of the left */
         MinIndexR = Temp;       /* edge, similarly for MinIndexR   */
      }
   }

   /* Set the # of scan lines in the polygon, skipping the bottom edge
      and also skipping the top vertex if the top isn't flat because
      in that case the top vertex has a right edge component, and set
      the top scan line to draw, which is likewise the second line of
      the polygon unless the top is flat */
   if ((WorkingHLineList.Length = MaxPoint_Y - MinPoint_Y - 1 + TopIsFlat) <= 0)
   {
      return;  /* there's nothing to draw, so we're done */
   }
   WorkingHLineList.YStart = MinPoint_Y + 1 - TopIsFlat;

   /* Get memory in which to store the line list we generate */
   if ((WorkingHLineList.HLinePtr =
         (struct HorLine *) (malloc(sizeof(struct HorLine) *
         WorkingHLineList.Length))) == NULL)
   {
      return;  /* couldn't get memory for the line list */
   }

   /* Scan the left edge and store the boundary points in the list */
   /* Initial pointer for storing scan converted left-edge coords */
   EdgePointPtr = WorkingHLineList.HLinePtr;
   /* Start from the top of the left edge */
   PreviousIndex = CurrentIndex = MinIndexL;
   /* Skip the first point of the first line unless the top is flat;
      if the top isn't flat, the top vertex is exactly on a right
      edge and isn't drawn */
   SkipFirst = TopIsFlat ? 0 : 1;
   /* Scan convert each line in the left edge from top to bottom */
   do {
      INDEX_MOVE(CurrentIndex,LeftEdgeDir);
      ScanEdge(VertexPtr[PreviousIndex].x,
            VertexPtr[PreviousIndex].y,
            VertexPtr[CurrentIndex].x,
            VertexPtr[CurrentIndex].y, 1, SkipFirst, &EdgePointPtr);
      PreviousIndex = CurrentIndex;
      SkipFirst = 0; /* scan convert the first point from now on */
   } while (CurrentIndex != MaxIndex);

   /* Scan the right edge and store the boundary points in the list */
   EdgePointPtr = WorkingHLineList.HLinePtr;
   PreviousIndex = CurrentIndex = MinIndexR;
   SkipFirst = TopIsFlat ? 0 : 1;
   /* Scan convert the right edge, top to bottom. X coordinates are
      adjusted 1 to the left, effectively causing scan conversion of
      the nearest points to the left of but not exactly on the edge */
   do {
      INDEX_MOVE(CurrentIndex,-LeftEdgeDir);
      ScanEdge(VertexPtr[PreviousIndex].x - 1,
            VertexPtr[PreviousIndex].y,
            VertexPtr[CurrentIndex].x - 1,
            VertexPtr[CurrentIndex].y, 0, SkipFirst, &EdgePointPtr);
      PreviousIndex = CurrentIndex;
      SkipFirst = 0; /* scan convert the first point from now on */
   } while (CurrentIndex != MaxIndex);

   /* Draw the line list representing the scan converted polygon */
   DrawHorizontalLineList(&WorkingHLineList, col16);

   /* Release the line list's memory and we're successfully done */
   free(WorkingHLineList.HLinePtr);
}

void CPolygon::ScanEdge(int X1, int Y1, int X2, int Y2, int SetXStart,
                        int SkipFirst, struct HorLine **EdgePointPtr)
{
   int DeltaX, Height, Width, AdvanceAmt, ErrorTerm, i;
   int ErrorTermAdvance, XMajorAdvanceAmt;
   struct HorLine *WorkingEdgePointPtr;

   WorkingEdgePointPtr = *EdgePointPtr; /* avoid double dereference */
   AdvanceAmt = ((DeltaX = X2 - X1) > 0) ? 1 : -1;
                            /* direction in which X moves (Y2 is
                               always > Y1, so Y always counts up) */

   if ((Height = Y2 - Y1) <= 0)  /* Y length of the edge */
      return;     /* guard against 0-length and horizontal edges */

   /* Figure out whether the edge is vertical, diagonal, X-major
      (mostly horizontal), or Y-major (mostly vertical) and handle
      appropriately */
   if ((Width = abs(DeltaX)) == 0) {
      /* The edge is vertical; special-case by just storing the same
         X coordinate for every scan line */
      /* Scan the edge for each scan line in turn */
      for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
         /* Store the X coordinate in the appropriate edge list */
         if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
         else
            WorkingEdgePointPtr->XEnd = X1;
      }
   } else if (Width == Height) {
      /* The edge is diagonal; special-case by advancing the X
         coordinate 1 pixel for each scan line */
      if (SkipFirst) /* skip the first point if so indicated */
         X1 += AdvanceAmt; /* move 1 pixel to the left or right */
      /* Scan the edge for each scan line in turn */
      for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
         /* Store the X coordinate in the appropriate edge list */
         if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
         else
            WorkingEdgePointPtr->XEnd = X1;
         X1 += AdvanceAmt; /* move 1 pixel to the left or right */
      }
   } else if (Height > Width) {
      /* Edge is closer to vertical than horizontal (Y-major) */
      if (DeltaX >= 0)
         ErrorTerm = 0; /* initial error term going left->right */
      else
         ErrorTerm = -Height + 1;   /* going right->left */
      if (SkipFirst) {   /* skip the first point if so indicated */
         /* Determine whether it's time for the X coord to advance */
         if ((ErrorTerm += Width) > 0) {
            X1 += AdvanceAmt; /* move 1 pixel to the left or right */
            ErrorTerm -= Height; /* advance ErrorTerm to next point */
         }
      }
      /* Scan the edge for each scan line in turn */
      for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
         /* Store the X coordinate in the appropriate edge list */
         if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
         else
            WorkingEdgePointPtr->XEnd = X1;
         /* Determine whether it's time for the X coord to advance */
         if ((ErrorTerm += Width) > 0) {
            X1 += AdvanceAmt; /* move 1 pixel to the left or right */
            ErrorTerm -= Height; /* advance ErrorTerm to correspond */
         }
      }
   } else {
      /* Edge is closer to horizontal than vertical (X-major) */
      /* Minimum distance to advance X each time */
      XMajorAdvanceAmt = (Width / Height) * AdvanceAmt;
      /* Error term advance for deciding when to advance X 1 extra */
      ErrorTermAdvance = Width % Height;
      if (DeltaX >= 0)
         ErrorTerm = 0; /* initial error term going left->right */
      else
         ErrorTerm = -Height + 1;   /* going right->left */
      if (SkipFirst) {   /* skip the first point if so indicated */
         X1 += XMajorAdvanceAmt;    /* move X minimum distance */
         /* Determine whether it's time for X to advance one extra */
         if ((ErrorTerm += ErrorTermAdvance) > 0) {
            X1 += AdvanceAmt;       /* move X one more */
            ErrorTerm -= Height; /* advance ErrorTerm to correspond */
         }
      }
      /* Scan the edge for each scan line in turn */
      for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
         /* Store the X coordinate in the appropriate edge list */
         if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
         else
            WorkingEdgePointPtr->XEnd = X1;
         X1 += XMajorAdvanceAmt;    /* move X minimum distance */
         /* Determine whether it's time for X to advance one extra */
         if ((ErrorTerm += ErrorTermAdvance) > 0) {
            X1 += AdvanceAmt;       /* move X one more */
            ErrorTerm -= Height; /* advance ErrorTerm to correspond */
         }
      }
   }

   *EdgePointPtr = WorkingEdgePointPtr;   /* advance caller's ptr */
}

void CPolygon::DrawHorizontalLineList(struct HorLineList * HLineListPtr, UINT16 col16)
{
   struct HorLine *HLinePtr;
   int Y;

   /* Point to the XStart/XEnd descriptor for the first (top)
      horizontal line */
   HLinePtr = HLineListPtr->HLinePtr;
   /* Draw each horizontal line in turn, starting with the top one and
      advancing one line each time */ 
//   RECT *pRect = CReDraw::GetDrawRect();
 //  int nMaxY = min(HLineListPtr->YStart + HLineListPtr->Length, pRect->bottom);
   for (Y = HLineListPtr->YStart; Y < (HLineListPtr->YStart +
         HLineListPtr->Length); Y++, HLinePtr++) 
   {
      /* Draw each pixel in the current horizontal line in turn,
         starting with the leftmost one */
	   DrawHLine( HLinePtr->XStart, HLinePtr->XEnd + 1, Y, col16); 
   }
}

void CPolygon::BuildGET(PtListHeader * VertexList,struct EdgeState * NextFreeEdgeStruc)
{
   int i, StartX, StartY, EndX, EndY, DeltaY, DeltaX, Width;
   struct EdgeState *NewEdgePtr;
   struct EdgeState *FollowingEdge, **FollowingEdgeLink;
   POINT *VertexPtr;

   /* Scan through the vertex list and put all non-0-height edges into
      the GET, sorted by increasing Y start coordinate */
   VertexPtr = VertexList->PointPtr;   /* point to the vertex list */
   GETPtr = NULL;    /* initialize the global edge table to empty */
   for (i = 0; i < VertexList->Length; i++) {
      /* Calculate the edge height and width */
      StartX = VertexPtr[i].x;
      StartY = VertexPtr[i].y;
      /* The edge runs from the current point to the previous one */
      if (i == 0) {
         /* Wrap back around to the end of the list */
         EndX = VertexPtr[VertexList->Length-1].x;
         EndY = VertexPtr[VertexList->Length-1].y;
      } else {
         EndX = VertexPtr[i-1].x;
         EndY = VertexPtr[i-1].y;
      }
      /* Make sure the edge runs top to bottom */
      if (StartY > EndY) {
         swap(StartX, EndX);
         swap(StartY, EndY);
      }
      /* Skip if this can't ever be an active edge (has 0 height) */
      if ((DeltaY = EndY - StartY) != 0) {
         /* Allocate space for this edge's info, and fill in the
            structure */
         NewEdgePtr = NextFreeEdgeStruc++;
         NewEdgePtr->XDirection =   /* direction in which X moves */
               ((DeltaX = EndX - StartX) > 0) ? 1 : -1;
         Width = abs(DeltaX);
         NewEdgePtr->X = StartX;
         NewEdgePtr->StartY = StartY;
         NewEdgePtr->Count = DeltaY;
         NewEdgePtr->ErrorTermAdjDown = DeltaY;
         if (DeltaX >= 0)  /* initial error term going L->R */
            NewEdgePtr->ErrorTerm = 0;
         else              /* initial error term going R->L */
            NewEdgePtr->ErrorTerm = -DeltaY + 1;
         if (DeltaY >= Width) {     /* Y-major edge */
            NewEdgePtr->WholePixelXMove = 0;
            NewEdgePtr->ErrorTermAdjUp = Width;
         } else {                   /* X-major edge */
            NewEdgePtr->WholePixelXMove =
                  (Width / DeltaY) * NewEdgePtr->XDirection;
            NewEdgePtr->ErrorTermAdjUp = Width % DeltaY;
         }
         /* Link the new edge into the GET so that the edge list is
            still sorted by Y coordinate, and by X coordinate for all
            edges with the same Y coordinate */
         FollowingEdgeLink = &GETPtr;
         for (;;) {
            FollowingEdge = *FollowingEdgeLink;
            if ((FollowingEdge == NULL) ||
                  (FollowingEdge->StartY > StartY) ||
                  ((FollowingEdge->StartY == StartY) &&
                  (FollowingEdge->X >= StartX))) {
               NewEdgePtr->NextEdge = FollowingEdge;
               *FollowingEdgeLink = NewEdgePtr;
               break;
            }
            FollowingEdgeLink = &FollowingEdge->NextEdge;
         }
      }
   }
}