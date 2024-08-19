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

/*
  This file contains some routines to do client-side parsing of CREATE TABLE
  statements. The goal is to extract the primary key, constraints, and
  secondary key. his is useful for optimizing the import process, to delay
  secondary index creation until after the data has been loaded.
*/

#include <string>
#include <vector>
#include <regex>

#include "import_util.h"
/**
 * Extract the first CREATE TABLE statement from a script.
 *
 * @param script The input script containing SQL statements.
 * @return std::string The first CREATE TABLE statement found, or an empty
 * string if not found.
 */
std::string extract_first_create_table(const std::string &script)
{
  std::regex create_table_regex(R"((CREATE\s+TABLE\s+[^;]+;)\s*\n)",
                                std::regex::icase);
  std::smatch match;
  if (std::regex_search(script, match, create_table_regex))
  {
    return match[1];
  }
  return "";
}

TableDDLInfo::TableDDLInfo(const std::string &create_table_stmt)
{
  std::regex primary_key_regex(R"(^\s*(PRIMARY\s+KEY\s+(.*?)),?\n)",
                               std::regex::icase);

  std::regex constraint_regex(
      R"(^\s*(CONSTRAINT\s+(`?(?:[^`]|``)+`?)\s+.*?),?\n)", std::regex::icase);

  std::regex index_regex(
      R"(^\s*((UNIQUE\s+)?(INDEX|KEY)\s+(`?(?:[^`]|``)+`?)\s+.*?),?\n)",
      std::regex::icase);

  std::regex engine_regex(R"(\bENGINE\s*=\s*(\w+))", std::regex::icase);
  std::smatch match;
  auto search_start= create_table_stmt.cbegin();

  // Extract primary key
  if (std::regex_search(search_start, create_table_stmt.cend(), match,
                        primary_key_regex))
  {
    primary_key= {KeyOrConstraintType::PRIMARY_KEY, match[0], match[2]};
    search_start= match.suffix().first;
  }

  // Extract constraints and foreign keys
  search_start= create_table_stmt.cbegin();
  while (std::regex_search(search_start, create_table_stmt.cend(), match,
                           constraint_regex))
  {
    auto type= KeyOrConstraintType::CONSTRAINT;
    auto name= match[2].matched ? match[2].str() : "";
    auto definition= match[1];
    constraints.push_back({type, definition, name});
    search_start= match.suffix().first;
  }

  // Extract secondary indexes
  search_start= create_table_stmt.cbegin();
  while (std::regex_search(search_start, create_table_stmt.cend(), match,
                           index_regex))
  {
    auto type= KeyOrConstraintType::INDEX;
    auto name= match[4].matched ? match[4].str() : "";
    auto definition= match[1];
    secondary_indexes.push_back({type, definition, name});
    search_start= match.suffix().first;
  }

  // Extract storage engine
  if (std::regex_search(create_table_stmt, match, engine_regex))
  {
    storage_engine= match[1];
  }

  std::regex table_name_regex(R"(CREATE\s+TABLE\s+(`?(?:[^`]|``)+`?)\s*\()",
                              std::regex::icase);
  std::smatch table_name_match;
  if (std::regex_search(create_table_stmt, table_name_match, table_name_regex))
  {
    table_name= table_name_match[1];
  }
}

/**
 Convert a KeyOrConstraintDefinitionType enum value to its
 corresponding string representation.

 @param type The KeyOrConstraintDefinitionType enum value.
 @return std::string The string representation of the
  KeyOrConstraintDefinitionType.
*/
static std::string definition_type_to_string(KeyOrConstraintType type)
{
  switch (type)
  {
  case KeyOrConstraintType::PRIMARY_KEY:
    return "PRIMARY KEY";
  case KeyOrConstraintType::UNIQUE_KEY:
    return "UNIQUE KEY";
  case KeyOrConstraintType::FOREIGN_KEY:
    return "FOREIGN KEY";
  case KeyOrConstraintType::CONSTRAINT:
    return "CONSTRAINT";
  case KeyOrConstraintType::INDEX:
    return "INDEX";
  case KeyOrConstraintType::KEY:
    return "KEY";
  default:
    return "UNKNOWN";
  }
}

std::string TableDDLInfo::generate_alter_add(
    const std::vector<KeyDefinition> &definitions) const
{
  if (definitions.empty())
  {
    return "";
  }

  std::string sql= "ALTER TABLE " + table_name + " ";
  bool need_comma= false;
  for (const auto &definition : definitions)
  {
    if (need_comma)
      sql+= ", ";
    else
      need_comma= true;
    sql+= "ADD " + definition.definition;
  }
  return sql;
}

std::string TableDDLInfo::generate_alter_drop(
    const std::vector<KeyDefinition> &definitions) const
{
  if (definitions.empty())
  {
    return "";
  }

  std::string sql= "ALTER TABLE " + table_name + " ";
  bool need_comma= false;
  for (const auto &definition : definitions)
  {
    if (need_comma)
      sql+= ", ";
    else
      need_comma= true;
    sql+= "DROP " + definition_type_to_string(definition.type) + " " +
          definition.name;
  }
  return sql;
}

#ifdef MAIN
int main()
{
  std::string script= R"(
        -- Some SQL script
         CREATE TABLE `book` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `author_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_book_author` (`author_id`),
  CONSTRAINT `fk_book_author` FOREIGN KEY (`author_id`) REFERENCES `author` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
    )";

  auto create_table_stmt= extract_first_create_table(script);
  if (create_table_stmt.empty())
  {
    std::cerr << "No CREATE TABLE statement found in the script." << std::endl;
    return 1;
  }

  TableDDLInfo table_definitions(create_table_stmt);

  std::cout << "Primary Key:" << std::endl;
  std::cout << "Type: "
            << definition_type_to_string(table_definitions.primary_key.type)
            << std::endl;
  std::cout << "Definition: " << table_definitions.primary_key.definition
            << std::endl;
  std::cout << "Name: " << table_definitions.primary_key.name << std::endl;

  std::cout << "\nConstraints and Foreign Keys:" << std::endl;
  for (const auto &entry : table_definitions.constraints)
  {
    std::cout << "Type: " << definition_type_to_string(entry.type)
              << std::endl;
    std::cout << "Definition: " << entry.definition << std::endl;
    std::cout << "Name: " << entry.name << std::endl;
  }

  std::cout << "\nSecondary Indexes:" << std::endl;
  for (const auto &entry : table_definitions.secondary_indexes)
  {
    std::cout << "Type: " << definition_type_to_string(entry.type)
              << std::endl;
    std::cout << "Definition: " << entry.definition << std::endl;
    std::cout << "Name: " << entry.name << std::endl;
  }

  std::cout << "\nStorage Engine: " << table_definitions.storage_engine
            << std::endl;

  std::cout << "\nTable Name: " << table_definitions.table_name << std::endl;
  std::cout << "\nDrop Constraints SQL: "
            << table_definitions.drop_constraints_sql() << std::endl;
  std::cout << "\nCreate Constraints SQL: "
            << table_definitions.create_constraints_sql() << std::endl;
  std::cout << "\nDrop Indexes SQL: "
            << table_definitions.drop_secondary_indexes_sql() << std::endl;
  std::cout << "\nCreate Indexes SQL: "
            << table_definitions.create_secondary_indexes_sql() << std::endl;
  return 0;
}
#endif
