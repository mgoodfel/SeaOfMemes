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
#ifndef MGDISPLAYSERVICES_H
#define MGDISPLAYSERVICES_H

// superclass of the various graphics implementations (DirectX/OpenGL)

const double MG_CUBE_RADIUS = sqrt(3.0);

const int MG_TEXTURE_REPEAT = 0;
const int MG_TEXTURE_CLAMP = 1;

const int MG_TEXTURE_NEAREST = 0;
const int MG_TEXTURE_QUALITY = 1;

class mgApplication;
class mgVertexBuffer;
class mgVertexAttrib;
class mgIndexBuffer;
class mgTextureSurface;
class mgFontList;

class mgTextureImage
{
public:
  mgString m_fileName;
  int m_width;
  int m_height;
  BOOL m_transparent;
  int m_xWrap;
  int m_yWrap;
  int m_filter;

  // constructor
  mgTextureImage();

  // destructor
  virtual ~mgTextureImage();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap) = 0;

  // set texture filtering
  virtual void setFilter(
    int filter) = 0;

  // update memory texture
  virtual void updateMemory(
    int x,
    int y,
    int width,
    int height,
    const BYTE* data) = 0;
};

class mgTextureArray
{
public:
  mgStringArray m_fileList;
  int m_width;
  int m_height;
  BOOL* m_imgTransparent;
  int m_xWrap;
  int m_yWrap;
  int m_filter;

  // constructor
  mgTextureArray();

  // destructor
  virtual ~mgTextureArray();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap) = 0;

  // set texture filtering
  virtual void setFilter(
    int filter) = 0;
};

class mgTextureCube
{
public:
  mgString m_xminImage;
  mgString m_xmaxImage;
  mgString m_yminImage;
  mgString m_ymaxImage;
  mgString m_zminImage;
  mgString m_zmaxImage;
  int m_width;
  int m_height;
  BOOL m_imgTransparent[6];
  int m_xWrap;
  int m_yWrap;
  int m_filter;

  // constructor
  mgTextureCube();

  // destructor
  virtual ~mgTextureCube();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap) = 0;

  // set texture filtering
  virtual void setFilter(
    int filter) = 0;
};

const int MG_COLOR_BUFFER = 1;
const int MG_DEPTH_BUFFER = 2;

const int MG_MEMORY_FORMAT_RGBA = 0;
const int MG_MEMORY_FORMAT_BGRA = 1;

const int MG_TRIANGLES        = 0;
const int MG_TRIANGLE_FAN     = 1;
const int MG_TRIANGLE_STRIP   = 2;
const int MG_LINES            = 3;
const int MG_LINE_STRIP       = 4;
const int MG_POINTS           = 5;

// services offered by the display framework to the application
class mgDisplayServices
{ 
public:
  //----------------------- platform methods ------------------
  // pass these calls through to platform so we don't have mix of calls

  virtual void setMouseRelative(
    BOOL relative)
  {
    mgPlatform->setMouseRelative(relative);
  }

  virtual void swapBuffers()
  {
    mgPlatform->swapBuffers();
  }

  virtual int getDepthBits()
  {
    return mgPlatform->getDepthBits();
  }

  //----------------------- screen management ------------------

  // release device resources 
  virtual void deleteBuffers() = 0;

  // reclaim device resources
  virtual void createBuffers() = 0;

  // handle window resize
  virtual void setScreenSize(
    int width,
    int height);

  // return screen size
  void getScreenSize(
    int& width,
    int& height)
  {
    width = m_graphicsWidth;
    height = m_graphicsHeight;
  }

  // set screen resolution (affects font sizes)
  virtual void setDPI(
    int dpi);

  virtual int getDPI()
  {
    return m_dpi;
  }

  // enable the cursor
  virtual void cursorEnable(
    BOOL enabled);

  // track the cursor
  virtual void cursorTrack(
    BOOL track);

  // set cursor texture
  virtual void setCursorTexture(
    const char* fileName,
    int hotX,
    int hotY) = 0;

  // return cursor position
  virtual void getCursorPosn(
    int &x,
    int &y)
  {
    x = m_cursorX;
    y = m_cursorY;
  }

  // set cursor position
  virtual void cursorSetPosn(
    int x, 
    int y);

  // move the cursor based on mouse dx,dy
  virtual void cursorMove(
    int dx,
    int dy);

  // draw the cursor
  virtual void drawCursor() = 0;

  //----------------------- texture methods ------------------

  // load/create texture from file
  virtual mgTextureImage* loadTexture(
    const char* fileName) = 0;

  // load texture array from file list
  virtual mgTextureArray* loadTextureArray(
    const mgStringArray& fileList) = 0;       

  // load texture cube from files
  virtual mgTextureCube* loadTextureCube(
    const char* xminImage,
    const char* xmaxImage,
    const char* yminImage,
    const char* ymaxImage,
    const char* zminImage,
    const char* zmaxImage) = 0;

