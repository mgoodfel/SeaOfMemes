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

#include "Saucer.h"

mgVertexBuffer* Saucer::m_vertexes = NULL;
mgIndexBuffer* Saucer::m_indexes = NULL;
mgTextureArray* Saucer::m_texture = NULL;

const int SAUCER_POINTS = 80;
const int SAUCER_TEXTURES = 10;

class SaucerSegment
{
public:
  mgPoint2 m_pt;
  mgPoint2 m_normal;
  double m_ty;
  BOOL m_last;
};

//--------------------------------------------------------------
// constructor
Saucer::Saucer(
  const mgOptionsFile& options)
{
  mgVertex::loadShader("saucer");

  m_outline = NULL;
  m_outlineSize = 0;

  m_xAngle = 0.0;
  m_yAngle = 0.0;
  m_zAngle = 0.0;
  m_sound = -1;

  if (m_texture == NULL)
  {
    // load saucer textures.  must all be same size.
    // last texture is dead saucer texture
    mgStringArray fileList;
    mgString fileName;
    for (int i = 0; i <= SAUCER_TEXTURES; i++)
    {
      mgString name;
      name.format("saucer%d", i);
      options.getFileName(name, options.m_sourceFileName, name, fileName);
      fileList.add(fileName);
    }

    m_texture = mgDisplay->loadTextureArray(fileList);
    m_texture->setWrap(MG_TEXTURE_CLAMP, MG_TEXTURE_CLAMP);
  }

  m_textureCycle = SAUCER_TEXTURES*mgRandom();
  m_wobbleRate = mgRandom();
  m_wobble = 360.0*mgRandom();
}

//--------------------------------------------------------------
// destructor
Saucer::~Saucer()
{
  delete m_outline;
  m_outline = NULL;

  deleteBuffers();
}

