/* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
   Copyright (c) 2016, 2020, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */


#include "sql_plugin.h"
// #include "table.h"
// #include "rpl_gtid.h"
// #include "sql_class.h"
// #include "sql_show.h"
// #include "set_var.h"
#include "sql_path.h"


Sql_path::Sql_path() : db_list(PSI_INSTRUMENT_MEM) // kokseng
{
}

void Sql_path::append_db(char *in)
{
  char *tmp = my_strdup(key_memory_Sys_var_charptr_value,
                              in, MYF(MY_WME | MY_THREAD_SPECIFIC));
  LEX_CSTRING db = {tmp, strlen(tmp)};
  db_list.append(db);
}

void Sql_path::strtok_db(char *in)
{
  char *tmp;
  int len;
  const char *curr;
  const char *token;
  const char *end;
  CHARSET_INFO *cs;
  LEX_CSTRING db;

  curr = token = in;
  end = curr + strlen(curr);
  cs = system_charset_info_for_i_s;

  while (curr < end)
  {
    len = my_ismbchar(cs, curr, end - 1);
    if (len)
    {
      curr += len;
      if (curr < end)
        continue;
    }

    if (*curr == ',' || curr >= end || end - 1 == curr)
    {
      if (end - 1 == curr)
        curr++;

      tmp = my_strndup(key_memory_Sys_var_charptr_value,
                       token,
                       curr - token,
                       MYF(MY_WME | MY_THREAD_SPECIFIC));
      db = {tmp, (size_t)(curr - token)};
      db_list.append(db);

      if (curr < end)
      {
        curr++;
        token = curr;
      }
    }
    else
      curr++;
  }
}

void Sql_path::free_db_list()
{
  for(size_t i= 0; i < db_list.elements(); i++)
  {
    my_free((char*)db_list.at(i).str);
  }
  db_list.free_memory();
} 
