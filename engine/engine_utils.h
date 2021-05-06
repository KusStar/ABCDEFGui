#ifndef ABCDEFGui_ENGINE_UTILS
#define ABCDEFGui_ENGINE_UTILS

#include "engine.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <unistd.h>

namespace ABCDEFGui {
namespace Engine {

namespace Functions {

static JSValue Log(JSContext *ctx, JSValueConst this_val, int argc,
                   JSValueConst *argv) {
  for (int i = 0; i < argc; i++) {
    std::cout << Stringify(ctx, argv[i]) << " ";
  }
  std::cout << std::endl;
  return JS_UNDEFINED;
}

static JSValue ImGui_NewFrame(JSContext *ctx, JSValueConst this_val, int argc,
                              JSValueConst *argv) {
  std::cout << "Called NewFrame()" << std::endl;
  return JS_UNDEFINED;
}

static JSValue ImGui_CreateContext(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv) {
  std::cout << "Called CreateContext()" << std::endl;
  return JS_UNDEFINED;
}

} // namespace Functions

namespace Helpers {

bool has_prefix(const char *str, const char *prefix) {
  size_t len = strlen(str);
  size_t plen = strlen(prefix);
  return (len >= plen && !memcmp(str, prefix, plen));
}

bool has_suffix(const char *str, const char *suffix) {
  size_t len = strlen(str);
  size_t slen = strlen(suffix);
  return (len >= slen && !memcmp(str + len - slen, suffix, slen));
}

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

char *GetCwd() {
  char buffer[PATH_MAX];
  char *answer = getcwd(buffer, sizeof(buffer));
  free(buffer);
  return answer;
}

bool IsDir() {
  std::string path = "/path/to/directory";
}

} // namespace Helpers

static void AddConsole(JSContext *ctx, JSValue &obj) {
  JSValue console = JS_NewObject(ctx);

  static const JSCFunctionListEntry funcs[] = {
      JS_CFUNC_DEF("log", 1, Functions::Log),
  };

  JS_SetPropertyFunctionList(ctx, console, funcs, countof(funcs));

  JS_SetPropertyStr(ctx, obj, "console", console);
}

static void AddImGui(JSContext *ctx, JSValue &obj) {
  JSValue ui = JS_NewObject(ctx);

  static const JSCFunctionListEntry funcs[] = {
      JS_CFUNC_DEF("NewFrame", 1, Functions::ImGui_NewFrame),
      JS_CFUNC_DEF("CreateContext", 1, Functions::ImGui_CreateContext),
  };
  JS_SetPropertyFunctionList(ctx, ui, funcs, countof(funcs));

  JS_SetPropertyStr(ctx, obj, "ImGui", ui);
}

static void InitContext(JSContext *ctx,
                        std::function<void(JSContext *, JSValue &)> *funcs,
                        int len) {
  JSValue globalObj = JS_GetGlobalObject(ctx);
  for (int i = 0; i < len; i++) {
    funcs[i](ctx, globalObj);
  }
  JS_FreeValue(ctx, globalObj);
}

static void InitContext(JSContext *ctx) {
  std::function<void(JSContext *, JSValue &)> funcs[] = {AddConsole, AddImGui};
  InitContext(ctx, funcs, countof(funcs));
}

/* Modified from quickjs-libc.c -> js_module_loader */
static JSModuleDef *ModuleLoader(JSContext *ctx, const char *module_name,
                                 void *opaque) {
  JSModuleDef *m;
  char *name = strdup(module_name);

  // if (!Helpers::has_suffix(name, ".js")) {
  //   name = strcat(name, ".js");
  // }

  size_t buf_len;
  uint8_t *buf;
  JSValue func_val;

  buf = js_load_file(ctx, &buf_len, name);
  if (!buf) {
    // if (!Helpers::has_suffix(name, "/index.js")) {
    //   name = strndup(name, strlen(name) - 3);
    //   name = strcat(name, "/index.js");
    //   return ModuleLoader(ctx, name, opaque);
    // }
    JS_ThrowReferenceError(ctx, "could not load module filename '%s'", name);
    return NULL;
  }

  /* compile the module */
  func_val = JS_Eval(ctx, (char *)buf, buf_len, name,
                     JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
  js_free(ctx, buf);
  if (JS_IsException(func_val))
    return NULL;
  /* XXX: could propagate the exception */
  js_module_set_import_meta(ctx, func_val, TRUE, FALSE);
  /* the module is already referenced, so we must free it */
  m = (JSModuleDef *)JS_VALUE_GET_PTR(func_val);
  JS_FreeValue(ctx, func_val);
  return m;
}

/* Modified from quickjs.c -> js_default_module_normalize_name */
static char *ModuleNormalizeName(JSContext *ctx, const char *base_name,
                                 const char *name) {
  char *filename, *p;
  const char *r;
  int len;

  if (name[0] != '.') {
    /* if no initial dot, the module name is not modified */
    return js_strdup(ctx, name);
  }

  p = (char *)strrchr(base_name, '/');
  if (p)
    len = p - base_name;
  else
    len = 0;

  filename = (char *)js_malloc(ctx, len + strlen(name) + 1 + 1);
  if (!filename)
    return NULL;
  memcpy(filename, base_name, len);
  filename[len] = '\0';

  /* we only normalize the leading '..' or '.' */
  r = name;
  for (;;) {
    if (r[0] == '.' && r[1] == '/') {
      r += 2;
    } else if (r[0] == '.' && r[1] == '.' && r[2] == '/') {
      /* remove the last path element of filename, except if "."
         or ".." */
      if (filename[0] == '\0')
        break;
      p = strrchr(filename, '/');
      if (!p)
        p = filename;
      else
        p++;
      if (!strcmp(p, ".") || !strcmp(p, ".."))
        break;
      if (p > filename)
        p--;
      *p = '\0';
      r += 3;
    } else {
      break;
    }
  }
  if (filename[0] != '\0') {
    strcat(filename, "/");
  }
  strcat(filename, r);
  printf("normalize: %s %s -> %s\n", base_name, name, filename);
  return filename;
}

} // namespace Engine
} // namespace ABCDEFGui

#endif