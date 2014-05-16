#include "mruby.h"
#include <dispatch/dispatch.h>

void mrb_mruby_dispatch_gem_init(mrb_state *mrb)
{
  mrb_define_module(mrb, "Dispatch");
}

void mrb_mruby_dispatch_gem_final(mrb_state *mrb)
{

}
