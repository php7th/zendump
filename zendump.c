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

/* If you declare any globals in php_zendump.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(zendump)
*/

/* True global resources - no need for thread safety here */
// static int le_zendump;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("zendump.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_zendump_globals, zendump_globals)
	STD_PHP_INI_ENTRY("zendump.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_zendump_globals, zendump_globals)
PHP_INI_END()
*/
/* }}} */

zend_string *unescape_zend_string(zend_string *org, int persistent)
{
	zend_string *str = org;
	int nrt0 = 0;
	int idx;
	for(idx = 0; idx < ZSTR_LEN(str); ++idx) {
		if('\n' == ZSTR_VAL(str)[idx] || '\r' == ZSTR_VAL(str)[idx] || '\t' == ZSTR_VAL(str)[idx] || '\0' == ZSTR_VAL(str)[idx]) {
			++nrt0;
		}
	}
	if(nrt0) {
		str = zend_string_dup(str, 0);
		str = zend_string_extend(str, ZSTR_LEN(str), 0);
		ZSTR_LEN(str) = ZSTR_LEN(org) + nrt0;
		const char *src = ZSTR_VAL(org);
		char *dst = ZSTR_VAL(str);
		for(idx = 0; idx < ZSTR_LEN(org); ++idx) {
			if('\n' == src[idx]) {
				*dst++ = '\\';
				*dst++ = 'n';
			} else if('\r' == src[idx]) {
				*dst++ = '\\';
				*dst++ = 'r';
			} else if('\t' == src[idx]) {
				*dst++ = '\\';
				*dst++ = 't';
			} else if('\0' == src[idx]) {
				*dst++ = '\\';
				*dst++ = '0';
			} else {
				*dst++ = src[idx];
			}
		}
	}
	return str;
}

#if ZEND_DEBUG
void zendump_zval_type(zval *val)
{
	switch(Z_TYPE_P(val)) {
		case IS_UNDEF:
			PHPWRITE("undefined\n", 10);
			break;
		case IS_NULL:
			PHPWRITE("null\n", 5);
			break;
		case IS_FALSE:
			PHPWRITE("false\n", 6);
			break;
		case IS_TRUE:
			PHPWRITE("true\n", 5);
			break;
		case IS_LONG:
			PHPWRITE("long\n", 5);
			break;
		case IS_DOUBLE:
			PHPWRITE("double\n", 7);
			break;
		case IS_STRING:
			PHPWRITE("string\n", 7);
			break;
		case IS_ARRAY:
			PHPWRITE("array\n", 6);
			break;
		case IS_OBJECT:
			PHPWRITE("object\n", 7);
			break;
		case IS_RESOURCE:
			PHPWRITE("resource\n", 9);
			break;
		case IS_REFERENCE:
			PHPWRITE("reference\n", 10);
			break;
		case IS_INDIRECT:
			zendump_zval_type(Z_INDIRECT_P(val));
			break;
		default:
			php_printf("unknown: %d\n", Z_TYPE_P(val));
			break;
	}
}
#endif

void zendump_zval_dump(zval *val, int level)
{
	if(level > 0) {
		php_printf("%*c", level, ' ');
	}
again:
	php_printf("zval(0x" ZEND_XLONG_FMT ") ", val);
	switch(Z_TYPE_P(val)) {
		case IS_UNDEF:
			PHPWRITE(": undefined\n", 12);
			break;
		case IS_NULL:
			PHPWRITE(": null\n", 7);
			break;
		case IS_FALSE:
			PHPWRITE(": false\n", 8);
			break;
		case IS_TRUE:
			PHPWRITE(": true\n", 7);
			break;
		case IS_LONG:
			php_printf(": long(" ZEND_LONG_FMT ")\n", Z_LVAL_P(val));
			break;
		case IS_DOUBLE:
			php_printf(": double(%.*G)\n", (int) EG(precision), Z_DVAL_P(val));
			break;
		case IS_STRING: {
			zend_string *str = unescape_zend_string(Z_STR_P(val), 0);
			php_printf("-> string(%zd,\"", Z_STRLEN_P(val));
			PHPWRITE(ZSTR_VAL(str), ZSTR_LEN(str));
			php_printf("\") addr(0x" ZEND_XLONG_FMT ") refcount(%u)\n", Z_STR_P(val), Z_REFCOUNTED_P(val) ? Z_REFCOUNT_P(val) : 1);
			if(str != Z_STR_P(val)) {
				zend_string_release(str);
			}
			break;
		}
		case IS_ARRAY: {
			zend_array *arr = val->value.arr;
			uint32_t hashSize = -(int32_t)arr->nTableMask;
			uint32_t hashUsed = 0;
			uint32_t *hash = (uint32_t*)arr->arData - hashSize;
			uint32_t idx;

			for(idx = 0; idx < hashSize; ++idx) {
				if(hash[idx] != HT_INVALID_IDX) {
					++hashUsed;
				}
			}

			php_printf("-> array(%u) addr(0x" ZEND_XLONG_FMT ") refcount(%u) hash(%u,%u) bucket(%u,%u) data(0x" ZEND_XLONG_FMT ")\n", arr->nNumOfElements, arr, arr->gc.refcount, hashSize, hashUsed, arr->nTableSize, arr->nNumUsed, arr->arData);
			if(arr->nNumOfElements == 0) {
				break;
			}
			if(level > 0) {
				php_printf("%*c", level, ' ');
			}
			PUTS("{\n");
			zendump_zend_array_dump(arr, level + INDENT_SIZE);
			if(level > 0) {
				php_printf("%*c", level, ' ');
			}
			PUTS("}\n");
			break;
		}
		case IS_OBJECT: {
			zend_string *class_name = Z_OBJ_HANDLER_P(val, get_class_name)(Z_OBJ_P(val));
			php_printf("-> object(%s) addr(0x" ZEND_XLONG_FMT ") refcount(%u)\n", ZSTR_VAL(class_name), Z_OBJ_P(val), Z_REFCOUNT_P(val));
			zend_string_release(class_name);
			break;
		}
		case IS_RESOURCE: {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(val));
			php_printf("-> resource addr(0x" ZEND_XLONG_FMT ") data(0x" ZEND_XLONG_FMT ") type (%s) refcount(%u)\n", Z_RES_P(val), Z_RES_VAL_P(val), type_name ? type_name : "unknown", Z_REFCOUNT_P(val));
			break;
		}
		case IS_REFERENCE:
			php_printf("-> reference(%u) addr(0x" ZEND_XLONG_FMT ") ", Z_REFCOUNT_P(val), Z_REF_P(val));
			val = Z_REFVAL_P(val);
			goto again;
		case IS_INDIRECT:
			PHPWRITE("-> ", 3);
			val = Z_INDIRECT_P(val);
			goto again;
		default:
			php_printf("unknown(%u)\n", Z_TYPE_P(val));
			break;
	}
}

