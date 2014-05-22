#include "mruby_dispatch_gem.h"

static mrb_value
mrb_group_initialize(mrb_state *mrb, mrb_value self)
{
  dispatch_group_t g;

  g = dispatch_group_create();

  DATA_TYPE(self) = &mrb_dispatch_object_type;
  DATA_PTR(self) = g;

  return self;
}

static mrb_value
mrb_group_notify(mrb_state *mrb, mrb_value self)
{
  struct RClass *dispatch_class, *queue_class;
  mrb_value queue, blk;
  dispatch_group_t g;
  dispatch_queue_t q;

  dispatch_class = mrb_module_get(mrb, "Dispatch");
  queue_class = mrb_class_get_under(mrb, dispatch_class, "Queue");

  mrb_get_args(mrb, "o&", &queue, &blk);

  DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk);
  DISPATCH_ENSURE_PARAM_IS_KIND_OF(mrb, queue, queue_class);

  g = (dispatch_group_t)DATA_PTR(self);
  q = DISPATCH_GET_OBJECT(mrb, queue, dispatch_queue_t);

  dispatch_group_notify(g, q, ^{
    mrb_yield(mrb, blk, mrb_nil_value());
  });

  return mrb_nil_value();
}

static mrb_value
mrb_group_wait(mrb_state *mrb, mrb_value self)
{
  mrb_float delay_f = 0;
  mrb_value blk;
  dispatch_group_t g;

  mrb_get_args(mrb, "|f&", &delay_f, &blk);

  DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk);

  g = (dispatch_group_t)DATA_PTR(self);

  dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, (dispatch_time_t)(delay_f * NSEC_PER_SEC));
  long result = dispatch_group_wait(g, timeout);

  return mrb_bool_value(result == 0);
}

void
mrb_group_init(mrb_state *mrb)
{
  struct RClass *dispatch_class, *object_class, *group_class;

  dispatch_class = mrb_module_get(mrb, "Dispatch");
  object_class = mrb_class_get_under(mrb, dispatch_class, "Object");
  group_class = mrb_define_class_under(mrb, dispatch_class, "Group", object_class);

  mrb_define_method(mrb, group_class, "initialize", mrb_group_initialize, MRB_ARGS_NONE());
  mrb_define_method(mrb, group_class, "notify", mrb_group_notify, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, group_class, "wait", mrb_group_wait, MRB_ARGS_OPT(1));
}

void
mrb_group_final(mrb_state *mrb)
{
}
