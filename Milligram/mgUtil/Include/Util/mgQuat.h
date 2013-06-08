/*
  Copyright (C) 1995-2013 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source)
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.)

  This source code is distributed "AS IS", with no warranty expressed or implied.
  The user assumes all risks related to quality, accuracy and fitness of use.

  Except where noted, this source code is the sole work of the author, but it has 
  not been checked for any intellectual property infringements such as copyrights, 
  trademarks or patents.  The user assumes all legal risks.  The original version 
  may be found at "http://www.sea-of-memes.com".  The author is not responsible 
  for subsequent alterations.

  Retain this copyright notice and add your own copyrights and revisions above
  this notice.
*/

#ifndef MGQUAT_H
#define MGQUAT_H

class mgPoint3;
class mgMatrix4;

/*
  Quaternion implementation
*/
class mgQuat
{
public:
  double x;
  double y;
  double z;
  double w;

  // default constructor
  mgQuat();

  // constructor
  mgQuat(
    double nx,
    double ny,
    double nz,
    double nw);

  // constructor
  mgQuat(
    const mgPoint3& axis,     // unit axis of rotation
    double angle);            // rotation angle (radians)

  // copy constructor
  mgQuat(
    const mgQuat& other);

  // assignment operator
  mgQuat& operator=(
    const mgQuat& other);

  // multiply
  void multiply(
    const mgQuat& other);

  // multiply
  void multiply(
    const mgPoint3& axis,     // unit axis of rotation
    double angle)             // rotation angle (radians)
  {
    mgQuat other(axis, angle);
    multiply(other);
  }

  // left multiply
  void leftMultiply(
    const mgQuat& other);

  // left multiply
  void leftMultiply(
    const mgPoint3& axis,     // unit axis of rotation
    double angle)             // rotation angle (radians)
  {
    mgQuat other(axis, angle);
    leftMultiply(other);
  }

  // invert rotation
  void inverse();

  // transform point
  void mapPt(
    double fromX, 
    double fromY, 
    double fromZ, 
    double &toX,
    double &toY,
    double &toZ) const;

  // transform point
  void mapPt(
    const mgPoint3 &from,
    mgPoint3 &to) const
  {
    mapPt(from.x, from.y, from.z, to.x, to.y, to.z);
  }

  // create matrix from SQT
  void toMatrix(
    const mgPoint3& scale,
    const mgPoint3& transform,
    mgMatrix4& matrix) const;

  // create matrix from SQT
  void toMatrix(
    double scale,
    const mgPoint3& transform,
    mgMatrix4& matrix) const;

  // create matrix from quaternion
  void toMatrix(
    mgMatrix4& matrix) const;

  // linear interpolation
  void lerp(
    const mgQuat& other,
    double factor,
    mgQuat& result);

  // spherical linear interpolation
  void slerp(
    const mgQuat& other,
    double factor,
    mgQuat& result);

};

#endif