  // create texture to be updated from memory
  virtual mgTextureImage* createTextureMemory(
    int width,
    int height,
    int format,
    BOOL mipmap) = 0;

  //----------------------- rendering methods ------------------

  // supports non-float shader arguments, bit operations
  virtual BOOL supportsIntegerVertex() = 0;

  // can the display repeat textures
  virtual BOOL canRepeatTextures() = 0;

  // clear the view
  virtual void clearView() = 0;

  // clear the buffer
  virtual void clearBuffer(
    int flags) = 0;

  // load shader
  virtual void loadShader(
    const char* shaderName,             // name of shader
    const mgVertexAttrib* attribs) = 0; // vertex attributes

  // delete shader
  virtual void deleteShader(
    const char* shaderName) = 0;        // name of shader

  // set current shader
  virtual void setShader(
    const char* shaderName) = 0;        // name of shader

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgMatrix4& matrix) = 0;        // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgMatrix3& matrix) = 0;        // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgPoint3& point) = 0;        // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgPoint4& point) = 0;         // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    int value) = 0;                     // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    float value) = 0;                   // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    int count,                          // size of array
    const mgPoint3* point) = 0;         // point array

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    int count,                          // size of array
    const float* value) = 0;            // float array

  // allocate vertex buffer
  virtual mgVertexBuffer* newVertexBuffer(
    int vertexSize,                   // size of vertex in bytes
    const mgVertexAttrib* attribs,    // array of attributes.  end with offset=-1
    int maxVertexes,                  // max number of vertexes
    BOOL dynamic=false) = 0;          // support reset and reuse

  // allocate an index array
  virtual mgIndexBuffer* newIndexBuffer(
    int size,                         // max number of indexes
    BOOL dynamic=false,               // support reset and reuse
    BOOL longIndexes=false) = 0;      // true for 32-bit indexes

  // set the texture to use
  virtual void setTexture(
    const mgTextureImage* texture,
    int unit = 0) = 0;
  
  // set the texture to use
  virtual void setTexture(
    const mgTextureArray* texture,
    int unit = 0) = 0;
  
  // set the texture to use
  virtual void setTexture(
    const mgTextureCube* texture,
    int unit = 0) = 0;
      
  // draw from vertex buffer
  virtual void draw(
    int primType,
    mgVertexBuffer* vertexes) = 0;

  // draw from separate vertex and index buffers
  virtual void draw(
    int primType,
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes) = 0;

  // draw from separate vertex and index buffers
  virtual void draw(
    int primType,
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int startIndex,
    int endIndex) = 0;

  // start drawing decal background.  draw=false just writes depth buffer
  virtual void decalBackground(
    BOOL draw) = 0;

  // background complete, start drawing decals
  virtual void decalStart() = 0;

  // end of decals
  virtual void decalEnd() = 0;

  // create surface for 2D graphics on world object
  virtual mgTextureSurface* createTextureSurface() = 0;

  // create surface for 2D graphics in overlay plane
  virtual mgTextureSurface* createOverlaySurface() = 0;

  // draw a texture to the overlay
  virtual void drawOverlayTexture(
    const mgTextureImage* texture,
    int x,
    int y,
    int width,
    int height) = 0;

  // draw a surface to the overlay
  virtual void drawOverlaySurface(
    const mgTextureSurface* surface,
    int x,
    int y) = 0;

  //----------------------- rendering state ------------------

  // set the eye point
  virtual void setEyePt(
    const mgPoint3& pt);

  // get eye position
  void getEyePt(
    mgPoint3& eyePt)
  {
    eyePt = m_eyePt;
  }

  // set the eye rotation matrix
  virtual void setEyeMatrix(
    const mgMatrix4& matrix);

  // eye matrix
  void getEyeMatrix(
    mgMatrix4& matrix)
  {
    matrix = m_eyeMatrix;
  }

  // figure vector corresponding to eye rotations
  virtual void eyeVector(
    mgPoint3& vect);

  // figure vector from eye to cursor
  virtual void cursorVector(
    mgPoint3& vect);

  // set horizontal field of view
  virtual void setFOV(
    double angle);

  virtual double getFOV()
  {
    return m_FOV;
  }

  // set the front and back plane distances
  virtual void setFrontAndBack(
    double frontDist,
    double backDist);

  // return front and back plant distances
  void getFrontAndBack(
    double& frontDist,
    double& backDist)
  {
    frontDist = m_viewFront;
    backDist = m_viewBack;
  }

  // return true if sphere within frustum
  virtual BOOL withinFrustum(
    double x,
    double y, 
    double z,
    double radius);

  // return view frustum dimensions
  void getFrustumBounds(
    double& front,
    double& back,
    double& width,
    double& height)
  {
    front = m_viewFront;
    back = m_viewBack;
    width = m_viewWidth;
    height = m_viewHeight;
  }
  
  // set model transform
  virtual void setModelTransform(
    const mgMatrix4& xform) = 0;

  // append model transform
  virtual void appendModelTransform(
    const mgMatrix4& xform) = 0;

  // get model transform
  virtual void getModelTransform(
    mgMatrix4& xform) = 0;

  // get model-view-perspective transform
  virtual void getMVPTransform(
    mgMatrix4& xform) = 0;

  // get model-view transform
  virtual void getMVTransform(
    mgMatrix4& xform) = 0;

  // set culling 
  virtual void setCulling(
    BOOL enable) = 0;

  // set front face clockwise
  virtual void setFrontCW(
    BOOL enable) = 0;

  // set zenable
  virtual void setZEnable(
    BOOL enable) = 0;

  // set transparent
  virtual void setTransparent(
    BOOL enable) = 0;

  // set light direction
  virtual void setLightDir(
    double x,
    double y,
    double z) = 0;

  // set light direction
  void setLightDir(
    const mgPoint3& lightDir)
  {
    setLightDir(lightDir.x, lightDir.y, lightDir.z);
  }

  // light direction
  void getLightDir(
    mgPoint3& dir)
  {
    dir = m_lightDir;
  }

  // set light ambient
  virtual void setLightAmbient(
    double r,
    double g,
    double b) = 0;

  // set light ambient
  void setLightAmbient(
    const mgPoint3& ambient)
  {
    setLightAmbient(ambient.x, ambient.y, ambient.z);
  }

  // light ambient
  void getLightAmbient(
    mgPoint3& color)
  {
    color = m_lightAmbient;
  }

  // set light color
  virtual void setLightColor(
    double r,
    double g,
    double b) = 0;

  // set light color
  void setLightColor(
    const mgPoint3& color)
  {
    setLightColor(color.x, color.y, color.z);
  }

  // light color
  void getLightColor(
    mgPoint3& color)
  {
    color = m_lightColor;
  }

  // set material color
  virtual void setMatColor(
    double r,
    double g,
    double b,
    double a = 1.0) = 0;

  // set material color
  void setMatColor(
    const mgPoint3& color)
  {
    setMatColor(color.x, color.y, color.z);
  }

  // material color
  void getMatColor(
    mgPoint4& color)
  {
    color = m_matColor;
  }

