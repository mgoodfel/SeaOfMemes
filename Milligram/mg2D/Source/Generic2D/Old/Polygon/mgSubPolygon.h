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


#ifndef SUB_POLYGON_H
#define SUB_POLYGON_H

#include "mgClipRectangle.h"

// #define SUBPOLYGON_DEBUG

enum POLYGON_CLIP_FLAGS
{
    POLYGON_CLIP_NONE = 0x00,
    POLYGON_CLIP_LEFT = 0x01,
    POLYGON_CLIP_RIGHT = 0x02,
    POLYGON_CLIP_TOP = 0x04,
    POLYGON_CLIP_BOTTOM = 0x08,
};

#define CLIP_SUM_SHIFT 8
#define CLIP_UNION_SHIFT 4

/*! Fixed point math inevitably introduces rounding error to the DDA. The error is
 *  fixed every now and then by a separate fix value. The defines below set these.
 */
#define SLOPE_FIX_SHIFT 8
#define SLOPE_FIX_STEP (1 << SLOPE_FIX_SHIFT)
#define SLOPE_FIX_MASK (SLOPE_FIX_STEP - 1)
#define SLOPE_FIX_SCANLINES (1 << (SLOPE_FIX_SHIFT - SUBPIXEL_SHIFT))
#define SLOPE_FIX_SCANLINE_MASK (SLOPE_FIX_SCANLINES - 1)

//! A polygon edge.
class PolygonEdge
{
public:
    int mFirstLine;
    int mLastLine;
    short mWinding;
    RATIONAL mX;
    RATIONAL mSlope;
};

//! A more optimized representation of a polygon edge.
class PolygonScanEdge
{
public:
    int mLastLine;
    int mFirstLine;
    short mWinding;
    FIXED_POINT mX;
    FIXED_POINT mSlope;
    FIXED_POINT mSlopeFix;
    class PolygonScanEdge *mNextEdge;
};

//! A class for holding processing data regarding the vertex.
class VertexData
{
public:
    VECTOR2D mPosition;
    int mClipFlags;
    int mLine;
};

//! A class for handling sub-polygons. Each sub-polygon is a continuos, closed set of edges.
class mgSubPolygon
{
public:
    //! Constructor.
    /*! Note that the ownership of the vertex array is transferred.
     */
    mgSubPolygon(
      VECTOR2D *aVertices, 
      int aVertexCount);

    //! Initializer.
    BOOL init();

    //! Destructor.
    ~mgSubPolygon();

    //! Returns the amount of vertices in the polygon.
    inline int getVertexCount() const
    {
        return mVertexCount;
    }

    //! Returns a vertex at given position.
    inline const VECTOR2D & getVertex(int aIndex) const
    {
        return mVertices[aIndex];
    }

    //! Calculates the edges of the polygon with transformation and clipping to aEdges array.
    /*! Note that this may return upto three times the amount of edges that aVertexCount defines,
     *  in the unlucky case where both left and right side get clipped for all edges.
     *  \param aFirstVertex the index for the first vertex.
     *  \param aVertexCount the amount of vertices to convert.
     *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
     *  \param aTransformation the transformation matrix for the polygon.
     *  \param aClipRectangle the clip rectangle.
     *  \return the amount of edges in the result.
     */
    int getEdges(
      int aFirstVertex, 
      int aVertexCount, 
      PolygonEdge *aEdges, 
      const MATRIX2D &aTransformation, 
      const mgClipRectangle &aClipRectangle) const;

    //! Calculates the edges of the polygon with transformation and clipping to aEdges array.
    /*! Note that this may return upto three times the amount of edges that the polygon has vertices,
     *  in the unlucky case where both left and right side get clipped for all edges.
     *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
     *  \param aTransformation the transformation matrix for the polygon.
     *  \param aClipRectangle the clip rectangle.
     *  \return the amount of edges in the result.
     */
    int getScanEdges(
      PolygonScanEdge *aEdges, 
      const MATRIX2D &aTransformation, 
      const mgClipRectangle &aClipRectangle) const;

protected:
    //! Calculates the clip flags for a point.
    inline int getClipFlags(
      const VECTOR2D &aPoint, 
      const mgClipRectangle &aClipRectangle) const;

    //! Creates a polygon edge between two vectors.
    /*! Clips the edge vertically to the clip rectangle. Returns true for edges that
     *  should be rendered, false for others.
     */
    BOOL getEdge(
      const VECTOR2D &aStart, 
      const VECTOR2D &aEnd, 
      PolygonEdge &aEdge, 
      const mgClipRectangle &aClipRectangle) const;

    //! Creates a vertical polygon edge between two y values.
    /*! Clips the edge vertically to the clip rectangle. Returns true for edges that
     *  should be rendered, false for others.
     */
    BOOL getVerticalEdge(
      RATIONAL aStartY, 
      RATIONAL aEndY, 
      RATIONAL aX, 
      PolygonEdge &aEdge, 
      const mgClipRectangle &aClipRectangle) const;

    //! Returns the edge(s) between two vertices in aVertexData.
    inline int getScanEdge(
      VertexData *aVertexData, 
      short aWinding, 
      PolygonScanEdge *aEdges, 
      const mgClipRectangle &aClipRectangle) const;

#ifdef SUBPOLYGON_DEBUG
    //! A debug routine for checking that generated edge is valid.
    BOOL verifyEdge(const PolygonEdge &aEdge, const mgClipRectangle &aClipRectangle) const;

    //! A debug routine for checking that generated edge is valid.
    BOOL verifyEdge(const PolygonScanEdge &aEdge, const mgClipRectangle &aClipRectangle) const;
#endif

    int mVertexCount;
    VECTOR2D *mVertices;
    VertexData *mVertexData;
};

#endif // !SUB_POLYGON_H
