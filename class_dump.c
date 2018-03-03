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

zend_string *zendump_properties_offset_to_name(zend_class_entry *ce, uint32_t offset);
void zendump_access_flags_dump(uint32_t flags);
void zendump_properties_info_dump(zend_class_entry *ce);
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
	if(ce->num_interfaces || ce->num_traits || ce->properties_info.nNumOfElements) {
		PUTS(" {\n");
		zendump_class_interfaces_dump(ce);
		zendump_class_traits_dump(ce);
		zendump_properties_info_dump(ce);
		zendump_static_properties_dump(ce, 0);
		PUTS("}");
	}
	PUTS("\n");
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
	php_printf("%*cinterfaces(%u) {\n", INDENT_SIZE, ' ', ce->num_interfaces);
	for(idx = 0; idx < ce->num_interfaces; ++idx) {
		zend_class_entry *interface = ce->interfaces[idx];
		if(interface->name) {
			php_printf("%*c\"%s\"\n", INDENT_SIZE << 1, ' ', ZSTR_VAL(interface->name));
		}
	}
	php_printf("%*c}\n", INDENT_SIZE, ' ');
}

void zendump_class_traits_dump(zend_class_entry *ce)
{
	uint32_t idx;
	if(ce->num_traits == 0) {
		return;
	}
	php_printf("%*ctraits(%d) {\n", INDENT_SIZE, ' ', ce->num_traits);
	for(idx = 0; idx < ce->num_traits; ++idx) {
		zend_class_entry *trait = ce->traits[idx];
		if(trait->name) {
			php_printf("%*c\"%s\"\n", INDENT_SIZE << 1, ' ', ZSTR_VAL(trait->name));
		}
	}
	php_printf("%*c}\n", INDENT_SIZE, ' ');
	if(ce->trait_aliases) {
		php_printf("%*calias {\n", INDENT_SIZE, ' ');
		idx = 0;
		while(1) {
			zend_trait_alias *alias = ce->trait_aliases[idx];
			if(alias == NULL) {
				break;
			}
			if(alias->alias) {
				php_printf("%*c\"%s\" => ", INDENT_SIZE << 1, ' ', ZSTR_VAL(alias->alias));
			}
			if(alias->trait_method) {
				zend_string *class_name = alias->trait_method->class_name;
				if(!class_name && alias->trait_method->ce) {
					class_name = alias->trait_method->ce->name;
				}
				php_printf("\"%s%s%s\"\n", class_name ? ZSTR_VAL(class_name) : "", class_name ? "::" : "", alias->trait_method->method_name ? ZSTR_VAL(alias->trait_method->method_name) : "");
			}
			++idx;
		}
		php_printf("%*c}\n", INDENT_SIZE, ' ');
	}
	if(ce->trait_precedences) {
		php_printf("%*cexclude {\n", INDENT_SIZE, ' ');
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
				php_printf("%*c\"%s%s%s\"", INDENT_SIZE << 1, ' ', class_name ? ZSTR_VAL(class_name) : "", class_name ? "::" : "", precedence->trait_method->method_name ? ZSTR_VAL(precedence->trait_method->method_name) : "");
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
				PUTS("\n");
			}
			++idx;
		}
		php_printf("%*c}\n", INDENT_SIZE, ' ');
	}
}

zend_string *zendump_properties_offset_to_name(zend_class_entry *ce, uint32_t offset)
{
	uint32_t idx;
	for(idx = 0; idx < ce->properties_info.nNumUsed; ++idx) {
		Bucket *bucket = ce->properties_info.arData + idx;
		zend_property_info *info = (zend_property_info*)Z_PTR(bucket->val);
		if(info->offset == offset) {
			return bucket->key;
		}
	}
	return NULL;
}

void zendump_properties_info_dump(zend_class_entry *ce)
{
	uint32_t idx;
	if(!ce->properties_info.nNumOfElements) {
		return;
	}
	php_printf("%*cproperties(%u) {\n", INDENT_SIZE, ' ', ce->properties_info.nNumOfElements);
	for(idx = 0; idx < ce->properties_info.nNumUsed; ++idx) {
		Bucket *bucket = ce->properties_info.arData + idx;
		zend_property_info *info = (zend_property_info*)Z_PTR(bucket->val);
		php_printf("%*c", INDENT_SIZE << 1, ' ');
		zendump_access_flags_dump(info->flags);
		if(bucket->key) {
			php_printf("$%s;", ZSTR_VAL(bucket->key));
		}
		if(info->ce && info->ce->name) {
			php_printf(" class(\"%s\")", ZSTR_VAL(info->ce->name));
		}
		if(info->name) {
			zend_string *name = unescape_zend_string(info->name, 0);
			php_printf(" name(\"%s\") offset(%u) default: ", ZSTR_VAL(name), info->offset);
			if(name != info->name) {
				zend_string_release(name);
			}
			zval *default_value = NULL;
			if(info->flags & ZEND_ACC_STATIC) {
				default_value = ce->default_static_members_table + info->offset;
			} else {
				default_value = ce->default_properties_table + (info->offset + sizeof(zval) - sizeof(zend_object)) / sizeof(zval);
			}
			zendump_zval_dump(default_value, 0);
		}
	}
	php_printf("%*c}\n", INDENT_SIZE, ' ');
}

void zendump_access_flags_dump(uint32_t flags)
{
	if(flags & ZEND_ACC_PUBLIC) {
		php_printf("public ");
	}
	if(flags & ZEND_ACC_PROTECTED) {
		php_printf("protected ");
	}
	if(flags & ZEND_ACC_PRIVATE) {
		php_printf("private ");
	}
	if(flags & ZEND_ACC_STATIC) {
		php_printf("static ");
	}
	if(flags & ZEND_ACC_ABSTRACT) {
		php_printf("abstract ");
	}
	if(flags & ZEND_ACC_FINAL) {
		php_printf("final ");
	}
}

void zendump_static_properties_dump(zend_class_entry *ce, int level)
{
	int idx;
	if(!ce->default_static_members_count) {
		return;
	}
	php_printf("%*cstatic_members(%d) {\n", level + INDENT_SIZE, ' ', ce->default_static_members_count);
	for(idx = 0; idx < ce->default_static_members_count; ++idx) {
		zend_string *name = zendump_properties_offset_to_name(ce, idx);
		if(name) {
			php_printf("%*c$%s =>\n", level + (INDENT_SIZE << 1), ' ', ZSTR_VAL(name));
		}
		zendump_zval_dump(&ce->static_members_table[idx], level + (INDENT_SIZE << 1));
	}
	php_printf("%*c}\n", level + INDENT_SIZE, ' ');
}

void zendump_properties_dump(zend_object *obj, int level)
{
	int idx;
	if(!obj->ce->default_properties_count) {
		return;
	}
	php_printf("%*cproperties(%d) {\n", level + INDENT_SIZE, ' ', obj->ce->default_properties_count);
	for(idx = 0; idx < obj->ce->default_properties_count; ++idx) {
		uint32_t offset = sizeof(zend_object) + sizeof(zval) * (idx - 1);
		zend_string *name = zendump_properties_offset_to_name(obj->ce, offset);
		if(name) {
			php_printf("%*c$%s =>\n", level + (INDENT_SIZE << 1), ' ', ZSTR_VAL(name));
		}
		zendump_zval_dump(&obj->properties_table[idx], level + (INDENT_SIZE << 1));
	}
	php_printf("%*c}\n", level + INDENT_SIZE, ' ');
}
