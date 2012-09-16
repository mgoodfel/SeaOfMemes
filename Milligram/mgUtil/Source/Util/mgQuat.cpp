/*
  Copyright (C) 1995-2012 by Michael J. Goodfellow

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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

//--------------------------------------------------------------
// default constructor
mgQuat::mgQuat()
{
  // default to unit X axis
  x = 1.0;
  y = 0.0;
  z = 0.0;
  w = 0.0;
}

//--------------------------------------------------------------
// constructor
mgQuat::mgQuat(
  double nx,  
  double ny,
  double nz,
  double nw)  
{
  x = nx;
  y = ny;
  z = nz;
  w = nw;
}

//--------------------------------------------------------------
// constructor
mgQuat::mgQuat(
  const mgPoint3& axis,     // unit axis of rotation
  double angle)             // rotation angle (radians)
{
  double hsa = sin(angle/2.0);
  x = axis.x * hsa;
  y = axis.y * hsa;
  z = axis.z * hsa;
  w = cos(angle/2.0);
}

//--------------------------------------------------------------
// copy constructor
mgQuat::mgQuat(
  const mgQuat& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
}

//--------------------------------------------------------------
// assignment operator
mgQuat& mgQuat::operator=(
  const mgQuat& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
  return *this;
}

//--------------------------------------------------------------
// multiply
void mgQuat::multiply(
  const mgQuat& other)
{
  mgPoint3 p(x, y, z);
  mgPoint3 q(other.x, other.y, other.z);
  double dot = p.dot(q);
  q.cross(p);
  x = other.w * x + w * other.x + q.x;
  y = other.w * y + w * other.y + q.y;
  z = other.w * z + w * other.z + q.z;
  w = other.w * w - dot;
}

//--------------------------------------------------------------
// left multiply
void mgQuat::leftMultiply(
  const mgQuat& other)
{
  mgPoint3 p(x, y, z);
  mgPoint3 q(other.x, other.y, other.z);
  double dot = p.dot(q);
  p.cross(q);
  x = w * other.x + other.w * x + p.x;
  y = w * other.y + other.w * y + p.y;
  z = w * other.z + other.w * z + p.z;
  w = w * other.w - dot;
}

//--------------------------------------------------------------
// invert rotation
void mgQuat::inverse()
{
  x = -x;
  y = -y;
  z = -z;
}

//--------------------------------------------------------------
// transform point
void mgQuat::mapPt(
  double fromX, 
  double fromY, 
  double fromZ, 
  double &toX,
  double &toY,
  double &toZ) const
{
  mgQuat v(fromX, fromY, fromZ, 0.0);
  mgQuat inv(-x, -y, -z, w);
  mgQuat result(*this);
  result.leftMultiply(v);
  result.leftMultiply(inv);
  toX = result.x;
  toY = result.y;
  toZ = result.z;
}

//--------------------------------------------------------------
// create matrix from SQT
void mgQuat::toMatrix(
  const mgPoint3& scale,
  const mgPoint3& transform,
  mgMatrix4& matrix) const
{
  matrix._11 = scale.x * (1.0 - 2.0*y*y - 2.0*z*z);
  matrix._12 = scale.y * (2.0*x*y + 2.0*z*w);
  matrix._13 = scale.z * (2.0*x*z - 2.0*y*w);
  matrix._14 = 0.0;

  matrix._21 = scale.x * (2.0*x*y - 2.0*z*w);
  matrix._22 = scale.y * (1.0 - 2.0*x*x - 2.0*z*z);
  matrix._23 = scale.z * (2.0*y*z + 2.0*x*w);
  matrix._24 = 0.0;

  matrix._31 = scale.x * (2.0*x*z + 2.0*y*w);
  matrix._32 = scale.y * (2.0*y*z - 2.0*x*w);
  matrix._33 = scale.z * (1.0 - 2.0*x*x - 2.0*y*y);
  matrix._34 = 0.0;

  matrix._41 = transform.x;
  matrix._42 = transform.y;
  matrix._43 = transform.z;
  matrix._44 = 1.0;
}

//--------------------------------------------------------------
// create matrix from SQT
void mgQuat::toMatrix(
  double scale,
  const mgPoint3& transform,
  mgMatrix4& matrix) const
{
  matrix._11 = scale * (1.0 - 2.0*y*y - 2.0*z*z);
  matrix._12 = scale * (2.0*x*y + 2.0*z*w);
  matrix._13 = scale * (2.0*x*z - 2.0*y*w);
  matrix._14 = 0.0;

  matrix._21 = scale * (2.0*x*y - 2.0*z*w);
  matrix._22 = scale * (1.0 - 2.0*x*x - 2.0*z*z);
  matrix._23 = scale * (2.0*y*z + 2.0*x*w);
  matrix._24 = 0.0;

  matrix._31 = scale * (2.0*x*z + 2.0*y*w);
  matrix._32 = scale * (2.0*y*z - 2.0*x*w);
  matrix._33 = scale * (1.0 - 2.0*x*x - 2.0*y*y);
  matrix._34 = 0.0;

  matrix._41 = transform.x;
  matrix._42 = transform.y;
  matrix._43 = transform.z;
  matrix._44 = 1.0;
}

//--------------------------------------------------------------
// create matrix from quaternion
void mgQuat::toMatrix(
  mgMatrix4& matrix) const
{
  matrix._11 = 1.0 - 2.0*y*y - 2.0*z*z;
  matrix._12 = 2.0*x*y + 2.0*z*w;
  matrix._13 = 2.0*x*z - 2.0*y*w;
  matrix._14 = 0.0;

  matrix._21 = 2.0*x*y - 2.0*z*w;
  matrix._22 = 1.0 - 2.0*x*x - 2.0*z*z;
  matrix._23 = 2.0*y*z + 2.0*x*w;
  matrix._24 = 0.0;

  matrix._31 = 2.0*x*z + 2.0*y*w;
  matrix._32 = 2.0*y*z - 2.0*x*w;
  matrix._33 = 1.0 - 2.0*x*x - 2.0*y*y;
  matrix._34 = 0.0;

  matrix._41 = 0.0;
  matrix._42 = 0.0;
  matrix._43 = 0.0;
  matrix._44 = 1.0;
}

//--------------------------------------------------------------
// linear interpolation
void mgQuat::lerp(
  const mgQuat& other,
  double factor,
  mgQuat& result)
{
  double inv = 1.0-factor;
  result.x = inv*x + factor*other.x;
  result.y = inv*y + factor*other.y;
  result.z = inv*z + factor*other.z;
  result.w = inv*w + factor*other.w;

  // normalize
  double len = sqrt(result.x*result.x + result.y*result.y + result.z*result.z + result.w*result.w);
  result.x /= len;
  result.y /= len;
  result.z /= len;
  result.w /= len;
}

//--------------------------------------------------------------
// spherical linear interpolation
void mgQuat::slerp(
  const mgQuat& other,
  double factor,
  mgQuat& result)
{
  double angle = acos(x*other.x + y*other.y + z*other.z + w*other.w);
  double sinAngle = sin(angle);
  double pw = sin((1.0-factor)*angle) / sinAngle;
  double qw = sin(factor*angle)/ sinAngle;

  result.x = pw*x + qw*other.x;
  result.y = pw*y + qw*other.y;
  result.z = pw*z + qw*other.z;
  result.w = pw*w + qw*other.w;

  // normalize
  double len = sqrt(result.x*result.x + result.y*result.y + result.z*result.z + result.w*result.w);
  result.x /= len;
  result.y /= len;
  result.z /= len;
  result.w /= len;
}

