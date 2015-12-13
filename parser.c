#include "parser.h"

token_t next_token, curr_token;

//for parse_asgnFollow to pass into parse_expr
token_t cached_identificator;

scanner_t* token_stream;

size_t param_count;

//precedence table operators
typedef enum _tab_op_t {TAB_SP, TAB_P, TAB_R, TAB_END, TAB_ERR} tab_op_t;
//symbols for pushdown automata
typedef enum _stack_sym {S_REL=0, S_ADDSUB, S_MULDIV, S_PAR_O, S_PAR_C,
                         S_ID, S_END, S_SEP, S_EXPR} stack_sym_t;
//rules to parse an expression, note: rules are used contrariwise
#define RULE_REL        ((S_EXPR << 16) | (S_REL << 8) | S_EXPR)    //E -> E <>= E
#define RULE_ADDSUB     ((S_EXPR << 16) | (S_ADDSUB << 8) | S_EXPR) //E -> E +(-) E
#define RULE_MULDIV     ((S_EXPR << 16) | (S_MULDIV << 8) | S_EXPR) //E -> E *(/) E
#define RULE_PAR        ((S_PAR_C << 16) | (S_EXPR << 8) | S_PAR_O) //E -> ( E )
#define RULE_ID         (S_ID) //E -> id

/* Precedence table for expression parsing:
 *
 * < - push separator and actual token (TAB_SP)
 * = - push only actual token (TAB_P)
 * > - reduce the top of stack (TAB_R)
 * E - syntax error (TAB_E)
 *  _______ _______ _____ _____ ___ ___ ____ ___
 * |       | <,>,= | +,- | *,/ | ( | ) | id | $ |
 * |_______|_______|_____|_____|___|___|____|___|
 * | <,>,= |   >   |  <  |  <  | < | > | <  | > |
 * |_______|_______|_____|_____|___|___|____|___|
 * | +,-   |   >   |  >  |  <  | < | > | <  | > |
 * |_______|_______|_____|_____|___|___|____|___|
 * | *,/   |   >   |  >  |  >  | < | > | <  | > |
 * |_______|_______|_____|_____|___|___|____|___|
 * | (     |   <   |  <  |  <  | < | = | <  | E |
 * |_______|_______|_____|_____|___|___|____|___|
 * | )     |   >   |  >  |  >  | E | > | E  | > |
 * |_______|_______|_____|_____|___|___|____|___|
 * | id    |   >   |  >  |  >  | E | > | E  | > |
 * |_______|_______|_____|_____|___|___|____|___|
 * | $     |   <   |  <  |  <  | < | E | <  | & |
 * |_______|_______|_____|_____|___|___|____|___|
 *
 */
tab_op_t prec_table[][7] = {
    { TAB_R,  TAB_SP, TAB_SP, TAB_SP,  TAB_R,   TAB_SP,   TAB_R   },
    { TAB_R,  TAB_R,  TAB_SP, TAB_SP,  TAB_R,   TAB_SP,   TAB_R   },
    { TAB_R,  TAB_R,  TAB_R,  TAB_SP,  TAB_R,   TAB_SP,   TAB_R   },
    { TAB_SP, TAB_SP, TAB_SP, TAB_SP,  TAB_P,   TAB_SP,   TAB_ERR },
    { TAB_R,  TAB_R,  TAB_R,  TAB_ERR, TAB_R,   TAB_ERR,  TAB_R   },
    { TAB_R,  TAB_R,  TAB_R,  TAB_ERR, TAB_R,   TAB_ERR,  TAB_R   },
    { TAB_SP, TAB_SP, TAB_SP, TAB_SP,  TAB_ERR, TAB_SP,   TAB_END }
};

void parse_asgn();
expression_t parse_asgnFollow();
void parse_block(bool new_scope);
void parse_cinStmt();
void parse_cinStmtFollow();
void parse_coutStmt();
void parse_coutStmtFollow();
expression_t parse_expr();
void parse_forClause();
void parse_funcBody();
void parse_funcHead();
void parse_funcs();
void parse_ifClause();
void parse_paramList(symbol_t* func);
void parse_paramListFollow(unode_str_t* param);
void parse_paramSpec();
void parse_paramSpecFollow();
void parse_stmt();
void parse_stmts();
void parse_term();
void parse_type();
void parse_varDef();
void parse_varDefFollow(symbol_t* var);

