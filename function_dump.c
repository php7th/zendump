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

#define IS_JMP_OP1_INS 1
#define IS_JMP_OP2_INS 2
#define IS_RECV_INS    3
#define IS_SEND_EX_INS 4

void zendump_operand_value(zval *val, int column_width);
void zendump_znode_op_dump(znode_op *op, zend_uchar type, zend_op_array *op_array, int column_width);
void zendump_zend_op_dump(zend_op *opcode, zend_op_array *op_array, int column_width, int show_internal_operand);

void zendump_zend_internal_function_dump(zend_internal_function *internal_function)
{
	php_printf("internal_function(\"%s%s%s\")", (internal_function->scope && internal_function->scope->name) ? ZSTR_VAL(internal_function->scope->name) : "", (internal_function->scope && internal_function->scope->name) ? "::" : "", internal_function->function_name ? ZSTR_VAL(internal_function->function_name) : "");
	zendump_zend_internal_function_proto_dump(internal_function, 1);
	php_printf(" handler(0x" ZEND_XLONG_FMT ")", internal_function->handler);
	if(internal_function->module) {
		php_printf(" module(%d,\"%s\",\"%s\")\n", internal_function->module->module_number, internal_function->module->name, internal_function->module->version);
	}
}

void zendump_zend_internal_function_proto_dump(zend_internal_function *internal_function, int level)
{
	uint32_t idx;
	uint32_t count = internal_function->num_args;
	if(internal_function->fn_flags & ZEND_ACC_VARIADIC) {
		++count;
	}
	if(level > 0) {
		php_printf("%*c", level, ' ');
	}
	if(internal_function->function_name) {
		php_printf("%s%s(", (internal_function->fn_flags & ZEND_ACC_RETURN_REFERENCE) ? "&" : "", ZSTR_VAL(internal_function->function_name));
	}
	for(idx = 0; idx < count; ++idx) {
		zend_internal_arg_info *info = internal_function->arg_info + idx;
		if(info->name) {
			const char *type = NULL;
			if(ZEND_TYPE_IS_SET(info->type)) {
				if(ZEND_TYPE_IS_CODE(info->type)) {
					type = zendump_get_type_name(ZEND_TYPE_CODE(info->type));
				} else if(ZEND_TYPE_IS_CLASS(info->type)) {
					type = ZSTR_VAL(ZEND_TYPE_NAME(info->type));
				}
			}
			php_printf("%s%s%s%s%s$%s", idx ? ", " : "", info->is_variadic ? "..." : "", type ? type : "", type ? " " : "", info->pass_by_reference ? "&" : "", info->name);
		}
	}
	if(internal_function->function_name) {
		PUTS(")");
	}
}

void zendump_zend_op_array_dump(zend_op_array *op_array, int column_width, int show_internal_operand)
{
	int idx;
	const char *columns[] = {"OPCODE", "OP1", "OP2", "RESULT"};

	php_printf("op_array(\"%s%s%s\")", (op_array->scope && op_array->scope->name) ? ZSTR_VAL(op_array->scope->name) : "", (op_array->scope && op_array->scope->name) ? "::" : "", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "");
	zendump_zend_op_array_proto_dump(op_array, 1);
	if(op_array->refcount) {
		php_printf(" refcount(%u)", *op_array->refcount);
	}
	php_printf(" addr(0x" ZEND_XLONG_FMT ") vars(%u) T(%u)", op_array, op_array->last_var, op_array->T);
	if(op_array->filename) {
		php_printf(" filename(%s) line(%u,%u)\n", ZSTR_VAL(op_array->filename), op_array->line_start, op_array->line_end);
	}

	for(idx = 0; idx < ARRAY_LENGTH(columns); ++idx) {
		php_printf("%-*s", column_width, columns[idx]);
	}
	PUTS("\n");

	for(idx = 0; idx < op_array->last; ++idx) {
		zendump_zend_op_dump(op_array->opcodes + idx, op_array, column_width, show_internal_operand);
	}
}

