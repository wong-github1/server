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

DROP PACKAGE IF EXISTS DBMS_OUTPUT;

SET @old_sql_mode = @@session.sql_mode, @@session.sql_mode = ORACLE;

DELIMITER $$

CREATE DEFINER='mariadb.sys'@'localhost' PACKAGE DBMS_OUTPUT
    COMMENT '
             Description
             -----------
             DBMS_OUTPUT package
            '
    SQL SECURITY INVOKER
    AS
        
        PROCEDURE ENABLE
            COMMENT '
                     Description
                     -----------
                     Activate the package.
                    '
            SQL SECURITY INVOKER
            NO SQL
            ;

        PROCEDURE PUT_LINE(item VARCHAR2)
            COMMENT '
                     Description
                     -----------
                     Place a line in the buffer.
                    '
            SQL SECURITY INVOKER
            NO SQL
            ;
END$$



CREATE DEFINER='mariadb.sys'@'localhost' PACKAGE BODY DBMS_OUTPUT
    COMMENT '
             Description
             -----------
             DBMS_OUTPUT package body
            '
    SQL SECURITY INVOKER
    AS
        f_enable bool:= false;

        PROCEDURE ENABLE
            COMMENT '
                     Description
                     -----------
                     Activate the package.
                    '
            SQL SECURITY INVOKER
            NO SQL
            AS
        BEGIN
            f_enable := true;
        END;

        PROCEDURE PUT_LINE(item VARCHAR2)
            COMMENT '
                     Description
                     -----------
                     Place a line in the buffer.
                    '
            SQL SECURITY INVOKER
            NO SQL
            AS
        BEGIN
            IF f_enable THEN
                SET @res = DPUT_LINE(item);
            END IF;
        END;
END$$

DELIMITER ;

SET @@session.sql_mode = @old_sql_mode;
