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

#ifndef PHP_ZENDUMP_H
#define PHP_ZENDUMP_H

extern zend_module_entry zendump_module_entry;
#define phpext_zendump_ptr &zendump_module_entry

#define PHP_ZENDUMP_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_ZENDUMP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ZENDUMP_API __attribute__ ((visibility("default")))
#else
#	define PHP_ZENDUMP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#if SIZEOF_SIZE_T != 8
# error "Currently only supported on 64-bit platform"
#endif

#ifdef ZEND_ENABLE_ZVAL_LONG64
# define ZEND_XLONG_FMT_SPEC PRIx64
#else
# define ZEND_XLONG_FMT_SPEC PRIx32
#endif

/*
 * For PHP 7.0
 */
#if !defined(ZEND_XLONG_FMT)
# ifdef ZEND_ENABLE_ZVAL_LONG64
#  define ZEND_XLONG_FMT "%" PRIx64
# else
#  define ZEND_XLONG_FMT "%" PRIx32
# endif
#endif

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

#define EX_OFFSET_TO_VAR_IDX(offset) ((zval*)((zend_long)offset - sizeof(zend_execute_data)) - (zval*)0)

#define INDENT_SIZE 2

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(zendump)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(zendump)
*/

/* Always refer to the globals in your function as ZENDUMP_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define ZENDUMP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(zendump, v)

#if defined(ZTS) && defined(COMPILE_DL_ZENDUMP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

const char *zendump_get_type_name(uint32_t type);

zend_string *zendump_unescape_zend_string(zend_string *org, int persistent);

void zendump_zval_dump(zval *val, int level);
void zendump_zend_array_dump(zend_array *arr, int level);

void zendump_zend_internal_function_proto_dump(zend_internal_function *internal_function, int level);
void zendump_zend_op_array_proto_dump(zend_op_array *op_array, int level);
void zendump_zend_internal_function_dump(zend_internal_function *internal_function);
void zendump_zend_op_array_dump(zend_op_array *op_array, int column_width, int show_internal_operand);
void zendump_zend_function_dump(zend_function *function, int column_width, int show_internal_operand);

void zendump_zend_class_entry_dump(zend_class_entry *ce, int show_magic_functions, int column_width, int show_internal_operand);
void zendump_static_properties_dump(zend_class_entry *ce, int level);
void zendump_properties_dump(zend_object *obj, int level);

#endif	/* PHP_ZENDUMP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
