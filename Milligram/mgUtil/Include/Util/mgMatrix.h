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

  Note: the matrix inversion routines mgMatrix4::inverse, ludcmp and lubksb are not
  the work of Michael Goodfellow.  The source is unknown.
*/
#ifndef MGMATRIX_H
#define MGMATRIX_H

#ifndef PI
const double PI = 2.0*asin(1.0);
#endif

/*
  A two-dimensional point.
*/
class mgPoint2
{
public:
  double x;
  double y;

  // constructor
  mgPoint2()
  {
    x = y = 0.0;
  }

  // constructor
  mgPoint2(
    double newX, 
    double newY)
  {
    x = newX;
    y = newY;
  }

  // copy constructor
  mgPoint2(
    const mgPoint2 &other)
  {
    x = other.x;
    y = other.y;
  }

  // set point
  void set(
    double newX, 
    double newY)
  {
    x = newX;
    y = newY;
  }

  // return dot product with other vector
  double dot(
    const mgPoint2 &pt) const
  {
    return x * pt.x + y * pt.y;
  }

  // return dot product
  double dot(
    double otherX, 
    double otherY) const
  {
    return x * otherX + y * otherY;
  }

  // add points
  void add(
    const mgPoint2 &pt)
  {
    x += pt.x;
    y += pt.y;
  }

  // subtract points
  void subtract(
    const mgPoint2 &pt)
  {
    x -= pt.x;
    y -= pt.y;
  }

  // return length
  double length() const
  {
    return sqrt(x*x + y*y);
  }

  // scale by constant
  void scale(
    double s)
  {
    x *= s;
    y *= s;
  }

  // normalize
  void normalize()
  {
    double len = sqrt(x*x + y*y);
    x /= len;
    y /= len;
  }

  // linear interpolation
  void lerp(
    const mgPoint2& other,
    double t)
  {
    x += (other.x - x)*t;
    y += (other.y - y)*t;
  }
};

/*
  A three-dimensional point.
*/
class mgPoint3
{
public:
  double x;
  double y;
  double z;

  // constructor
  mgPoint3()
  {
    x = y = z = 0.0;
  }

  // constructor
  mgPoint3(
    double newX, 
    double newY, 
    double newZ)
  {
    x = newX;
    y = newY;
    z = newZ;
  }

  // copy constructor
  mgPoint3(
    const mgPoint3 &other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
  }

  // assignment operator
  mgPoint3& operator=(
    const mgPoint3 &other)
  {
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
  }

  // set values
  void set(
    double newX,
    double newY, 
    double newZ)
  {
    x = newX;
    y = newY;
    z = newZ;
  }

  // comparison
  BOOL operator==(
    const mgPoint3 &other)
  {
    return x == other.x && y == other.y && z == other.z;
  }

  // return dot product with other vector
  double dot(
    const mgPoint3 &pt) const
  {
    return x * pt.x + y * pt.y + z * pt.z;
  }

  // return dot product
  double dot(
    double otherX, 
    double otherY, 
    double otherZ) const
  {
    return x * otherX + y * otherY + z * otherZ;
  }

  // replace with cross product with other vector
  void cross(
    const mgPoint3 &pt)
  {
    double newX = y*pt.z - z*pt.y;
    double newY = z*pt.x - x*pt.z;
    double newZ = x*pt.y - y*pt.x;
    x = newX;
    y = newY;
    z = newZ;
  }

  // return length
  double length() const
  {
    return sqrt(x*x + y*y + z*z);
  }

  // scale by constant
  void scale(
    double s)
  {
    x *= s;
    y *= s;
    z *= s;
  }

  // add points
  void add(
    const mgPoint3 &pt)
  {
    x += pt.x;
    y += pt.y;
    z += pt.z;
  }

  // subtract points
  void subtract(
    const mgPoint3 &pt)
  {
    x -= pt.x;
    y -= pt.y;
    z -= pt.z;
  }

  // normalize
  void normalize()
  {
    double len = sqrt(x*x + y*y + z*z);
    x /= len;
    y /= len;
    z /= len;
  }

