/*!
 * \file
 * Scanline edge-flag algorithm for antialiasing <br>
 * Copyright (c) 2005-2007 Kiia Kallio
 *
 * http://mlab.uiah.fi/~kkallio/antialiasing/
 * 
 * This code is distributed under the three-clause BSD license.
 * Read the LICENSE file or visit the URL above for details.
 *
 * \brief A class for handling sub-polygons. Each sub-polygon is a continuos, closed set of edges.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */

#include "stdafx.h"
#ifdef SUPPORT_GEN2D

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPolyTypes.h"
#include "mgSubPolygon.h"


//! Constructor.
/*! Note that the ownership of the vertex array is transferred.
 */
mgSubPolygon::mgSubPolygon(
    VECTOR2D *aVertices, 
    int aVertexCount)
{
    mVertices = aVertices;
    mVertexCount = aVertexCount;
    mVertexData = NULL;
}


//! Initializer.
BOOL mgSubPolygon::init()
{
    mVertexData = new VertexData[mVertexCount + 1];
    if (mVertexData == NULL)
        return false;
    return true;
}


//! Destructor.
mgSubPolygon::~mgSubPolygon()
{
    delete mVertices;
    delete mVertexData;
}


//! Calculates the edges of the polygon with transformation and clipping to aEdges array.
/*! \param aFirstVertex the index for the first vertex.
 *  \param aVertexCount the amount of vertices to convert.
 *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
 *  \param aTransformation the transformation matrix for the polygon.
 *  \param aClipRectangle the clip rectangle.
 *  \return the amount of edges in the result.
 */
