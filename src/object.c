#include "mruby_dispatch_gem.h"

void
mrb_dispatch_release(mrb_state *mrb, void *data)
{
  dispatch_release(data);
}

static mrb_value
mrb_dispatch_resume(mrb_state *mrb, mrb_value self)
{
  dispatch_resume(DATA_PTR(self));

  return mrb_nil_value();
}

static mrb_value
mrb_dispatch_suspend(mrb_state *mrb, mrb_value self)
{
  dispatch_suspend(DATA_PTR(self));

  return mrb_nil_value();
}

void
mrb_object_init(mrb_state *mrb)
{
  struct RClass *dispatch, *object;

  dispatch = mrb_module_get(mrb, "Dispatch");
  object = mrb_define_class_under(mrb, dispatch, "Object", mrb->object_class);

  mrb_define_method(mrb, object, "resume!", mrb_dispatch_resume, MRB_ARGS_NONE());
  mrb_define_method(mrb, object, "suspend!", mrb_dispatch_suspend, MRB_ARGS_NONE());
}

void
mrb_object_final(mrb_state *mrb)
{
}
