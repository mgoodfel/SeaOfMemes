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

#ifdef SUPPORT_DX9

#include <D3DX9.h>
#include "DX9DevMode.h"

//--------------------------------------------------------------
// write device modes
void mgWriteDeviceModes(
  FILE* output,
  LPDIRECT3D9 d3d)
{
  int count = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
  fprintf(output, "\n");
  fprintf(output, "------------------ device modes -----------------\n");
  fprintf(output, "%d X8R8G8B8 modes.\n", count);
  D3DDISPLAYMODE mode;
  for (int i = 0; i < count; i++)
  {
    d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode);
    const char* format = "";
    switch (mode.Format)
    {
      case D3DFMT_R8G8B8: format="R8G8B8"; break;
      case D3DFMT_A8R8G8B8: format="A8R8G8B8"; break;
      case D3DFMT_X8R8G8B8: format="X8R8G8B8"; break;
      case D3DFMT_R5G6B5: format="R5G6B5"; break;
      case D3DFMT_X1R5G5B5: format="X1R5G5B5"; break;
      case D3DFMT_A1R5G5B5: format="A1R5G5B5"; break;
      case D3DFMT_A4R4G4B4: format="A4R4G4B4"; break;
      case D3DFMT_R3G3B2: format="R3G3B2"; break;
      case D3DFMT_A8R3G3B2: format="A8R3G3B2"; break;
      case D3DFMT_X4R4G4B4: format="X4R4G4B4"; break;
      case D3DFMT_A2B10G10R10: format="A2B10G10R10"; break;
      case D3DFMT_A2R10G10B10: format="A2R10G10B10"; break;
      default: format="??"; break;
    }
    fprintf(output, "%d by %d (%.2f), refresh=%d, format=%s\n", mode.Width, mode.Height, 
      (double) mode.Width / mode.Height, mode.RefreshRate, (const char*) format);
  }
}

//--------------------------------------------------------------
// add flag bit to line
void capFlag(
  FILE* output,
  DWORD flag,
  DWORD mask,
  const char* name)
{
  mgString msg;
  if ((flag & mask) == 0)
    msg += "NO ";
  msg += name;
  fprintf(output, "  %s\n", (const char *) msg);
}

