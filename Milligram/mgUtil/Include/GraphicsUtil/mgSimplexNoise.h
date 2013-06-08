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
#ifndef MGSIMPLEXNOISE_H
#define MGSIMPLEXNOISE_H

/*
  Generates 2, 3 and 4 dimensional Simplex Noise.
*/
class mgSimplexNoise 
{
public:
  // 2D noise
  static double noise(
    double xin, 
    double yin);

  // 3D noise
  static double noise(
    double xin, 
    double yin, 
    double zin);

  // 4D noise
  static double noise(
    double x, 
    double y, 
    double z, 
    double w);

  // octaves of 2D noise
  static double noiseSum(
    int octaves,
    double xin, 
    double yin);

  // octaves of 3D noise
  static double noiseSum(
    int octaves,
    double xin, 
    double yin, 
    double zin);

  // octaves of 4D noise
  static double noiseSum(
    int octaves,
    double x, 
    double y, 
    double z, 
    double w);

  // octaves of 2D fabs(noise)
  static double noiseSumAbs(
    int octaves,
    double xin, 
    double yin);

  // octaves of 3D fabs(noise)
  static double noiseSumAbs(
    int octaves,
    double xin, 
    double yin, 
    double zin);

  // octaves of 4D fabs(noise)
  static double noiseSumAbs(
    int octaves,
    double x, 
    double y, 
    double z, 
    double w);
};

#endif