int mgSubPolygon::getEdges(
    int aFirstVertex, 
    int aVertexCount, 
    PolygonEdge* aEdges, 
    const MATRIX2D& aTransformation, 
    const mgClipRectangle& aClipRectangle) const
{
    int startIndex = aFirstVertex;
    int endIndex = startIndex + aVertexCount;
    if (endIndex > mVertexCount)
        endIndex = mVertexCount;

    VECTOR2D prevPosition = mVertices[startIndex];
    MATRIX2D_TRANSFORM(aTransformation,mVertices[startIndex],prevPosition);
    int prevClipFlags = getClipFlags(prevPosition, aClipRectangle);

    int edgeCount = 0;
    int n;
    for (n = startIndex; n < endIndex; n++)
    {
        VECTOR2D position = mVertices[(n + 1) % mVertexCount];
        MATRIX2D_TRANSFORM(aTransformation,mVertices[(n + 1) % mVertexCount],position);

        int clipFlags = getClipFlags(position, aClipRectangle);

        int clipSum = prevClipFlags | clipFlags;
        int clipUnion = prevClipFlags & clipFlags;

        // Skip all edges that are either completely outside at the top or at the bottom.
        if ((clipUnion & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM)) == 0)
        {
            if (clipUnion & POLYGON_CLIP_RIGHT)
            {
                // Both clip to right, edge is a vertical line on the right side
                if (getVerticalEdge(VECTOR2D_GETY(prevPosition),
                                    VECTOR2D_GETY(position),
                                    aClipRectangle.getMaxXf(),
                                    aEdges[edgeCount],
                                    aClipRectangle))
                {
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif
                    edgeCount++;
                }
            }
            else if (clipUnion & POLYGON_CLIP_LEFT)
            {
                // Both clip to left, edge is a vertical line on the left side
                if (getVerticalEdge(VECTOR2D_GETY(prevPosition),VECTOR2D_GETY(position),
                    aClipRectangle.getMinXf(),aEdges[edgeCount],aClipRectangle))
                {
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif
                    edgeCount++;
                }
            }
            else if ((clipSum & (POLYGON_CLIP_RIGHT | POLYGON_CLIP_LEFT)) == 0)
            {
                // No clipping in the horizontal direction
                if (getEdge(prevPosition,position,aEdges[edgeCount],aClipRectangle))
                {
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif
                    edgeCount++;
                }
            }
            else
            {
                // Clips to left or right or both.
                VECTOR2D left, right;
                short swapWinding;
                if (VECTOR2D_GETX(position) < VECTOR2D_GETX(prevPosition))
                {
                    left = position;
                    right = prevPosition;
                    swapWinding = -1;
                }
                else
                {
                    left = prevPosition;
                    right = position;
                    swapWinding = 1;
                }

                RATIONAL slope = (VECTOR2D_GETY(right) - VECTOR2D_GETY(left)) / 
                                 (VECTOR2D_GETX(right) - VECTOR2D_GETX(left));

                if (clipSum & POLYGON_CLIP_RIGHT)
                {
                    // calculate new position for the right vertex
                    RATIONAL oldY = VECTOR2D_GETY(right);
                    RATIONAL maxX = aClipRectangle.getMaxXf();

                    VECTOR2D_SETY(right, VECTOR2D_GETY(left) + (maxX - VECTOR2D_GETX(left)) * slope);
                    VECTOR2D_SETX(right, maxX);

                    // add vertical edge for the overflowing part
                    if (getVerticalEdge(VECTOR2D_GETY(right),oldY,maxX,aEdges[edgeCount],aClipRectangle))
                    {
                        aEdges[edgeCount].mWinding *= swapWinding;
#ifdef SUBPOLYGON_DEBUG
                        DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif
                        edgeCount++;
                    }
                }

                if (clipSum & POLYGON_CLIP_LEFT)
                {
                    // calculate new position for the left vertex
                    RATIONAL oldY = VECTOR2D_GETY(left);
                    RATIONAL minX = aClipRectangle.getMinXf();

                    VECTOR2D_SETY(left,VECTOR2D_GETY(left) + (minX - VECTOR2D_GETX(left)) * slope);
                    VECTOR2D_SETX(left,minX);

                    // add vertical edge for the overflowing part
                    if (getVerticalEdge(oldY,VECTOR2D_GETY(left),minX,aEdges[edgeCount],aClipRectangle))
                    {
                        aEdges[edgeCount].mWinding *= swapWinding;
#ifdef SUBPOLYGON_DEBUG
                        DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif
                        edgeCount++;
                    }
                }

                if (getEdge(left,right,aEdges[edgeCount],aClipRectangle))
                {
                    aEdges[edgeCount].mWinding *= swapWinding;
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif
                    edgeCount++;
                }
            }
        }

        prevClipFlags = clipFlags;
        prevPosition = position;
    }

    return edgeCount;
}


//! Calculates the clip flags for a point.
inline int mgSubPolygon::getClipFlags(
    const VECTOR2D& aPoint, 
    const mgClipRectangle& aClipRectangle) const
{
    int flags = POLYGON_CLIP_NONE;

    if (VECTOR2D_GETX(aPoint) < aClipRectangle.getMinXf())
        flags |= POLYGON_CLIP_LEFT;
    else if (VECTOR2D_GETX(aPoint) >= aClipRectangle.getMaxXf())
        flags |= POLYGON_CLIP_RIGHT;

    if (VECTOR2D_GETY(aPoint) < aClipRectangle.getMinYf())
        flags |= POLYGON_CLIP_TOP;
    else if (VECTOR2D_GETY(aPoint) >= aClipRectangle.getMaxYf())
        flags |= POLYGON_CLIP_BOTTOM;

    return flags;
}


//! Creates a polygon edge between two vectors.
/*! Clips the edge vertically to the clip rectangle. Returns true for edges that
 *  should be rendered, false for others.
 */