//--------------------------------------------------------------
// write device caps to log
void mgWriteDeviceCaps(
  FILE* output,
  D3DCAPS9 &caps)
{
  fprintf(output, "\n");
  fprintf(output, "------------------ device capabilities -----------------\n");
  fprintf(output, "MasterAdapterOrdinal: %d\n", caps.MasterAdapterOrdinal);
  fprintf(output, "AdapterOrdinalInGroup: %d\n", caps.AdapterOrdinalInGroup);
  fprintf(output, "NumberOfAdaptersInGroup: %d\n", caps.NumberOfAdaptersInGroup);

  fprintf(output, "MaxTextureWidth: %d\n", caps.MaxTextureWidth);
  fprintf(output, "MaxTextureHeight: %d\n", caps.MaxTextureHeight);
  fprintf(output, "MaxTextureRepeat: %d\n", caps.MaxTextureRepeat);
  fprintf(output, "MaxTextureAspectRatio: %d\n", caps.MaxTextureAspectRatio);
  fprintf(output, "MaxVolumeExtent: %d\n", caps.MaxVolumeExtent);
  fprintf(output, "MaxAnisotropy: %d\n", caps.MaxAnisotropy);
  fprintf(output, "MaxTextureBlendStages: %d\n", caps.MaxTextureBlendStages);
  fprintf(output, "MaxSimultaneousTextures: %d\n", caps.MaxSimultaneousTextures);
  fprintf(output, "MaxActiveLights: %d\n", caps.MaxActiveLights);
  fprintf(output, "MaxUserClipPlanes: %d\n", caps.MaxUserClipPlanes);
  fprintf(output, "MaxVertexBlendMatricies: %d\n", caps.MaxVertexBlendMatrices);
  fprintf(output, "MaxVertexBlendMatrixIndex: %d\n", caps.MaxVertexBlendMatrixIndex);

  fprintf(output, "VertexShaderVersion: %08x\n", caps.VertexShaderVersion);
  fprintf(output, "PixelShaderVersion: %08x\n", caps.PixelShaderVersion);
  fprintf(output, "NumSimultaneousRTs: %d\n", caps.NumSimultaneousRTs);
  fprintf(output, "MaxPrimitiveCount: %d\n", caps.MaxPrimitiveCount);
  fprintf(output, "MaxVertexIndex: %d\n", caps.MaxVertexIndex);
  fprintf(output, "MaxStreams: %d\n", caps.MaxStreams);
  fprintf(output, "MaxStreamStride: %d\n", caps.MaxStreamStride);
  fprintf(output, "MaxVertexShaderConst: %d\n", caps.MaxVertexShaderConst);
  fprintf(output, "MaxVShaderInstructionsExecuted: %d\n", caps.MaxVShaderInstructionsExecuted);
  fprintf(output, "MaxPShaderInstructionsExecuted: %d\n", caps.MaxPShaderInstructionsExecuted);
  fprintf(output, "MaxVertexShader30InstructionSlots: %d\n", caps.MaxVertexShader30InstructionSlots);
  fprintf(output, "MaxPixelShader30InstructionSlots: %d\n", caps.MaxPixelShader30InstructionSlots);

  fprintf(output, "MaxNpatchTessellationLevel: %f\n", caps.MaxNpatchTessellationLevel);
  fprintf(output, "MaxVertexW: %f\n", caps.MaxVertexW);
  fprintf(output, "GuardBandLeft: %f\n", caps.GuardBandLeft);
  fprintf(output, "GuardBandTop: %f\n", caps.GuardBandTop);
  fprintf(output, "GuardBandRight: %f\n", caps.GuardBandRight);
  fprintf(output, "GuardBandBottom: %f\n", caps.GuardBandBottom);
  fprintf(output, "ExtentsAdjust: %f\n", caps.ExtentsAdjust);
  fprintf(output, "MaxPointSize: %f\n", caps.MaxPointSize);
  fprintf(output, "PixelShader1xMaxValue: %f\n", caps.PixelShader1xMaxValue);

  fprintf(output, "Presentation intervals:\n");
  capFlag(output, caps.PresentationIntervals, D3DPRESENT_INTERVAL_IMMEDIATE, "immediate");
  capFlag(output, caps.PresentationIntervals, D3DPRESENT_INTERVAL_ONE, "one");
  capFlag(output, caps.PresentationIntervals, D3DPRESENT_INTERVAL_TWO, "two");
  capFlag(output, caps.PresentationIntervals, D3DPRESENT_INTERVAL_THREE, "three");
  capFlag(output, caps.PresentationIntervals, D3DPRESENT_INTERVAL_FOUR, "four ");
   

  fprintf(output, "Cursor Caps:\n");
  capFlag(output, caps.CursorCaps, D3DCURSORCAPS_COLOR, "color");
  capFlag(output, caps.CursorCaps, D3DCURSORCAPS_LOWRES, "low res");
   

  fprintf(output, "DevCaps:\n");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_CANBLTSYSTONONLOCAL, "Can Blt Syst NonLocal");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_CANRENDERAFTERFLIP, "Can Render after Flip");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_DRAWPRIMITIVES2, "Draw Primitives2");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_DRAWPRIMITIVES2EX, "Draw Primitives2 EX");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_DRAWPRIMTLVERTEX, "Draw Prim mgVertex");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_EXECUTESYSTEMMEMORY, "Execute System Memory");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_EXECUTEVIDEOMEMORY, "Execute Video Memory");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_HWRASTERIZATION, "HW Rasterization");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_HWTRANSFORMANDLIGHT, "HW Transform and Light");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_NPATCHES, "NPatches");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_PUREDEVICE, "Pure Device");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_QUINTICRTPATCHES, "Quintic Patches");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_RTPATCHES, "RT Patches");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_RTPATCHHANDLEZERO, "RT Patch Handle Zero");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_SEPARATETEXTUREMEMORIES, "Separate Texture Memories");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_TEXTURENONLOCALVIDMEM, "Texture nonLocal Video Memory");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_TEXTURESYSTEMMEMORY, "Texture System Memory");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_TEXTUREVIDEOMEMORY, "Texture Video Memory");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_TLVERTEXSYSTEMMEMORY, "TLVertex system Memory");
  capFlag(output, caps.DevCaps, D3DDEVCAPS_TLVERTEXVIDEOMEMORY, "TLVertex Video Memory");
   

  fprintf(output, "Caps2:\n");
  capFlag(output, caps.Caps2, D3DCAPS2_CANAUTOGENMIPMAP, "Can Autogen MipMap");
  capFlag(output, caps.Caps2, D3DCAPS2_CANCALIBRATEGAMMA, "Can Calibrate Gamma");
  capFlag(output, caps.Caps2, D3DCAPS2_CANMANAGERESOURCE, "Can manage resource");
  capFlag(output, caps.Caps2, D3DCAPS2_DYNAMICTEXTURES, "Dynamic Textures");
  capFlag(output, caps.Caps2, D3DCAPS2_FULLSCREENGAMMA, "full screen gamma");
   

  fprintf(output, "Caps3:\n");
  capFlag(output, caps.Caps3, D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD, "Alpha full screen flip or discard");
  capFlag(output, caps.Caps3, D3DCAPS3_COPY_TO_VIDMEM, "copy to video memory");
  capFlag(output, caps.Caps3, D3DCAPS3_COPY_TO_SYSTEMMEM, "copy to system memory");
  capFlag(output, caps.Caps3, D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION, "linear to SRGB presentation");

   
  fprintf(output, "Primitive Misc Caps:\n");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_MASKZ, "MaskZ");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_CULLNONE, "CullNone");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_CULLCW, "CullCW");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_CULLCCW, "CullCCW");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_COLORWRITEENABLE, "ColorWriteEnable");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_CLIPPLANESCALEDPOINTS, "ClipPlaneScaledPoints");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_CLIPTLVERTS, "ClipTLVerts");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_TSSARGTEMP, "TSSARGTemp");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_BLENDOP, "BLENDOP");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_NULLREFERENCE, "NullReference");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_INDEPENDENTWRITEMASKS, "IndependentWriteMasks");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_PERSTAGECONSTANT, "PerStageConstant");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_FOGANDSPECULARALPHA, "FogAndSpecularAlpha");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_SEPARATEALPHABLEND, "SeparateAlphaBlend");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS, "MRTIndependentBitDepths");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING, "MRTPostPixelShaderBlending");
  capFlag(output, caps.PrimitiveMiscCaps, D3DPMISCCAPS_FOGVERTEXCLAMPED, "FogVertexClamped");
   

  fprintf(output, "Raster Caps:\n");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_ANISOTROPY, "Anisotropy");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_COLORPERSPECTIVE, "ColorPerspective");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_DITHER, "Dither");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_DEPTHBIAS, "DepthBias");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_FOGRANGE, "FogRange");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_FOGTABLE, "FogTable");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_FOGVERTEX, "FogVertex");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_MIPMAPLODBIAS, "MipMapLodBias");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_MULTISAMPLE_TOGGLE, "MultiSampleToggle");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_SCISSORTEST, "ScissorTest");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS, "SlopeScaleDepthBias");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_WBUFFER, "WBuffer");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_WFOG, "WFog");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_ZBUFFERLESSHSR, "zBufferLessHSR");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_ZFOG, "ZFog");
  capFlag(output, caps.RasterCaps, D3DPRASTERCAPS_ZTEST, "ZTest");
  
  fprintf(output, "ZCmp Caps:\n");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_ALWAYS, "Always");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_EQUAL, "Equal");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_GREATER, "Greater");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_GREATEREQUAL, "GreaterEqual");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_LESS, "Less");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_LESSEQUAL, "LessEqual");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_NEVER, "Never");
  capFlag(output, caps.ZCmpCaps, D3DPCMPCAPS_NOTEQUAL, "NotEqual");

  fprintf(output, "AlphaCmpCaps:\n");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_ALWAYS, "Always");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_EQUAL, "Equal");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_GREATER, "Greater");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_GREATEREQUAL, "GreaterEqual");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_LESS, "Less");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_LESSEQUAL, "LessEqual");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_NEVER, "Never");
  capFlag(output, caps.AlphaCmpCaps, D3DPCMPCAPS_NOTEQUAL, "NotEqual");

  fprintf(output, "SrcBlendCaps:\n");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_BLENDFACTOR, "BlendFactor");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA, "BothInvSrcAlpha");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA, "BothSrcAlpha");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_DESTALPHA, "DestAlpha");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_DESTCOLOR, "DestColor");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "InvDestAlpha");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR, "InvDestColor");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "InvSrcAlpha");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR, "InvSrcColor");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_ONE, "One");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_SRCALPHA, "SrcAlpha");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_SRCALPHASAT, "SrcAlphaSAT");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_SRCCOLOR, "SrcColor");
  capFlag(output, caps.SrcBlendCaps, D3DPBLENDCAPS_ZERO, "Zero");

  fprintf(output, "DestBlendCaps:\n");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_BLENDFACTOR, "BlendFactor");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA, "BothInvSrcAlpha");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA, "BothSrcAlpha");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_DESTALPHA, "DestAlpha");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_DESTCOLOR, "DestColor");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "InvDestAlpha");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR, "InvDestColor");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "InvSrcAlpha");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR, "InvSrcColor");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_ONE, "One");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_SRCALPHA, "SrcAlpha");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_SRCALPHASAT, "SrcAlphaSAT");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_SRCCOLOR, "SrcColor");
  capFlag(output, caps.DestBlendCaps, D3DPBLENDCAPS_ZERO, "Zero");

  fprintf(output, "ShadeCaps:\n");
  capFlag(output, caps.ShadeCaps, D3DPSHADECAPS_ALPHAGOURAUDBLEND, "AlphaGouraudBlend");
  capFlag(output, caps.ShadeCaps, D3DPSHADECAPS_COLORGOURAUDRGB, "ColorGouraudRGB");
  capFlag(output, caps.ShadeCaps, D3DPSHADECAPS_FOGGOURAUD, "FogGouraud");
  capFlag(output, caps.ShadeCaps, D3DPSHADECAPS_SPECULARGOURAUDRGB, "SpecularGouraudRGB");

  fprintf(output, "TextureCaps:\n");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_ALPHA, "Alpha");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_ALPHAPALETTE, "AlphaPalette");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_CUBEMAP, "CubeMap");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_CUBEMAP_POW2, "CubeMap Pow2");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_MIPCUBEMAP, "MipCubeMap");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_MIPMAP, "MipMap");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_MIPVOLUMEMAP, "MipVolumeMap");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_NONPOW2CONDITIONAL, "NonPow2Conditional");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_NOPROJECTEDBUMPENV, "NoProjectedBumpEnv");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_PERSPECTIVE, "Perspective");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_POW2, "Pow2");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_PROJECTED, "Projected");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_SQUAREONLY, "SquareOnly");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE, "TexRepeatNotScaledBySize");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_VOLUMEMAP, "VolumeMap");
  capFlag(output, caps.TextureCaps, D3DPTEXTURECAPS_VOLUMEMAP_POW2, "VolumeMap Pow2");

  fprintf(output, "TextureFilterCaps:\n");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "Mag Point"); 
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "Mag Linear");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC, "Mag Anisotropic");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD, "Mag Pyramidal Quad");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD, "Mag Guassian Quad");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "Min Point");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "Min Linear");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC, "Min Anisotropic");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD, "Min Pyramidal Quad");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD, "Min Gaussian Quad");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "Mip Point");
  capFlag(output, caps.TextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR, "Mip Linear"); 

  fprintf(output, "CubeTextureFilterCaps:\n");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "Mag Point"); 
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "Mag Linear");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC, "Mag Anisotropic");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD, "Mag Pyramidal Quad");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD, "Mag Guassian Quad");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "Min Point");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "Min Linear");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC, "Min Anisotropic");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD, "Min Pyramidal Quad");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD, "Min Gaussian Quad");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "Mip Point");
  capFlag(output, caps.CubeTextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR, "Mip Linear"); 

  fprintf(output, "VolumeTextureFilterCaps:\n");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "Mag Point"); 
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "Mag Linear");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC, "Mag Anisotropic");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD, "Mag Pyramidal Quad");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD, "Mag Guassian Quad");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "Min Point");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "Min Linear");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC, "Min Anisotropic");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD, "Min Pyramidal Quad");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD, "Min Gaussian Quad");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "Mip Point");
  capFlag(output, caps.VolumeTextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR, "Mip Linear"); 

  fprintf(output, "VertexTextureFilterCaps:\n");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "Mag Point"); 
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "Mag Linear");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC, "Mag Anisotropic");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD, "Mag Pyramidal Quad");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD, "Mag Guassian Quad");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "Min Point");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "Min Linear");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC, "Min Anisotropic");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD, "Min Pyramidal Quad");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD, "Min Gaussian Quad");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "Mip Point");
  capFlag(output, caps.VertexTextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR, "Mip Linear"); 

  fprintf(output, "StretchRectFilterCaps:\n");
  capFlag(output, caps.StretchRectFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "Mag Point"); 
  capFlag(output, caps.StretchRectFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "Mag Linear");
  capFlag(output, caps.StretchRectFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "Min Point");
  capFlag(output, caps.StretchRectFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "Min Linear");

  fprintf(output, "TextureAddressCaps:\n");
  capFlag(output, caps.TextureAddressCaps, D3DPTADDRESSCAPS_BORDER, "Border");
  capFlag(output, caps.TextureAddressCaps, D3DPTADDRESSCAPS_CLAMP, "Clamp");
  capFlag(output, caps.TextureAddressCaps, D3DPTADDRESSCAPS_INDEPENDENTUV, "Independent UV");
  capFlag(output, caps.TextureAddressCaps, D3DPTADDRESSCAPS_MIRROR, "Mirror");
  capFlag(output, caps.TextureAddressCaps, D3DPTADDRESSCAPS_MIRRORONCE, "MirrorOnce");
  capFlag(output, caps.TextureAddressCaps, D3DPTADDRESSCAPS_WRAP, "Wrap");

  fprintf(output, "VolumeTextureAddressCaps:\n");
  capFlag(output, caps.VolumeTextureAddressCaps, D3DPTADDRESSCAPS_BORDER, "Border");
  capFlag(output, caps.VolumeTextureAddressCaps, D3DPTADDRESSCAPS_CLAMP, "Clamp");
  capFlag(output, caps.VolumeTextureAddressCaps, D3DPTADDRESSCAPS_INDEPENDENTUV, "Independent UV");
  capFlag(output, caps.VolumeTextureAddressCaps, D3DPTADDRESSCAPS_MIRROR, "Mirror");
  capFlag(output, caps.VolumeTextureAddressCaps, D3DPTADDRESSCAPS_MIRRORONCE, "MirrorOnce");
  capFlag(output, caps.VolumeTextureAddressCaps, D3DPTADDRESSCAPS_WRAP, "Wrap");

  fprintf(output, "LineCaps:\n");
  capFlag(output, caps.LineCaps, D3DLINECAPS_ALPHACMP, "AlphaCmp");
  capFlag(output, caps.LineCaps, D3DLINECAPS_ANTIALIAS, "AntiAlias");
  capFlag(output, caps.LineCaps, D3DLINECAPS_BLEND, "Blend");
  capFlag(output, caps.LineCaps, D3DLINECAPS_FOG, "Fog");
  capFlag(output, caps.LineCaps, D3DLINECAPS_TEXTURE, "Texture");
  capFlag(output, caps.LineCaps, D3DLINECAPS_ZTEST, "ZTest");

  fprintf(output, "StencilCaps:\n");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_KEEP, "Keep");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_ZERO, "Zero");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_REPLACE, "Replace");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_INCRSAT, "Increment");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_DECRSAT, "Decrement");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_INVERT, "Invert");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_INCR, "IncrementWrap");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_DECR, "DecrementWrap");
  capFlag(output, caps.StencilCaps, D3DSTENCILCAPS_TWOSIDED, "TwoSided");

  fprintf(output, "FVFCaps:\n");
  capFlag(output, caps.FVFCaps, D3DFVFCAPS_DONOTSTRIPELEMENTS, "Do Not Strip Elements");
  capFlag(output, caps.FVFCaps, D3DFVFCAPS_PSIZE, "PSize");
  capFlag(output, caps.FVFCaps, D3DFVFCAPS_TEXCOORDCOUNTMASK, "TexCoordCountMask");

  fprintf(output, "TextureOpCaps:\n");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_ADD, "Add");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_ADDSIGNED, "Add Signed");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_ADDSIGNED2X, "Add Signed 2X");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_ADDSMOOTH, "Add Smooth");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BLENDCURRENTALPHA, "Blend Current Alpha");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BLENDDIFFUSEALPHA, "Blend Diffuse Alpha");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BLENDFACTORALPHA, "Blend Factor Alpha");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BLENDTEXTUREALPHA, "Blend Texture Alpha");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BLENDTEXTUREALPHAPM, "Blend Texture Alpha PM");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BUMPENVMAP, "Bump Env Map");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_BUMPENVMAPLUMINANCE, "Bump Env Map Luminance");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_DISABLE, "Disable");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_DOTPRODUCT3, "DotProduct3");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_LERP, "LERP");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATE, "Modulate");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATE2X, "Modulate2X");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATE4X, "Modulate4X");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR, "Modulate Alpha Add Color");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA, "Modulate Color Add Alpha");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR, "Inv Alpha Add Color");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA, "Inv Color Add Alpha");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_MULTIPLYADD, "Multiply Add");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_PREMODULATE, "Premodulate");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_SELECTARG1, "Select Arg1");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_SELECTARG2, "Select Arg2");
  capFlag(output, caps.TextureOpCaps, D3DTEXOPCAPS_SUBTRACT, "Subtract");

  fprintf(output, "DeclTypes:\n");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_UBYTE4, "UBYTE4");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_UBYTE4N, "UBYTE4N");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_SHORT2N, "Short2N");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_SHORT4N, "Short4N");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_USHORT2N, "UShort2N");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_USHORT4N, "UShort4N");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_UDEC3, "UDec3");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_DEC3N, "Dec3N");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_FLOAT16_2, "Float16_2");
  capFlag(output, caps.DeclTypes, D3DDTCAPS_FLOAT16_4, "Float16_4"); 

  fprintf(output, "VertexProcessingCaps:\n");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_DIRECTIONALLIGHTS, "Directional Lights");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_LOCALVIEWER, "Local Viewer");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_MATERIALSOURCE7, "Material Source");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER, "No Texgen nonlocal veiwer");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_POSITIONALLIGHTS, "Positional Lights");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_TEXGEN, "Texgen");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_TEXGEN_SPHEREMAP, "Spheremap");
  capFlag(output, caps.VertexProcessingCaps, D3DVTXPCAPS_TWEENING, "tweening");

/*
    DWORD DevCaps2;
    D3DVSHADERCAPS2_0 VS20Caps;
    D3DPSHADERCAPS2_0 PS20Caps;

    */
}

#endif
