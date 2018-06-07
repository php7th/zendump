/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Youlin Feng <fengyoulin@php7th.com>                          |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_zendump.h"

ZEND_DECLARE_MODULE_GLOBALS(zendump)

void zendump_execute(zend_execute_data *ex)
{
    if(ex && ex->func && ex->func->common.function_name)
    {
        zend_string *name = ex->func->common.function_name;
        zend_string *file = NULL;
        if(ZEND_USER_CODE(ex->func->common.type))
        {
            file = ex->func->op_array.filename;
        }
        if(file)
        {
            zendump_errorf("%-30s%s:%d\n", ZSTR_VAL(name), file ? ZSTR_VAL(file) : "", ex->func->op_array.line_start);
        }
        else
        {
            zendump_errorf("%s\n", ZSTR_VAL(name));
        }
    }
    if(ZENDUMP_G(origin_execute))
    {
        ZENDUMP_G(origin_execute)(ex);
    }
}
