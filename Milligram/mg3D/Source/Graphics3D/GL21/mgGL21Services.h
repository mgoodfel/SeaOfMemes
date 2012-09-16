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
#if !defined(MGGL21SERVICES_H) && defined(SUPPORT_GL21)
#define MGGL21SERVICES_H

#include "mgDisplayServices.h"

#include "Graphics3D/GL21/mgGL21Types.h"

class mgGL21VertexBuffer;

class mgGL21TextureImage : public mgTextureImage
{
public:
  mgTextureHandle m_handle;
  int m_format;
  BOOL m_mipmap;

  // constructor
  mgGL21TextureImage();

  // destructor
  virtual ~mgGL21TextureImage();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap);

  // set texture filtering
  virtual void setFilter(
    int filter);

  // update memory texture
  virtual void updateMemory(
    int x,
    int y,
    int width,
    int height,
    const BYTE* data);
};

class mgGL21TextureArray : public mgTextureArray
{
public:
  mgTextureHandle m_handle;
  int m_atlasWidth;
  int m_atlasHeight;

  // constructor
  mgGL21TextureArray();

  // destructor
  virtual ~mgGL21TextureArray();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap);

  // set texture filtering
  virtual void setFilter(
    int filter);
};

class mgGL21TextureCube : public mgTextureCube
{
public:
  mgTextureHandle m_handle;

  // constructor
  mgGL21TextureCube();

  // destructor
  virtual ~mgGL21TextureCube();

  // set texture wrapping
  virtual void setWrap(
    int xWrap,
    int yWrap);

  // set texture filtering
  virtual void setFilter(
    int filter);
};

// services offered by the display framework to the application
class mgGL21Services : public mgDisplayServices
{ 
public:
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

  // supports non-float shader arguments, bit operations
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

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    int count,                          // size of array
    const mgPoint3* point);             // point array

  // set shader uniform
  virtual void setShaderUniform(
    const char* shaderName,             // name of shader
    const char* varName,                // variable name
    int count,                          // size of array
    const float* value);                // float array

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

  // create surface for 2D graphics in world
  virtual mgTextureSurface* createTextureSurface();

  // create surface for 2D graphics in overlay
  virtual mgTextureSurface* createOverlaySurface();

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

  // get mvp transform
  virtual void getMVPTransform(
    mgMatrix4& xform);

  // get mv transform
  virtual void getMVTransform(
    mgMatrix4& xform);

  // set culling 
  virtual void setCulling(
    BOOL enable);

  // set front face clockwise
  virtual void setFrontCW(
    BOOL enable);

  // set zenable
  virtual void setZEnable(
    BOOL enable);

  // set transparent
  virtual void setTransparent(
    BOOL enable);


protected:
  mgMapStringToDWord m_shaders;

  mgGL21TextureImage* m_cursorTexture;

  // rendering state
  mgMatrix4 m_worldProjection;
  mgMatrix4 m_modelMatrix;
  mgMatrix4 m_worldMatrix;                // view*model matrix

  mgGL21TextureArray* m_textureArray;     // current texture array
  mgShaderHandle m_shader;                // current shader
  int m_maxVertexAttrib;                  // current max vertex attrib

  mgMapStringToPtr m_textureImages;       // map file name to mgTextureImage* instance
  mgPtrArray m_textureArrays;             // mgTextureArray* instances
  mgPtrArray m_textureCubes;              // mgTextureCube* instances

  // constructor
  mgGL21Services(
    const char* shaderDir,
    const char* fontDir);

  // destructor
  virtual ~mgGL21Services();

  // initialize view 
  virtual void initView();

  // set projection
  virtual void setProjection();

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

  // set vertex buffer up for use
  void useVertexBuffer(
    mgGL21VertexBuffer* vertexes);

  // scale texture array down by factor of 2.
  void scaleTextureImage(
    int width,
    int height,
    BYTE* data);

  // reload texture image from file list
  void reloadTextureImage(
    mgGL21TextureImage* texture);

  // reload texture array from file list
  void reloadTextureArray(
    mgGL21TextureArray* texture);

  // reload texture cube from file list
  void reloadTextureCube(
    mgGL21TextureCube* texture);

  // release texture display memory
  void unloadTextures();

  // reload textures into memory
  void reloadTextures();

  friend class mgTextureSurface;

  friend void mgInitDisplayServices(
    const char* shaderDir,
    const char* fontDir);
};

#endif
