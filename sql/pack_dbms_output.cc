#include "sql_plugin.h"
#include "sql_class.h"


Pack_dbms_output::Pack_dbms_output() : m_buffer(PSI_INSTRUMENT_MEM)
{
}

void Pack_dbms_output::clear_buffer()
{
  size_t size= m_buffer.elements();
  if (size > 0)
  {
    char *tmp;
    while (size--)
    {
      tmp= m_buffer.pop();
      free(tmp);
    }
    tmp= NULL;
  }
}

void Pack_dbms_output::put_line_start()
{
  clear_buffer();
}

// void Pack_dbms_output::put_line(const char *str)
void Pack_dbms_output::put_line(THD *thd, const String *str)
{
//   char *stroot = strmake_root(thd->mem_root, str->ptr(), str->length());
//   m_buffer.append(stroot);
//   char *tmp = m_buffer.at(0);
//   printf("ks tmp = %s\n", tmp);
//   printf("ks tmp = %s\n", tmp);

  size_t len= str->length();
  char *strmalloc= (char*) malloc(len + 1);

  if (strmalloc)
  {
    strncpy(strmalloc, str->ptr(), len);
    strmalloc[len]= 0;
    m_buffer.append(strmalloc);
  }
}

void Pack_dbms_output::put_line_end(THD *thd)
{
  size_t size= m_buffer.elements();
  if (size > 0)
  {
    char *output;
    size_t lengths = 0;

    if (thd->is_error() == false)
    {
      thd->get_stmt_da()->reset_diagnostics_area();

      for(size_t i= 0; i< size; i++)
      {
        lengths++;
        lengths += strlen(m_buffer.at(i));
      }

      output = (char *) calloc( lengths + 1, sizeof(char) );
      if (output)
      {
        for(size_t i= 0; i< size; i++)
        {
          strcat(output, "\n");
          strcat(output, m_buffer.at(i));
        }

        my_message(ER_FOR_PACK_DBMS_OUTPUT, output, MYF(0));
        free(output);
        output= NULL;
      }
    }

    clear_buffer();
  }
}
