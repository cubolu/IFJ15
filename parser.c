#include "parser.h"

token_t next_token;

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
void parse_block();
void parse_cinStmt();
void parse_cinStmtFollow();
void parse_coutStmt();
void parse_coutStmtFollow();
void parse_expr();
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

int reduce_sequence(stack_char_t *stack) {
    stack_sym_t sym;
    int seq = 0;
    while ((sym = stack_pop(stack)) != S_SEP) {
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
                match(TT_PARENTHESES_OPEN);
                parse_paramList();
                match(TT_PARENTHESES_CLOSE);
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

void parse_block() {
    switch(next_token.type) {
        case TT_BLOCK_START:
            match(TT_BLOCK_START);
            parse_stmts();
            match(TT_BLOCK_END);
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
            parse_block();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_funcBody() {
    switch(next_token.type) {
        case TT_SEMICOLON:
            match(TT_SEMICOLON);
            break;
        case TT_BLOCK_START:
            parse_block();
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
            match(next_token.type);
            match(TT_IDENTIFICATOR);
            match(TT_PARENTHESES_OPEN);
            parse_paramSpec();
            match(TT_PARENTHESES_CLOSE);
            parse_funcBody();
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
            parse_block();
            match(TT_KW_ELSE);
            parse_block();
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
            match(TT_IDENTIFICATOR);
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
            parse_type();
            match(TT_IDENTIFICATOR);
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
            parse_block();
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
            parse_type();
            match(TT_IDENTIFICATOR);
            parse_varDefFollow();
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

bool top_term_cmp(char stack_item) { return stack_item != S_EXPR; }

void parse_expr() {
    stack_sym_t top;
    stack_sym_t next;
    stack_char_t* stack = stack_init(SI_CHAR);
    stack_push(stack, S_END);

    bool use_cached = cached_identificator.type != TT_NONE;
    token_t expr_token = use_cached ? cached_identificator : next_token ;
    next = token_to_sym(&expr_token);

    do {
        top = stack_find(stack, top_term_cmp);
        switch (prec_table[top][next]) {
            case TAB_SP:
                stack_insert_after(stack, S_SEP, top_term_cmp);
            case TAB_P:
                stack_push(stack, next);
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
                switch (reduce_sequence(stack)) {
                    case RULE_REL:
                    case RULE_ADDSUB:
                    case RULE_MULDIV:
                    case RULE_PAR:
                    case RULE_ID:
                        break;
                    default:
                        error("Syntactic error: Failed to parse the expression", ERROR_SYN);
                }
                stack_push(stack, S_EXPR);
                break;
            case TAB_END:
                if (stack_pop(stack) != S_EXPR) {
                    error("Syntactic error: Failed to parse the expression", ERROR_SYN);
                }
                break;
            case TAB_ERR:
            default:
                error("Syntactic error: Failed to parse the expression", ERROR_SYN);
        }
    } while (top != S_END || next != S_END);

    ifj15_free(stack);
}