//entry point function
void parse_program(scanner_t* s) {
    token_stream = s;
    next_token = get_next_token(token_stream);
    cached_identificator.type = TT_NONE;
    parse_funcs();
}

void match(enum e_token_t token) {
    if (next_token.type != token)
        error("Syntactic error: Failed to parse the program", ERROR_SYN);
    curr_token = next_token;
    next_token = get_next_token(token_stream);
}

void match_deduction(enum e_token_t token) {
    if (next_token.type != token)
        error("Unitinitialized auto variable", ERROR_TYPE_DEDUCTION);
    curr_token = next_token;
    next_token = get_next_token(token_stream);
}

stack_sym_t token_to_sym(token_t *tok) {
    //count an open parentheses in expression
    static int parentheses_pairs = 0;

    switch(tok->type) {
        case TT_OP_RELATIONAL:
            return S_REL;
        case TT_OP_ARITHMETIC:
            switch (tok->op_arith) {
                case OP_ARITH_ADD:
                case OP_ARITH_SUBTRACT:
                    return S_ADDSUB;
                case OP_ARITH_MULTIPLY:
                case OP_ARITH_DIVIDE:
                    return S_MULDIV;
            }
        case TT_PARENTHESES_OPEN:
            parentheses_pairs++;
            return S_PAR_O;
        case TT_PARENTHESES_CLOSE:
            if (parentheses_pairs == 0) {
                //not an expression close parenthesis
                return S_END;
            } else {
                parentheses_pairs--;
                return S_PAR_C;
            }
        case TT_IDENTIFICATOR:
        case TT_LIT_INT:
        case TT_LIT_DOUBLE:
        case TT_LIT_STRING:
            return S_ID;
        default:
            return S_END;

    }
}

int reduce_sequence(vector_char_t* stack) {
    stack_sym_t sym;
    int seq = 0;
    while ((sym = vector_pop(stack)) != S_SEP) {
        seq |= sym;
        seq <<= 8;
    }
    return seq >> 8;
}

