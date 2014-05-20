#ifndef MRUBY_DISPATCH_GEM_H
#define MRUBY_DISPATCH_GEM_H

#include <dispatch/dispatch.h>
#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>

void mrb_queue_init(mrb_state *mrb);
void mrb_queue_final(mrb_state *mrb);

#endif
