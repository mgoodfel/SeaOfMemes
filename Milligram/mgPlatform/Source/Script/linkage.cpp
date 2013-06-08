#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

extern void mgIdle();
extern void mgShutdown();
extern void mgViewResized(int width, int height);
extern void mgMouseMoveAbs(int x, int y, int flags);
extern void mgMouseMoveRel(int dx, int dy, int flags);
extern void mgMouseDown(int button, int flags);
extern void mgMouseUp(int button, int flags);
extern void mgMouseEnter(int x, int y);
extern void mgMouseExit();
extern void mgKeyDown(int keyCode, int flags);
extern void mgKeyUp(int keyCode, int flags);
extern void mgKeyChar(int unicode, int flags);

EMSCRIPTEN_BINDINGS(([]()
{
  function("mgIdle", &mgIdle);
  function("mgViewResized", &mgViewResized);
  function("mgMouseMoveAbs", &mgMouseMoveAbs);
  function("mgMouseMoveRel", &mgMouseMoveRel);
  function("mgMouseDown", &mgMouseDown);
  function("mgMouseUp", &mgMouseUp);
  function("mgMouseEnter", &mgMouseEnter);
  function("mgMouseExit", &mgMouseExit);
  function("mgKeyDown", &mgKeyDown);
  function("mgKeyUp", &mgKeyUp);
  function("mgKeyChar", &mgKeyChar);
  function("mgShutdown", &mgShutdown);
}));

#endif
