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

//--------------------------------------------------------------
// Provides requestAnimationFrame in a cross browser way.
window.requestAnimFrame = (function() {
  return window.requestAnimationFrame ||
         window.webkitRequestAnimationFrame ||
         window.mozRequestAnimationFrame ||
         window.oRequestAnimationFrame ||
         window.msRequestAnimationFrame ||
         function(/* function FrameRequestCallback */ callback, /* DOMElement Element */ element) {
           return window.setTimeout(callback, 1000/60);
         };
})();

//--------------------------------------------------------------
// Provides cancelAnimationFrame in a cross browser way.
window.cancelAnimFrame = (function() {
  return window.cancelAnimationFrame ||
         window.webkitCancelAnimationFrame ||
         window.mozCancelAnimationFrame ||
         window.oCancelAnimationFrame ||
         window.msCancelAnimationFrame ||
         window.clearTimeout;
})();

var canvas = document.getElementById("canvas");

var animationRequestId = null;
var pointerNotLocked = true;
var graphicsWidth = -1;
var graphicsHeight = -1;
var eventFlags = 0;
var lastMouseX = 0;
var lastMouseY = 0;

// platform-independent keys and events
var MG_EVENT_ALT_DOWN    = 0x0001;
var MG_EVENT_SHIFT_DOWN  = 0x0002;
var MG_EVENT_CNTL_DOWN   = 0x0004;
var MG_EVENT_META_DOWN   = 0x0008;

var MG_EVENT_MOUSE1_DOWN = 0x0010;
var MG_EVENT_MOUSE2_DOWN = 0x0020;
var MG_EVENT_MOUSE3_DOWN = 0x0040;

var MG_EVENT_MOUSE_BUTTONS = 0x0070;

var MG_EVENT_KEYREPEAT   = 0x0080;

var MG_EVENT_KEY_F1         = 0x40000001;
var MG_EVENT_KEY_F2         = 0x40000002;
var MG_EVENT_KEY_F3         = 0x40000003;
var MG_EVENT_KEY_F4         = 0x40000004;
var MG_EVENT_KEY_F5         = 0x40000005;
var MG_EVENT_KEY_F6         = 0x40000006;
var MG_EVENT_KEY_F7         = 0x40000007;
var MG_EVENT_KEY_F8         = 0x40000008;
var MG_EVENT_KEY_F9         = 0x40000009;
var MG_EVENT_KEY_F10        = 0x4000000A;
var MG_EVENT_KEY_F11        = 0x4000000B;
var MG_EVENT_KEY_F12        = 0x4000000C;

var MG_EVENT_KEY_LEFT       = 0x4000000D;
var MG_EVENT_KEY_RIGHT      = 0x4000000E;
var MG_EVENT_KEY_UP         = 0x4000000F;
var MG_EVENT_KEY_DOWN       = 0x40000010;

var MG_EVENT_KEY_INSERT     = 0x40000011;
var MG_EVENT_KEY_DELETE     = 0x40000012;
var MG_EVENT_KEY_HOME       = 0x40000013;
var MG_EVENT_KEY_END        = 0x40000014;
var MG_EVENT_KEY_PAGEUP     = 0x40000015;
var MG_EVENT_KEY_PAGEDN     = 0x40000016;

var MG_EVENT_KEY_RETURN     = 0x40000017;
var MG_EVENT_KEY_TAB        = 0x40000018;
var MG_EVENT_KEY_ESCAPE     = 0x40000019;
var MG_EVENT_KEY_BACKSPACE  = 0x40000020;

/*

  Font functions for overlay text

*/

//--------------------------------------------------------------
// initialize font handling
function _mgCanvasInit()
{
  m_charCanvas = document.createElement('canvas');
  m_charMaxWidth = 256;
  m_charMaxHeight = 256;
  m_charCanvas.width = m_charMaxWidth;
  m_charCanvas.height = m_charMaxHeight;
  m_charCtx = m_charCanvas.getContext("2d");
  if (!m_charCtx)
    console.log("unable to get 2D context on font canvas");
  m_charCtx.textBaseline = "alphabetic";
  m_charFonts = new Array();
  m_charImage = _malloc(m_charMaxWidth * m_charMaxHeight)
}

