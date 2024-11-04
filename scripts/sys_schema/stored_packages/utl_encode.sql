-- Copyright (c) 2014, 2015, Oracle and/or its affiliates. All rights reserved.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; version 2 of the License.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

DROP PACKAGE IF EXISTS UTL_ENCODE;

DELIMITER $$

CREATE DEFINER='mariadb.sys'@'localhost' PACKAGE UTL_ENCODE
    COMMENT '
             Description
             -----------
             UTL_ENCODE package
            '
    SQL SECURITY INVOKER
        FUNCTION BASE64_DECODE(r VARBINARY(4000))
            RETURNS VARBINARY(4000)
            COMMENT '
                     Description
                     -----------
                     Decodes the given base-64 encode string, returning the result as a binary string.

                     Returns
                     -----------

                     VARBINARY

                     Example
                     -----------

                     MariaDB [(none)]> SELECT sys.UTL_ENCODE.BASE64_DECODE(''TWFyaWFEQg=='');
                     +------------------------------------------------+
                     | sys.UTL_ENCODE.BASE64_DECODE(''TWFyaWFEQg=='') |
                     +------------------------------------------------+
                     | MariaDB                                        |
                     +------------------------------------------------+
                     1 row in set (0.001 sec)
                    '
            SQL SECURITY INVOKER
            DETERMINISTIC
            NO SQL
            ;
END$$



CREATE DEFINER='mariadb.sys'@'localhost' PACKAGE BODY UTL_ENCODE
    COMMENT '
             Description
             -----------
             UTL_ENCODE package body
            '
    SQL SECURITY INVOKER
        FUNCTION BASE64_DECODE(r VARBINARY(4000))
            RETURNS VARBINARY(4000)
            COMMENT '
                     Description
                     -----------
                     Decodes the given base-64 encode string, returning the result as a binary string.

                     Returns
                     -----------

                     VARBINARY

                     Example
                     -----------

                     MariaDB [(none)]> SELECT sys.UTL_ENCODE.BASE64_DECODE(''TWFyaWFEQg=='');
                     +------------------------------------------------+
                     | sys.UTL_ENCODE.BASE64_DECODE(''TWFyaWFEQg=='') |
                     +------------------------------------------------+
                     | MariaDB                                        |
                     +------------------------------------------------+
                     1 row in set (0.001 sec)
                    '
            SQL SECURITY INVOKER
            DETERMINISTIC
            NO SQL
        BEGIN
            RETURN FROM_BASE64(r);
        END;
END$$

DELIMITER ;
