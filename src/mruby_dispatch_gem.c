#include "mruby_dispatch_gem.h"

static char const MRB_DISPATCH_ONCE_PRED_KEY[] = "$mrb_dispatch_once_pred";
static struct mrb_data_type mrb_dispatch_once_type = { MRB_DISPATCH_ONCE_PRED_KEY, mrb_free };

static void
mrb_dispatch_once_pred_init(mrb_state *mrb)
{
  dispatch_once_t *predicate_ptr;
  mrb_value self;

  self = mrb_obj_value(mrb->top_self);
  DATA_TYPE(self) = &mrb_dispatch_once_type;
  DATA_PTR(self) = NULL;

  predicate_ptr = (dispatch_once_t *)mrb_malloc(mrb, sizeof(dispatch_once_t));
  *predicate_ptr = 0;

  DATA_PTR(self) = predicate_ptr;
}

static dispatch_once_t *
mrb_dispatch_once_pred_ptr(mrb_state *mrb)
{
  dispatch_once_t *predicate_ptr;
  mrb_value self;

  self = mrb_obj_value(mrb->top_self);

  predicate_ptr = (dispatch_once_t *)DATA_PTR(self);

  return predicate_ptr;
}

static mrb_value
mrb_dispatch_once(mrb_state *mrb, mrb_value self)
{
  mrb_value blk;

  mrb_get_args(mrb, "&", &blk);

  DISPATCH_ENSURE_BLOCK_GIVEN(mrb, blk);

  dispatch_once(mrb_dispatch_once_pred_ptr(mrb), ^{
    mrb_yield(mrb, blk, mrb_nil_value());
  });

  return mrb_nil_value();
}

void
mrb_mruby_dispatch_gem_init(mrb_state *mrb)
{
  struct RClass *dispatch;

  dispatch = mrb_define_module(mrb, "Dispatch");

  mrb_dispatch_once_pred_init(mrb);

  mrb_define_const(mrb, dispatch, "TIME_NOW", mrb_fixnum_value(DISPATCH_TIME_NOW));
  mrb_define_const(mrb, dispatch, "TIME_FOREVER", mrb_fixnum_value(DISPATCH_TIME_FOREVER));

  mrb_define_class_method(mrb, dispatch, "once", mrb_dispatch_once, MRB_ARGS_NONE());

  mrb_object_init(mrb);
  mrb_group_init(mrb);
  mrb_queue_init(mrb);
  mrb_sema_init(mrb);
  mrb_source_init(mrb);
}

void
mrb_mruby_dispatch_gem_final(mrb_state *mrb)
{
  mrb_source_final(mrb);
  mrb_sema_final(mrb);
  mrb_queue_final(mrb);
  mrb_group_final(mrb);
  mrb_object_final(mrb);
}
