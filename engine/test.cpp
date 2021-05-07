#include "engine.h"
#include "engine_utils.h"
#include <iostream>

using namespace ABCDEFGui::Engine;
using namespace std;

int main() {
  JSRuntime *rt;
  JSContext *ctx;

  rt = JS_NewRuntime();
  ctx = JS_NewContext(rt);

  JS_SetModuleLoaderFunc(rt, (JSModuleNormalizeFunc *)ModuleNormalizeName,
                         ModuleLoader, NULL);

  InitContext(ctx);

  const char *buf = "import './build/out.js';";

  JSValue result = JS_Eval(ctx, buf, strlen(buf), "",
                           JS_EVAL_TYPE_GLOBAL | JS_EVAL_TYPE_MODULE);

  cout << Stringify(ctx, result) << endl;

  JS_FreeValue(ctx, result);
  JS_FreeContext(ctx);
  JS_FreeRuntime(rt);
  return 0;
}