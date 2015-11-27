#include "parser.h"

token_t next_token, curr_token;

//for parse_asgnFollow to pass into parse_expr
token_t cached_identificator;

scanner_t* token_stream;

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
void parse_asgnFollow();
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
void parse_paramList();
void parse_paramListFollow();
void parse_paramSpec();
void parse_paramSpecFollow();
void parse_stmt();
void parse_stmts();
void parse_term();
void parse_type();
void parse_varDef();
void parse_varDefFollow();

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
            match(TT_OP_ASSIGNMENT);
            parse_asgnFollow();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_asgnFollow() {
    switch(next_token.type) {
        case TT_IDENTIFICATOR:
            cached_identificator = next_token;
            match(TT_IDENTIFICATOR);
            // TO REMOVE IF FUNEXP
            if (next_token.type == TT_PARENTHESES_OPEN) {
                func_call_init();
                func_call_set_name(curr_token.str);
                match(TT_PARENTHESES_OPEN);
                parse_paramList();
                match(TT_PARENTHESES_CLOSE);
                func_call_t* func_call = func_call_finish();
                symbol_t* func_def = func_table_find(func_call->name);
                if (func_def) {
                    if (!is_valid_func_call(func_call, func_def))
                        error("Bad function call parameters types/count", ERROR_TYPE_COMPAT);
                } else {
                    error("Call to unknown function", ERROR_SEM);
                }
                cached_identificator.type = TT_NONE;
            } else {
                parse_expr();
            }
            break;
        case TT_LIT_INT:
        case TT_LIT_DOUBLE:
        case TT_LIT_STRING:
        case TT_PARENTHESES_OPEN:
            parse_expr();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_block(bool new_scope) {
    switch(next_token.type) {
        case TT_BLOCK_START:
            match(TT_BLOCK_START);
            if (new_scope)
                var_table_scope_enter();
            parse_stmts();
            match(TT_BLOCK_END);
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
            parse_coutStmtFollow();
            break;
        default:
            return;
    }
}

void parse_forClause() {
    switch(next_token.type) {
        case TT_KW_FOR:
            match(TT_KW_FOR);
            match(TT_PARENTHESES_OPEN);
            parse_varDef();
            match(TT_SEMICOLON);
            parse_expr();
            match(TT_SEMICOLON);
            parse_asgn();
            match(TT_PARENTHESES_CLOSE);
            parse_block(true);
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_funcBody(symbol_t* funcRef) {
    switch(next_token.type) {
        case TT_SEMICOLON:
            match(TT_SEMICOLON);
            break;
        case TT_BLOCK_START:
            var_table_scope_enter();
            if (funcRef->paramList) {
                unode_str_t* paramIter = funcRef->paramList->front;
                while (paramIter) {
                    var_table_add(&(paramIter->item));
                    paramIter = paramIter->next;
                }
            }
            parse_block(false);
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
            printf("Successful parse\n");
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_ifClause() {
    switch(next_token.type) {
        case TT_KW_IF:
            match(TT_KW_IF);
            match(TT_PARENTHESES_OPEN);
            parse_expr();
            match(TT_PARENTHESES_CLOSE);
            parse_block(true);
            match(TT_KW_ELSE);
            parse_block(true);
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_paramList() {
    switch(next_token.type) {
        case TT_LIT_INT:
        case TT_LIT_DOUBLE:
        case TT_LIT_STRING:
        case TT_IDENTIFICATOR:
            parse_term();
            var_init();
            if (curr_token.type == TT_IDENTIFICATOR) {
                symbol_t* var = var_table_find(curr_token.str);
                if (!var)
                    error("Reference to undefined variable", ERROR_SEM);
                else {
                    func_call_add_param(var->type);
                }
            } else {
                func_call_add_param(curr_token.type - 8);
            }
            parse_paramListFollow();
            break;
        default:
            return;
    }
}

void parse_paramListFollow() {
    switch(next_token.type) {
        case TT_COMMA:
            match(TT_COMMA);
            parse_term();
            var_init();
            if (curr_token.type == TT_IDENTIFICATOR) {
                symbol_t* var = var_table_find(curr_token.str);
                if (!var)
                    error("Reference to undefined variable", ERROR_SEM);
                else {
                    func_call_add_param(var->type);
                }
            } else {
                func_call_add_param(curr_token.type - 8);
            }
            parse_paramListFollow();
            break;
        default:
            return;
    }
}

void parse_paramSpec() {
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
            match(next_token.type);
            var_init();
            var_set_type(curr_token.type);
            match(TT_IDENTIFICATOR);
            var_set_name(curr_token.str);
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
            parse_expr();
            match(TT_SEMICOLON);
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
    switch(next_token.type) {
        case TT_TYPE_DOUBLE:
        case TT_TYPE_INT:
        case TT_TYPE_STRING:
            var_init();
            parse_type();
            var_set_type(curr_token.type);
            match(TT_IDENTIFICATOR);
            var_set_name(curr_token.str);
            parse_varDefFollow();
            var_table_add(var_finish());
            break;
        case TT_TYPE_AUTO:
            match(TT_TYPE_AUTO);
            match(TT_IDENTIFICATOR);
            match(TT_OP_ASSIGNMENT);
            parse_expr();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_varDefFollow() {
    switch(next_token.type) {
        case TT_OP_ASSIGNMENT:
            match(TT_OP_ASSIGNMENT);
            parse_asgnFollow();
        default:
            return;
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
