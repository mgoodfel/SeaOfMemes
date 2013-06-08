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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

//-----------------------------------------------------------------------------
// constructor
mgMatrix3::mgMatrix3()
{
  loadIdentity();
}

//-----------------------------------------------------------------------------
// constructor
mgMatrix3::mgMatrix3(
  const mgMatrix3 &other)
{
  memcpy(&_11, &other._11, sizeof(mgMatrix3));
}

//-----------------------------------------------------------------------------
// Does the matrix operation: [Q] = [A] * [B]. 
void mgMatrix3::multiply(
  const mgMatrix3& m)
{
  double* pA = (double*)&_11;
  double* pB = (double*)&m._11;
  double pM[9];

  memset(pM, 0, sizeof(mgMatrix3));

  for (WORD i = 0; i<3; i++) 
    for (WORD j = 0; j<3; j++) 
      for (WORD k = 0; k<3; k++) 
        pM[3*i+j] += pA[3*i+k] * pB[3*k+j];

  memcpy(&_11, pM, sizeof(mgMatrix3));
}

//-----------------------------------------------------------------------------
// Does the matrix operation: [Q] = [B] * [A]. 
void mgMatrix3::leftMultiply(
  const mgMatrix3& m)
{
  double* pA = (double*)&m._11;
  double* pB = (double*)&_11;
  double pM[9];

  memset(pM, 0, sizeof(mgMatrix3));

  for (WORD i = 0; i<3; i++) 
    for (WORD j = 0; j<3; j++) 
      for (WORD k = 0; k<3; k++) 
        pM[3*i+j] += pA[3*i+k] * pB[3*k+j];

  memcpy(&_11, pM, sizeof(mgMatrix3));
}

//-----------------------------------------------------------------------------
// load with identity matrix
void mgMatrix3::loadIdentity()
{
  _11 = _22 = _33 = 1.0;
  _12 = _13 =  0.0;
  _21 = _23 =  0.0;
  _31 = _32 =  0.0;
}

//-----------------------------------------------------------------------------
// transpose matrix
void mgMatrix3::transpose()
{
  double a;
  a = _12; _12 = _21; _21 = a;
  a = _13; _13 = _31; _31 = a;
  a = _23; _23 = _32; _32 = a;
}

//-----------------------------------------------------------------------------
// translate matrix by coordinates
void mgMatrix3::translate(
  const mgPoint2& pt)
{
  _31 += pt.x;
  _32 += pt.y;
}

