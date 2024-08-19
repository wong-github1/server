/*
   Copyright (c) 2024, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA
*/
#include <string>
#include <vector>

/* TABLE DDL INFO - representation of parsed CREATE TABLE Statement */

enum class KeyOrConstraintType
{
  PRIMARY_KEY,
  UNIQUE_KEY,
  FOREIGN_KEY,
  CONSTRAINT,
  INDEX,
  KEY,
  UNKNOWN
};

/**
 *  Struct representing a table key definition
 */
struct KeyDefinition
{
  KeyOrConstraintType type; /* e.g PRIMARY_KEY, UNIQUE_KEY */
  std::string definition;   /* The full key or constraint definition string, e.g
                                UNIQUE KEY `uniq_idx` (`col`)
                             */
  std::string name;         /*The name of key or constraint */
};

/**
   Extracts of index information from a database table.
 */
class TableDDLInfo
{
public:
  TableDDLInfo(const std::string &create_table_stmt);

  KeyDefinition primary_key;
  std::vector<KeyDefinition> constraints;
  std::vector<KeyDefinition> secondary_indexes;
  std::string storage_engine;
  std::string table_name;
  std::string
  generate_alter_add(const std::vector<KeyDefinition> &definitions) const;

  std::string
  generate_alter_drop(const std::vector<KeyDefinition> &definitions) const;

public:
 
  std::string drop_constraints_sql() const
  {
    return generate_alter_drop(constraints);
  }
  std::string create_constraints_sql() const
  {
    return generate_alter_add(constraints);
  }
  std::string drop_secondary_indexes_sql() const
  {
    return generate_alter_drop(secondary_indexes);
  }
  std::string create_secondary_indexes_sql() const
  {
    return generate_alter_add(secondary_indexes);
  }
};
std::string extract_first_create_table(const std::string &script);