//--------------------------------------------------------------
// terminate font handling
function _mgCanvasTerm()
{
}

//--------------------------------------------------------------
// load a font and return handle
function _mgCanvasLoadFont(
  faceName,
  size,
  dpi,
  bold,
  italic,
  fontHeight,
  fontAscent,
  charWidth)
{
  var font = new Object();
  font.faceName = Pointer_stringify(faceName); 
 
  // convert points to pixels using dpi
  font.size = Math.ceil(size*dpi/72);
  font.ascent = Math.floor(font.size*0.8);

  font.fontSpec = (bold ? "bold " : "") + 
                  (italic ? "italic " : "") +
                  (font.size+"px ") +
                  font.faceName;

  m_charCtx.font = font.fontSpec;
  console.log("load font " + font.fontSpec);

  var metrics = m_charCtx.measureText("n");

  /* 
     Because Canvas TextMetric contains nothing but width as of
     this writing, we fake all the information we want. 
  */
  setValue(charWidth, metrics.width, "double");
  setValue(fontHeight, font.size*1.1, "double");
  setValue(fontAscent, font.ascent, "double");

  var slot = m_charFonts.length;
  m_charFonts[slot] = font;

  return slot;  // handle used in C++ code
}

//--------------------------------------------------------------
// delete font
function _mgCanvasDeleteFont(
  handle)
{
  // =-= free the slot?
}

//--------------------------------------------------------------
// get a character in the font
function _mgCanvasGetChar(
  slot,
  letter,
  ptrAdvanceX,      // from start to end point
  ptrAdvanceY,
  ptrPosnX,         // from start to top-left of image
  ptrPosnY,
  ptrImageWidth,       // size of image
  ptrImageHeight,
  ptrImageData)
{
  var text = String.fromCharCode(letter);

  var font = m_charFonts[slot];
  var size = 0;
  var ascent = 0;
  if (font)
  {
    m_charCtx.font = font.fontSpec;
    size = font.size;
    ascent = font.ascent;
  }
  var metrics = m_charCtx.measureText(text);

  /*
    Since we have no character metrics other than width, 
    we must pick some border around the character image 
    sufficient to handle italic overhangs.  Then we
    use the entire area as the character image.  Since
    zero pixels in the character bitmap are not drawn,
    this should work, at the expense of larger characters.
  */
  var posnX = metrics.width;  // assume left side (A space) less than width
  var posnY = Math.ceil(ascent*1.2);
  var charWidth = 3*metrics.width;
  // char width must be a multiple of 4 for OpenGL
  charWidth = 4*Math.floor((charWidth+3)/4);
  var charHeight = Math.ceil(size*1.5);

  if (charWidth > 0)
  {  
    m_charCtx.fillStyle = "black";
    m_charCtx.fillRect(0, 0, m_charMaxWidth, m_charMaxHeight);
    m_charCtx.fillStyle = "white";
    m_charCtx.fillText(text, posnX, posnY);

    // copy red component of rendered letter to char image buffer
    var image = m_charCtx.getImageData(0, 0, charWidth, charHeight);
    for (var i = 0; i < charHeight; i++)
    {
      var linePosn = i*charWidth;
      for (var j = 0; j < charWidth; j++)
      {
        var pixelPosn = linePosn + j;
        HEAPU8[m_charImage + pixelPosn] = image.data[pixelPosn*4];
      }
    }
  }

  setValue(ptrAdvanceX, metrics.width, "double");
  setValue(ptrAdvanceY, 0, "double");
  setValue(ptrPosnX, -posnX, "double");
  setValue(ptrPosnY, posnY, "double");
  setValue(ptrImageWidth, charWidth, "i32");
  setValue(ptrImageHeight, charHeight, "i32");
  setValue(ptrImageData, m_charImage, "*");
}


