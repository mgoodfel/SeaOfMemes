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

#include "stdafx.h"
#ifdef SUPPORT_GEN2D

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPolyTypes.h"
#include "mgSubPolygon.h"
#include "mgPolygon.h"

//! Constructor.
/*! Note that the ownership of the sub-polygon array is transferred.
 */
mgPolygon::mgPolygon(mgSubPolygon **aSubPolygons, int aSubPolygonCount)
{
    mSubPolygons = aSubPolygons;
    mSubPolygonCount = aSubPolygonCount;
}


//! Virtual destructor.
mgPolygon::~mgPolygon()
{
    int n;
    for (n = 0; n < mSubPolygonCount; n++)
        delete mSubPolygons[n];
    delete mSubPolygons;
}


#endif
