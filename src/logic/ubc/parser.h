#ifndef UBCPARSER_H
#define UBCPARSER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ubc.h"
#include "types.h"
#include "tokenreader.h"

#include "../errorcodes.h"
#include "../lexer/lexer.h"
#include "../dynamicbuffer/dynamicbuffer.h"
#include "../util.h"



enum UbcParserErrorType {
    UBCPARSERERROR_ERRORMESSAGE,
    UBCPARSERERROR_TRACEBACK,
};

typedef int   (*UBCErrorReportFunction)   (void* userdata, const char* filename, int line, const char* message, enum UbcParserErrorType type);
typedef void* (*UBCCustomReallocFunction) (void* userdata, void* address, size_t new_size, size_t old_size);
typedef void  (*UBCCustomFreeFunction)    (void* userdata, void* address, size_t size);
typedef void* (*UBCCustomMallocFunction)  (void* userdata, size_t size);

typedef struct UbcFile {
    char* fileName;
    char* source;
} ubcfile_t;

typedef struct UbcForeignFunction {
	char*    identifier;
	uint16_t num_args;
	char**   argument_typenames;
	char*    return_typename;
	int (*callable) (void* vm, void* user_data);
} ubcforeignfunction_t;

typedef struct UbcParserConfig {
    void* userdata;
    UBCErrorReportFunction error_report;
    int report_return;
    
    ubcfile_t* files;
    uint16_t   file_count;
    
    ubccustomtype_t* foreign_types;
    uint16_t         type_count;

    ubcforeignfunction_t* foreign_functions;
    uint16_t              function_count;

    UBCCustomMallocFunction  malloc_function;
    UBCCustomFreeFunction    free_function;
    UBCCustomReallocFunction realloc_function;
} ubcparserconfig_t;

typedef struct UbcLexerStack {
    uint16_t stack_size;
    lexer_t* lexers;
} ubclexerstack_t;

typedef struct UbcParserLookahead {
    uint32_t available;
    token_t  tokens[3];
} ubcparserlookahead_t;

typedef struct UbcParserTypeArray {
    ubccustomtype_t* array;
    uint16_t         count;
} ubcparsertypearray_t;

typedef struct UbcParserBuffer {
    void* memory;
    size_t capacity;
    size_t used;
} ubcparserbuffer_t;

enum UbcScopeType {
    UBCSCOPE_FILE,
    UBCSCOPE_FUNCTION,
    UBCSCOPE_WHILE,
    UBCSCOPE_LOCAL
};

enum UbcVariableType {
    UBCVARIABLETYPE_LOCAL,
    UBCVARIABLETYPE_PERSISTENT,
    UBCVARIABLETYPE_PARAMETER
};

typedef struct UbcVariable {
    char* name;
	size_t name_length;
    enum UbcType type;
    char flags;
    char* typename;
} ubcvariable_t;

typedef struct UbcScope {
    enum UbcScopeType type;
    ubcparserbuffer_t variables;
    uint32_t          temporary_bytes; // Bytes on the stack above variables for temporary use
} ubcscope_t;

typedef struct UbcParser {
    struct UbcLexerStack      lexer_stack;
    ubcparserbuffer_t         bytecode_buffer;
    ubcparserconfig_t         config;
    struct UbcParserLookahead lookahead;
    struct UbcParserTypeArray types;
    ubcparserbuffer_t         scopes;
} ubcparser_t;

enum UbcExpressionType {
    UBCEXPRESSIONTYPE_NONE,
	UBCEXPRESSIONTYPE_COMPARISON,
    UBCEXPRESSIONTYPE_ADDITION,
    UBCEXPRESSIONTYPE_DIVISION,
    UBCEXPRESSIONTYPE_NEGATE,
    UBCEXPRESSIONTYPE_PARENTHESES,
    UBCEXPRESSIONTYPE_VALUE,
};

struct UbcExpression;

typedef struct UbcExpressionBase {
	struct UbcExpression* parent;
	char*                 result_typename; // Not owned by this struct but by the type
	bool                  needs_parsing;
} ubcexpressionbase_t;


// Value Expression

typedef struct UbcParenthesesExpression {
	struct UbcExpressionBase base;
	
	struct UbcCompareExpression* parenthesized;
} ubcparenthesesexpression_t;

typedef struct UbcCallExpression {
	struct UbcExpressionBase base;
	
	char*  function_name;
	size_t name_length;

	struct UbcCompareExpression* arguments;
	uint16_t argument_count;
} ubccallexpression_t;

