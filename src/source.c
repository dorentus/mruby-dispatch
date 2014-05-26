#include "mruby_dispatch_gem.h"

#define DISPATCH_SOURCE_DEF_CONST(mrb, source_class, const_name) do { \
  mrb_define_const(mrb, source_class, #const_name, mrb_symbol_value(mrb_intern_cstr(mrb, #const_name))); \
} while (0)

void
mrb_source_init(mrb_state *mrb)
{
  struct RClass *dispatch_class, *object_class, *source_class;

  dispatch_class = mrb_module_get(mrb, "Dispatch");
  object_class = mrb_class_get_under(mrb, dispatch_class, "Object");
  source_class = mrb_define_class_under(mrb, dispatch_class, "Source", object_class);

  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, DATA_ADD);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, DATA_OR);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, MACH_SEND);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, MACH_RECV);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, MEMORYPRESSURE);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, PROC);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, READ);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, SIGNAL);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, TIMER);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, VNODE);
  DISPATCH_SOURCE_DEF_CONST(mrb, source_class, WRITE);
}

void
mrb_source_final(mrb_state *mrb)
{
}