void parse_asgn() {
    switch(next_token.type) {
        case TT_IDENTIFICATOR:
            match(TT_IDENTIFICATOR);
            symbol_t* var = var_table_find(curr_token.str);
            if (var == NULL)
                error("Trying to assign to undefined variable", ERROR_SEM);
            match(TT_OP_ASSIGNMENT);
            parse_asgnFollow(var);
            var->def = true;
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

expression_t parse_asgnFollow(symbol_t* var) {
    expression_t expr;
    expr.type = NONE_DT;
    switch(next_token.type) {
        case TT_IDENTIFICATOR:
            cached_identificator = next_token;
            match(TT_IDENTIFICATOR);
            // TODO REMOVE IF FUNEXP
            if (next_token.type == TT_PARENTHESES_OPEN) {
                func_call_init();
                func_call_set_name(curr_token.str);
                symbol_t* func_def = func_table_find(curr_token.str);
                size_t return_value_addr;
                if (func_def){
                    if (!func_def->def)
                        error("Function call for an undefined function", ERROR_SEM);
                    //alocate space for return value
                    return_value_addr = generate_call_init();
                } else {
                    error("Call to unknown function", ERROR_SEM);
                }
                match(TT_PARENTHESES_OPEN);
                parse_paramList(func_def);
                match(TT_PARENTHESES_CLOSE);
                func_call_t* func_call = func_call_finish();
                if (!is_valid_func_call(func_call, func_def))
                    error("Bad function call parameters types/count", ERROR_TYPE_COMPAT);
                generate_call(func_def->addr, param_count);
                expr.type = func_def->type;
                expr.addr = return_value_addr;
                cached_identificator.type = TT_NONE;
            } else {
                expr = parse_expr();
            }
            break;
        case TT_LIT_INT:
        case TT_LIT_DOUBLE:
        case TT_LIT_STRING:
        case TT_PARENTHESES_OPEN:
            expr = parse_expr();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
    //semantic analysis
    switch (expr.type) {
        case DOUBLE_DT:
        case DOUBLE_LIT_DT:
            if (var->type != DOUBLE_DT &&
                var->type != INT_DT)
                error("Incompatible type variable assignment",
                    ERROR_TYPE_COMPAT);
            break;
        case INT_DT:
        case INT_LIT_DT:
            if (var->type != DOUBLE_DT &&
                var->type != INT_DT)
                error("Incompatible type variable assignment",
                    ERROR_TYPE_COMPAT);
            break;
        case STRING_DT:
        case STRING_LIT_DT:
            if (var->type != STRING_DT)
                error("Incompatible type variable assignment",
                    ERROR_TYPE_COMPAT);
            break;
        default:
            ;
    }
    //generate assignment instruction
    if (expr.type != NONE_DT) { //TODO: REMOVE ?!
        switch (expr.type) {
            case DOUBLE_DT:
                if(var->type == DOUBLE_DT) {
                    generate_mov(var->addr, expr.addr);
                } else {
                    size_t conv_expr = generate_double_to_int(expr.addr);
                    generate_mov(var->addr, conv_expr);
                }
                break;
            case INT_DT:
                if(var->type == INT_DT) {
                    generate_mov(var->addr, expr.addr);
                } else {
                    size_t conv_expr = generate_int_to_double(expr.addr);
                    generate_mov(var->addr, conv_expr);
                }
                break;
            case STRING_DT:
                generate_mov(var->addr, expr.addr);
                break;
            case DOUBLE_LIT_DT:
                if(var->type == DOUBLE_DT)
                    generate_mov_double(var->addr, expr.double_val);
                else
                    generate_mov_double(var->addr, (int)expr.double_val);
                break;
            case INT_LIT_DT:
                if(var->type == INT_DT)
                    generate_mov_int(var->addr, expr.int_val);
                else
                    generate_mov_int(var->addr, (double)expr.int_val);
                break;
            case STRING_LIT_DT:
                generate_mov_string(var->addr, expr.str_val);
                break;
            default:
                ;
        }
    }
    return expr;
}

void parse_block(bool new_scope) {
    switch(next_token.type) {
        case TT_BLOCK_START:
            match(TT_BLOCK_START);
            bool emptyBlock = false;
            if (next_token.type == TT_BLOCK_END)
                emptyBlock = true;
            if (new_scope && !emptyBlock)
                var_table_scope_enter();
            parse_stmts();
            match(TT_BLOCK_END);
            if (!new_scope || (new_scope && !emptyBlock))
                var_table_scope_exit();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_cinStmt() {
    switch(next_token.type) {
        case TT_KW_CIN:
            match(TT_KW_CIN);
            match(TT_OP_STREAM_IN);
            match(TT_IDENTIFICATOR);
            symbol_t* var = var_table_find(curr_token.str);
            if (var == NULL)
                error("Trying to use undefined variable", ERROR_SEM);
            switch(var->type) {
                case INT_DT:
                    generate_cin_int(var->addr);
                    break;
                case DOUBLE_DT:
                    generate_cin_double(var->addr);
                    break;
                case STRING_DT:
                    generate_cin_string(var->addr);
                    break;
                default:
                    ;
            }
            var->def = true;
            parse_cinStmtFollow();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_cinStmtFollow() {
    switch(next_token.type) {
        case TT_OP_STREAM_IN:
            match(TT_OP_STREAM_IN);
            match(TT_IDENTIFICATOR);
            symbol_t* var = var_table_find(curr_token.str);
            if (var == NULL)
                error("Trying to use undefined variable", ERROR_SEM);
            switch(var->type) {
                case INT_DT:
                    generate_cin_int(var->addr);
                    break;
                case DOUBLE_DT:
                    generate_cin_double(var->addr);
                    break;
                case STRING_DT:
                    generate_cin_string(var->addr);
                    break;
                default:
                    ;
            }
            var->def = true;
            parse_cinStmtFollow();
            break;
        default:
            return;
    }
}

void parse_coutStmt() {
    switch(next_token.type) {
        case TT_KW_COUT:
            match(TT_KW_COUT);
            match(TT_OP_STREAM_OUT);
            parse_term();
            symbol_t* var;
            if (curr_token.type == TT_IDENTIFICATOR) {
                var = var_table_find(curr_token.str);
                if (var == NULL) {
                    error("Reference to undefined variable", ERROR_SEM);
                } else if (var->def != true) {
                    error("Trying to use uninitialized variable", ERROR_UNDEF);
                }
                switch(var->type) {
                    case INT_DT:
                        generate_cout_int(var->addr);
                        break;
                    case DOUBLE_DT:
                        generate_cout_double(var->addr);
                        break;
                    case STRING_DT:
                        generate_cout_string(var->addr);
                        break;
                    default:
                        ;
                }
            }
            switch(curr_token.type) {
                case TT_LIT_INT:
                    generate_cout_int_lit(curr_token.int_val);
                    break;
                case TT_LIT_DOUBLE:
                    generate_cout_double_lit(curr_token.double_val);
                    break;
                case TT_LIT_STRING:
                    generate_cout_string_lit(curr_token.str);
                    break;
                default:
                    ;
            }
            parse_coutStmtFollow();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_coutStmtFollow() {
    switch(next_token.type) {
        case TT_OP_STREAM_OUT:
            match(TT_OP_STREAM_OUT);
            parse_term();
            symbol_t* var;
            if (curr_token.type == TT_IDENTIFICATOR) {
                var = var_table_find(curr_token.str);
                if (var == NULL) {
                    error("Reference to undefined variable", ERROR_SEM);
                } else if (var->def != true) {
                    error("Trying to use uninitialized variable", ERROR_UNDEF);
                }
                switch(var->type) {
                    case INT_DT:
                        generate_cout_int(var->addr);
                        break;
                    case DOUBLE_DT:
                        generate_cout_double(var->addr);
                        break;
                    case STRING_DT:
                        generate_cout_string(var->addr);
                        break;
                    default:
                        ;
                }
            }
            switch(curr_token.type) {
                case TT_LIT_INT:
                    generate_cout_int_lit(curr_token.int_val);
                    break;
                case TT_LIT_DOUBLE:
                    generate_cout_double_lit(curr_token.double_val);
                    break;
                case TT_LIT_STRING:
                    generate_cout_string_lit(curr_token.str);
                    break;
                default:
                    ;
            }
            parse_coutStmtFollow();
            break;
        default:
            return;
    }
}

void parse_forClause() {
    expression_t expr;
    size_t conv_expr;
    size_t loop_start;
    size_t loop_end;
    size_t asgn_start;
    size_t block_start;
    size_t jc_addr;
    size_t jmp_to_block_addr;
    size_t asgn_frame;
    size_t block_frame;
    bool jmp_used = true;
    switch(next_token.type) {
        case TT_KW_FOR:
            match(TT_KW_FOR);
            match(TT_PARENTHESES_OPEN);
            var_table_scope_enter();
            block_frame = store_stack_frame();
            parse_varDef();
            match(TT_SEMICOLON);
            //loop start
            loop_start = get_code_seg_top();
            generate_data_seg_restore(store_stack_frame());
            expr = parse_expr();
            match(TT_SEMICOLON);
            //gen jc
            jc_addr = get_code_seg_top();
            switch (expr.type) {
                case DOUBLE_LIT_DT:
                    if(!((int)expr.double_val))
                        generate_jump(0); //address is unknown yet
                    else
                        jmp_used = false;
                    break;
                case INT_LIT_DT:
                    if(!expr.int_val)
                        generate_jump(0); //address is unknown yet
                    else
                        jmp_used = false;
                    break;
                case DOUBLE_DT:
                    conv_expr = generate_double_to_int(expr.addr);
                    generate_neg_cond_jump(0, conv_expr); //address is unknown yet
                    break;
                case INT_DT:
                    generate_neg_cond_jump(0, expr.addr); //address is unknown yet
                    break;
                case STRING_LIT_DT:
                case STRING_DT:
                    error("String value in a if statement", ERROR_TYPE_COMPAT);
                    break;
                default:
                    ;
            }
            //jump to block start
            jmp_to_block_addr = get_code_seg_top();
            generate_jump(0); //address is unknown yet
            //assignment start
            asgn_start = get_code_seg_top();
            asgn_frame = store_stack_frame();
            parse_asgn();
            load_stack_frame(asgn_frame);
            //jump to loop start
            generate_jump(loop_start);
            match(TT_PARENTHESES_CLOSE);
            //block start
            block_start = get_code_seg_top();
            set_jump_addr(jmp_to_block_addr, block_start);
            parse_block(false);
            //jump to assignment
            generate_jump(asgn_start);
            //loop end
            loop_end = get_code_seg_top();
            if (jmp_used)
                set_jump_addr(jc_addr, loop_end);
            break;
            load_stack_frame(block_frame);
            generate_data_seg_restore(block_frame);
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_funcBody(symbol_t* funcRef) {
    symbol_t* func;
    switch(next_token.type) {
        case TT_SEMICOLON:
            match(TT_SEMICOLON);
            break;
        case TT_BLOCK_START:
            func = func_table_find(func_get_name());
            func->def = true; //function is defined
            func->addr = get_code_seg_top();
            var_table_scope_enter();
            init_new_stack_frame(param_count);
            if (funcRef->paramList) {
                unode_str_t* paramIter = funcRef->paramList->front;
                while (paramIter) {
                    var_table_add(&(paramIter->item));
                    paramIter = paramIter->next;
                }
            }
            parse_block(false);
            generate_no_return_exception();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_funcHead() {
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
            func_init();
            match(next_token.type);
            func_set_return_type(curr_token.type);
            match(TT_IDENTIFICATOR);
            func_set_name(curr_token.str);
            match(TT_PARENTHESES_OPEN);
            parse_paramSpec();
            func_set_param_count(param_count);
            match(TT_PARENTHESES_CLOSE);
            symbol_t* funcRef = func_finish();
            func_table_add(funcRef);
            parse_funcBody(funcRef);
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_funcs() {
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
            parse_funcHead();
            parse_funcs();
            break;
        case TT_EOF:
            //printf("Successful parse\n");
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_ifClause() {
    expression_t expr;
    size_t conv_expr;
    size_t jc_addr;
    size_t jmp_addr;
    size_t stack_frame;
    bool jmp_used = true;
    switch(next_token.type) {
        case TT_KW_IF:
            match(TT_KW_IF);
            match(TT_PARENTHESES_OPEN);
            expr = parse_expr();
            match(TT_PARENTHESES_CLOSE);
            jc_addr = get_code_seg_top();
            switch (expr.type) {
                case DOUBLE_LIT_DT:
                    if(!((int)expr.double_val))
                        generate_jump(0); //address is unknown yet
                    else
                        jmp_used = false;
                    break;
                case INT_LIT_DT:
                    if(!expr.int_val)
                        generate_jump(0); //address is unknown yet
                    else
                        jmp_used = false;
                    break;
                case DOUBLE_DT:
                    conv_expr = generate_double_to_int(expr.addr);
                    generate_neg_cond_jump(0, conv_expr); //address is unknown yet
                    break;
                case INT_DT:
                    generate_neg_cond_jump(0, expr.addr); //address is unknown yet
                    break;
                case STRING_LIT_DT:
                case STRING_DT:
                    error("String value in a if statement", ERROR_TYPE_COMPAT);
                    break;
                default:
                    ;
            }
            stack_frame = store_stack_frame();
            parse_block(true);
            load_stack_frame(stack_frame);
            generate_data_seg_restore(stack_frame);
            jmp_addr = get_code_seg_top();
            generate_jump(0); //address is unknown yet
            if (jmp_used)
                set_jump_addr(jc_addr, get_code_seg_top());
            match(TT_KW_ELSE);
            stack_frame = store_stack_frame();
            parse_block(true);
            load_stack_frame(stack_frame);
            generate_data_seg_restore(stack_frame);
            set_jump_addr(jmp_addr, get_code_seg_top());
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_paramList(symbol_t* func) {
    param_count = 0; //initialize param counter
    symbol_t* var;
    unode_str_t* param;
    e_data_t func_param_type;
    switch(next_token.type) {
        case TT_LIT_INT:
        case TT_LIT_DOUBLE:
        case TT_LIT_STRING:
        case TT_IDENTIFICATOR:
            parse_term();
            if (func->paramList && (param = func->paramList->back))
                func_param_type = param->item.type;
            else
                error("Bad function call parameters types/count", ERROR_TYPE_COMPAT);
            //var_init(); //TODO: ???
            switch (curr_token.type) {
                case TT_LIT_INT:
                    func_call_add_param(INT_DT);
                    if (func_param_type == INT_DT)
                        generate_param_push_int(curr_token.int_val);
                    else
                        generate_param_push_double((double)curr_token.int_val);
                    break;
                case TT_LIT_DOUBLE:
                    func_call_add_param(DOUBLE_DT);
                    if (func_param_type == DOUBLE_DT)
                        generate_param_push_double(curr_token.double_val);
                    else
                        generate_param_push_int((int)curr_token.double_val);
                    break;
                case TT_LIT_STRING:
                    func_call_add_param(STRING_DT);
                    generate_param_push_string(curr_token.str);
                    break;
                case TT_IDENTIFICATOR:
                    var = var_table_find(curr_token.str);
                    if (var == NULL) {
                        error("Reference to undefined variable", ERROR_SEM);
                    } else if (var->def != true) {
                        error("Trying to use uninitialized variable", ERROR_UNDEF);
                    } else {
                        func_call_add_param(var->type);
                        if (var->type == DOUBLE_DT && func_param_type == INT_DT)
                            generate_param_conv_push(var->addr, true, false);
                        else if (var->type == INT_DT && func_param_type == DOUBLE_DT)
                            generate_param_conv_push(var->addr, false, true);
                        else
                            generate_param_push(var->addr);
                    }
                    break;
                default:
                    ;
            }
            ++param_count;
            parse_paramListFollow(param->prev);
            break;
        default:
            return;
    }
}

void parse_paramListFollow(unode_str_t* param) {
    symbol_t* var;
    e_data_t func_param_type;
    switch(next_token.type) {
        case TT_COMMA:
            match(TT_COMMA);
            parse_term();
            if (param)
                func_param_type = param->item.type;
            else
                error("Bad function call parameters types/count", ERROR_TYPE_COMPAT);
            //var_init(); //TODO: ???
            switch (curr_token.type) {
                case TT_LIT_INT:
                    func_call_add_param(INT_DT);
                    if (func_param_type == INT_DT)
                        generate_param_push_int(curr_token.int_val);
                    else
                        generate_param_push_double((double)curr_token.int_val);
                    break;
                case TT_LIT_DOUBLE:
                    func_call_add_param(DOUBLE_DT);
                    if (func_param_type == DOUBLE_DT)
                        generate_param_push_double(curr_token.double_val);
                    else
                        generate_param_push_int((int)curr_token.double_val);
                    break;
                case TT_LIT_STRING:
                    func_call_add_param(STRING_DT);
                    generate_param_push_string(curr_token.str);
                    break;
                case TT_IDENTIFICATOR:
                    var = var_table_find(curr_token.str);
                    if (var == NULL) {
                        error("Reference to undefined variable", ERROR_SEM);
                    } else if (var->def != true) {
                        error("Trying to use uninitialized variable", ERROR_UNDEF);
                    } else {
                        func_call_add_param(var->type);
                        if (var->type == DOUBLE_DT && func_param_type == INT_DT)
                            generate_param_conv_push(var->addr, true, false);
                        else if (var->type == INT_DT && func_param_type == DOUBLE_DT)
                            generate_param_conv_push(var->addr, false, true);
                        else
                            generate_param_push(var->addr);
                    }
                    break;
                default:
                    ;
            }
            ++param_count;
            parse_paramListFollow(param->prev);
            break;
        default:
            return;
    }
}

void parse_paramSpec() {
    param_count = 0; //initialize param counter
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
            match(next_token.type);
            var_init();
            var_set_type(curr_token.type);
            match(TT_IDENTIFICATOR);
            var_set_name(curr_token.str);
            var_set_addr(++param_count);
            var_set_initialized();
            func_add_param(var_finish());
            parse_paramSpecFollow();
            break;
        default:
            return;
    }
}

void parse_paramSpecFollow() {
    switch(next_token.type) {
        case TT_COMMA:
            match(TT_COMMA);
            var_init();
            parse_type();
            var_set_type(curr_token.type);
            match(TT_IDENTIFICATOR);
            var_set_name(curr_token.str);
            var_set_addr(++param_count);
            var_set_initialized();
            func_add_param(var_finish());
            parse_paramSpecFollow();
            break;
        default:
            return;
    }
}

void parse_stmt() {
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
        case TT_TYPE_AUTO:
            parse_varDef();
            match(TT_SEMICOLON);
            break;
        case TT_IDENTIFICATOR:
            parse_asgn();
            match(TT_SEMICOLON);
            break;
        case TT_KW_RETURN:
            match(TT_KW_RETURN);
            expression_t expr = parse_expr();
            switch (expr.type) {
                case DOUBLE_DT:
                case DOUBLE_LIT_DT:
                    if (func_get_return_type() != DOUBLE_DT &&
                        func_get_return_type() != INT_DT)
                        error("Variable with bad type in a return statement",
                            ERROR_TYPE_COMPAT);
                    break;
                case INT_DT:
                case INT_LIT_DT:
                    if (func_get_return_type() != DOUBLE_DT &&
                        func_get_return_type() != INT_DT)
                        error("Variable with bad type in a return statement",
                            ERROR_TYPE_COMPAT);
                    break;
                case STRING_DT:
                case STRING_LIT_DT:
                    if (func_get_return_type() != STRING_DT)
                        error("Variable with bad type in a return statement",
                            ERROR_TYPE_COMPAT);
                    break;
                default:
                    ;
            }
            match(TT_SEMICOLON);
            if (str_equal(func_get_name(), "main"))
                generate_halt();
            else {
                switch (expr.type) {
                    case DOUBLE_DT:
                        if(func_get_return_type() == DOUBLE_DT) {
                            generate_return(expr.addr, func_get_param_count());
                        } else {
                            size_t conv_expr = generate_double_to_int(expr.addr);
                            generate_return(conv_expr, func_get_param_count());
                        }
                        break;
                    case INT_DT:
                        if(func_get_return_type() == INT_DT) {
                            generate_return(expr.addr, func_get_param_count());
                        } else {
                            size_t conv_expr = generate_int_to_double(expr.addr);
                            generate_return(conv_expr, func_get_param_count());
                        }
                        break;
                    case STRING_DT:
                        generate_return(expr.addr, func_get_param_count());
                        break;
                    case DOUBLE_LIT_DT:
                        if(func_get_return_type() == DOUBLE_DT)
                            generate_return_double(expr.double_val, func_get_param_count());
                        else
                            generate_return_double((int)expr.double_val, func_get_param_count());
                        break;
                    case INT_LIT_DT:
                        if(func_get_return_type() == INT_DT)
                            generate_return_int(expr.int_val, func_get_param_count());
                        else
                            generate_return_int((double)expr.int_val, func_get_param_count());
                        break;
                    case STRING_LIT_DT:
                        generate_return_string(expr.str_val, func_get_param_count());
                        break;
                    default:
                        ;
                }
            }
            break;
        case TT_KW_COUT:
            parse_coutStmt();
            match(TT_SEMICOLON);
            break;
        case TT_KW_CIN:
            parse_cinStmt();
            match(TT_SEMICOLON);
            break;
        case TT_BLOCK_START:
            parse_block(true);
            break;
        case TT_KW_IF:
            parse_ifClause();
            break;
        case TT_KW_FOR:
            parse_forClause();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_stmts() {
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
        case TT_TYPE_AUTO:
        case TT_IDENTIFICATOR:
        case TT_KW_RETURN:
        case TT_KW_COUT:
        case TT_KW_CIN:
        case TT_BLOCK_START:
        case TT_KW_IF:
        case TT_KW_FOR:
            parse_stmt();
            parse_stmts();
            break;
        default:
            return;
    }
}

void parse_term() {
    switch(next_token.type) {
        case TT_LIT_INT:
            match(TT_LIT_INT);
            break;
        case TT_LIT_DOUBLE:
            match(TT_LIT_DOUBLE);
            break;
        case TT_LIT_STRING:
            match(TT_LIT_STRING);
            break;
        case TT_IDENTIFICATOR:
            match(TT_IDENTIFICATOR);
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_type() {
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
            match(TT_TYPE_DOUBLE);
            break;
        case TT_TYPE_INT:
            match(TT_TYPE_INT);
            break;
        case TT_TYPE_STRING:
            match(TT_TYPE_STRING);
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_varDef() {
    expression_t expr;
    size_t addr;
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
            var_init();
            parse_type();
            var_set_type(curr_token.type);
            match(TT_IDENTIFICATOR);
            var_set_name(curr_token.str);
            addr = generate_push();
            var_set_addr(addr);
            symbol_t* var = var_finish();
            var_table_add(var);
            parse_varDefFollow(var_table_find(var->name));
            break;
        case TT_TYPE_AUTO:
            var_init();
            match(TT_TYPE_AUTO);
            match(TT_IDENTIFICATOR);
            var_set_name(curr_token.str);
            match_deduction(TT_OP_ASSIGNMENT);
            var_set_initialized();
            addr = generate_push();
            var_set_addr(addr);
            expr = parse_expr();
            switch (expr.type) {
                case DOUBLE_DT:
                case INT_DT:
                case STRING_DT:
                    var_set_type(expr.type);
                    generate_mov(addr, expr.addr);
                    break;
                case DOUBLE_LIT_DT:
                    var_set_type(DOUBLE_DT);
                    generate_mov_double(addr, expr.double_val);
                    break;
                case INT_LIT_DT:
                    var_set_type(INT_DT);
                    generate_mov_int(addr, expr.int_val);
                    break;
                case STRING_LIT_DT:
                    var_set_type(STRING_DT);
                    generate_mov_string(addr, expr.str_val);
                    break;
                default:
                    ;
            }
            var_table_add(var_finish());
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_varDefFollow(symbol_t* var) {
    switch(next_token.type) {
        case TT_OP_ASSIGNMENT:
            match(TT_OP_ASSIGNMENT);
            parse_asgnFollow(var);
            var->def = true;
        default:
            ;
    }
}

bool top_term_cmp(char vector_item) { return vector_item != S_EXPR; }

expression_t parse_expr() {
    expression_t parsed_expr;
    stack_sym_t top;
    stack_sym_t next;
    vector_char_t* stack = vector_init(VI_CHAR);
    vector_token_t* token_buffer = vector_init(VI_TOKEN);
    vector_expr_t* expr_buffer = vector_init(VI_EXPR);
    vector_push(stack, S_END);

    bool use_cached = cached_identificator.type != TT_NONE;
    token_t last_token;
    token_t expr_token = use_cached ? cached_identificator : next_token ;
    next = token_to_sym(&expr_token);

    do {
        top = vector_find(stack, top_term_cmp);
        switch (prec_table[top][next]) {
            case TAB_SP:
                vector_insert_after(stack, S_SEP, top_term_cmp);
            case TAB_P:
                vector_push(stack, next);
                //next is now top terminal on stack
                top = next;
                //store last token
                vector_push(token_buffer, expr_token);
                //choose between cached_identificator and next_token
                if (use_cached) {
                    use_cached = false;
                    cached_identificator.type = TT_NONE;
                    expr_token = next_token;
                } else {
                    expr_token = next_token = get_next_token(token_stream);
                }
                //convert next token to table symbol
                next = token_to_sym(&expr_token);
                break;
            case TAB_R:
                last_token = vector_pop(token_buffer);
                switch (reduce_sequence(stack)) {
                    case RULE_REL:
                        check_rule_rel(last_token.op_rel, expr_buffer);
                        break;
                    case RULE_ADDSUB:
                        check_rule_arith(last_token.op_arith, expr_buffer);
                        break;
                    case RULE_MULDIV:
                        check_rule_arith(last_token.op_arith, expr_buffer);
                        break;
                    case RULE_PAR:
                        //only pop next token(TT_PARENTHESES_OPEN) from token buffer
                        vector_pop(token_buffer);
                        break;
                    case RULE_ID:
                        check_rule_id(&last_token, expr_buffer);
                        break;
                    default:
                        error("Syntactic error: Failed to parse the expression", ERROR_SYN);
                }
                vector_push(stack, S_EXPR);
                break;
            case TAB_END:
                if (vector_pop(stack) != S_EXPR) {
                    error("Syntactic error: Failed to parse the expression", ERROR_SYN);
                }
                parsed_expr = vector_top(expr_buffer);
                break;
            case TAB_ERR:
            default:
                error("Syntactic error: Failed to parse the expression", ERROR_SYN);
        }
    } while (top != S_END || next != S_END);

    ifj15_free(stack);
    ifj15_free(token_buffer);
    ifj15_free(expr_buffer);
    return parsed_expr;
}