void zendump_zend_array_dump(zend_array *arr, int level)
{
	int idx;
	for(idx = 0; idx < arr->nNumUsed; ++idx) {
		Bucket *bucket = arr->arData + idx;
		if(Z_TYPE(bucket->val) != IS_UNDEF) {
			if(bucket->key) {
				php_printf("%*c\"", level, ' ');
				PHPWRITE(ZSTR_VAL(bucket->key), ZSTR_LEN(bucket->key));
				PUTS("\" =>\n");
			} else {
				php_printf("%*c[%d] =>\n", level, ' ', bucket->h);
			}
			zendump_zval_dump(&bucket->val, level);
		}
	}
}

PHP_FUNCTION(zendump)
{
	zval *args;
	int argc;
	int i;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	for (i = 0; i < argc; i++) {
		zendump_zval_dump(&args[i], 0);
	}
}

PHP_FUNCTION(zendump_symbols)
{
	zend_execute_data *prev = EX(prev_execute_data);

	if(!prev || !prev->symbol_table) {
		return;
	}

	php_printf("symbols(%d): {\n", prev->symbol_table->nNumOfElements);
	zendump_zend_array_dump(prev->symbol_table, INDENT_SIZE);
	PUTS("}\n");
}

PHP_FUNCTION(zendump_vars)
{
	int idx;
	zend_execute_data *prev = EX(prev_execute_data);

	if(!prev || !prev->func || prev->func->type != ZEND_USER_FUNCTION) {
		return;
	}

	php_printf("vars(%d): {\n", prev->func->op_array.last_var);

	for(idx = 0; idx < prev->func->op_array.last_var; ++idx) {
		zend_string *var = prev->func->op_array.vars[idx];
		php_printf("%*c$", INDENT_SIZE, ' ');
		PHPWRITE(var->val, var->len);
		PUTS(" ->\n");

		zval *val = (zval*)(prev + 1) + idx;
		zendump_zval_dump(val, INDENT_SIZE);
	}

	PUTS("}\n");
}

