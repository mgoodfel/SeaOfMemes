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
 * \brief A class for defining a clip rectangle.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef CLIPRECTANGLE_H
#define CLIPRECTANGLE_H

//! A class for defining a clip rectangle.
class mgClipRectangle
{
public:
    //! Constructor.
    mgClipRectangle()
    {
        setClip(0, 0, 0, 0, 1);
    }

    //! Constructor with initialization.
    mgClipRectangle(int aX, int aY, int aWidth, int aHeight, int aScale)
    {
        setClip(aX, aY, aWidth, aHeight, aScale);
    }

    //! Sets the clip values.
    void setClip(int aX, int aY, int aWidth, int aHeight, int aScale)
    {
        mMinXi = aX;
        mMinYi = aY * aScale;
        mMaxXi = aX + aWidth;
        mMaxYi = (aY + aHeight) * aScale;

        mXOffset = FLOAT_TO_RATIONAL(0.99f) / (RATIONAL)aScale;

        mMinXf = (RATIONAL)mMinXi + mXOffset;
        mMaxXf = (RATIONAL)mMaxXi - mXOffset;

        mMinYf = (RATIONAL)mMinYi;
        mMaxYf = (RATIONAL)mMaxYi;
    }

    //! Intersects the clip rectangle with another clip rectangle.
    void intersect(const mgClipRectangle &aClipRectangle)
    {
        if (mMinXi < aClipRectangle.mMinXi)
            mMinXi = aClipRectangle.mMinXi;
        if (mMaxXi > aClipRectangle.mMaxXi)
            mMaxXi = aClipRectangle.mMaxXi;
        if (mMinXi > mMaxXi)
            mMinXi = mMaxXi;

        if (mMinYi < aClipRectangle.mMinYi)
            mMinYi = aClipRectangle.mMinYi;
        if (mMaxYi > aClipRectangle.mMaxYi)
            mMaxYi = aClipRectangle.mMaxYi;
        if (mMinYi > mMaxYi)
            mMinYi = mMaxYi;

        mMinXf = (RATIONAL)mMinXi + mXOffset;
        mMaxXf = (RATIONAL)mMaxXi - mXOffset;

        mMinYf = (RATIONAL)mMinYi;
        mMaxYf = (RATIONAL)mMaxYi;
    }

    //! Returns the minimum X as integer.
    inline int getMinXi() const { return mMinXi; }

    //! Returns the minimum Y as integer.
    inline int getMinYi() const { return mMinYi; }

    //! Returns the maximum X as integer.
    inline int getMaxXi() const { return mMaxXi; }

    //! Returns the maximum Y as integer.
    inline int getMaxYi() const { return mMaxYi; }

    //! Returns the minimum X as rational number (typically float).
    inline RATIONAL getMinXf() const { return mMinXf; }

    //! Returns the minimum Y as rational number (typically float).
    inline RATIONAL getMinYf() const { return mMinYf; }

    //! Returns the maximum X as rational number (typically float).
    inline RATIONAL getMaxXf() const { return mMaxXf; }

    //! Returns the maximum Y as rational number (typically float).
    inline RATIONAL getMaxYf() const { return mMaxYf; }

protected:
    int mMinXi;
    int mMinYi;
    int mMaxXi;
    int mMaxYi;
    RATIONAL mMinXf;
    RATIONAL mMinYf;
    RATIONAL mMaxXf;
    RATIONAL mMaxYf;
    RATIONAL mXOffset;
};

#endif // !CLIPRECTANGLE_H
