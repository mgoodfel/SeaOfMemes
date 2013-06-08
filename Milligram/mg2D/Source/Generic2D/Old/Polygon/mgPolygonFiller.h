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
 * \brief PolygonFiller implementation version F. Similar to E, but does the rendering one scanline at a time.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef POLYGON_FILLER_H
#define POLYGON_FILLER_H

#include "mgPolyTypes.h"
#include "mgSpanExtents.h"
#include "mgPolygon.h"
#include "mgSubPolygon.h"
#include "mgSubPixel.h"
#include "mgNonZeroMask.h"

//! Version F of the polygon filler. Similar to E, but does the rendering one scanline at a time.
class mgPolygonFiller 
{
public:

    //! Constructor.
    mgPolygonFiller();

    //! Virtual destructor.
    virtual ~mgPolygonFiller();

    //! Initializer.
    /*! aWidth and aHeight define the maximum output size for the filler.
     *  The filler will output to larger bitmaps as well, but the output will
     *  be cropped. aEdgeCount defines the initial amount of edges available for
     *  rendering.
     */
    BOOL init(
        DWORD* aBitmapData,
        unsigned int aBitmapWidth,
        unsigned int aBitmapHeight,
        unsigned int aBitmapPitch,
        unsigned int aEdgeCount);

    //! Sets the clip rectangle for the polygon filler.
    virtual void setClipRect(
        unsigned int aX, 
        unsigned int aY, 
        unsigned int aWidth, 
        unsigned int aHeight);

    //! Renders the polygon with even-odd fill.
    /*! \param aTarget the target bitmap.
     *  \param aPolygon the polygon to render.
     *  \param aColor the color to be used for rendering.
     *  \param aAlphaMode treatment of alpha color values.
     *  \param aTransformation the transformation matrix.
     */
    virtual void renderEvenOdd(
        const mgPolygon *aPolygon, 
        DWORD aColor,
        int aAlphaMode,
        const MATRIX2D &aTransformation);

    //! Renders the polygon with non-zero winding fill.
    /*! \param aTarget the target bitmap.
     *  \param aPolygon the polygon to render.
     *  \param aColor the color to be used for rendering.
     *  \param aAlphaMode treatment of alpha color values.
     *  \param aTransformation the transformation matrix.
     */
    virtual void renderNonZeroWinding(
        const mgPolygon *aPolygon, 
        DWORD aColor, 
        int aAlphaMode,
        const MATRIX2D &aTransformation);

protected:
    //! Adds an edge.
    BOOL addEdge(
        const PolygonEdge &aEdge);

    //! Renders the edges from the current vertical index using even-odd fill.
    inline void renderEvenOddEdges(
        PolygonScanEdge*& aActiveEdgeTable, 
        mgSpanExtents& aEdgeExtents, 
        int aCurrentLine);

    //! Renders the mask to the canvas with even-odd fill, alpha set mode
    void fillEvenOddAlphaSet(
        DWORD aColor);

    //! Renders the mask to the canvas with even-odd fill, alpha merge mode
    void fillEvenOddAlphaMerge(
        DWORD aColor);

    //! Renders the mask to the canvas with even-odd fill, alpha RGB mode
    void fillEvenOddAlphaRGB(
        DWORD aColor);

    //! Renders the edges from the current vertical index using non-zero winding fill.
    inline void renderNonZeroEdges(
        PolygonScanEdge*& aActiveEdgeTable, 
        mgSpanExtents& aEdgeExtents, 
        int aCurrentLine);

    //! Renders the mask to the canvas with non-zero winding fill, alpha set mode
    void fillNonZeroAlphaSet(
        DWORD aColor);

    //! Renders the mask to the canvas with non-zero winding fill, alpha merge mode
    void fillNonZeroAlphaMerge(
        DWORD aColor);

    //! Renders the mask to the canvas with non-zero winding fill, alpha RGB mode
    void fillNonZeroAlphaRGB(
        DWORD aColor);

    //! Resets the fill extents.
    void resetExtents();

    //! Resizes the edge storage.
    BOOL resizeEdgeStorage(
        int aIncrement);

    //! Returns the amount of free edges in the edge storage.
    inline int getFreeEdgeCount()
    {
        return mEdgeCount - mCurrentEdge - 1;
    }

    SUBPIXEL_DATA *mMaskBuffer;
    mgNonZeroMask *mWindingBuffer;
    mgSpanExtents mVerticalExtents;

    PolygonScanEdge **mEdgeTable;
    PolygonScanEdge *mEdgeStorage;
    unsigned int mEdgeCount;
    unsigned int mCurrentEdge;

    DWORD *mBitmapData;
    unsigned int mBitmapPitch;
    unsigned int mBitmapWidth;
    unsigned int mBufferWidth;
    unsigned int mBitmapHeight;

    mgClipRectangle mClipRect;

    MATRIX2D mRemappingMatrix;
};

#endif