void zendump_zend_op_array_proto_dump(zend_op_array *op_array, int level)
{
	uint32_t idx;
	uint32_t count = op_array->num_args;
	if(op_array->fn_flags & ZEND_ACC_VARIADIC) {
		++count;
	}
	if(level > 0) {
		php_printf("%*c", level, ' ');
	}
	if(op_array->function_name) {
		php_printf("%s%s(", (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE) ? "&" : "", ZSTR_VAL(op_array->function_name));
	}
	for(idx = 0; idx < count; ++idx) {
		zend_arg_info *info = op_array->arg_info + idx;
		if(info->name) {
			const char *type = NULL;
			if(ZEND_TYPE_IS_SET(info->type)) {
				if(ZEND_TYPE_IS_CODE(info->type)) {
					type = zendump_get_type_name(ZEND_TYPE_CODE(info->type));
				} else if(ZEND_TYPE_IS_CLASS(info->type)) {
					type = ZSTR_VAL(ZEND_TYPE_NAME(info->type));
				}
			}
			php_printf("%s%s%s%s%s$%s", idx ? ", " : "", info->is_variadic ? "..." : "", type ? type : "", type ? " " : "", info->pass_by_reference ? "&" : "", ZSTR_VAL(info->name));
		}
	}
	if(op_array->function_name) {
		PUTS(")");
	}
}