BOOL mgSubPolygon::getEdge(
    const VECTOR2D& aStart, 
    const VECTOR2D& aEnd, 
    PolygonEdge& aEdge, 
    const mgClipRectangle& aClipRectangle) const
{
    RATIONAL startX, startY, endX, endY;
    short winding;

    if (VECTOR2D_GETY(aStart) <= VECTOR2D_GETY(aEnd))
    {
        startX = VECTOR2D_GETX(aStart);
        startY = VECTOR2D_GETY(aStart);
        endX = VECTOR2D_GETX(aEnd);
        endY = VECTOR2D_GETY(aEnd);
        winding = 1;
    }
    else
    {
        startX = VECTOR2D_GETX(aEnd);
        startY = VECTOR2D_GETY(aEnd);
        endX = VECTOR2D_GETX(aStart);
        endY = VECTOR2D_GETY(aStart);
        winding = -1;
    }

    // Essentially, firstLine is floor(startY + 1) and lastLine is floor(endY).
    // These are refactored to integer casts in order to avoid function
    // calls. The difference with integer cast is that numbers are always
    // rounded towards zero. Since values smaller than zero get clipped away,
    // only coordinates between 0 and -1 require greater attention as they
    // also round to zero. The problems in this range can be avoided by
    // adding one to the values before conversion and subtracting after it.

    int firstLine = rationalToIntRoundDown(startY) + 1;
    int lastLine = rationalToIntRoundDown(endY);

    int minClip = aClipRectangle.getMinYi();
    int maxClip = aClipRectangle.getMaxYi();

    // If start and end are on the same line, the edge doesn't cross
    // any lines and thus can be ignored.
    // If the end is smaller than the first line, edge is out.
    // If the start is larger than the last line, edge is out.
    if (firstLine > lastLine ||
        lastLine < minClip ||
        firstLine >= maxClip)
        return false;

    // Adjust the start based on the target.
    if (firstLine < minClip)
        firstLine = minClip;

    if (lastLine >= maxClip)
        lastLine = maxClip - 1;

    aEdge.mSlope = (endX - startX) / (endY - startY);
    aEdge.mX = startX + ((RATIONAL)firstLine - startY) * aEdge.mSlope;
    aEdge.mWinding = winding;
    aEdge.mFirstLine = firstLine;
    aEdge.mLastLine = lastLine;

    return true;
}


//! Creates a vertical polygon edge between two y values.
/*! Clips the edge vertically to the clip rectangle. Returns true for edges that
 *  should be rendered, false for others.
 */
BOOL mgSubPolygon::getVerticalEdge(
    RATIONAL aStartY, 
    RATIONAL aEndY, 
    RATIONAL aX, 
    PolygonEdge& aEdge, 
    const mgClipRectangle& aClipRectangle) const
{
    RATIONAL start, end;
    short winding;
    if (aStartY < aEndY)
    {
        start = aStartY;
        end = aEndY;
        winding = 1;
    }
    else
    {
        start = aEndY;
        end = aStartY;
        winding = -1;
    }
    
    int firstLine = rationalToIntRoundDown(start) + 1;
    int lastLine = rationalToIntRoundDown(end);

    int minClip = aClipRectangle.getMinYi();
    int maxClip = aClipRectangle.getMaxYi();

    // If start and end are on the same line, the edge doesn't cross
    // any lines and thus can be ignored.
    // If the end is smaller than the first line, edge is out.
    // If the start is larger than the last line, edge is out.
    if (firstLine > lastLine ||
        lastLine < minClip ||
        firstLine >= maxClip)
        return false;

    // Adjust the start based on the clip rect.
    if (firstLine < minClip)
        firstLine = minClip;

    if (lastLine >= maxClip)
        lastLine = maxClip - 1;

    aEdge.mSlope = INT_TO_RATIONAL(0);
    aEdge.mX = aX;
    aEdge.mWinding = winding;
    aEdge.mFirstLine = firstLine;
    aEdge.mLastLine = lastLine;

    return true;
}


//! Calculates the edges of the polygon with transformation and clipping to aEdges array.
/*! Note that this may return upto three times the amount of edges that the polygon has vertices,
 *  in the unlucky case where both left and right side get clipped for all edges.
 *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
 *  \param aTransformation the transformation matrix for the polygon.
 *  \param aClipRectangle the clip rectangle.
 *  \return the amount of edges in the result.
 */
