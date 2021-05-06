#ifndef ABCDEFGui_ENGINE
#define ABCDEFGui_ENGINE

#include "cutils.h"
#include "quickjs-libc.h"
#include "quickjs.h"
#include <string>

namespace ABCDEFGui {

namespace Engine {

static std::string StringifyError(JSContext *ctx, JSValueConst exception_val);

static std::string Stringify(JSContext *ctx, JSValueConst val) {
  if (JS_IsException(val)) {
    return StringifyError(ctx, val);
  }
  const char *str = JS_ToCString(ctx, val);
  if (str) {
    std::string result(str);
    JS_FreeCString(ctx, str);
    return result;
  } else {
    return std::string(JS_AtomToCString(ctx, JS_ValueToAtom(ctx, val)));
  }
}

static std::string StringifyError(JSContext *ctx, JSValueConst exception_val) {
  JSValue val;
  if (JS_IsError(ctx, exception_val)) {
    // an "Error" is an object of the Error type
    val = JS_GetPropertyStr(ctx, exception_val, "stack");
    if (!JS_IsUndefined(val)) {
      return Stringify(ctx, exception_val) + "\n" + Stringify(ctx, val);
    }
    return "[Error]";
  } else if (JS_IsException(exception_val)) {
    // ...while an "Exception" is a native error type, and we must call
    // JS_GetException to find the actual error object
    JSValue err = JS_GetException(ctx);
    return StringifyError(ctx, err);
  } else {
    return Stringify(ctx, exception_val);
  }
}

} // namespace Engine

} // namespace ABCDEFGui

#endif