protected:
  mgString m_shaderDir;
  mgStringArray m_fontDirs;
  mgFontList* m_fontList;

  // eye transform
  mgPoint3 m_eyePt;                       
  mgMatrix4 m_eyeMatrix;                 

  // lighting state
  mgPoint3 m_lightDir;
  mgPoint3 m_lightColor;
  mgPoint3 m_lightAmbient;
  mgPoint4 m_matColor;
  BOOL m_updateShaderVars;

  // cursor state  
  mgString m_cursorFileName;
  int m_cursorHotX;
  int m_cursorHotY;
  int m_cursorX;
  int m_cursorY;
  BOOL m_cursorEnabled;
  BOOL m_cursorTrack;

  // screen size
  int m_graphicsWidth;
  int m_graphicsHeight;

  // view frustum
  double m_FOV;
  double m_viewFront;
  double m_viewBack;
  double m_viewWidth;
  double m_viewHeight;

  // frustum planes
  mgPoint3 m_viewFrontPt;
  mgPoint3 m_viewFrontNormal;
  mgPoint3 m_viewBackPt;
  mgPoint3 m_viewBackNormal;
  mgPoint3 m_viewLeftPt;
  mgPoint3 m_viewLeftNormal;
  mgPoint3 m_viewRightPt;
  mgPoint3 m_viewRightNormal;
  mgPoint3 m_viewTopPt;
  mgPoint3 m_viewTopNormal;
  mgPoint3 m_viewBottomPt;
  mgPoint3 m_viewBottomNormal;

  int m_dpi;

  // constructor
  mgDisplayServices(
    const char* shaderDir,
    const char* fontDir);

  // destructor
  virtual ~mgDisplayServices();

  // initialize state
  virtual void initView() = 0;

  // build the view frustum planes
  virtual void frustumBuildPlanes();

  // set the projection matrix
  virtual void setProjection() = 0;

  // find a font file
  virtual BOOL findFont(
    const char* faceName, 
    BOOL bold, 
    BOOL italic, 
    mgString& fontFile);

  friend class mgTextureSurface;

  friend void mgInitDisplayServices(
    const char* shaderDir,
    const char* fontDir);

  friend void mgTermDisplayServices();
};

// call this method to get the appropriate display services for the
// display library as set in mgPlatform
extern void mgInitDisplayServices(
  const char* shaderDir,
  const char* fontDir);

// InitDisplayServices sets this global variable
extern mgDisplayServices* mgDisplay;

// clean up display services
extern void mgTermDisplayServices();

#endif
