#ifndef MRUBY_DISPATCH_GEM_H
#define MRUBY_DISPATCH_GEM_H

#include <dispatch/dispatch.h>
#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>

void mrb_object_init(mrb_state *mrb);
void mrb_object_final(mrb_state *mrb);
void mrb_queue_init(mrb_state *mrb);
void mrb_queue_final(mrb_state *mrb);

#define DISPATCH_ENSURE_BLOCK_GIVEN(blk) do { \
        if (mrb_nil_p((blk))) { \
          mrb_raise(mrb, E_ARGUMENT_ERROR, "no block given"); \
        } } while (0)

void mrb_dispatch_release(mrb_state *mrb, void *data);
static struct mrb_data_type mrb_dispatch_object_type = { "$mrb_dispatch_object", mrb_dispatch_release };

#endif
