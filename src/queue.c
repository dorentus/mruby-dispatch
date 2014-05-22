#include "mruby_dispatch_gem.h"

#define DISPATCH_QUEUE_DEFUN(FUNNAME1, FUNNAME2) \
  static mrb_value \
  FUNNAME1(mrb_state *mrb, mrb_value self) \
  { \
    mrb_value blk; \
    dispatch_queue_t q; \
   \
    mrb_get_args(mrb, "&", &blk); \
   \
    DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk); \
   \
    q = (dispatch_queue_t)DATA_PTR(self); \
   \
    FUNNAME2(q, ^{ \
      mrb_yield(mrb, blk, mrb_nil_value()); \
    }); \
   \
    return mrb_nil_value(); \
  }

static mrb_value
mrb_queue_create_instance(mrb_state *mrb)
{
  struct RClass *dispatch_class, *queue_class;

  dispatch_class = mrb_module_get(mrb, "Dispatch");
  queue_class = mrb_class_get_under(mrb, dispatch_class, "Queue");

  return mrb_obj_value(mrb_obj_alloc(mrb, MRB_TT_OBJECT, queue_class));
}

static mrb_value
mrb_queue_set_queue(mrb_value self, dispatch_queue_t q)
{
  dispatch_queue_t old_q;

  old_q = (dispatch_queue_t)DATA_PTR(self);
  if (old_q) {
    dispatch_release(old_q);
  }

  DATA_TYPE(self) = &mrb_dispatch_object_type;
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

  instance = mrb_queue_create_instance(mrb);
  q = dispatch_get_current_queue();

  return mrb_queue_set_queue(instance, q);
}

static mrb_value
mrb_queue_main(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;
  dispatch_queue_t q;

  instance = mrb_queue_create_instance(mrb);
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

  instance = mrb_queue_create_instance(mrb);

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

  DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk);

  q = (dispatch_queue_t)DATA_PTR(self);

  dispatch_time_t pop_time = dispatch_time(DISPATCH_TIME_NOW, (dispatch_time_t)(delay_f * NSEC_PER_SEC));
  dispatch_after(pop_time, q, ^{
    mrb_yield(mrb, blk, mrb_nil_value());
  });

  return mrb_nil_value();
}

static mrb_value
mrb_queue_apply(mrb_state *mrb, mrb_value self)
{
  mrb_int iterations_i;
  mrb_value blk;
  dispatch_queue_t q;

  mrb_get_args(mrb, "i&", &iterations_i, &blk);

  DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk);

  q = (dispatch_queue_t)DATA_PTR(self);

  dispatch_apply(iterations_i, q, ^(size_t idx){
    mrb_yield(mrb, blk, mrb_fixnum_value((mrb_int)idx));
  });

  return mrb_nil_value();
}

static mrb_value
mrb_queue_async(mrb_state *mrb, mrb_value self)
{
  mrb_value blk;
  mrb_value group;
  mrb_bool group_p;
  dispatch_queue_t q;

  mrb_get_args(mrb, "&|o?", &blk, &group, &group_p);

  DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk);

  q = (dispatch_queue_t)DATA_PTR(self);

  dispatch_block_t block = ^{
    mrb_yield(mrb, blk, mrb_nil_value());
  };

  if (group_p) {
    struct RClass *dispatch_class = mrb_module_get(mrb, "Dispatch");
    struct RClass *group_class = mrb_class_get_under(mrb, dispatch_class, "Group");
    DISPATCH_ENSURE_PARAM_IS_KIND_OF(mrb, group, group_class);

    dispatch_group_t g = DISPATCH_GET_OBJECT(mrb, group, dispatch_group_t);

    dispatch_group_async(g, q, block);
  }
  else {
    dispatch_async(q, block);
  }

  return mrb_nil_value();
}


DISPATCH_QUEUE_DEFUN(mrb_queue_sync, dispatch_sync);

#if defined(__MAC_10_7) || defined(__IPHONE_4_3)
DISPATCH_QUEUE_DEFUN(mrb_queue_barrier_async, dispatch_barrier_async);
DISPATCH_QUEUE_DEFUN(mrb_queue_barrier_sync, dispatch_barrier_sync);
#endif

void
mrb_queue_init(mrb_state *mrb)
{
  struct RClass *dispatch_class, *object_class, *queue_class;

  dispatch_class = mrb_module_get(mrb, "Dispatch");
  object_class = mrb_class_get_under(mrb, dispatch_class, "Object");
  queue_class = mrb_define_class_under(mrb, dispatch_class, "Queue", object_class);

  mrb_define_class_method(mrb, queue_class, "current", mrb_queue_current, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, queue_class, "main", mrb_queue_main, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, queue_class, "concurrent", mrb_queue_concurrent, MRB_ARGS_OPT(1));

  mrb_define_method(mrb, queue_class, "initialize", mrb_queue_initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, queue_class, "to_s", mrb_queue_to_s, MRB_ARGS_NONE());
  mrb_define_method(mrb, queue_class, "after", mrb_queue_after, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, queue_class, "apply", mrb_queue_apply, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, queue_class, "async", mrb_queue_async, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, queue_class, "sync", mrb_queue_sync, MRB_ARGS_NONE());

#if defined(__MAC_10_7) || defined(__IPHONE_4_3)
  mrb_define_method(mrb, queue_class, "barrier_async", mrb_queue_barrier_async, MRB_ARGS_NONE());
  mrb_define_method(mrb, queue_class, "barrier_sync", mrb_queue_barrier_sync, MRB_ARGS_NONE());
#endif
}

void
mrb_queue_final(mrb_state *mrb)
{
}