int mgSubPolygon::getScanEdges(
    PolygonScanEdge* aEdges, 
    const MATRIX2D& aTransformation, 
    const mgClipRectangle& aClipRectangle) const
{
    int n;
    for (n = 0; n < mVertexCount; n++)
    {
        // Transform all vertices.
        MATRIX2D_TRANSFORM(aTransformation,mVertices[n],mVertexData[n].mPosition);
    }

    for (n = 0; n < mVertexCount; n++)
    {
        // Calculate clip flags for all vertices.
        mVertexData[n].mClipFlags = getClipFlags(mVertexData[n].mPosition, aClipRectangle);

        // Calculate line of the vertex. If the vertex is clipped by top or bottom, the line
        // is determined by the clip rectangle.
        if (mVertexData[n].mClipFlags & POLYGON_CLIP_TOP)
        {
            mVertexData[n].mLine = aClipRectangle.getMinYi();
        }
        else if (mVertexData[n].mClipFlags & POLYGON_CLIP_BOTTOM)
        {
            mVertexData[n].mLine = aClipRectangle.getMaxYi() - 1;
        }
        else
        {
            mVertexData[n].mLine = rationalToIntRoundDown(VECTOR2D_GETY(mVertexData[n].mPosition));
        }
    }

    // Copy the data from 0 to the last entry to make the data to loop.
    mVertexData[mVertexCount] = mVertexData[0];

    // Transform the first vertex; store.
    // Process mVertexCount - 1 times, next is n+1
    // copy the first vertex to
    // Process 1 time, next is n

    int edgeCount = 0;
    for (n = 0; n < mVertexCount; n++)
    {
        int clipSum = mVertexData[n].mClipFlags | mVertexData[n + 1].mClipFlags;
        int clipUnion = mVertexData[n].mClipFlags & mVertexData[n + 1].mClipFlags;

        if ((clipUnion & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM)) == 0 &&
            mVertexData[n].mLine != mVertexData[n + 1].mLine)
        {
            int startIndex, endIndex;
            short winding;
            if (VECTOR2D_GETY(mVertexData[n].mPosition) < VECTOR2D_GETY(mVertexData[n + 1].mPosition))
            {
                startIndex = n;
                endIndex = n + 1;
                winding = 1;
            }
            else
            {
                startIndex = n + 1;
                endIndex = n;
                winding = -1;
            }

            int firstLine = mVertexData[startIndex].mLine + 1;
            int lastLine = mVertexData[endIndex].mLine;

            if (clipUnion & POLYGON_CLIP_RIGHT)
            {
                // Both clip to right, edge is a vertical line on the right side
                aEdges[edgeCount].mFirstLine = firstLine;
                aEdges[edgeCount].mLastLine = lastLine;
                aEdges[edgeCount].mWinding = winding;
                aEdges[edgeCount].mX = rationalToFixed(aClipRectangle.getMaxXf());
                aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);

#ifdef SUBPOLYGON_DEBUG
                DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif

                edgeCount++;
            }
            else if (clipUnion & POLYGON_CLIP_LEFT)
            {
                // Both clip to left, edge is a vertical line on the left side
                aEdges[edgeCount].mFirstLine = firstLine;
                aEdges[edgeCount].mLastLine = lastLine;
                aEdges[edgeCount].mWinding = winding;
                aEdges[edgeCount].mX = rationalToFixed(aClipRectangle.getMinXf());
                aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);

#ifdef SUBPOLYGON_DEBUG
                DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif

                edgeCount++;
            }
            else if ((clipSum & (POLYGON_CLIP_RIGHT | POLYGON_CLIP_LEFT)) == 0)
            {
                // No clipping in the horizontal direction
                RATIONAL slope = (VECTOR2D_GETX(mVertexData[endIndex].mPosition) -
                                  VECTOR2D_GETX(mVertexData[startIndex].mPosition)) /
                                 (VECTOR2D_GETY(mVertexData[endIndex].mPosition) -
                                  VECTOR2D_GETY(mVertexData[startIndex].mPosition));

                // If there is vertical clip (for the top) it will be processed here. The calculation
                // should be done for all non-clipping edges as well to determine the accurate position
                // where the edge crosses the first scanline.
                RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                                  ((RATIONAL)firstLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                aEdges[edgeCount].mFirstLine = firstLine;
                aEdges[edgeCount].mLastLine = lastLine;
                aEdges[edgeCount].mWinding = winding;
                aEdges[edgeCount].mX = rationalToFixed(startx);
                aEdges[edgeCount].mSlope = rationalToFixed(slope);

                if (lastLine - firstLine >= SLOPE_FIX_STEP)
                {
                    aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                }
                else
                {
                    aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                }

#ifdef SUBPOLYGON_DEBUG
                DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif

                edgeCount++;
            }
            else
            {
                // Clips to left or right or both.
                RATIONAL slope = (VECTOR2D_GETX(mVertexData[endIndex].mPosition) -
                                  VECTOR2D_GETX(mVertexData[startIndex].mPosition)) /
                                 (VECTOR2D_GETY(mVertexData[endIndex].mPosition) -
                                  VECTOR2D_GETY(mVertexData[startIndex].mPosition));

                // The edge may clip to both left and right.
                // The clip results in one or two new vertices, and one to three segments.
                // The rounding for scanlines may produce a result where any of the segments is
                // ignored.

                // The start is always above the end. Calculate the clip positions to clipVertices.
                // It is possible that only one of the vertices exist. This will be detected from the
                // clip flags of the vertex later, so they are initialized here.
                VertexData clipVertices[2];

                if (VECTOR2D_GETX(mVertexData[startIndex].mPosition) <
                    VECTOR2D_GETX(mVertexData[endIndex].mPosition))
                {
                    VECTOR2D_SETX(clipVertices[0].mPosition,aClipRectangle.getMinXf());
                    VECTOR2D_SETX(clipVertices[1].mPosition,aClipRectangle.getMaxXf());
                    clipVertices[0].mClipFlags = POLYGON_CLIP_LEFT;
                    clipVertices[1].mClipFlags = POLYGON_CLIP_RIGHT;
                }
                else
                {
                    VECTOR2D_SETX(clipVertices[0].mPosition,aClipRectangle.getMaxXf());
                    VECTOR2D_SETX(clipVertices[1].mPosition,aClipRectangle.getMinXf());
                    clipVertices[0].mClipFlags = POLYGON_CLIP_RIGHT;
                    clipVertices[1].mClipFlags = POLYGON_CLIP_LEFT;
                }

                int p;
                for (p = 0; p < 2; p++)
                {
                    // Check if either of the vertices crosses the edge marked for the clip vertex
                    if (clipSum & clipVertices[p].mClipFlags)
                    {
                        // The the vertex is required, calculate it.
                        VECTOR2D_SETY(clipVertices[p].mPosition,VECTOR2D_GETY(mVertexData[startIndex].mPosition) +
                                                               (VECTOR2D_GETX(clipVertices[p].mPosition) - 
                                                                VECTOR2D_GETX(mVertexData[startIndex].mPosition)) / slope);

                        // If there is clipping in the vertical direction, the new vertex may be clipped.
                        if (clipSum & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM))
                        {
                            if (VECTOR2D_GETY(clipVertices[p].mPosition) < aClipRectangle.getMinYf())
                            {
                                clipVertices[p].mClipFlags = POLYGON_CLIP_TOP;
                                clipVertices[p].mLine = aClipRectangle.getMinYi();
                            }
                            else if (VECTOR2D_GETY(clipVertices[p].mPosition) > aClipRectangle.getMaxYf())
                            {
                                clipVertices[p].mClipFlags = POLYGON_CLIP_BOTTOM;
                                clipVertices[p].mLine = aClipRectangle.getMaxYi() - 1;
                            }
                            else
                            {
                                clipVertices[p].mClipFlags = 0;
                                clipVertices[p].mLine = rationalToIntRoundDown(VECTOR2D_GETY(clipVertices[p].mPosition));
                            }
                        }
                        else
                        {
                            clipVertices[p].mClipFlags = 0;
                            clipVertices[p].mLine = rationalToIntRoundDown(VECTOR2D_GETY(clipVertices[p].mPosition));
                        }
                    }
                }

                // Now there are three or four vertices, in the top-to-bottom order of start, clip0, clip1,
                // end. What kind of edges are required for connecting these can be determined from the
                // clip flags.
                // -if clip vertex has horizontal clip flags, it doesn't exist. No edge is generated.
                // -if start vertex or end vertex has horizontal clip flag, the edge to/from the clip vertex is vertical
                // -if the line of two vertices is the same, the edge is not generated, since the edge doesn't
                //  cross any scanlines.

                // The alternative patterns are:
                // start - clip0 - clip1 - end
                // start - clip0 - end
                // start - clip1 - end

                int topClipIndex;
                int bottomClipIndex;
                if (((clipVertices[0].mClipFlags | clipVertices[1].mClipFlags) &
                     (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT)) == 0)
                {
                    // Both sides are clipped, the order is start-clip0-clip1-end
                    topClipIndex = 0;
                    bottomClipIndex = 1;

                    // Add the edge from clip0 to clip1
                    // Check that the line is different for the vertices.
                    if (clipVertices[0].mLine != clipVertices[1].mLine)
                    {
                        int firstClipLine = clipVertices[0].mLine + 1;

                        RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                                          ((RATIONAL)firstClipLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                        aEdges[edgeCount].mX = rationalToFixed(startx);
                        aEdges[edgeCount].mSlope = rationalToFixed(slope);
                        aEdges[edgeCount].mFirstLine = firstClipLine;
                        aEdges[edgeCount].mLastLine = clipVertices[1].mLine;
                        aEdges[edgeCount].mWinding = winding;

                        if (aEdges[edgeCount].mLastLine - aEdges[edgeCount].mFirstLine >= SLOPE_FIX_STEP)
                        {
                            aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                        (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                        }

#ifdef SUBPOLYGON_DEBUG
                        DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif

                        edgeCount++;
                    }
                }
                else
                {
                    // Clip at either side, check which side. The clip flag is on for the vertex
                    // that doesn't exist, i.e. has not been clipped to be inside the rect.
                    if (clipVertices[0].mClipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        topClipIndex = 1;
                        bottomClipIndex = 1;
                    }
                    else
                    {
                        topClipIndex = 0;
                        bottomClipIndex = 0;
                    }
                }

                // Generate the edges from start - clip top and clip bottom - end
                // Clip top and clip bottom may be the same vertex if there is only one 
                // clipped vertex.

                // Check that the line is different for the vertices.
                if (mVertexData[startIndex].mLine != clipVertices[topClipIndex].mLine)
                {
                    aEdges[edgeCount].mFirstLine = firstLine;
                    aEdges[edgeCount].mLastLine = clipVertices[topClipIndex].mLine;
                    aEdges[edgeCount].mWinding = winding;

                    // If startIndex is clipped, the edge is a vertical one.
                    if (mVertexData[startIndex].mClipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        aEdges[edgeCount].mX = rationalToFixed(VECTOR2D_GETX(clipVertices[topClipIndex].mPosition));
                        aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                        aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                    }
                    else
                    {
                        RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                                          ((RATIONAL)firstLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                        aEdges[edgeCount].mX = rationalToFixed(startx);
                        aEdges[edgeCount].mSlope = rationalToFixed(slope);

                        if (aEdges[edgeCount].mLastLine - aEdges[edgeCount].mFirstLine >= SLOPE_FIX_STEP)
                        {
                            aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                        (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                        }
                    }


#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif

                    edgeCount++;
                }

                // Check that the line is different for the vertices.
                if (clipVertices[bottomClipIndex].mLine != mVertexData[endIndex].mLine)
                {
                    int firstClipLine = clipVertices[bottomClipIndex].mLine + 1;

                    aEdges[edgeCount].mFirstLine = firstClipLine;
                    aEdges[edgeCount].mLastLine = lastLine;
                    aEdges[edgeCount].mWinding = winding;

                    // If endIndex is clipped, the edge is a vertical one.
                    if (mVertexData[endIndex].mClipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        aEdges[edgeCount].mX = rationalToFixed(VECTOR2D_GETX(clipVertices[bottomClipIndex].mPosition));
                        aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                        aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                    }
                    else
                    {
                        RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                                          ((RATIONAL)firstClipLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                        aEdges[edgeCount].mX = rationalToFixed(startx);
                        aEdges[edgeCount].mSlope = rationalToFixed(slope);

                        if (aEdges[edgeCount].mLastLine - aEdges[edgeCount].mFirstLine >= SLOPE_FIX_STEP)
                        {
                            aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                        (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                        }
                    }


#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount],aClipRectangle));
#endif

                    edgeCount++;
                }

            }
        }
    }

    return edgeCount;
}

#ifdef SUBPOLYGON_DEBUG

//! A debug routine for checking that generated edge is valid.
BOOL mgSubPolygon::verifyEdge(
    const PolygonEdge& aEdge, 
    const mgClipRectangle& aClipRectangle) const
{
    if (aEdge.mFirstLine < (int)aClipRectangle.getMinYi())
        return false;

    if (aEdge.mFirstLine >= (int)aClipRectangle.getMaxYi())
        return false;

    if (aEdge.mLastLine < (int)aClipRectangle.getMinYi())
        return false;

    if (aEdge.mLastLine >= (int)aClipRectangle.getMaxYi())
        return false;

    FIXED_POINT xf = rationalToFixed(aEdge.mX);
    FIXED_POINT slopef = rationalToFixed(aEdge.mSlope);
    RATIONAL xr = aEdge.mX;

    int y;
    for (y = aEdge.mFirstLine; y <= aEdge.mLastLine; y++)
    {
        int xi = FIXED_TO_INT(xf);

        if (xi < (int)aClipRectangle.getMinXi())
            return false;

        if (xi > (int)aClipRectangle.getMaxXi())
            return false;

        xi = (int)xr;

        if (xi < (int)aClipRectangle.getMinXi())
            return false;

        if (xi > (int)aClipRectangle.getMaxXi())
            return false;

        xf += slopef;
        xr += aEdge.mSlope;
    }

    return true;
}

//! A debug routine for checking that generated edge is valid.
BOOL mgSubPolygon::verifyEdge(
    const PolygonScanEdge& aEdge, 
    const mgClipRectangle& aClipRectangle) const
{
    if (aEdge.mFirstLine < (int)aClipRectangle.getMinYi())
        return false;

    if (aEdge.mFirstLine >= (int)aClipRectangle.getMaxYi())
        return false;

    if (aEdge.mLastLine < (int)aClipRectangle.getMinYi())
        return false;

    if (aEdge.mLastLine >= (int)aClipRectangle.getMaxYi())
        return false;

    FIXED_POINT x = aEdge.mX;

    int y;
    for (y = aEdge.mFirstLine; y <= aEdge.mLastLine; y++)
    {
        int xi = FIXED_TO_INT(x);

        if (xi < (int)aClipRectangle.getMinXi())
            return false;

        if (xi > (int)aClipRectangle.getMaxXi())
            return false;

        x += aEdge.mSlope;

        if ((y & SLOPE_FIX_MASK) == 0)
            x += aEdge.mSlopeFix;
    }

    return true;
}

#endif

#endif  // SUPPORT_GEN2D