//-----------------------------------------------------------------------------
// rotate the matrix 
void mgMatrix3::rotate(
  double rads)
{
  mgMatrix3 rot;
  rot._22 = cos(rads);
  rot._23 = sin(rads);
  rot._32 = -rot._23; // -sinf(rads);
  rot._33 = rot._22; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// scale matrix 
void mgMatrix3::scale(
  const mgPoint2& pt)
{
  mgMatrix3 scale;
  scale._11 = pt.x;
  scale._22 = pt.y;
  multiply(scale);
}

//-----------------------------------------------------------------------------
// map a point through the matrix
void mgMatrix3::mapPt(
  const mgPoint2 &from,
  mgPoint2 &to) const
{
  to.x = _11 * from.x + _21 * from.y + _31;
  to.y = _12 * from.x + _22 * from.y + _32;
}

//-----------------------------------------------------------------------------
// constructor
mgMatrix4::mgMatrix4()
{
  loadIdentity();
}

//-----------------------------------------------------------------------------
// constructor
mgMatrix4::mgMatrix4(
  const mgMatrix4 &other)
{
  memcpy(&_11, &other._11, sizeof(mgMatrix4));
}

//-----------------------------------------------------------------------------
// Does the matrix operation: [Q] = [A] * [B]. 
void mgMatrix4::multiply(
  const mgMatrix4& m)
{
  double* pA = (double*)&_11;
  double* pB = (double*)&m._11;
  double  pM[16];

  memset(pM, 0, sizeof(pM));

  for (WORD i = 0; i<4; i++) 
    for (WORD j = 0; j<4; j++) 
      for (WORD k = 0; k<4; k++) 
        pM[4*i+j] += pA[4*i+k] * pB[4*k+j];

  memcpy(&_11, pM, sizeof(mgMatrix4));
}

//-----------------------------------------------------------------------------
// Does the matrix operation: [Q] = [B] * [A]. 
void mgMatrix4::leftMultiply(
  const mgMatrix4& m)
{
  double* pA = (double*)&m._11;
  double* pB = (double*)&_11;
  double  pM[16];

  memset(pM, 0, sizeof(pM));

  for (WORD i = 0; i<4; i++) 
    for (WORD j = 0; j<4; j++) 
      for (WORD k = 0; k<4; k++) 
        pM[4*i+j] += pA[4*i+k] * pB[4*k+j];

  memcpy(&_11, pM, sizeof(mgMatrix4));
}

//-----------------------------------------------------------------------------
// load with identity matrix
void mgMatrix4::loadIdentity()
{
  _11 = _22 = _33 = _44 = 1.0;
  _12 = _13 = _14 = 0.0;
  _21 = _23 = _24 = 0.0;
  _31 = _32 = _34 = 0.0;
  _41 = _42 = _43 = 0.0;
}

//-----------------------------------------------------------------------------
// transpose matrix
void mgMatrix4::transpose()
{
  double a;
  a = _12; _12 = _21; _21 = a;
  a = _13; _13 = _31; _31 = a;
  a = _14; _14 = _41; _41 = a;
  a = _23; _23 = _32; _32 = a;
  a = _24; _24 = _42; _42 = a;
  a = _34; _34 = _43; _43 = a;
}

//-----------------------------------------------------------------------------
// translate matrix by coordinates
void mgMatrix4::translate(
  double x,
  double y, 
  double z)
{
  _41 += x;
  _42 += y;
  _43 += z;
}

//-----------------------------------------------------------------------------
// rotate the matrix about X axis
void mgMatrix4::rotateXDeg(
  double degs)
{
  mgMatrix4 rot;
  double rads = (degs * PI)/180.0;
  rot._22 = cos(rads);
  rot._23 = sin(rads);
  rot._32 = -rot._23; // -sinf(rads);
  rot._33 = rot._22; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// rotate the matrix about X axis
void mgMatrix4::rotateXRad(
  double rads)
{
  mgMatrix4 rot;
  rot._22 = cos(rads);
  rot._23 = sin(rads);
  rot._32 = -rot._23; // -sinf(rads);
  rot._33 = rot._22; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// Create Rotation matrix about Y axis
void mgMatrix4::rotateYDeg(
  double degs)
{
  mgMatrix4 rot;
  double rads = (degs * PI)/180.0;
  rot._11 = cos(rads);
  rot._13 = -sin(rads);
  rot._31 = -rot._13; // sinf(rads);
  rot._33 = rot._11; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// Create Rotation matrix about Y axis
void mgMatrix4::rotateYRad(
  double rads)
{
  mgMatrix4 rot;
  rot._11 = cos(rads);
  rot._13 = -sin(rads);
  rot._31 = -rot._13; // sinf(rads);
  rot._33 = rot._11; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// Create Rotation matrix about Z axis
void mgMatrix4::rotateZDeg(
  double degs)
{
  mgMatrix4 rot;
  double rads = (degs * PI)/180.0;
  rot._11 = cos(rads);
  rot._12 = sin(rads);
  rot._21 = -rot._12; // -sinf(rads);
  rot._22 = rot._11; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// Create Rotation matrix about Z axis
void mgMatrix4::rotateZRad(
  double rads)
{
  mgMatrix4 rot;
  rot._11 = cos(rads);
  rot._12 = sin(rads);
  rot._21 = -rot._12; // -sinf(rads);
  rot._22 = rot._11; // cosf(rads);
  multiply(rot);
}

//-----------------------------------------------------------------------------
// scale matrix 
void mgMatrix4::scale(
  double fXSize,
  double fYSize,
  double fZSize)
{
  mgMatrix4 scale;
  scale._11 = fXSize;
  scale._22 = fYSize;
  scale._33 = fZSize;
  multiply(scale);
}

//-----------------------------------------------------------------------------
// map a point through the matrix
void mgMatrix4::mapPt(
  double fFromX, 
  double fFromY, 
  double fFromZ, 
  double &fToX,
  double &fToY,
  double &fToZ) const
{
  fToX = _11 * fFromX + _21 * fFromY + _31 * fFromZ + _41;
  fToY = _12 * fFromX + _22 * fFromY + _32 * fFromZ + _42;
  fToZ = _13 * fFromX + _23 * fFromY + _33 * fFromZ + _43;
}

//-----------------------------------------------------------------------------
// map a point through the matrix
void mgMatrix4::mapPt(
  const mgPoint3 &from,
  mgPoint3 &to) const
{
  to.x = _11 * from.x + _21 * from.y + _31 * from.z + _41;
  to.y = _12 * from.x + _22 * from.y + _32 * from.z + _42;
  to.z = _13 * from.x + _23 * from.y + _33 * from.z + _43;
}

//-----------------------------------------------------------------------------
// map a point through the matrix
void mgMatrix4::mapPt(
  double fromx, 
  double fromy, 
  double fromz, 
  double fromw, 
  double &tox,
  double &toy,
  double &toz,
  double &tow) const
{
  tox = _11 * fromx + _21 * fromy + _31 * fromz + _41 * fromw;
  toy = _12 * fromx + _22 * fromy + _32 * fromz + _42 * fromw;
  toz = _13 * fromx + _23 * fromy + _33 * fromz + _43 * fromw;
  tow = _14 * fromx + _24 * fromy + _34 * fromz + _44 * fromw;
}

//-----------------------------------------------------------------------------
// map a point through the matrix
void mgMatrix4::mapPt(
  const mgPoint4 &from,
  mgPoint4 &to) const
{
  to.x = _11 * from.x + _21 * from.y + _31 * from.z + _41 * from.w;
  to.y = _12 * from.x + _22 * from.y + _32 * from.z + _42 * from.w;
  to.z = _13 * from.x + _23 * from.y + _33 * from.z + _43 * from.w;
  to.w = _14 * from.x + _24 * from.y + _34 * from.z + _44 * from.w;
}

//-----------------------------------------------------------------------------
// inverse map a point through the matrix
void mgMatrix4::invertPt(
  double fFromX, 
  double fFromY, 
  double fFromZ, 
  double &fToX,
  double &fToY,
  double &fToZ) const
{
  fFromX -= _41;
  fFromY -= _42;
  fFromZ -= _43;
  fToX = _11 * fFromX + _12 * fFromY + _13 * fFromZ;
  fToY = _21 * fFromX + _22 * fFromY + _23 * fFromZ;
  fToZ = _31 * fFromX + _32 * fFromY + _33 * fFromZ;
}

//-----------------------------------------------------------------------------
// inverse map a point through the matrix
void mgMatrix4::invertPt(
  const mgPoint3 &from,
  mgPoint3 &to) const
{
  double fX = from.x - _41;
  double fY = from.y - _42;
  double fZ = from.z - _43;
  to.x = _11 * fX + _12 * fY + _13 * fZ;
  to.y = _21 * fX + _22 * fY + _23 * fZ;
  to.z = _31 * fX + _32 * fY + _33 * fZ;
}
  
/*
**-----------------------------------------------------------------------------
**  Name:       lubksb
**  Purpose:	backward subsitution
**-----------------------------------------------------------------------------
*/

static void 
lubksb(mgMatrix4 & a, int *indx, double *b)
{
	int		i, j, ii=-1, ip;
	double	sum;

	for (i=0; i<4; i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii>=0) {
			for (j=ii; j<=i-1; j++) {
				sum -= a(i, j) * b[j];
			}
		} else if (sum != 0.0) {
			ii = i;
		}
		b[i] = sum;
	}
	for (i=3; i>=0; i--) {
		sum = b[i];
		for (j=i+1; j<4; j++) {
			sum -= a(i, j) * b[j];
		}
		b[i] = sum/a(i, i);
	}
} // end lubksb

/*
**-----------------------------------------------------------------------------
**  Name:       ludcmp
**  Purpose:	LU decomposition
**-----------------------------------------------------------------------------
*/

static void 
ludcmp(mgMatrix4 & a, int *indx, double *d)
{
	double	vv[4];               /* implicit scale for each row */
	double	big, dum, sum, tmp;
	int		i, imax, j, k;

	*d = 1.0;
	for (i=0; i<4; i++) {
		big = 0.0;
		for (j=0; j<4; j++) {
			if ((tmp = abs(a(i, j))) > big) {
				big = tmp;
			}
		}
		/*
		if (big == 0.0) {
			printf("ludcmp(): singular matrix found...\n");
			exit(1);
		}
		*/
		vv[i] = 1.0/big;
	}
	for (j=0; j<4; j++) {
		for (i=0; i<j; i++) {
			sum = a(i, j);
			for (k=0; k<i; k++) {
				sum -= a(i, k) * a(k, j);
			}
			a(i, j) = sum;
		}
		big = 0.0;
		for (i=j; i<4; i++) {
			sum = a(i, j);
			for (k=0; k<j; k++) {
				sum -= a(i, k)*a(k, j);
			}
			a(i, j) = sum;
			if ((dum = vv[i] * (double)fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k=0; k<4; k++) {
				dum = a(imax, k);
				a(imax, k) = a(j, k);
				a(j, k) = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a(j, j) == 0.0) {
			a(j, j) = 1.0e-20;      /* can be 0.0 also... */
		}
		if (j != 3) {
			dum = 1.0/a(j, j);
			for (i=j+1; i<4; i++) {
				a(i, j) *= dum;
			}
		}
	}
} // end ludcmp

//-----------------------------------------------------------------------------
// create inverse of argument matrix
void mgMatrix4::inverse(
  const mgMatrix4 &m)
{
	mgMatrix4 n;
	int i, j, indx[4];
	double d, col[4];

	n = m;
	ludcmp(n, indx, &d);

	for (j=0; j<4; j++) 
  {
		for (i=0; i<4; i++) 
    {
			col[i] = 0.0;
		}
		col[j] = 1.0;
		lubksb(n, indx, col);
		for (i=0; i<4; i++) 
    {
			(*this)(i, j) = col[i];
		}
	}
} // end MatrixInverse

  
//-----------------------------------------------------------------------------
// print on output
void mgMatrix4::print(
  FILE* outFile)
{
  fprintf(outFile, "%10.4g %10.4g %10.4g %10.4g\n", _11, _12, _13, _14);
  fprintf(outFile, "%10.4g %10.4g %10.4g %10.4g\n", _21, _22, _23, _24);
  fprintf(outFile, "%10.4g %10.4g %10.4g %10.4g\n", _31, _32, _33, _34);
  fprintf(outFile, "%10.4g %10.4g %10.4g %10.4g\n", _41, _42, _43, _44);
}
