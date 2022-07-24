grammar BeautifulAsm;

RREG  : 'rr'[0-9]+;
LREG  : 'l'[0-9]+;
IPREG : 'p'[0-9]+;
OPREG : 'op'[0-9]+;
SREG  : 'sr';

ID  : [_a-z]+[_a-z0-9]*;

INT_NUM : [-]?('0' | [1-9][0-9]*);
FLOAT_NUM : INT_NUM '.' ([0-9]*[1-9])?;

COMMENT : [;]~('\r'|'\n')* -> skip;
WS      : [ \t\r\n]+ -> skip;
NEWLINE : '\r'? '\n';

any_number : INT_NUM | FLOAT_NUM;

ARROW : '<-';

program : statement* EOF;

statement : function_definition
          | type_definition;

any_lvalue : RREG | LREG | IPREG | OPREG;
any_rvalue : LREG | IPREG | OPREG | SREG;

register_type : 'long'
              | 'double'
              | 'ptr' '<' datatype=object_type '>';

object_type : ID
            | register_type;

type_definition : 'type' name=ID '{' (constructor | destructor | field)* '}';

CTOR : 'ctor';
DTOR : 'dtor';
constructor : CTOR '{' instructions '}';
destructor  : DTOR '{' instructions '}';

field : field_name=ID ':' field_type=register_type;

function_definition : 'fn' name=ID parameter_list? '{' instructions '}';
parameter_list : '(' type=register_type (',' type=register_type)* ')';

instructions : instruction*;

instruction : arrow_instruction
            | no_arg_instruction
            | print_instruction
            | call_instruction
            | binary_operator_instruction
            | memory_instruction
            | if_statement;

arrow_instruction           : arrow_lhs ARROW arrow_rhs;
no_arg_instruction          : no_arg_operator;
call_instruction            : 'call' ID;
print_instruction           : 'print' arg1=any_argument;
binary_operator_instruction : binary_operator arg1=any_lvalue ',' arg2=any_argument ',' arg3=any_argument;
memory_instruction          : memory_operator arg1=any_lvalue ',' arg2=memory_destination;

IF : 'if';
ELSE : 'else';
if_statement : IF condition=any_argument '{' instructions '}' elif_branch* else_branch?;
elif_branch  : ELSE condition=any_argument '{' instructions '}';
else_branch  : ELSE '{' instructions '}';

any_argument : any_rvalue | any_number;

arrow_lhs : any_lvalue
          | any_field;

arrow_rhs : make_constructor
          | any_rvalue
          | any_field;

make_constructor : 'make' type=ID (any_argument)*;

any_field : any_rvalue '.' field_name=ID;

memory_destination : any_rvalue
                   | any_field;

no_arg_operator : 'nop'
                | 'trap'
                | 'ret'
                | 'break'
                | 'continue';

binary_operator : 'add'
                | 'mul';

memory_operator : 'load'
                | 'store';