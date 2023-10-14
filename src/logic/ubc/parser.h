#ifndef UBCPARSER_H
#define UBCPARSER_H

#include <stdarg.h>
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

#define ADDRESS_BYTE_SIZE 4
#define UBC_TRUE_VALUE    0x01
#define UBC_FALSE_VALUE   0x00



enum UbcParserErrorType {
	UBCPARSERERROR_INTERNAL,
    UBCPARSERERROR_PARSERERROR,
    UBCPARSERERROR_PARSERTRACEBACK,
};

typedef enum UbcDebugSymbol {
	UBCDEBUGSYMBOL_NONE,
	UBCDEBUGSYMBOL_PUSH_LITERAL_FLOAT,
	UBCDEBUGSYMBOL_UNSIGNED_SUBTRACT,
	UBCDEBUGSYMBOL_PUSH_LITERAL_BOOL,
	UBCDEBUGSYMBOL_PUSH_JUMP_TARGET,
	UBCDEBUGSYMBOL_COMPARE_BOOLEANS,
	UBCDEBUGSYMBOL_PUSH_LITERAL_INT,
	UBCDEBUG_PUSH_COMPARISON_RESULT,
	UBCDEBUGSYMBOL_PUSH_STACK_TOP,
	UBCDEBUGSYMBOL_FLOAT_MULTIPLY,
	UBCDEBUGSYMBOL_SKIP_ELSE_JUMP,
	UBCDEBUGSYMBOL_FLOAT_SUBTRACT,
	UBCDEBUGSYMBOL_COMPARE_FLOATS,
	UBCDEBUGSYMBOL_SET_STACK_TOP,
	UBCDEBUGSYMBOL_SKIP_IF_JUMP,
	UBCDEBUGSYMBOL_INT_MULTIPLY,
	UBCDEBUGSYMBOL_FLOAT_DIVIDE,
	UBCDEBUGSYMBOL_INT_SUBTRACT,
	UBCDEBUGSYMBOL_COMPARE_INTS,
	UBCDEBUGSYMBOL_INT_DIVIDE,
	UBCDEBUGSYMBOL_FLOAT_ADD,
	UBCDEBUGSYMBOL_INT_ADD,
} ubcdebugsymbol_t;


typedef int   (*UBCErrorReportCallback)   (void* userdata, const char* filename, int line, const char* message, enum UbcParserErrorType type);
typedef void* (*UBCCustomReallocFunction) (void* userdata, void* address, size_t new_size, size_t old_size);
typedef void  (*UBCCustomFreeFunction)    (void* userdata, void* address, size_t size);
typedef void* (*UBCCustomMallocFunction)  (void* userdata, size_t size);
typedef int   (*UBCEmitBytecodeCallback)  (void* userdata, const void* bytes, size_t count, const char* string, enum UbcDebugSymbol);

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
    UBCErrorReportCallback error_report;
    int report_return;
    
    UBCEmitBytecodeCallback bytecode_callback;
    bool                    store_explanations;    // should there be explanations to the bytecode?
    bool                    store_strings;         // string explanations?
    bool                    optimize_explanations; // dramatic time impact, search whether exact this explanation is somewhere else already
    											   // if so, do not allocate it again

    ubcfile_t* files;
    uint16_t   file_count;
    
    ubccustomtype_t* foreign_types;
    uint16_t         type_count;

	uint32_t api_version;
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
    UBCSCOPE_LOCAL,
	UBCSCOPE_GLOBAL
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

typedef struct UbcBytecodeExplanation {
	enum UbcDebugSymbol symbolic; // What does this bytecode do
	     uintptr_t string_position; // What does this bytecode do, position of string description in string buffer
	     uintptr_t byte;    // Where is it in the closure
	     size_t    range;   // How many bytes does this explain
} ubcbytecodeexplanation_t;

typedef struct UbcNativeFunction {
	/// @brief Name of the native ubc function
	char* name;
	/// @brief Where the function is located in the bytecode buffer
	uint32_t bytecode_start;
	/// @brief End of the function code in the bytecode buffer
	uint32_t bytecode_end;
	/// @brief Argument names separated by spaces
	char* arguments;
	/// @brief Argument types separated by spaces
	char* argument_types;
} ubcnativefunction_t;