//--------------------------------------------------------------
// build outline of saucer
void Saucer::buildOutline()
{
  m_outlineSize = 200;
  m_outline = new SaucerSegment[m_outlineSize];

  m_outlineCount = 0;
  SaucerSegment* segment = NULL;

  int edgeCount = 15;
  double splineLen;
  mgBezier spline;

  // top bubble
  spline.addVertex(mgPoint3(0, 3.5, 0), mgPoint3(0+0.75, 3.5+0, 0));
  spline.addVertex(mgPoint3(1.5, 2, 0), mgPoint3(1.5, 2+0.75, 0));
  splineLen = spline.getLength();

  segment = &m_outline[m_outlineCount];
  segment->m_normal.x = 0.0;
  segment->m_normal.y = 1.0;
  for (int i = 0; i <= edgeCount; i++)
  {
    mgPoint3 pt;
    spline.splinePt((splineLen*i)/edgeCount, pt);
    segment = &m_outline[m_outlineCount++];
    segment->m_pt.x = pt.x;
    segment->m_pt.y = pt.y;

    if (i != 0)
    {
      segment->m_normal.x = m_outline[m_outlineCount-2].m_pt.y - pt.y;  
      segment->m_normal.y = pt.x - m_outline[m_outlineCount-2].m_pt.x;  
      segment->m_normal.normalize();
    }
    segment->m_last = i == edgeCount;
  }

  // top side
  spline.removeAll();
  spline.addVertex(mgPoint3(1.5, 2, 0), mgPoint3(2.5, 2, 0));
  spline.addVertex(mgPoint3(4, 1, 0), mgPoint3(3.5, 1.5, 0));
  splineLen = spline.getLength();

  segment = &m_outline[m_outlineCount];
  segment->m_normal.x = 0.0;
  segment->m_normal.y = 1.0;

  for (int i = 0; i <= edgeCount; i++)
  {
    mgPoint3 pt;
    spline.splinePt((splineLen*i)/edgeCount, pt);
    segment = &m_outline[m_outlineCount++];
    segment->m_pt.x = pt.x;
    segment->m_pt.y = pt.y;

    if (i != 0)
    {
      segment->m_normal.x = m_outline[m_outlineCount-2].m_pt.y - pt.y;  
      segment->m_normal.y = pt.x - m_outline[m_outlineCount-2].m_pt.x;  
      segment->m_normal.normalize();
    }
    segment->m_last = i == edgeCount;
  }

  // top edge
  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 4.0;
  segment->m_pt.y = 1.0;
  segment->m_normal.x = 1.0-0.25;
  segment->m_normal.y = 5.5-4.0;
  segment->m_normal.normalize();
  segment->m_last = false;

  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 5.5;
  segment->m_pt.y = 0.25;
  segment->m_normal.x = 1.0-0.25;
  segment->m_normal.y = 5.5-4.0;
  segment->m_normal.normalize();
  segment->m_last = true;

  // rim edge
  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 5.5;
  segment->m_pt.y = 0.25;
  segment->m_normal.x = 0.25-0.25;
  segment->m_normal.y = 5.0-5.5;
  segment->m_normal.normalize();
  segment->m_last = false;

  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 5.0;
  segment->m_pt.y = 0.25;
  segment->m_normal.x = 0.25-0.25;
  segment->m_normal.y = 5.0-5.5;
  segment->m_normal.normalize();
  segment->m_last = true;

  // underside edge
  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 5.0;
  segment->m_pt.y = 0.25;
  segment->m_normal.x = 0.25-0.5;
  segment->m_normal.y = 4.0-5.0;
  segment->m_normal.normalize();
  segment->m_last = false;

  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 4.0;
  segment->m_pt.y = 0.5;
  segment->m_normal.x = 0.25-0.5;
  segment->m_normal.y = 4.0-5.0;
  segment->m_normal.normalize();
  segment->m_last = true;

  // bottom side
  spline.removeAll();
  spline.addVertex(mgPoint3(4.0, 0.5, 0), mgPoint3(4.0-0.5, 0.5-0.5, 0));
  spline.addVertex(mgPoint3(2.5, 0, 0), mgPoint3(2.5+0.5, 0, 0));
  splineLen = spline.getLength();

  segment = &m_outline[m_outlineCount];
  segment->m_normal.x = 0.0;
  segment->m_normal.y = -1.0;

  for (int i = 0; i <= edgeCount; i++)
  {
    mgPoint3 pt;
    spline.splinePt((splineLen*i)/edgeCount, pt);
    segment = &m_outline[m_outlineCount++];
    segment->m_pt.x = pt.x;
    segment->m_pt.y = pt.y;

    if (i != 0)
    {
      segment->m_normal.x = m_outline[m_outlineCount-2].m_pt.y - pt.y;  
      segment->m_normal.y = pt.x - m_outline[m_outlineCount-2].m_pt.x;  
      segment->m_normal.normalize();
    }
    segment->m_last = i == edgeCount;
  }

  // bottom bubble
  spline.removeAll();
  spline.addVertex(mgPoint3(2.5, 0, 0), mgPoint3(2.5-0.5, 0-0.5, 0));
  spline.addVertex(mgPoint3(1, -0.5, 0), mgPoint3(1+0.5, -0.5+0, 0));
  splineLen = spline.getLength();

  segment = &m_outline[m_outlineCount];
  segment->m_normal.x = 0.0;
  segment->m_normal.y = -1.0;

  for (int i = 0; i <= edgeCount; i++)
  {
    mgPoint3 pt;
    spline.splinePt((splineLen*i)/edgeCount, pt);
    segment = &m_outline[m_outlineCount++];
    segment->m_pt.x = pt.x;
    segment->m_pt.y = pt.y;

    if (i != 0)
    {
      segment->m_normal.x = m_outline[m_outlineCount-2].m_pt.y - pt.y;  
      segment->m_normal.y = pt.x - m_outline[m_outlineCount-2].m_pt.x;  
      segment->m_normal.normalize();
    }
    segment->m_last = i == edgeCount;
  }

  // inside edge
  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 1.0;
  segment->m_pt.y = -0.5;
  segment->m_normal.x = -1.0;
  segment->m_normal.y = 0.0;
  segment->m_last = false;

  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 1.0;
  segment->m_pt.y = 0.0;
  segment->m_normal.x = -1.0;
  segment->m_normal.y = 0.0;
  segment->m_last = true;

  // bottom center
  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 1.0;
  segment->m_pt.y = 0.0;
  segment->m_normal.x = 0.0;
  segment->m_normal.y = -1.0;
  segment->m_last = false;

  segment = &m_outline[m_outlineCount++];
  segment->m_pt.x = 0.0;
  segment->m_pt.y = 0.0;
  segment->m_normal.x = 0.0;
  segment->m_normal.y = -1.0;
  segment->m_last = true;


  // get total length of segments
  double totalLen = 0.0;
  SaucerSegment* prev = NULL;
  for (int i = 0; i < m_outlineCount; i++)
  {
    SaucerSegment* segment = &m_outline[i];
    if (prev != NULL)
    {
      double dx = segment->m_pt.x - prev->m_pt.x;
      double dy = segment->m_pt.y - prev->m_pt.y;
      double len = sqrt(dx*dx + dy*dy);
      totalLen += len;
    }
    if (segment->m_last)
      prev = NULL;
    else prev = segment;
  }

  // set ty based on percent of texture used
  prev = NULL;
  double usedLen = 0.0;
  for (int i = 0; i < m_outlineCount; i++)
  {
    SaucerSegment* segment = &m_outline[i];
    if (prev != NULL)
    {
      double dx = segment->m_pt.x - prev->m_pt.x;
      double dy = segment->m_pt.y - prev->m_pt.y;
      double len = sqrt(dx*dx + dy*dy);
      usedLen += len;
    }
    segment->m_ty = usedLen/totalLen;

    if (segment->m_last)
      prev = NULL;
    else prev = segment;
  }
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Saucer::createBuffers()
{
  if (m_vertexes != NULL)
    return;  // already done

  buildOutline();

  // count number of segments
  int vertexCount = 0;
  int indexCount = 0;

  BOOL first = true;
  for (int i = 0; i < m_outlineCount; i++)
  {
    SaucerSegment* segment = &m_outline[i];
    vertexCount += SAUCER_POINTS+1;
    if (!first)
      indexCount += 6*SAUCER_POINTS;
    first = segment->m_last;
  }

  m_vertexes = mgVertex::newBuffer(vertexCount);
  m_indexes = mgDisplay->newIndexBuffer(indexCount);

  mgVertex v;

  int vertexBase = m_vertexes->getLength();
  int rows = 0;
  for (int i = 0; i < m_outlineCount; i++)
  {
    SaucerSegment* segment = &m_outline[i];

    for (int j = 0; j <= SAUCER_POINTS; j++)
    {
      double angle = (2*PI*j)/SAUCER_POINTS;
      double dx = cos(angle);
      double dz = sin(angle);
      v.m_px = (float) (segment->m_pt.x * dx);
      v.m_py = (float) (segment->m_pt.y);
      v.m_pz = (float) (segment->m_pt.x * dz);

      mgPoint3 normal(segment->m_normal.x * dx, segment->m_normal.y, segment->m_normal.x * dz);
      normal.normalize();
      v.setNormal(normal.x, normal.y, normal.z);

      v.m_tx = j/(float) SAUCER_POINTS;
      v.m_ty = (float) segment->m_ty;

      m_vertexes->addVertex(&v);
    }
    if (segment->m_last)
    {
      m_indexes->addGrid(vertexBase, SAUCER_POINTS+1, rows, SAUCER_POINTS, true);
      vertexBase = m_vertexes->getLength();
      rows = 0;
    }
    else rows++;
  }
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Saucer::deleteBuffers()
{
  delete m_indexes;
  m_indexes = NULL;
  delete m_vertexes;
  m_vertexes = NULL;
}

//-----------------------------------------------------------------------------
// stop texture animation
void Saucer::stopTexture()
{
  m_textureCycle = -1;
}

//-----------------------------------------------------------------------------
// animate the saucer
void Saucer::animate(
  double now,
  double since)
{
  m_wobble += (0.8+m_wobbleRate/5) * since/7.0;
  if (m_wobble > 180)
    m_wobble -= 360;

  m_zAngle = 5 * sin(PI*m_wobble/180);
  m_xAngle = 5 * cos(PI*m_wobble/180);

  if (m_textureCycle >= 0)
  {
    m_textureCycle += since/200.0;
    if (m_textureCycle > SAUCER_TEXTURES)
      m_textureCycle -= SAUCER_TEXTURES;
  }
}

//-----------------------------------------------------------------------------
// render the saucer
void Saucer::render()
{
  if (m_texture == NULL || m_vertexes == NULL)
    return;

/*
  mgMatrix4 model;
  model.rotateXDeg(m_xAngle);
  model.rotateYDeg(m_yAngle);
  model.rotateZDeg(m_zAngle);
  model.translate(m_origin);

  mgDisplay->setModelTransform(model);
*/
  mgDisplay->setMatColor(1.0, 1.0, 1.0);

  // draw triangles using texture and shader
  mgDisplay->setShader("saucer");
  mgDisplay->setShaderUniform("saucer", "textureCycle", (float) m_textureCycle);
  mgDisplay->setTexture(m_texture);
  mgDisplay->draw(MG_TRIANGLES, m_vertexes, m_indexes);

  mgDisplay->setTransparent(false);
}
