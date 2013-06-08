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
#ifndef TREES_H
#define TREES_H

#include "MovementApp.h"
#include "Colonization.h"

class HelpUI;
class StarrySky;

class Trees : public MovementApp
{
public:
  // constructor
  Trees();

  // destructor
  virtual ~Trees();

  //--- subclass MovementApp

  // key press
  virtual BOOL moveKeyDown(
    int keyCode,
    int modifiers);

  //--- implement mgApplication interface

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // update animation 
  virtual BOOL appViewAnimate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // render the view
  virtual void appViewDraw();

protected:
  HelpUI* m_help;
  StarrySky* m_sky;

  double m_animateGrowth;
  BOOL m_runAnimation;
  int m_lastCount;

  mgTextureArray* m_leafTexture;
  mgIndexBuffer* m_leafIndexes;
  mgVertexBuffer* m_leafVertexes;
  mgShader* m_leafShader;

  mgTextureImage* m_branchTexture;
  mgIndexBuffer* m_branchIndexes;
  mgVertexBuffer* m_branchVertexes;
  mgShader* m_branchShader;

  mgTextureImage* m_floorTexture;
  mgVertexBuffer* m_floorVertexes;
  mgShader* m_floorShader;

  mgTextureImage* m_shapeTexture;
  mgIndexBuffer* m_shapeIndexes;
  mgVertexBuffer* m_shapeVertexes;
  mgShader* m_shapeShader;

  Colonization m_tree;

  // load the textures we use
  virtual void loadTextures();

  // initialize leaf points and branches
  virtual void initTree();

  // initialize leaf points and branches around shape
  virtual void initShape();

  // create buffers for rendering tree
  virtual void renderTree();

  // add a cube to the buffers
  virtual void addLeaf(
    const mgPoint3& center,
    double size,
    double tz);

  // create the buffers for a set of leaves
  virtual void createLeafBuffers();

  // get axis vectors for cross-section at point
  virtual void getAxis(
    mgPoint3& from,
    mgPoint3& to,
    mgPoint3& xaxis,
    mgPoint3& zaxis);

  // add a branch
  virtual void addBranch(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int count,
    int steps,
    double* branchWidths,
    mgPoint3* branchPts);

  // create vertex buffer for floor
  virtual void createFloor();

  // create vertex buffer for shape
  virtual void createShape();
};

#endif