PHP_FUNCTION(zendump_args)
{
	int idx;
	zend_execute_data *prev = EX(prev_execute_data);

	if(!prev || !prev->func || prev->func->type != ZEND_USER_FUNCTION) {
		return;
	}

	php_printf("args(%d): {\n", ZEND_CALL_NUM_ARGS(prev));

	for(idx = 0; idx < prev->func->op_array.num_args; ++idx) {
		zval *val = (zval*)(prev + 1) + idx;
		zendump_zval_dump(val, INDENT_SIZE);
	}

	for(idx = 0; idx < ZEND_CALL_NUM_ARGS(prev) - prev->func->op_array.num_args; ++idx) {
		zval *val = (zval*)(prev + 1) + prev->func->op_array.last_var + prev->func->op_array.T + idx;
		zendump_zval_dump(val, INDENT_SIZE);
	}

	PUTS("}\n");
}

PHP_FUNCTION(zendump_literals)
{
	int idx;
	zend_execute_data *prev = EX(prev_execute_data);

	if(!prev || !prev->func || prev->func->type != ZEND_USER_FUNCTION) {
		return;
	}

	php_printf("literals(%d): {\n", prev->func->op_array.last_literal);
	for(idx = 0; idx < prev->func->op_array.last_literal; ++idx) {
		zval *val = prev->func->op_array.literals + idx;
		zendump_zval_dump(val, INDENT_SIZE);
	}

	PUTS("}\n");
}

PHP_FUNCTION(zendump_opcodes)
{
	zend_long column_width = 35;
	zend_execute_data *prev = EX(prev_execute_data);

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(column_width)
	ZEND_PARSE_PARAMETERS_END();

	if(!prev || !prev->func || prev->func->type != ZEND_USER_FUNCTION) {
		return;
	}

	zendump_zend_op_array_dump(&prev->func->op_array, column_width);
}

PHP_FUNCTION(zendump_function)
{
	zval *val = NULL;
	char *buf = NULL;
	size_t buf_len;
	zend_long column_width = 35;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(buf, buf_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(column_width)
	ZEND_PARSE_PARAMETERS_END();

	val = zend_hash_str_find(EG(function_table), buf, buf_len);
	if(!val || !Z_FUNC_P(val)) {
		return;
	}

	if(ZEND_USER_CODE(Z_FUNC_P(val)->type)) {
		zendump_zend_op_array_dump(&Z_FUNC_P(val)->op_array, column_width);
	} else if(Z_FUNC_P(val)->type == ZEND_INTERNAL_FUNCTION) {
		php_printf("internal_function(\"%s\") handler(0x" ZEND_XLONG_FMT ")", Z_FUNC_P(val)->internal_function.function_name ? ZSTR_VAL(Z_FUNC_P(val)->internal_function.function_name) : "", Z_FUNC_P(val)->internal_function.handler);
		if(Z_FUNC_P(val)->internal_function.module) {
			php_printf(" module(%d,\"%s\",\"%s\")", Z_FUNC_P(val)->internal_function.module->module_number, Z_FUNC_P(val)->internal_function.module->name, Z_FUNC_P(val)->internal_function.module->version);
		}
	}
}

/* {{{ php_zendump_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_zendump_init_globals(zend_zendump_globals *zendump_globals)
{
	zendump_globals->global_value = 0;
	zendump_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(zendump)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(zendump)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(zendump)
{
#if defined(COMPILE_DL_ZENDUMP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(zendump)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(zendump)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "zendump support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_zendump, 0, 0, 1)
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zendump_vars, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zendump_args, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zendump_symbols, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zendump_literals, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zendump_opcodes, 0)
	ZEND_ARG_INFO(0, column_width)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zendump_function, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, column_width)
ZEND_END_ARG_INFO()

/* {{{ zendump_functions[]
 *
 * Every user visible function must have an entry in zendump_functions[].
 */
const zend_function_entry zendump_functions[] = {
	PHP_FE(zendump,          arginfo_zendump)
	PHP_FE(zendump_vars,     arginfo_zendump_vars)
	PHP_FE(zendump_args,     arginfo_zendump_args)
	PHP_FE(zendump_symbols,  arginfo_zendump_symbols)
	PHP_FE(zendump_literals, arginfo_zendump_literals)
	PHP_FE(zendump_opcodes,  arginfo_zendump_opcodes)
	PHP_FE(zendump_function, arginfo_zendump_function)
	PHP_FE_END /* Must be the last line in zendump_functions[] */
};
/* }}} */

/* {{{ zendump_module_entry
 */
zend_module_entry zendump_module_entry = {
	STANDARD_MODULE_HEADER,
	"zendump",
	zendump_functions,
	PHP_MINIT(zendump),
	PHP_MSHUTDOWN(zendump),
	PHP_RINIT(zendump),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(zendump),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(zendump),
	PHP_ZENDUMP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ZENDUMP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(zendump)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
