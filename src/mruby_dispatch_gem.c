#include "mruby.h"
#include <dispatch/dispatch.h>

static dispatch_once_t predicate;

static mrb_value
mrb_dispatch_once(mrb_state *mrb, mrb_value self)
{
  mrb_value blk;

  mrb_get_args(mrb, "&", &blk);

  if (mrb_nil_p(blk)) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "no block given");
  }

  dispatch_once(&predicate, ^{
    mrb_yield(mrb, blk, mrb_nil_value());
  });

  return mrb_nil_value();
}

void
mrb_mruby_dispatch_gem_init(mrb_state *mrb)
{
  struct RClass *dispatch;

  dispatch = mrb_define_module(mrb, "Dispatch");

  mrb_define_const(mrb, dispatch, "TIME_NOW", mrb_fixnum_value(DISPATCH_TIME_NOW));
  mrb_define_const(mrb, dispatch, "TIME_FOREVER", mrb_fixnum_value(DISPATCH_TIME_FOREVER));

  mrb_define_class_method(mrb, dispatch, "once", mrb_dispatch_once, MRB_ARGS_NONE());
}

void
mrb_mruby_dispatch_gem_final(mrb_state *mrb)
{
  predicate = 0;
}
