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
#if !defined(MGDX9STATE_H) && defined(SUPPORT_DX9)
#define MGDX9STATE_H

#include <D3DX9.h>

// global handle to DirectX device.  Is recreated when we toggle fullscreen mode,
// so do not copy.
extern LPDIRECT3DDEVICE9 mg_d3dDevice;

class mgDX9State
{
public:
  // DirectX state
  D3DPRESENT_PARAMETERS m_presentParms; 
  LPDIRECT3D9 m_D3D;
  UINT m_adapter;
  D3DCAPS9 m_deviceCaps;

  // rendering state
  D3DXMATRIX m_projMatrix;
  D3DXMATRIX m_viewMatrix;
  D3DXMATRIX m_worldMatrix;
  D3DVIEWPORT9 m_viewport;

  // constructor
  mgDX9State()
  {
    m_D3D = NULL;
  }

  // destructor
  virtual ~mgDX9State()
  {
  }
};

#endif
