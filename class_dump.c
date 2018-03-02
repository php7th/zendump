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

void zendump_class_traits_dump(zend_class_entry *ce);
void zendump_class_interfaces_dump(zend_class_entry *ce);

void zendump_zend_class_entry_dump(zend_class_entry *ce, int show_magic_functions, int column_width, int show_internal_operand)
{
	PUTS(ce->type == ZEND_INTERNAL_CLASS ? "internal" : "user"); // ZEND_USER_CLASS
	if(ce->name) {
		php_printf(" class(\"%s\") refcount(%u)", ZSTR_VAL(ce->name), ce->refcount);
	}
	if(ce->parent && ce->parent->name) {
		php_printf(" parent(\"%s\")", ZSTR_VAL(ce->parent->name));
	}
	PUTS("\n");
	zendump_class_interfaces_dump(ce);
	zendump_class_traits_dump(ce);
	if(show_magic_functions) {
		if(ce->constructor) {
			zendump_zend_function_dump(ce->constructor, column_width, show_internal_operand);
		}
		if(ce->destructor) {
			zendump_zend_function_dump(ce->destructor, column_width, show_internal_operand);
		}
		if(ce->clone) {
			zendump_zend_function_dump(ce->clone, column_width, show_internal_operand);
		}
		if(ce->__get) {
			zendump_zend_function_dump(ce->__get, column_width, show_internal_operand);
		}
		if(ce->__set) {
			zendump_zend_function_dump(ce->__set, column_width, show_internal_operand);
		}
		if(ce->__unset) {
			zendump_zend_function_dump(ce->__unset, column_width, show_internal_operand);
		}
		if(ce->__isset) {
			zendump_zend_function_dump(ce->__isset, column_width, show_internal_operand);
		}
		if(ce->__call) {
			zendump_zend_function_dump(ce->__call, column_width, show_internal_operand);
		}
		if(ce->__callstatic) {
			zendump_zend_function_dump(ce->__callstatic, column_width, show_internal_operand);
		}
		if(ce->__tostring) {
			zendump_zend_function_dump(ce->__tostring, column_width, show_internal_operand);
		}
		if(ce->__debugInfo) {
			zendump_zend_function_dump(ce->__debugInfo, column_width, show_internal_operand);
		}
		if(ce->serialize_func) {
			zendump_zend_function_dump(ce->serialize_func, column_width, show_internal_operand);
		}
		if(ce->unserialize_func) {
			zendump_zend_function_dump(ce->unserialize_func, column_width, show_internal_operand);
		}
	}
}

void zendump_class_interfaces_dump(zend_class_entry *ce)
{
	uint32_t idx;
	if(ce->num_interfaces == 0) {
		return;
	}
	PUTS("interfaces(");
	for(idx = 0; idx < ce->num_interfaces; ++idx) {
		zend_class_entry *interface = ce->interfaces[idx];
		if(interface->name) {
			php_printf("%s\"%s\"", idx > 0 ? ", " : "", ZSTR_VAL(interface->name));
		}
	}
	PUTS(")\n");
}

void zendump_class_traits_dump(zend_class_entry *ce)
{
	uint32_t idx;
	if(ce->num_traits == 0) {
		return;
	}
	php_printf("traits(%d) {\n", ce->num_traits);
	for(idx = 0; idx < ce->num_traits; ++idx) {
		zend_class_entry *trait = ce->traits[idx];
		if(trait->name) {
			php_printf("  \"%s\"\n", ZSTR_VAL(trait->name));
		}
	}
	PUTS("}\n");
	if(ce->trait_aliases) {
		idx = 0;
		while(1) {
			zend_trait_alias *alias = ce->trait_aliases[idx];
			if(alias == NULL) {
				break;
			}
			if(alias->alias) {
				php_printf("alias(\"%s\" => ", ZSTR_VAL(alias->alias));
			}
			if(alias->trait_method) {
				zend_string *class_name = alias->trait_method->class_name;
				if(!class_name && alias->trait_method->ce) {
					class_name = alias->trait_method->ce->name;
				}
				php_printf("\"%s%s%s\")\n", class_name ? ZSTR_VAL(class_name) : "", class_name ? "::" : "", alias->trait_method->method_name ? ZSTR_VAL(alias->trait_method->method_name) : "");
			}
			++idx;
		}
	}
	if(ce->trait_precedences) {
		idx = 0;
		while(1) {
			zend_trait_precedence *precedence = ce->trait_precedences[idx];
			if(precedence == NULL) {
				break;
			}
			if(precedence->trait_method) {
				zend_string *class_name = precedence->trait_method->class_name;
				if(!class_name && precedence->trait_method->ce) {
					class_name = precedence->trait_method->ce->name;
				}
				php_printf("exclude(\"%s%s%s\"", class_name ? ZSTR_VAL(class_name) : "", class_name ? "::" : "", precedence->trait_method->method_name ? ZSTR_VAL(precedence->trait_method->method_name) : "");
				if(precedence->exclude_from_classes) {
					int index = 0;
					PUTS(" from");
					while(1) {
						zend_class_entry *entry = precedence->exclude_from_classes[idx].ce;
						if(entry == NULL) {
							break;
						}
						if(entry->name) {
							php_printf(" \"%s\"", ZSTR_VAL(entry->name));
						}
						++index;
					}
				}
				PUTS(")\n");
			}
			++idx;
		}
	}
}