//--------------------------------------------------------------
// WebGL rendering context lost
function mgContextLost(
  evt) 
{
  evt.preventDefault();

  if (animationRequestId) 
  {
    window.cancelAnimFrame(animationRequestId);
    animationRequestId = undefined;
  }
}

//--------------------------------------------------------------
// WebGL rendering context regained.  reinitialize
function mgContextRestored() 
{
  console.log("Restoring context");

//  mgDisplay.deleteBuffers();

  // reload the textures
//  mgDisplay.reloadTextures();

//  mgDisplay.runApp(app);
}

//--------------------------------------------------------------
// handle mouse move
function mgMouseMove(
  evt) 
{
//console.log("mouse move " + evt.clientX + ", " + evt.clientY);
  evt.preventDefault();

  if (pointerNotLocked)
  {
    var rect = canvas.getBoundingClientRect();
    var x = evt.clientX - rect.left;
    var y = evt.clientY - rect.top;

    Module.mgMouseMoveAbs(x, y, eventFlags);
  }
  else
  {
    var dx = evt.movementX || evt.mozMovementX || evt.webkitMovementX || 0;
    var dy = evt.movementY || evt.mozMovementY || evt.webkitMovementY || 0;

    Module.mgMouseMoveRel(dx, dy, eventFlags);
  }

  return false;
}

//--------------------------------------------------------------
// handle mouse down
function mgMouseDown(
  evt) 
{
  evt.preventDefault();

  // Ask the browser to lock the pointer
  pointerNotLocked = false;
  canvas.requestPointerLock();

  var button = 0;
  switch (evt.button)
  {
    case 0: button = MG_EVENT_MOUSE1_DOWN;
      break;
    case 1: button = MG_EVENT_MOUSE3_DOWN;
      break;
    case 2: button = MG_EVENT_MOUSE2_DOWN;
      break;
  }
  eventFlags |= button;

//  var rect = canvas.getBoundingClientRect();
//  lastMouseX = evt.clientX - rect.left;
//  lastMouseY = evt.clientY - rect.top;

  Module.mgMouseDown(button, eventFlags);
//  console.log("mouse down, flags="+eventFlags);
  return false;
}

//--------------------------------------------------------------
// handle mouse up
function mgMouseUp(
  evt) 
{
  evt.preventDefault();

  // Ask the browser to release the pointer
  document.exitPointerLock();
  pointerNotLocked = false;

  var button = 0;
  switch (evt.button)
  {
    case 0: button = MG_EVENT_MOUSE1_DOWN;
      break;
    case 1: button = MG_EVENT_MOUSE3_DOWN;
      break;
    case 2: button = MG_EVENT_MOUSE2_DOWN;
      break;
  }
  eventFlags &= ~button;

  Module.mgMouseUp(button, eventFlags);
//  console.log("mouse up, flags="+eventFlags);
  return false;
}

//--------------------------------------------------------------
// handle mouse enter
function mgMouseEnter(
  evt) 
{
  evt.preventDefault();

  var rect = canvas.getBoundingClientRect();
  var x = evt.clientX - rect.left;
  var y = evt.clientY - rect.top;
//  console.log("mouse enter " + x + ", " + y);

  Module.mgMouseEnter(x, y);
  return false;
}

//--------------------------------------------------------------
// handle mouse exit
function mgMouseExit(
  evt) 
{
  evt.preventDefault();

//  console.log("mouse exit");
  Module.mgMouseExit();

  return false;
}

//--------------------------------------------------------------
// handle touch down
function mgTouchDown(
  evt) 
{
  evt.preventDefault();

  eventFlags |= MG_EVENT_MOUSE1_DOWN;

  var x = 99999999;
  var y = 99999999;
  for (var i = 0; i < evt.touches.length; i++)
  {
    var touch = evt.touches[i];
    x = Math.min(x, touch.clientX);
    y = Math.min(y, touch.clientY);
  }
  var rect = canvas.getBoundingClientRect();
  lastMouseX = x;
  lastMouseY = y;

  console.log("touch at " + lastMouseX +", " + lastMouseY);

//  mgDisplay.app.appTouchDown();

  return false;
}