void zendump_zend_op_dump(zend_op *opcode, zend_op_array *op_array, int column_width, int show_internal_operand)
{
	const char *op_str;
	uint32_t ins_type = 0;
	switch(opcode->opcode) {
		case 0: {
			op_str = "ZEND_NOP";
			break;
		}
		case 1: {
			op_str = "ZEND_ADD";
			break;
		}
		case 2: {
			op_str = "ZEND_SUB";
			break;
		}
		case 3: {
			op_str = "ZEND_MUL";
			break;
		}
		case 4: {
			op_str = "ZEND_DIV";
			break;
		}
		case 5: {
			op_str = "ZEND_MOD";
			break;
		}
		case 6: {
			op_str = "ZEND_SL";
			break;
		}
		case 7: {
			op_str = "ZEND_SR";
			break;
		}
		case 8: {
			op_str = "ZEND_CONCAT";
			break;
		}
		case 9: {
			op_str = "ZEND_BW_OR";
			break;
		}
		case 10: {
			op_str = "ZEND_BW_AND";
			break;
		}
		case 11: {
			op_str = "ZEND_BW_XOR";
			break;
		}
		case 12: {
			op_str = "ZEND_BW_NOT";
			break;
		}
		case 13: {
			op_str = "ZEND_BOOL_NOT";
			break;
		}
		case 14: {
			op_str = "ZEND_BOOL_XOR";
			break;
		}
		case 15: {
			op_str = "ZEND_IS_IDENTICAL";
			break;
		}
		case 16: {
			op_str = "ZEND_IS_NOT_IDENTICAL";
			break;
		}
		case 17: {
			op_str = "ZEND_IS_EQUAL";
			break;
		}
		case 18: {
			op_str = "ZEND_IS_NOT_EQUAL";
			break;
		}
		case 19: {
			op_str = "ZEND_IS_SMALLER";
			break;
		}
		case 20: {
			op_str = "ZEND_IS_SMALLER_OR_EQUAL";
			break;
		}
		case 21: {
			op_str = "ZEND_CAST";
			break;
		}
		case 22: {
			op_str = "ZEND_QM_ASSIGN";
			break;
		}
		case 23: {
			op_str = "ZEND_ASSIGN_ADD";
			break;
		}
		case 24: {
			op_str = "ZEND_ASSIGN_SUB";
			break;
		}
		case 25: {
			op_str = "ZEND_ASSIGN_MUL";
			break;
		}
		case 26: {
			op_str = "ZEND_ASSIGN_DIV";
			break;
		}
		case 27: {
			op_str = "ZEND_ASSIGN_MOD";
			break;
		}
		case 28: {
			op_str = "ZEND_ASSIGN_SL";
			break;
		}
		case 29: {
			op_str = "ZEND_ASSIGN_SR";
			break;
		}
		case 30: {
			op_str = "ZEND_ASSIGN_CONCAT";
			break;
		}
		case 31: {
			op_str = "ZEND_ASSIGN_BW_OR";
			break;
		}
		case 32: {
			op_str = "ZEND_ASSIGN_BW_AND";
			break;
		}
		case 33: {
			op_str = "ZEND_ASSIGN_BW_XOR";
			break;
		}
		case 34: {
			op_str = "ZEND_PRE_INC";
			break;
		}
		case 35: {
			op_str = "ZEND_PRE_DEC";
			break;
		}
		case 36: {
			op_str = "ZEND_POST_INC";
			break;
		}
		case 37: {
			op_str = "ZEND_POST_DEC";
			break;
		}
		case 38: {
			op_str = "ZEND_ASSIGN";
			break;
		}
		case 39: {
			op_str = "ZEND_ASSIGN_REF";
			break;
		}
		case 40: {
			op_str = "ZEND_ECHO";
			break;
		}
		case 41: {
			op_str = "ZEND_GENERATOR_CREATE";
			break;
		}
		case 42: {
			op_str = "ZEND_JMP";
			ins_type = IS_JMP_OP1_INS;
			break;
		}
		case 43: {
			op_str = "ZEND_JMPZ";
			ins_type = IS_JMP_OP2_INS;
			break;
		}
		case 44: {
			op_str = "ZEND_JMPNZ";
			ins_type = IS_JMP_OP2_INS;
			break;
		}
		case 45: {
			op_str = "ZEND_JMPZNZ";
			ins_type = IS_JMP_OP2_INS;
			break;
		}
		case 46: {
			op_str = "ZEND_JMPZ_EX";
			ins_type = IS_JMP_OP2_INS;
			break;
		}
		case 47: {
			op_str = "ZEND_JMPNZ_EX";
			ins_type = IS_JMP_OP2_INS;
			break;
		}
		case 48: {
			op_str = "ZEND_CASE";
			break;
		}
		case 49: {
			op_str = "ZEND_CHECK_VAR";
			break;
		}
		case 50: {
			op_str = "ZEND_SEND_VAR_NO_REF_EX";
			ins_type = IS_SEND_EX_INS;
			break;
		}
		case 51: {
			op_str = "ZEND_MAKE_REF";
			break;
		}
		case 52: {
			op_str = "ZEND_BOOL";
			break;
		}
		case 53: {
			op_str = "ZEND_FAST_CONCAT";
			break;
		}
		case 54: {
			op_str = "ZEND_ROPE_INIT";
			break;
		}
		case 55: {
			op_str = "ZEND_ROPE_ADD";
			break;
		}
		case 56: {
			op_str = "ZEND_ROPE_END";
			break;
		}
		case 57: {
			op_str = "ZEND_BEGIN_SILENCE";
			break;
		}
		case 58: {
			op_str = "ZEND_END_SILENCE";
			break;
		}
		case 59: {
			op_str = "ZEND_INIT_FCALL_BY_NAME";
			break;
		}
		case 60: {
			op_str = "ZEND_DO_FCALL";
			break;
		}
		case 61: {
			op_str = "ZEND_INIT_FCALL";
			break;
		}
		case 62: {
			op_str = "ZEND_RETURN";
			break;
		}
		case 63: {
			op_str = "ZEND_RECV";
			ins_type = IS_RECV_INS;
			break;
		}
		case 64: {
			op_str = "ZEND_RECV_INIT";
			ins_type = IS_RECV_INS;
			break;
		}
		case 65: {
			op_str = "ZEND_SEND_VAL";
			break;
		}
		case 66: {
			op_str = "ZEND_SEND_VAR_EX";
			ins_type = IS_SEND_EX_INS;
			break;
		}
		case 67: {
			op_str = "ZEND_SEND_REF";
			break;
		}
		case 68: {
			op_str = "ZEND_NEW";
			break;
		}
		case 69: {
			op_str = "ZEND_INIT_NS_FCALL_BY_NAME";
			break;
		}
		case 70: {
			op_str = "ZEND_FREE";
			break;
		}
		case 71: {
			op_str = "ZEND_INIT_ARRAY";
			break;
		}
		case 72: {
			op_str = "ZEND_ADD_ARRAY_ELEMENT";
			break;
		}
		case 73: {
			op_str = "ZEND_INCLUDE_OR_EVAL";
			break;
		}
		case 74: {
			op_str = "ZEND_UNSET_VAR";
			break;
		}
		case 75: {
			op_str = "ZEND_UNSET_DIM";
			break;
		}
		case 76: {
			op_str = "ZEND_UNSET_OBJ";
			break;
		}
		case 77: {
			op_str = "ZEND_FE_RESET_R";
			break;
		}
		case 78: {
			op_str = "ZEND_FE_FETCH_R";
			break;
		}
		case 79: {
			op_str = "ZEND_EXIT";
			break;
		}
		case 80: {
			op_str = "ZEND_FETCH_R";
			break;
		}
		case 81: {
			op_str = "ZEND_FETCH_DIM_R";
			break;
		}
		case 82: {
			op_str = "ZEND_FETCH_OBJ_R";
			break;
		}
		case 83: {
			op_str = "ZEND_FETCH_W";
			break;
		}
		case 84: {
			op_str = "ZEND_FETCH_DIM_W";
			break;
		}
		case 85: {
			op_str = "ZEND_FETCH_OBJ_W";
			break;
		}
		case 86: {
			op_str = "ZEND_FETCH_RW";
			break;
		}
		case 87: {
			op_str = "ZEND_FETCH_DIM_RW";
			break;
		}
		case 88: {
			op_str = "ZEND_FETCH_OBJ_RW";
			break;
		}
		case 89: {
			op_str = "ZEND_FETCH_IS";
			break;
		}
		case 90: {
			op_str = "ZEND_FETCH_DIM_IS";
			break;
		}
		case 91: {
			op_str = "ZEND_FETCH_OBJ_IS";
			break;
		}
		case 92: {
			op_str = "ZEND_FETCH_FUNC_ARG";
			break;
		}
		case 93: {
			op_str = "ZEND_FETCH_DIM_FUNC_ARG";
			break;
		}
		case 94: {
			op_str = "ZEND_FETCH_OBJ_FUNC_ARG";
			break;
		}
		case 95: {
			op_str = "ZEND_FETCH_UNSET";
			break;
		}
		case 96: {
			op_str = "ZEND_FETCH_DIM_UNSET";
			break;
		}
		case 97: {
			op_str = "ZEND_FETCH_OBJ_UNSET";
			break;
		}
		case 98: {
			op_str = "ZEND_FETCH_LIST";
			break;
		}
		case 99: {
			op_str = "ZEND_FETCH_CONSTANT";
			break;
		}
		case 101: {
			op_str = "ZEND_EXT_STMT";
			break;
		}
		case 102: {
			op_str = "ZEND_EXT_FCALL_BEGIN";
			break;
		}
		case 103: {
			op_str = "ZEND_EXT_FCALL_END";
			break;
		}
		case 104: {
			op_str = "ZEND_EXT_NOP";
			break;
		}
		case 105: {
			op_str = "ZEND_TICKS";
			break;
		}
		case 106: {
			op_str = "ZEND_SEND_VAR_NO_REF";
			break;
		}
		case 107: {
			op_str = "ZEND_CATCH";
			break;
		}
		case 108: {
			op_str = "ZEND_THROW";
			break;
		}
		case 109: {
			op_str = "ZEND_FETCH_CLASS";
			break;
		}
		case 110: {
			op_str = "ZEND_CLONE";
			break;
		}
		case 111: {
			op_str = "ZEND_RETURN_BY_REF";
			break;
		}
		case 112: {
			op_str = "ZEND_INIT_METHOD_CALL";
			break;
		}
		case 113: {
			op_str = "ZEND_INIT_STATIC_METHOD_CALL";
			break;
		}
		case 114: {
			op_str = "ZEND_ISSET_ISEMPTY_VAR";
			break;
		}
		case 115: {
			op_str = "ZEND_ISSET_ISEMPTY_DIM_OBJ";
			break;
		}
		case 116: {
			op_str = "ZEND_SEND_VAL_EX";
			ins_type = IS_SEND_EX_INS;
			break;
		}
		case 117: {
			op_str = "ZEND_SEND_VAR";
			break;
		}
		case 118: {
			op_str = "ZEND_INIT_USER_CALL";
			break;
		}
		case 119: {
			op_str = "ZEND_SEND_ARRAY";
			break;
		}
		case 120: {
			op_str = "ZEND_SEND_USER";
			break;
		}
		case 121: {
			op_str = "ZEND_STRLEN";
			break;
		}
		case 122: {
			op_str = "ZEND_DEFINED";
			break;
		}
		case 123: {
			op_str = "ZEND_TYPE_CHECK";
			break;
		}
		case 124: {
			op_str = "ZEND_VERIFY_RETURN_TYPE";
			break;
		}
		case 125: {
			op_str = "ZEND_FE_RESET_RW";
			break;
		}
		case 126: {
			op_str = "ZEND_FE_FETCH_RW";
			break;
		}
		case 127: {
			op_str = "ZEND_FE_FREE";
			break;
		}
		case 128: {
			op_str = "ZEND_INIT_DYNAMIC_CALL";
			break;
		}
		case 129: {
			op_str = "ZEND_DO_ICALL";
			break;
		}
		case 130: {
			op_str = "ZEND_DO_UCALL";
			break;
		}
		case 131: {
			op_str = "ZEND_DO_FCALL_BY_NAME";
			break;
		}
		case 132: {
			op_str = "ZEND_PRE_INC_OBJ";
			break;
		}
		case 133: {
			op_str = "ZEND_PRE_DEC_OBJ";
			break;
		}
		case 134: {
			op_str = "ZEND_POST_INC_OBJ";
			break;
		}
		case 135: {
			op_str = "ZEND_POST_DEC_OBJ";
			break;
		}
		case 136: {
			op_str = "ZEND_ASSIGN_OBJ";
			break;
		}
		case 137: {
			op_str = "ZEND_OP_DATA";
			break;
		}
		case 138: {
			op_str = "ZEND_INSTANCEOF";
			break;
		}
		case 139: {
			op_str = "ZEND_DECLARE_CLASS";
			break;
		}
		case 140: {
			op_str = "ZEND_DECLARE_INHERITED_CLASS";
			break;
		}
		case 141: {
			op_str = "ZEND_DECLARE_FUNCTION";
			break;
		}
		case 142: {
			op_str = "ZEND_YIELD_FROM";
			break;
		}
		case 143: {
			op_str = "ZEND_DECLARE_CONST";
			break;
		}
		case 144: {
			op_str = "ZEND_ADD_INTERFACE";
			break;
		}
		case 145: {
			op_str = "ZEND_DECLARE_INHERITED_CLASS_DELAYED";
			break;
		}
		case 146: {
			op_str = "ZEND_VERIFY_ABSTRACT_CLASS";
			break;
		}
		case 147: {
			op_str = "ZEND_ASSIGN_DIM";
			break;
		}
		case 148: {
			op_str = "ZEND_ISSET_ISEMPTY_PROP_OBJ";
			break;
		}
		case 149: {
			op_str = "ZEND_HANDLE_EXCEPTION";
			break;
		}
		case 150: {
			op_str = "ZEND_USER_OPCODE";
			break;
		}
		case 151: {
			op_str = "ZEND_ASSERT_CHECK";
			break;
		}
		case 152: {
			op_str = "ZEND_JMP_SET";
			break;
		}
		case 153: {
			op_str = "ZEND_DECLARE_LAMBDA_FUNCTION";
			break;
		}
		case 154: {
			op_str = "ZEND_ADD_TRAIT";
			break;
		}
		case 155: {
			op_str = "ZEND_BIND_TRAITS";
			break;
		}
		case 156: {
			op_str = "ZEND_SEPARATE";
			break;
		}
		case 157: {
			op_str = "ZEND_FETCH_CLASS_NAME";
			break;
		}
		case 158: {
			op_str = "ZEND_CALL_TRAMPOLINE";
			break;
		}
		case 159: {
			op_str = "ZEND_DISCARD_EXCEPTION";
			break;
		}
		case 160: {
			op_str = "ZEND_YIELD";
			break;
		}
		case 161: {
			op_str = "ZEND_GENERATOR_RETURN";
			break;
		}
		case 162: {
			op_str = "ZEND_FAST_CALL";
			break;
		}
		case 163: {
			op_str = "ZEND_FAST_RET";
			break;
		}
		case 164: {
			op_str = "ZEND_RECV_VARIADIC";
			ins_type = IS_RECV_INS;
			break;
		}
		case 165: {
			op_str = "ZEND_SEND_UNPACK";
			break;
		}
		case 166: {
			op_str = "ZEND_POW";
			break;
		}
		case 167: {
			op_str = "ZEND_ASSIGN_POW";
			break;
		}
		case 168: {
			op_str = "ZEND_BIND_GLOBAL";
			break;
		}
		case 169: {
			op_str = "ZEND_COALESCE";
			break;
		}
		case 170: {
			op_str = "ZEND_SPACESHIP";
			break;
		}
		case 171: {
			op_str = "ZEND_DECLARE_ANON_CLASS";
			break;
		}
		case 172: {
			op_str = "ZEND_DECLARE_ANON_INHERITED_CLASS";
			break;
		}
		case 173: {
			op_str = "ZEND_FETCH_STATIC_PROP_R";
			break;
		}
		case 174: {
			op_str = "ZEND_FETCH_STATIC_PROP_W";
			break;
		}
		case 175: {
			op_str = "ZEND_FETCH_STATIC_PROP_RW";
			break;
		}
		case 176: {
			op_str = "ZEND_FETCH_STATIC_PROP_IS";
			break;
		}
		case 177: {
			op_str = "ZEND_FETCH_STATIC_PROP_FUNC_ARG";
			break;
		}
		case 178: {
			op_str = "ZEND_FETCH_STATIC_PROP_UNSET";
			break;
		}
		case 179: {
			op_str = "ZEND_UNSET_STATIC_PROP";
			break;
		}
		case 180: {
			op_str = "ZEND_ISSET_ISEMPTY_STATIC_PROP";
			break;
		}
		case 181: {
			op_str = "ZEND_FETCH_CLASS_CONSTANT";
			break;
		}
		case 182: {
			op_str = "ZEND_BIND_LEXICAL";
			break;
		}
		case 183: {
			op_str = "ZEND_BIND_STATIC";
			break;
		}
		case 184: {
			op_str = "ZEND_FETCH_THIS";
			break;
		}
		case 186: {
			op_str = "ZEND_ISSET_ISEMPTY_THIS";
			break;
		}
		case 187: {
			op_str = "ZEND_SWITCH_LONG";
			break;
		}
		case 188: {
			op_str = "ZEND_SWITCH_STRING";
			break;
		}
		case 189: {
			op_str = "ZEND_IN_ARRAY";
			break;
		}
		case 190: {
			op_str = "ZEND_COUNT";
			break;
		}
		case 191: {
			op_str = "ZEND_GET_CLASS";
			break;
		}
		case 192: {
			op_str = "ZEND_GET_CALLED_CLASS";
			break;
		}
		case 193: {
			op_str = "ZEND_GET_TYPE";
			break;
		}
		case 194: {
			op_str = "ZEND_FUNC_NUM_ARGS";
			break;
		}
		case 195: {
			op_str = "ZEND_FUNC_GET_ARGS";
			break;
		}
		case 196: {
			op_str = "ZEND_UNSET_CV";
			break;
		}
		case 197: {
			op_str = "ZEND_ISSET_ISEMPTY_CV";
			break;
		}
		default: {
			op_str = "UNKNOWN";
			break;
		}
	}
	php_printf("%-*s", column_width, op_str);

	if(ins_type == IS_JMP_OP1_INS) {
		php_printf("%-*d", column_width, OP_JMP_ADDR(opcode, opcode->op1) - opcode - 1);
	} else if(show_internal_operand && ins_type == IS_RECV_INS) {
		php_printf("%-*d", column_width, opcode->op1.num);
	} else {
		zendump_znode_op_dump(&opcode->op1, opcode->op1_type, op_array, column_width);
	}

	if(opcode->opcode == ZEND_CAST) {
		php_printf("%-*s", column_width, zendump_get_type_name(opcode->extended_value));
	} else if(ins_type == IS_JMP_OP2_INS) {
		php_printf("%-*d", column_width, OP_JMP_ADDR(opcode, opcode->op2) - opcode - 1);
	} else if(show_internal_operand && ins_type == IS_SEND_EX_INS) {
		php_printf("%-*d", column_width, opcode->op2.num);
	} else {
		zendump_znode_op_dump(&opcode->op2, opcode->op2_type, op_array, column_width);
	}

	zendump_znode_op_dump(&opcode->result, opcode->result_type, op_array, column_width);
	PUTS("\n");
}

