#ifndef PACK_DBMS_OUTPUT_INCLUDED
#define PACK_DBMS_OUTPUT_INCLUDED

#include "mysqld.h"
#include "sql_array.h"

class Pack_dbms_output
{
private:
  Dynamic_array<char *> m_buffer;
  void clear_buffer();

public:
  Pack_dbms_output();
  // void put_line(const char *);
  void put_line_start();
  void put_line(THD *thd, const String *);
  void put_line_end(THD *thd);
};

#endif /* PACK_DBMS_OUTPUT_INCLUDED */
