#include "parser.h"

token_t next_token;
scanner_t* token_stream;

void parse_asgn();
void parse_asgnFollow();
void parse_block();
void parse_cinStmt();
void parse_cinStmtFollow();
void parse_coutStmt();
void parse_coutStmtFollow();
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
    parse_funcs();
}

void match(enum e_token_t token) {
    if (next_token.type != token)
        error("Syntactic error: Failed to parse the program", ERROR_SYN);
    next_token = get_next_token(token_stream);
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
            match(TT_IDENTIFICATOR);
            // TO REMOVE IF FUNEXP
            if (next_token.type == TT_PARENTHESES_OPEN) {
                match(TT_PARENTHESES_OPEN);
                parse_paramList();
                match(TT_PARENTHESES_CLOSE);
            } else {
                //parse_expr();
            }
            break;
        case TT_LIT_INT:
        case TT_LIT_DOUBLE:
        case TT_LIT_STRING:
        case TT_PARENTHESES_OPEN:
            //parse_expr();
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
            //parse_expr();
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
            //parse_expr();
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
            //parse_expr();
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
            //parse_expr();
            break;
        default:
            error("Syntactic error: Failed to parse the program", ERROR_SYN);
    }
}

void parse_varDefFollow() {
    switch(next_token.type) {
        case TT_OP_ASSIGNMENT:
            match(TT_OP_ASSIGNMENT);
            //parse_expr();
        default:
            return;
    }
}