void zendump_znode_op_dump(znode_op *op, zend_uchar type, zend_op_array *op_array, int column_width)
{
	switch(type) {
		case IS_CONST: {
			zval *val = (zval*)((char*)op_array->literals + op->constant);
			zendump_operand_value(val, column_width);
			break;
		}
		case IS_CV: {
			int index = EX_OFFSET_TO_VAR_IDX(op->var);
			if(index < op_array->last_var) {
				zend_string *var = op_array->vars[index];
				php_printf("$%-*s", column_width - 1, ZSTR_VAL(var));
			}
			break;
		}
		case IS_TMP_VAR: {
			int index = EX_OFFSET_TO_VAR_IDX(op->var) - op_array->last_var;
			php_printf("#tmp%-*d", column_width - 4, index);
			break;
		}
		case IS_VAR: {
			int index = EX_OFFSET_TO_VAR_IDX(op->var) - op_array->last_var;
			php_printf("#var%-*d", column_width - 4, index);
			break;
		}
		case IS_UNUSED: {
			php_printf("%*c", column_width, ' ');
			break;
		}
	}
}

void zendump_operand_value(zval *val, int column_width)
{
	switch(Z_TYPE_P(val)) {
		case IS_UNDEF:
			php_printf("%-*s", column_width, "undefined");
			break;
		case IS_NULL:
			php_printf("%-*s", column_width, "null");
			break;
		case IS_FALSE:
			php_printf("%-*s", column_width, "false");
			break;
		case IS_TRUE:
			php_printf("%-*s", column_width, "true");
			break;
		case IS_LONG:
			php_printf("%-*" ZEND_LONG_FMT_SPEC, column_width, Z_LVAL_P(val));
			break;
		case IS_DOUBLE:
			php_printf("%-*.*G", column_width, (int) EG(precision), Z_DVAL_P(val));
			break;
		case IS_STRING: {
			zend_string *str = zendump_unescape_zend_string(Z_STR_P(val), 0);
			PUTS("\"");
			PHPWRITE(ZSTR_VAL(str), ZSTR_LEN(str));
			PUTS("\"");
			if(column_width > ZSTR_LEN(str) + 2) {
				php_printf("%*c", column_width - 2 - ZSTR_LEN(str), ' ');
			}
			if(str != Z_STR_P(val)) {
				zend_string_release(str);
			}
			break;
		}
		case IS_ARRAY:
			php_printf("array:0x%-*" ZEND_XLONG_FMT_SPEC, column_width - 8, Z_ARRVAL_P(val));
			break;
		case IS_OBJECT:
			php_printf("object:0x%-*" ZEND_XLONG_FMT_SPEC, column_width - 9, Z_OBJ_P(val));
			break;
		case IS_RESOURCE:
			php_printf("resource:0x%-*" ZEND_XLONG_FMT_SPEC, column_width - 11, Z_RES_P(val));
			break;
		case IS_REFERENCE:
			php_printf("reference:0x%-*" ZEND_XLONG_FMT_SPEC, column_width - 12, Z_REF_P(val));
			break;
		case IS_INDIRECT:
			zendump_operand_value(Z_INDIRECT_P(val), column_width);
			break;
		default:
			php_printf("unknown:0x%-*" ZEND_XLONG_FMT_SPEC, column_width - 10, val);
			break;
	}
}
