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
#if !defined(MGGL33SERVICES_H) && defined(SUPPORT_GL33)
#define MGGL33SERVICES_H

#include "mgDisplayServices.h"
#include "Graphics3D/GL33/mgGL33Types.h"

class mgGL33TextureImage : public mgTextureImage
{
public:
  mgTextureHandle m_handle;
  int m_format;
  BOOL m_mipmap;

  // constructor
  mgGL33TextureImage();

  // destructor
  virtual ~mgGL33TextureImage();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap);

  // update memory texture
  virtual void updateMemory(
    int x,
    int y,
    int width,
    int height,
    const BYTE* data);
};

class mgGL33TextureArray : public mgTextureArray
{
public:
  mgTextureHandle m_handle;

  // constructor
  mgGL33TextureArray();

  // destructor
  virtual ~mgGL33TextureArray();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap);
};

class mgGL33TextureCube : public mgTextureCube
{
public:
  mgTextureHandle m_handle;

  // constructor
  mgGL33TextureCube();

  // destructor
  virtual ~mgGL33TextureCube();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap);
};

// services offered by the display framework to the application
class mgGL33Services : public mgDisplayServices
{ 
public:
#if defined(_DEBUG) && defined(WIN32)
  DWORD m_createThreadId;
#endif

  //----------------------- screen management ------------------

  // release device resources 
  virtual void deleteBuffers();

  // reclaim device resources
  virtual void createBuffers();

  // set cursor texture
  virtual void setCursorTexture(
    const char* fileName,
    int hotX,
    int hotY);

  // draw the cursor
  virtual void drawCursor();

  //----------------------- texture methods ------------------

  // load/create texture from file
  virtual mgTextureImage* loadTexture(
    const char* fileName);

  // load texture array from file list
  virtual mgTextureArray* loadTextureArray(
    const mgStringArray& fileList);  

  // load texture cube from files
  virtual mgTextureCube* loadTextureCube(
    const char* xminImage,
    const char* xmaxImage,
    const char* yminImage,
    const char* ymaxImage,
    const char* zminImage,
    const char* zmaxImage);

  // create texture to be updated from memory
  virtual mgTextureImage* createTextureMemory(
    int width,
    int height,
    int format,
    BOOL mipmap);

  //----------------------- rendering methods ------------------

  // supports integer vertex fields
  virtual BOOL supportsIntegerVertex();

  // can the display repeat textures
  virtual BOOL canRepeatTextures();

  // clear the view
  virtual void clearView();

  // clear the buffer
  virtual void clearBuffer(
    int flags);

  // load shader
  virtual void loadShader(
    const char* shaderName,             // name of shader
    const mgVertexAttrib* attribs);     // vertex attributes

  // delete shader
  virtual void deleteShader(
    const char* shaderName);            // name of shader