enum UbcLiteralType {
	UBCLITERALTYPE_NONE,
	UBCLITERALTYPE_INT,
	UBCLITERALTYPE_BOOL,
	UBCLITERALTYPE_FLOAT,
	UBCLITERALTYPE_STRING
};

typedef struct UbcStringLiteral {
	int32_t length;
	char*   pointer;
} ubcstringliteral_t;

union UbcLiteralValue {
	bool    boolean;
	int32_t integer;
	float   floating;
	ubcstringliteral_t string;
};

typedef struct UbcLValue {
	char* variable_path; // member accesses separated by .
	uintptr_t path_length;
} ubclvalue_t;

typedef struct UbcLiteral {
	enum UbcLiteralType type;
	union UbcLiteralValue as;
} ubcliteral_t;

typedef enum UbcValueType {
	UBCVALUETYPE_NONE,
	UBCVALUETYPE_LITERAL,
	UBCVALUETYPE_LVALUE,
	UBCVALUETYPE_PAREN,
	UBCVALUETYPE_CALL,
} ubcvaluetype_t;

typedef union UbcValueExpressionMember {
	struct UbcLiteral               literal;
	struct UbcLValue                lvalue;
	struct UbcParenthesesExpression paren;
	struct UbcCallExpression        call;
} ubcvalueexpressionmember_t;

typedef struct UbcValueExpression {
	struct UbcExpressionBase base;

	union UbcValueExpressionMember as;
	enum  UbcValueType             type;
} ubcvalueexpression_t;


// Negation Expression

typedef struct UbcNegateExpression {
	struct UbcExpressionBase base;
	
	bool negation;

	struct UbcValueExpression       value;
} ubcnegateexpression_t;


// Division Expression

typedef enum UbcDivisionOperator {
	UBCDIVISIONOPERATOR_MULTIPLY,
	UBCDIVISIONOPERATOR_DIVIDE
} ubcdivisionoperator_t;

typedef struct UbcDivisionOperand {
	struct UbcNegateExpression  expression;
	enum   UbcDivisionOperator  operator;
} ubcdivisionoperand_t;

typedef struct UbcDivisionExpression {
	struct UbcExpressionBase base;

	struct UbcDivisionOperand former;
	struct UbcDivisionOperand current;
} ubcdivisionexpression_t;


// Addition Expression

typedef enum UbcAdditionOperator {
	UBCADDITIONOPERATOR_PLUS,
	UBCADDITIONOPERATOR_MINUS
} ubcadditionoperator_t;

typedef struct UbcAdditionOperand {
	struct UbcDivisionExpression  expression;
	enum UbcAdditionOperator      operator;
} ubcadditionelement_t;

typedef struct UbcAdditionExpression {
	struct UbcExpressionBase base;

	struct UbcAdditionOperand current;
	struct UbcAdditionOperand former;
} ubcadditionexpression_t;


// Comparison Expression

typedef enum UbcComparatorType {
	UBCCOMPARATORTYPE_NONE,          // 
	UBCCOMPARATORTYPE_EQUALITY,      // ==
	UBCCOMPARATORTYPE_LESSTHAN,      // <
	UBCCOMPARATORTYPE_GREATERTHAN,   // >
	UBCCOMPARATORTYPE_INEQUALITY,    // !=
	UBCCOMPARATORTYPE_LESSEQUALS,    // <=
	UBCCOMPARATORTYPE_GREATEREQUALS, // >=
} ubccomparatortype_t;

typedef struct UbcCompareExpression {
	struct UbcExpressionBase base;

	struct UbcAdditionExpression left_hand_side;
	// Mandatory

	// Optional Fields
	// This expression can also have no comparison :d, it then evaluates to the type of the addition expression
	// otherwise, result_type is bool
	enum   UbcComparatorType     comparator_type;
	struct UbcAdditionExpression right_hand_side;
} ubccompareexpression_t;

union UbcExpressionUnion {
    ubcparenthesesexpression_t* parenthesized;
    ubcdivisionexpression_t*    division;
    ubcadditionexpression_t*    addition;
    ubccompareexpression_t*     comparison;
    ubcnegateexpression_t*      negation;
    ubcvalueexpression_t*       value;
};

typedef struct UbcExpression {
    union UbcExpressionUnion* as;
    enum UbcExpressionType    type;
} ubcexpression_t;

int Parser_Create(ubcparser_t* destination, ubcparserconfig_t* config);

int Parser_Parse(ubcparser_t* parser, char* filename);

int Parser_Destroy(ubcparser_t* parser);

#endif //UBCPARSER_H