//--------------------------------------------------------------
// handle touch up
function mgTouchUp(
  evt) 
{
  evt.preventDefault();

  eventFlags &= ~MG_EVENT_MOUSE1_DOWN;

//  mgDisplay.app.appTouchUp();

  return false;
}

//--------------------------------------------------------------
// handle touch move
function mgTouchMove(
  evt) 
{
  evt.preventDefault();

  var x = 99999999;
  var y = 99999999;
  for (var i = 0; i < evt.touches.length; i++)
  {
    var touch = evt.touches[i];
    x = Math.min(x, touch.clientX);
    y = Math.min(y, touch.clientY);
  }

  var rect = canvas.getBoundingClientRect();
  var dx = x - lastMouseX;
  var dy = y - lastMouseY;
  lastMouseX = x;
  lastMouseY = y;

//  mgDisplay.app.appTouchDrag(dx, dy, eventFlags);

  return false;
}

var JS_KEYCODE_TAB = 9;
var JS_KEYCODE_ESCAPE = 27;

var JS_KEYCODE_SPACE = 32;
var JS_KEYCODE_PAGEUP = 33;
var JS_KEYCODE_PAGEDN = 34;
var JS_KEYCODE_END = 35;
var JS_KEYCODE_HOME = 36;
var JS_KEYCODE_INSERT = 45;
var JS_KEYCODE_DELETE = 46;

var JS_KEYCODE_BACKSPACE = 8;
var JS_KEYCODE_RETURN = 13;

var JS_KEYCODE_LEFT = 37;
var JS_KEYCODE_UP = 38;
var JS_KEYCODE_RIGHT = 39;
var JS_KEYCODE_DOWN = 40;

var JS_KEYCODE_F1  = 112;
var JS_KEYCODE_F2  = 113;
var JS_KEYCODE_F3  = 114;
var JS_KEYCODE_F4  = 115;
var JS_KEYCODE_F5  = 116;
var JS_KEYCODE_F6  = 117;
var JS_KEYCODE_F7  = 118;
var JS_KEYCODE_F8  = 119;
var JS_KEYCODE_F9  = 120;
var JS_KEYCODE_F10 = 121;
var JS_KEYCODE_F11 = 122;
var JS_KEYCODE_F12 = 123;

var JS_KEYCODE_A = 65;
var JS_KEYCODE_B = 66;
var JS_KEYCODE_C = 67;
var JS_KEYCODE_D = 68;
var JS_KEYCODE_E = 69;
var JS_KEYCODE_F = 70;
var JS_KEYCODE_G = 71;
var JS_KEYCODE_H = 72;
var JS_KEYCODE_I = 73;
var JS_KEYCODE_J = 74;
var JS_KEYCODE_K = 75;
var JS_KEYCODE_L = 76;
var JS_KEYCODE_M = 77;
var JS_KEYCODE_N = 78;
var JS_KEYCODE_O = 79;
var JS_KEYCODE_P = 80;
var JS_KEYCODE_Q = 81;
var JS_KEYCODE_R = 82;
var JS_KEYCODE_S = 83;
var JS_KEYCODE_T = 84;
var JS_KEYCODE_U = 85;
var JS_KEYCODE_V = 86;
var JS_KEYCODE_W = 87;
var JS_KEYCODE_X = 88;
var JS_KEYCODE_Y = 89;
var JS_KEYCODE_Z = 90;