typedef struct UbcParserClosure {
	/// @brief Raw byte buffer
	ubcparserbuffer_t bytecode;
	/// @brief Stores UbcByteCodeExplanation
	ubcparserbuffer_t code_explanations;
	/// @brief Stores strings separated by \0
	ubcparserbuffer_t string_storage;
	/// @brief Stores the native function descriptor
	ubcparserbuffer_t native_functions;

	/// @brief effectively a 24-bit-integer on the virtual memory-space
	/// All memory from start to end will be saved at exit and restored at execution start
	uint32_t persistent_area_start;
	uint32_t persistent_area_end;

	/// @brief Address the vm will use as stack base
	uint32_t stack_base_address;

	/// @brief Version set by the parser config, this can be used to ensure script and API compatability
	uint32_t version;
} ubcparserclosure_t;

typedef struct UbcParser {
    struct UbcLexerStack      lexer_stack;
    ubcparserconfig_t         config;
    struct UbcParserLookahead lookahead;
    struct UbcParserTypeArray types;
    ubcparserbuffer_t         scopes;
    struct UbcParserClosure         closure;
} ubcparser_t;




// Expressions

enum UbcExpressionType {
    UBCEXPRESSIONTYPE_NONE,
    UBCEXPRESSIONTYPE_LOGICAL,
    UBCEXPRESSIONTYPE_COMPARISON,
    UBCEXPRESSIONTYPE_ADDITION,
    UBCEXPRESSIONTYPE_DIVISION,
    UBCEXPRESSIONTYPE_NEGATE,
    UBCEXPRESSIONTYPE_PARENTHESES,
    UBCEXPRESSIONTYPE_VALUE,
};

union UbcExpressionUnion {
    struct UbcParenthesesExpression* parenthesized;
    struct UbcDivisionExpression*    division;
    struct UbcAdditionExpression*    addition;
    struct UbcCompareExpression*     comparison;
    struct UbcNegateExpression*      negation;
    struct UbcValueExpression*       value;
    struct UbcLogicExpression*       logic;
};

typedef struct UbcExpression {
    union UbcExpressionUnion as;
    enum UbcExpressionType   type;
} ubcexpression_t;

typedef struct UbcExpressionBase {
	struct UbcExpression  parent;
	char*                 result_typename; // Not owned by this struct but by the type
	bool                  needs_parsing;
} ubcexpressionbase_t;


// Value Expression

typedef struct UbcParenthesesExpression {
	struct UbcExpressionBase base;
	
	struct UbLogicExpression* parenthesized;
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
	
	bool   negation;

	struct UbcValueExpression       value;
} ubcnegateexpression_t;


// Division Expression

typedef enum UbcDivisionOperator {
	UBCDIVISIONOPERATOR_NONE,
	UBCDIVISIONOPERATOR_MULTIPLY,
	UBCDIVISIONOPERATOR_DIVIDE
} ubcdivisionoperator_t;

typedef struct UbcDivisionExpression {
	struct UbcExpressionBase base;

	       char*               former_operand_typename;
	enum   UbcDivisionOperator operator;
	struct UbcNegateExpression child_expression;
} ubcdivisionexpression_t;


// Addition Expression

typedef enum UbcAdditionOperator {
	UBCADDITIONOPERATOR_NONE,
	UBCADDITIONOPERATOR_MINUS,
	UBCADDITIONOPERATOR_PLUS,
} ubcadditionoperator_t;

typedef struct UbcAdditionExpression {
	struct UbcExpressionBase base;

	       char*                 former_operand_typename;
	enum   UbcAdditionOperator   operator;
	struct UbcDivisionExpression child_expression;
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

	char*                        left_side_typename;
	enum   UbcComparatorType     comparator_type;
	struct UbcAdditionExpression child_expression;
} ubccompareexpression_t;

enum UbcLogicOperator {
	UBCLOGICOPERATOR_NONE,
	UBCLOGICOPERATOR_NAND,
	UBCLOGICOPERATOR_AND,
	UBCLOGICOPERATOR_XOR,
	UBCLOGICOPERATOR_OR,
};

typedef struct UbcLogicExpression {
    struct UbcExpressionBase base;

    struct UbcCompareExpression child_expression;
    char*                       former_operand_type;
    enum   UbcLogicOperator     operator;
} ubclogicexpression_t;

int ParserConfig_Init(ubcparserconfig_t* config);

int Parser_Create(ubcparser_t* destination, ubcparserconfig_t* config);

int Parser_Parse(ubcparser_t* parser, char* filename);

int Parser_Destroy(ubcparser_t* parser);

#endif //UBCPARSER_H
