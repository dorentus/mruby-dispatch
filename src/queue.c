#include "mruby_dispatch_gem.h"

static void
mrb_dispatch_queue_release(mrb_state *mrb, void *data)
{
  dispatch_release((dispatch_queue_t)data);
}

static struct mrb_data_type mrb_dispatch_queue_type = { "$mrb_dispatch_queue", mrb_dispatch_queue_release };

static mrb_value
mrb_queue_initialize(mrb_state *mrb, mrb_value self)
{
  const char *label_str = NULL;
  dispatch_queue_t q;

  mrb_get_args(mrb, "|z", &label_str);

  q = dispatch_queue_create(label_str, NULL);

  DATA_TYPE(self) = &mrb_dispatch_queue_type;
  DATA_PTR(self) = q;

  return self;
}

static mrb_value
mrb_queue_to_s(mrb_state *mrb, mrb_value self)
{
  dispatch_queue_t q;
  const char *label_str;

  q = (dispatch_queue_t)DATA_PTR(self);
  label_str = dispatch_queue_get_label(q);

  return mrb_str_new_cstr(mrb, label_str);
}

void
mrb_queue_init(mrb_state *mrb)
{
  struct RClass *dispatch, *queue;

  dispatch = mrb_module_get(mrb, "Dispatch");
  queue = mrb_define_class_under(mrb, dispatch, "Queue", mrb->object_class);

  mrb_define_method(mrb, queue, "initialize", mrb_queue_initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, queue, "to_s", mrb_queue_to_s, MRB_ARGS_NONE());
}

void
mrb_queue_final(mrb_state *mrb)
{
}
