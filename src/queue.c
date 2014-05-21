#include "mruby_dispatch_gem.h"

static void
mrb_dispatch_queue_release(mrb_state *mrb, void *data)
{
  dispatch_release((dispatch_queue_t)data);
}

static struct mrb_data_type mrb_dispatch_queue_type = { "$mrb_dispatch_queue", mrb_dispatch_queue_release };

static mrb_value
mrb_queue_set_queue(mrb_value self, dispatch_queue_t q)
{
  dispatch_queue_t old_q;

  old_q = (dispatch_queue_t)DATA_PTR(self);
  if (old_q) {
    dispatch_release(old_q);
  }

  DATA_TYPE(self) = &mrb_dispatch_queue_type;
  DATA_PTR(self) = q;

  return self;
}

static mrb_value
mrb_queue_initialize(mrb_state *mrb, mrb_value self)
{
  const char *label_str = NULL;
  dispatch_queue_t q;

  mrb_get_args(mrb, "|z", &label_str);

  q = dispatch_queue_create(label_str, NULL);

  return mrb_queue_set_queue(self, q);
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

static mrb_value
mrb_queue_current(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;
  dispatch_queue_t q;
  struct RClass *dispatch, *queue;

  dispatch = mrb_module_get(mrb, "Dispatch");
  queue = mrb_class_get_under(mrb, dispatch, "Queue");

  instance = mrb_obj_new(mrb, queue, 0, NULL);
  q = dispatch_get_current_queue();

  return mrb_queue_set_queue(instance, q);
}

static mrb_value
mrb_queue_main(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;
  dispatch_queue_t q;
  struct RClass *dispatch, *queue;

  dispatch = mrb_module_get(mrb, "Dispatch");
  queue = mrb_class_get_under(mrb, dispatch, "Queue");

  instance = mrb_obj_new(mrb, queue, 0, NULL);
  q = dispatch_get_main_queue();

  return mrb_queue_set_queue(instance, q);
}

static mrb_value
mrb_queue_concurrent(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;
  mrb_sym priority;
  long priority_value;
  dispatch_queue_t q;
  struct RClass *dispatch, *queue;

  mrb_get_args(mrb, "|n", &priority);
  if (priority == mrb_intern_cstr(mrb, "high")) {
    priority_value = DISPATCH_QUEUE_PRIORITY_HIGH;
  }
  else if (priority == mrb_intern_cstr(mrb, "low")) {
    priority_value = DISPATCH_QUEUE_PRIORITY_LOW;
  }
#ifdef DISPATCH_QUEUE_PRIORITY_BACKGROUND
  else if (priority == mrb_intern_cstr(mrb, "background")) {
    priority_value = DISPATCH_QUEUE_PRIORITY_BACKGROUND;
  }
#endif
  else {
    priority_value = DISPATCH_QUEUE_PRIORITY_DEFAULT;
  }

  dispatch = mrb_module_get(mrb, "Dispatch");
  queue = mrb_class_get_under(mrb, dispatch, "Queue");

  instance = mrb_obj_new(mrb, queue, 0, NULL);

  q = dispatch_get_global_queue(priority_value, 0);

  return mrb_queue_set_queue(instance, q);
}

static mrb_value
mrb_queue_after(mrb_state *mrb, mrb_value self)
{
  mrb_float delay_f;
  mrb_value blk;
  dispatch_queue_t q;

  mrb_get_args(mrb, "f&", &delay_f, &blk);

  if (mrb_nil_p(blk)) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "no block given");
  }

  q = (dispatch_queue_t)DATA_PTR(self);

  const char *label_str = dispatch_queue_get_label(q);

  dispatch_time_t pop_time = dispatch_time(DISPATCH_TIME_NOW, (dispatch_time_t)(delay_f * NSEC_PER_SEC));
  dispatch_after(pop_time, q, ^{
    mrb_yield(mrb, blk, mrb_nil_value());
  });

  return mrb_nil_value();
}

void
mrb_queue_init(mrb_state *mrb)
{
  struct RClass *dispatch, *queue;

  dispatch = mrb_module_get(mrb, "Dispatch");
  queue = mrb_define_class_under(mrb, dispatch, "Queue", mrb->object_class);

  mrb_define_class_method(mrb, queue, "current", mrb_queue_current, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, queue, "main", mrb_queue_main, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, queue, "concurrent", mrb_queue_concurrent, MRB_ARGS_OPT(1));

  mrb_define_method(mrb, queue, "initialize", mrb_queue_initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, queue, "to_s", mrb_queue_to_s, MRB_ARGS_NONE());
  mrb_define_method(mrb, queue, "after", mrb_queue_after, MRB_ARGS_REQ(1));
}

void
mrb_queue_final(mrb_state *mrb)
{
}
