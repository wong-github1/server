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

#ifndef RTTI_H_INCLUDED
#define RTTI_H_INCLUDED

#include "my_global.h"
#include "my_dbug.h"

#include <stdint.h>

namespace RTTI
{

template<typename T>
struct Method_intrn
{
  typedef int (T::*as_method)(const uchar*, const uchar*);
  typedef int (*as_func)(T*, const uchar*, const uchar*);

  union Ptr_to_member_representation
  {
    as_method member;
    struct
    {
      uintptr_t ptr;
      uintptr_t call_chain_stack;
    } s;
  };

  static
  as_method fetch_from_vtable(T *obj, as_method member)
  {
    Ptr_to_member_representation u;
    u.member= member;

    // Can't convert method of class nested into a function!
    DBUG_ASSERT(u.s.call_chain_stack == 0);

    if ((u.s.ptr & 1) == 1) // Virtual. Fetch the real function from vtable
    {
      char *vtable= *(char**)obj;
      uintptr_t func= *(uintptr_t*)&vtable[u.s.ptr-1];

      u.s.ptr= func;
    }

    return u.member;
  }

  static as_func method_to_func(T *obj, as_method member)
  {
    Ptr_to_member_representation u;
    u.member= fetch_from_vtable(obj, member);
    return (as_func) u.s.ptr;
  }
};

};

#endif // RTTI_H_INCLUDED
