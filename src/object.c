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

static mrb_value
mrb_get_dispatch_object(mrb_state *mrb, mrb_value self)
{
  struct Rclass *os_dispatch_object;

  os_dispatch_object = mrb_class_get(mrb, "OS_dispatch_object");

  return mrb_obj_value(Data_Wrap_Struct(mrb, os_dispatch_object, &mrb_dispatch_object_type, DATA_PTR(self)));
}

void
mrb_object_init(mrb_state *mrb)
{
  struct RClass *dispatch, *object, *os_dispatch_object;

  os_dispatch_object = mrb_define_class(mrb, "OS_dispatch_object", mrb->object_class);

  dispatch = mrb_module_get(mrb, "Dispatch");
  object = mrb_define_class_under(mrb, dispatch, "Object", mrb->object_class);

  mrb_define_method(mrb, object, "resume!", mrb_dispatch_resume, MRB_ARGS_NONE());
  mrb_define_method(mrb, object, "suspend!", mrb_dispatch_suspend, MRB_ARGS_NONE());
  mrb_define_method(mrb, object, "dispatch_object", mrb_get_dispatch_object, MRB_ARGS_NONE());
}

void
mrb_object_final(mrb_state *mrb)
{
}
