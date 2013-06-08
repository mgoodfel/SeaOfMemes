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
 * \brief A class for handling polygons. Each polygon is a set of sub-polygons.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */

#ifndef MGPOLYGON_H
#define MGPOLYGON_H

class mgSubPolygon;

//! A class for handling polygons. Each polygon is a set of sub-polygons.
class mgPolygon // : public PolygonWrapper
{
public:
    //! Constructor.
    /*! Note that the ownership of the sub-polygon array is transferred.
     */
    mgPolygon(
      mgSubPolygon** aSubPolygons, 
      int aSubPolygonCount);

    //! Virtual destructor.
    virtual ~mgPolygon();

    //! Returns the amount of sub-polygons.
    inline int getSubPolygonCount() const
    {
        return mSubPolygonCount;
    }

    //! Returns a sub-polygon at given index.
    inline const mgSubPolygon& getSubPolygon(
      int aIndex) const
    {
        return *mSubPolygons[aIndex];
    }

protected:
    int mSubPolygonCount;
    mgSubPolygon **mSubPolygons;
};

#endif // !POLYGON_H
