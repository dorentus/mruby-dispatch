#include "mruby_dispatch_gem.h"

static mrb_value
mrb_sema_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_int count_i;

  mrb_get_args(mrb, "i", &count_i);

  dispatch_semaphore_t sema = dispatch_semaphore_create(count_i);

  DATA_TYPE(self) = &mrb_dispatch_object_type;
  DATA_PTR(self) = sema;

  return self;
}

static mrb_value
mrb_sema_signal(mrb_state *mrb, mrb_value self)
{
  dispatch_semaphore_t sema = (dispatch_semaphore_t)DATA_PTR(self);

  long result = dispatch_semaphore_signal(sema);

  return mrb_bool_value(result == 0);
}

static mrb_value
mrb_sema_wait(mrb_state *mrb, mrb_value self)
{
  mrb_float delay_f;
  mrb_bool delay_f_p;
  dispatch_time_t timeout;
  dispatch_semaphore_t sema;

  mrb_get_args(mrb, "|f?", &delay_f, &delay_f_p);

  if (delay_f_p) {
    timeout = dispatch_time(DISPATCH_TIME_NOW, (dispatch_time_t)(delay_f * NSEC_PER_SEC));
  }
  else {
    timeout = DISPATCH_TIME_FOREVER;
  }

  sema = (dispatch_semaphore_t)DATA_PTR(self);
  long result = dispatch_semaphore_wait(sema, timeout);

  return mrb_bool_value(result == 0);
}

void
mrb_sema_init(mrb_state *mrb)
{
  struct RClass *dispatch_class, *object_class, *sema_class;

  dispatch_class = mrb_module_get(mrb, "Dispatch");
  object_class = mrb_class_get_under(mrb, dispatch_class, "Object");
  sema_class = mrb_define_class_under(mrb, dispatch_class, "Semaphore", object_class);

  mrb_define_method(mrb, sema_class, "initialize", mrb_sema_initialize, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, sema_class, "signal", mrb_sema_signal, MRB_ARGS_NONE());
  mrb_define_method(mrb, sema_class, "wait", mrb_sema_wait, MRB_ARGS_OPT(1));
}

void
mrb_sema_final(mrb_state *mrb)
{
}