  // linear interpolation
  void lerp(
    const mgPoint3& other,
    double t)
  {
    x += (other.x - x)*t;
    y += (other.y - y)*t;
    z += (other.z - z)*t;
  }
};

/*
  A four-dimensional point.
*/
class mgPoint4
{
public:
  double x;
  double y;
  double z;
  double w;

  // constructor
  mgPoint4()
  {
    x = y = z = w = 0.0;
  }

  // constructor
  mgPoint4(
    double newX, 
    double newY, 
    double newZ,
    double newW)
  {
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
  }

  // copy constructor
  mgPoint4(
    const mgPoint4 &other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
  }

  // assignment operator
  mgPoint4& operator=(
    const mgPoint4 &other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;

    return *this;
  }

  // set values
  void set(
    double newX,
    double newY, 
    double newZ,
    double newW)
  {
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
  }

  // comparison
  BOOL operator==(
    const mgPoint4 &other)
  {
    return x == other.x && y == other.y && z == other.z && w == other.w;
  }

};

/*
  A 3 by 3 matrix.
*/
class mgMatrix3
{
public:
  double _11;
  double _12;
  double _13;
  double _21;
  double _22;
  double _23;
  double _31;
  double _32;
  double _33;

  mgMatrix3();
  mgMatrix3(
    const mgMatrix3 &other);

  double& operator()(
    int row, 
    int column) 
  { 
    return ((double *) this)[row*3+column]; 
  }

  const double& operator()(
    int row, 
    int column) const 
  { 
    return ((double *) this)[row*3+column]; 
  }

  void loadIdentity();

  void translate(
    const mgPoint2& pt);

  void scale(
    const mgPoint2& pt);

  void rotate(
    double rad);

  void mapPt(
    const mgPoint2 &from,
    mgPoint2 &to) const;

  void transpose();

  void multiply(
    const mgMatrix3 &m);

  void leftMultiply(
    const mgMatrix3 &m);
};

/*
  A 4 by 4 matrix.
*/
class mgMatrix4 
{
public:
  double _11;
  double _12;
  double _13;
  double _14;
  double _21;
  double _22;
  double _23;
  double _24;
  double _31;
  double _32;
  double _33;
  double _34;
  double _41;
  double _42;
  double _43;
  double _44;

  mgMatrix4();
  mgMatrix4(
    const mgMatrix4 &other);

  double& operator()(
    int iRow, 
    int iColumn) 
  { 
    return ((double *) this)[iRow*4+iColumn]; 
  }

  const double& operator()(
    int iRow, 
    int iColumn) const 
  { 
    return ((double *) this)[iRow*4+iColumn]; 
  }

  void loadIdentity();

  void transpose();

  void multiply(
    const mgMatrix4 &m);

  void leftMultiply(
    const mgMatrix4 &m);

  void translate(
    double x, 
    double y,
    double z);

  void translate(
    const mgPoint3& pt)
  {
    translate(pt.x, pt.y, pt.z);
  }

  void rotateXDeg(
    double deg);

  void rotateXRad(
    double rads);

  void rotateYDeg(
    double deg);

  void rotateYRad(
    double rads);

  void rotateZDeg(
    double deg);

  void rotateZRad(
    double rads);

  void scale(
    double xSize,
    double ySize,
    double zSize);

  void scale(
    double size)
  {
    scale(size, size, size);
  }

  void mapPt(
    double fromX, 
    double fromY, 
    double fromZ, 
    double &toX,
    double &toY,
    double &toZ) const;

  void mapPt(
    const mgPoint3 &from,
    mgPoint3 &to) const;

  void mapPt(
    double fromX, 
    double fromY, 
    double fromZ, 
    double fromW, 
    double &toX,
    double &toY,
    double &toZ,
    double &toW) const;

  void mapPt(
    const mgPoint4 &from,
    mgPoint4 &to) const;

  void invertPt(
    double fromX, 
    double fromY, 
    double fromZ, 
    double &toX,
    double &toY,
    double &toZ) const;

  void invertPt(
    const mgPoint3 &from,
    mgPoint3 &to) const;

  // create inverse of argument matrix
  void inverse(
    const mgMatrix4 &m);

  // print on output
  void print(
    FILE* outFile);
};

#endif