//--------------------------------------------------------------
// translate from Javascript key to Milligram key
function translate(
  keyCode)
{
  switch (keyCode)
  {
    case JS_KEYCODE_TAB: return MG_EVENT_KEY_TAB;
    case JS_KEYCODE_ESCAPE: return MG_EVENT_KEY_ESCAPE;

    case JS_KEYCODE_PAGEUP: return MG_EVENT_KEY_PAGEUP;
    case JS_KEYCODE_PAGEDN: return MG_EVENT_KEY_PAGEDN;
    case JS_KEYCODE_END: return MG_EVENT_KEY_END;
    case JS_KEYCODE_HOME: return MG_EVENT_KEY_HOME;
    case JS_KEYCODE_INSERT: return MG_EVENT_KEY_INSERT;
    case JS_KEYCODE_DELETE: return MG_EVENT_KEY_DELETE;

    case JS_KEYCODE_BACKSPACE: return MG_EVENT_KEY_BACKSPACE;
    case JS_KEYCODE_RETURN: return MG_EVENT_KEY_RETURN;

    case JS_KEYCODE_LEFT: return MG_EVENT_KEY_LEFT;
    case JS_KEYCODE_UP: return MG_EVENT_KEY_UP;
    case JS_KEYCODE_RIGHT: return MG_EVENT_KEY_RIGHT;
    case JS_KEYCODE_DOWN: return MG_EVENT_KEY_DOWN;

    case JS_KEYCODE_F1: return MG_EVENT_KEY_F1;
    case JS_KEYCODE_F2: return MG_EVENT_KEY_F2;
    case JS_KEYCODE_F3: return MG_EVENT_KEY_F3;
    case JS_KEYCODE_F4: return MG_EVENT_KEY_F4;
    case JS_KEYCODE_F5: return MG_EVENT_KEY_F5;
    case JS_KEYCODE_F6: return MG_EVENT_KEY_F6;
    case JS_KEYCODE_F7: return MG_EVENT_KEY_F7;
    case JS_KEYCODE_F8: return MG_EVENT_KEY_F8;
    case JS_KEYCODE_F9: return MG_EVENT_KEY_F9;
    case JS_KEYCODE_F10: return MG_EVENT_KEY_F10;
    case JS_KEYCODE_F11: return MG_EVENT_KEY_F11;
    case JS_KEYCODE_F12: return MG_EVENT_KEY_F12;

    case JS_KEYCODE_SPACE:
    case JS_KEYCODE_A:
    case JS_KEYCODE_B:
    case JS_KEYCODE_C:
    case JS_KEYCODE_D:
    case JS_KEYCODE_E:
    case JS_KEYCODE_F:
    case JS_KEYCODE_G:
    case JS_KEYCODE_H:
    case JS_KEYCODE_I:
    case JS_KEYCODE_J:
    case JS_KEYCODE_K:
    case JS_KEYCODE_L:
    case JS_KEYCODE_M:
    case JS_KEYCODE_N:
    case JS_KEYCODE_O:
    case JS_KEYCODE_P:
    case JS_KEYCODE_Q:
    case JS_KEYCODE_R:
    case JS_KEYCODE_S:
    case JS_KEYCODE_T:
    case JS_KEYCODE_U:
    case JS_KEYCODE_V:
    case JS_KEYCODE_W:
    case JS_KEYCODE_X:
    case JS_KEYCODE_Y:
    case JS_KEYCODE_Z:
      return keyCode;

    default: return keyCode;
  }
}

//--------------------------------------------------------------
// process key down
function mgKeyDown(
  evt) 
{
//  evt.preventDefault();

  // =-= need to set key modifier flags
  Module.mgKeyDown(translate(evt.keyCode), eventFlags);

/*
  if (evt.keyCode == var JS_KEYCODE_F9)
  {
    loseContextExt = mgDisplay.gl.getExtension('WEBGL_lose_context') ||
              mgDisplay.gl.getExtension('WEBKIT_WEBGL_lose_context') ||
              mgDisplay.gl.getExtension('MOZ_WEBGL_lose_context');

    if (loseContextExt)
      loseContextExt.loseContext(); // trigger a context loss
    else console.log("No WEBGL_lose_context extension.");
  }

  if (evt.keyCode == var JS_KEYCODE_F10)
  {
    console.log("isContextLost = " + mgDisplay.gl.isContextLost());

    loseContextExt.restoreContext(); // restores the context
  }
*/

  return false;
}

