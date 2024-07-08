/*
   Copyright (c) 2024, MariaDB

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 of
   the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA
*/

#ifndef EXEC_PLAN_INCLUDED
#define EXEC_PLAN_INCLUDED

#include "my_global.h"
#include "my_dbug.h"

#include "rtti.h"

struct Update_execution_plan;
class handler;

struct Exec_plan
{
  union Update_func_ptr
  {
    RTTI::Method_intrn<Update_execution_plan>::as_func update;
    RTTI::Method_intrn<handler>::as_func handler;
  };

  typedef RTTI::Method_intrn<Update_execution_plan>::as_method Update_ptr_to_member;
  typedef RTTI::Method_intrn<handler>::as_method Handler_ptr_to_member;

  struct Callable
  {
    void *that;
    Update_func_ptr func;
    bool has_value() const { return func.handler != NULL; }
    int call(const uchar *old_rec, const uchar *new_rec) const
    { return func.handler((handler*)that, old_rec, new_rec); }
  };

  static constexpr uint MAX_UPDATE_FUNCS= 20;

  Callable call_list[MAX_UPDATE_FUNCS + 1] {}; // Initialize, so that the last cell will be NULL
  uint next= 0;

  // We don't care will it be built of handler or another class. Let's just convert everything to handler.
  using Method_intrn= RTTI::Method_intrn<handler>;

  void emplace(handler *h, Handler_ptr_to_member member_func, uint idx)
  {
    DBUG_ASSERT(idx < MAX_UPDATE_FUNCS);
    Update_func_ptr func;
    // Convert a pointer to member to a function pointer, and also fetch the correct pointer from vtable if needed.
    func.handler= Method_intrn::method_to_func(h, member_func);
    call_list[idx].that= h;
    call_list[idx].func= func;
  }

  uint add(handler *h, Handler_ptr_to_member member_func)
  {
    emplace(h, member_func, next);
    return next++;
  }

  void emplace(Update_execution_plan *up, Update_ptr_to_member member_func, uint idx)
  {
    union
    {
      Update_ptr_to_member update;
      Handler_ptr_to_member  handler;
    } member_conv;

    union
    {
      handler *handler;
      Update_execution_plan *update;
    } obj_conv;

    member_conv.update= member_func;
    obj_conv.update= up;

    emplace(obj_conv.handler, member_conv.handler, idx);
  }

  uint add(Update_execution_plan *up, Update_ptr_to_member member_func)
  {
    emplace(up, member_func, next);
    return next++;
  }
};

#endif // EXEC_PLAN_INCLUDED