  // set current shader
  virtual void setShader(
    const char* shaderName);            // name of shader

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgMatrix4& matrix);            // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgMatrix3& matrix);            // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgPoint3& point);             // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    const mgPoint4& point);             // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    int value);                         // value

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    float value);                       // value

  // allocate vertex buffer
  virtual mgVertexBuffer* newVertexBuffer(
    int vertexSize,                   // size of vertex in bytes
    const mgVertexAttrib* attribs,    // array of attributes.  end with offset=-1
    int maxVertexes,                  // max number of vertexes
    BOOL dynamic);                    // support reset and reuse

  // allocate an index array
  virtual mgIndexBuffer* newIndexBuffer(
    int size,                         // max number of indexes
    BOOL dynamic,                     // support reset and reuse
    BOOL longIndexes);                // support 32-bit indexes

  // set the texture to use
  virtual void setTexture(
    const mgTextureImage* texture,
    int unit);
  
  // set the texture to use
  virtual void setTexture(
    const mgTextureArray* texture,
    int unit);
  
  // set the texture to use
  virtual void setTexture(
    const mgTextureCube* texture,
    int unit);
  
  // draw from vertex buffer
  virtual void draw(
    int primType,
    mgVertexBuffer* vertexes);

  // draw from separate vertex and index buffers
  virtual void draw(
    int primType,
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes);

  // draw from separate vertex and index buffers
  virtual void draw(
    int primType,
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int startIndex,
    int endIndex);

  // start drawing decal background.  draw=false just writes depth buffer
  virtual void decalBackground(
    BOOL draw);

  // background complete, start drawing decals
  virtual void decalStart();

  // end of decals
  virtual void decalEnd();

  // draw a texture to the overlay
  virtual void drawOverlayTexture(
    const mgTextureImage* texture,
    int x,
    int y,
    int width,
    int height);

  // draw a surface to the overlay
  virtual void drawOverlaySurface(
    const mgTextureSurface* surface,
    int x,
    int y);

  // create surface for 2D graphics
  virtual mgTextureSurface* createOverlaySurface();

  // create surface for 2D graphics
  virtual mgTextureSurface* createTextureSurface();

  //----------------------- rendering state ------------------

  // set light direction
  virtual void setLightDir(
    double x,
    double y,
    double z);

  // set light ambient
  virtual void setLightAmbient(
    double r,
    double g,
    double b);

  // set light color
  virtual void setLightColor(
    double r,
    double g,
    double b);

  // set material color
  virtual void setMatColor(
    double r,
    double g,
    double b,
    double a = 1.0);

  // set model transform
  virtual void setModelTransform(
    const mgMatrix4& xform);

  // append model transform
  virtual void appendModelTransform(
    const mgMatrix4& xform);

  // get model transform
  virtual void getModelTransform(
    mgMatrix4& xform);

  // get model-view-perspective transform
  virtual void getMVPTransform(
    mgMatrix4& xform);

  // get model-view transform
  virtual void getMVTransform(
    mgMatrix4& xform);

  // set culling 
  virtual void setCulling(
    BOOL enable);

  // set front face
  virtual void setFrontCW(
    BOOL enable);

  // set zenable
  virtual void setZEnable(
    BOOL enable);

  // set transparent
  virtual void setTransparent(
    BOOL enable);

  // set projection
  virtual void setProjection();

protected:
  mgMapStringToDWord m_shaders;

  mgGL33TextureImage* m_cursorTexture;

  mgMapStringToPtr m_textureImages;  // mgTextureImage* instances
  mgPtrArray m_textureArrays;   // mgTextureArray* instances
  mgPtrArray m_textureCubes;    // mgTextureCube* instances

  // rendering state
  mgMatrix4 m_worldProjection;
  mgMatrix4 m_modelMatrix;
  mgMatrix4 m_worldMatrix;               // view*model matrix
  BOOL m_lighting;
  mgShaderHandle m_shader;

  // constructor
  mgGL33Services(
    const char* shaderDir,
    const char* fontDir);

  // destructor
  virtual ~mgGL33Services();

  // initialize view 
  virtual void initView();

  // return matrix as float[16] array
  void matrix4toGL(
    const mgMatrix4& m,
    float* values);

  // return matrix as float[9] array
  void matrix3toGL(
    const mgMatrix3& m,
    float* values);

  // extract a normals matrix from world matrix
  void normalMatrix(
    const mgMatrix4& matrix,
    float* result);

  // set standard variables for shader
  void setShaderStdUniforms(
    mgShaderHandle shader);

  // reload texture image from file
  void reloadTextureImage(
    mgGL33TextureImage* texture);

  // reload texture array from file list
  void reloadTextureArray(
    mgGL33TextureArray* texture);

  // reload texture cube from file list
  void reloadTextureCube(
    mgGL33TextureCube* texture);

  // delete any texture objects
  virtual void unloadTextures();
  
  // reload deleted texture objects
  virtual void reloadTextures();

  friend class mgTextureSurface;

  friend void mgInitDisplayServices(
    const char* shaderDir,
    const char* fontDir);
};

#if defined(_DEBUG) && defined(WIN32)
#define CHECK_THREAD() mgServicesCheckThread()
void mgServicesCheckThread();
#else 
#define CHECK_THREAD() 
#endif

#endif