//--------------------------------------------------------------
// process key up
function mgKeyUp(
  evt) 
{
  // =-= need to set key modifier flags
  Module.mgKeyUp(translate(evt.keyCode), eventFlags);
  return false;
}

//--------------------------------------------------------------
// process character key
function mgKeyChar(
  evt) 
{
  var char = evt.keyCode || evt.charCode || evt.which;

  Module.mgKeyChar(char, eventFlags);
  return false;
}

//--------------------------------------------------------------
// handle animation loop
function mgAppAnimate()
{
  // if canvas has resized
  if (graphicsWidth != canvas.clientWidth || graphicsHeight != canvas.clientHeight)
  {
    graphicsWidth = canvas.clientWidth;
    graphicsHeight = canvas.clientHeight;
    canvas.width = canvas.clientWidth;
    canvas.height = canvas.clientHeight;
    Module.mgViewResized(graphicsWidth, graphicsHeight);
  }

  Module.mgIdle();
  animationRequestId = window.requestAnimFrame(mgAppAnimate, canvas);
}

//--------------------------------------------------------------
// start app display and input processing after Module script runs
function mgAppStart()
{
  // find the correct pointer lock and release functions
  canvas.requestPointerLock = canvas.requestPointerLock ||
                              canvas.mozRequestPointerLock ||
                              canvas.webkitRequestPointerLock ||
                              function() { pointerNotLocked = true; };

  document.exitPointerLock = document.exitPointerLock ||
                             document.mozExitPointerLock ||
                             document.webkitExitPointerLock ||
                             function() {};

  function pointerLockError() 
  {
    pointerNotLocked = true;
    console.log("Error while locking pointer.");
  }

  document.addEventListener('pointerlockerror', pointerLockError, false);
  document.addEventListener('mozpointerlockerror', pointerLockError, false);
  document.addEventListener('webkitpointerlockerror', pointerLockError, false);

  canvas.addEventListener('webglcontextlost', mgContextLost, false);
  canvas.addEventListener('webglcontextrestored', mgContextRestored, false);

  canvas.addEventListener('mousemove', mgMouseMove, false);
  canvas.addEventListener('mousedown', mgMouseDown, false);
  canvas.addEventListener('mouseup', mgMouseUp, false);
  canvas.addEventListener('mouseover', mgMouseEnter, false);
  canvas.addEventListener('mouseout', mgMouseExit, false);

  canvas.addEventListener('touchmove', mgTouchMove, false);
  canvas.addEventListener('touchstart', mgTouchDown, false);
  canvas.addEventListener('touchend', mgTouchUp, false);

  window.addEventListener('keydown', mgKeyDown, false);
  window.addEventListener('keypress', mgKeyChar, false);
  window.addEventListener('keyup', mgKeyUp, false);

  mgAppAnimate();
}

/*

  Emscripten module setup.  The app main() does initialization, then ends.
  Set the Javascript event handling initialization mgAppStart as postRun step.
  Prevent Emscripten cleanup with "noExitRuntime" flag.

*/

// connect to canvas
var Module = {
  preRun: [],
  postRun: [mgAppStart],
  noExitRuntime: true,
  doNotCaptureKeyboard: true,

  print: (function() 
  {
    return function(text) 
    {
      console.log(text);
    };
  })(),

  printErr: function(text) 
  {
    console.log(text);
  },

  canvas: document.getElementById('canvas'),

  setStatus: function(text) 
  {
    console.log(text);
  },

  totalDependencies: 0,

  monitorRunDependencies: function(left) 
  {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(left ? 'Preparing... (' + 
      (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }

};

Module.setStatus('Downloading...');

