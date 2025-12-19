/*
 * Foldr Programming Language - Interpreter
 * Version 1.0.1
 * Compile: gcc -o foldr foldr.c -lm
 * Usage: ./foldr [file.fld]
 *        ./foldr (shows ASCII logo)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define VERSION "1.0.1"
#define MAX_TOKEN_LEN 256
#define MAX_TOKENS 10000
#define MAX_VARS 1000
#define MAX_FUNCS 100
#define MAX_STACK 1000

// ============= TOKEN TYPES =============
typedef enum {
    TOK_EOF, TOK_ERROR,
    // Keywords
    TOK_FUNC, TOK_LET, TOK_CONST, TOK_IF, TOK_ELSE, 
    TOK_FOR, TOK_WHILE, TOK_RETURN, TOK_IN,
    TOK_BREAK, TOK_CONTINUE,

    // Types
    TOK_INT, TOK_FLOAT, TOK_STRING, TOK_BOOL, TOK_ARRAY, TOK_VOID,
    // Literals
    TOK_NUMBER, TOK_STRING_LIT, TOK_TRUE, TOK_FALSE,
    // Identifiers
    TOK_IDENTIFIER,
    // Operators
    TOK_PLUS, TOK_MINUS, TOK_MULT, TOK_DIV, TOK_MOD,
    TOK_ASSIGN, TOK_PLUS_ASSIGN, TOK_MINUS_ASSIGN,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LTE, TOK_GTE,
    TOK_AND, TOK_OR, TOK_NOT,
    // Punctuation
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACK, TOK_RBRACK, TOK_SEMICOLON, TOK_COLON,
    TOK_COMMA, TOK_ARROW, TOK_DOT
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
    int line;
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int count;
    int current;
} Tokenizer;

// ============= AST NODE TYPES =============
typedef enum {
    NODE_PROGRAM, NODE_FUNC_DECL, NODE_VAR_DECL, 
    NODE_IF_STMT, NODE_FOR_STMT, NODE_WHILE_STMT,
    NODE_RETURN_STMT, NODE_EXPR_STMT, NODE_BLOCK,
    NODE_BREAK_STMT, NODE_CONTINUE_STMT,
    NODE_BINARY_OP, NODE_UNARY_OP, NODE_ASSIGN,
    NODE_CALL, NODE_LITERAL, NODE_IDENTIFIER,
    NODE_ARRAY_LIT, NODE_INDEX
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        struct { // Program/Block
            struct ASTNode **statements;
            int stmt_count;
        } block;
        struct { // Function
            char name[MAX_TOKEN_LEN];
            char **params;
            int param_count;
            struct ASTNode *body;
            char return_type[MAX_TOKEN_LEN];
        } func;
        struct { // Variable
            char name[MAX_TOKEN_LEN];
            struct ASTNode *init;
            char var_type[MAX_TOKEN_LEN];
            int is_const;
        } var;
        struct { // If
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;
        struct { // For
            char iterator[MAX_TOKEN_LEN];
            struct ASTNode *iterable;
            struct ASTNode *body;
        } for_stmt;
        struct { // While
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;
        struct { // Return
            struct ASTNode *value;
        } return_stmt;
        struct { // Binary Op
            char op[MAX_TOKEN_LEN];
            struct ASTNode *left;
            struct ASTNode *right;
        } binary;
        struct { // Call
            char name[MAX_TOKEN_LEN];
            struct ASTNode **args;
            int arg_count;
        } call;
        struct { // Literal
            char value[MAX_TOKEN_LEN];
            int is_number;
            int is_string;
        } literal;
        struct { // Identifier
            char name[MAX_TOKEN_LEN];
        } identifier;
        struct { // Array
            struct ASTNode **elements;
            int element_count;
        } array;
        struct { // Index
            char name[MAX_TOKEN_LEN];
            struct ASTNode *index;
        } index;
    } data;
} ASTNode;

// ============= RUNTIME VALUES =============
typedef enum {
    VAL_INT, VAL_FLOAT, VAL_STRING, VAL_BOOL, VAL_ARRAY, VAL_NULL
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        int int_val;
        double float_val;
        char *string_val;
        int bool_val;
        struct {
            struct Value **elements;
            int count;
        } array_val;
    } data;
} Value;

typedef struct {
    char name[MAX_TOKEN_LEN];
    Value value;
    int is_const;
} Variable;


typedef struct {
    char name[MAX_TOKEN_LEN];
    char **params;
    int param_count;
    ASTNode *body;
} Function;

typedef struct {
    Variable vars[MAX_VARS];
    int var_count;
    Function funcs[MAX_FUNCS];
    int func_count;
} Environment;

// ============= GLOBAL STATE =============
Environment global_env;
int return_flag = 0;
Value return_value;
int break_flag = 0;
int continue_flag = 0;

// ============= ASCII LOGO =============
void show_logo() {
    printf("\n");
    printf("  ███████╗ ██████╗ ██╗     ██████╗ ██████╗ \n");
    printf("  ██╔════╝██╔═══██╗██║     ██╔══██╗██╔══██╗\n");
    printf("  █████╗  ██║   ██║██║     ██║  ██║██████╔╝\n");
    printf("  ██╔══╝  ██║   ██║██║     ██║  ██║██╔══██╗\n");
    printf("  ██║     ╚██████╔╝███████╗██████╔╝██║  ██║\n");
    printf("  ╚═╝      ╚═════╝ ╚══════╝╚═════╝ ╚═╝  ╚═╝\n");
    printf("\n");
    printf("     Foldr Programming Language v%s\n", VERSION);
    printf("     Type 'foldr --help' for commands\n\n");
}

// ============= UTILITY FUNCTIONS =============
void error_at_token(const char *msg, Token *t) {
    fprintf(stderr, "Error: %s (line %d, token='%s', type=%d)\n",
            msg, t ? t->line : -1, t ? t->value : "?", t ? t->type : -1);
    exit(1);
}


char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);
    
    return content;
}

// ============= TOKENIZER =============
int is_keyword(const char *str, TokenType *type) {
    if (strcmp(str, "break") == 0) { *type = TOK_BREAK; return 1; }
    if (strcmp(str, "continue") == 0) { *type = TOK_CONTINUE; return 1; }
    if (strcmp(str, "func") == 0) { *type = TOK_FUNC; return 1; }
    if (strcmp(str, "let") == 0) { *type = TOK_LET; return 1; }
    if (strcmp(str, "const") == 0) { *type = TOK_CONST; return 1; }
    if (strcmp(str, "if") == 0) { *type = TOK_IF; return 1; }
    if (strcmp(str, "else") == 0) { *type = TOK_ELSE; return 1; }
    if (strcmp(str, "for") == 0) { *type = TOK_FOR; return 1; }
    if (strcmp(str, "while") == 0) { *type = TOK_WHILE; return 1; }
    if (strcmp(str, "return") == 0) { *type = TOK_RETURN; return 1; }
    if (strcmp(str, "in") == 0) { *type = TOK_IN; return 1; }
    if (strcmp(str, "true") == 0) { *type = TOK_TRUE; return 1; }
    if (strcmp(str, "false") == 0) { *type = TOK_FALSE; return 1; }
    return 0;
}

void tokenize(const char *source, Tokenizer *tok) {
    tok->count = 0;
    tok->current = 0;
    int line = 1;
    const char *p = source;
    
    while (*p) {
        // Skip whitespace
        while (*p && isspace(*p)) {
            if (*p == '\n') line++;
            p++;
        }
        if (!*p) break;
        
        // Skip comments
        if (*p == '#') {
            while (*p && *p != '\n') p++;
            continue;
        }
        
        Token *token = &tok->tokens[tok->count++];
        token->line = line;
        
        // String literals (supports "..." and '...')
        if (*p == '"' || *p == '\'') {
            char quote = *p;
            p++;
            int i = 0;
            while (*p && *p != quote && i < MAX_TOKEN_LEN - 1) {
                token->value[i++] = *p++;
            }
            token->value[i] = '\0';
            if (*p == quote) p++;
            token->type = TOK_STRING_LIT;
        }

        // Numbers
        else if (isdigit(*p)) {
            int i = 0;
            while ((isdigit(*p) || *p == '.') && i < MAX_TOKEN_LEN - 1) {
                token->value[i++] = *p++;
            }
            token->value[i] = '\0';
            token->type = TOK_NUMBER;
        }
        // Identifiers and keywords
        else if (isalpha(*p) || *p == '_') {
            int i = 0;
            while ((isalnum(*p) || *p == '_') && i < MAX_TOKEN_LEN - 1) {
                token->value[i++] = *p++;
            }
            token->value[i] = '\0';
            TokenType kw_type;
            if (is_keyword(token->value, &kw_type)) {
                token->type = kw_type;
            } else {
                token->type = TOK_IDENTIFIER;
            }
        }
        // Operators and punctuation
        else {
            token->value[0] = *p;
            token->value[1] = '\0';
            
            if (*p == '+') {
                if (*(p+1) == '=') { token->type = TOK_PLUS_ASSIGN; p++; token->value[1] = '='; token->value[2] = '\0'; }
                else token->type = TOK_PLUS;
            }
            else if (*p == '-') {
                if (*(p+1) == '=') { token->type = TOK_MINUS_ASSIGN; p++; token->value[1] = '='; token->value[2] = '\0'; }
                else if (*(p+1) == '>') { token->type = TOK_ARROW; p++; token->value[1] = '>'; token->value[2] = '\0'; }
                else token->type = TOK_MINUS;
            }
            else if (*p == '*') token->type = TOK_MULT;
            else if (*p == '/') token->type = TOK_DIV;
            else if (*p == '%') token->type = TOK_MOD;
            else if (*p == '=') {
                if (*(p+1) == '=') { token->type = TOK_EQ; p++; token->value[1] = '='; token->value[2] = '\0'; }
                else token->type = TOK_ASSIGN;
            }
            else if (*p == '!') {
                if (*(p+1) == '=') { token->type = TOK_NEQ; p++; token->value[1] = '='; token->value[2] = '\0'; }
                else token->type = TOK_NOT;
            }
            else if (*p == '<') {
                if (*(p+1) == '=') { token->type = TOK_LTE; p++; token->value[1] = '='; token->value[2] = '\0'; }
                else token->type = TOK_LT;
            }
            else if (*p == '>') {
                if (*(p+1) == '=') { token->type = TOK_GTE; p++; token->value[1] = '='; token->value[2] = '\0'; }
                else token->type = TOK_GT;
            }
            else if (*p == '&' && *(p+1) == '&') { token->type = TOK_AND; p++; token->value[1] = '&'; token->value[2] = '\0'; }
            else if (*p == '|' && *(p+1) == '|') { token->type = TOK_OR; p++; token->value[1] = '|'; token->value[2] = '\0'; }
            else if (*p == '(') token->type = TOK_LPAREN;
            else if (*p == ')') token->type = TOK_RPAREN;
            else if (*p == '{') token->type = TOK_LBRACE;
            else if (*p == '}') token->type = TOK_RBRACE;
            else if (*p == '[') token->type = TOK_LBRACK;
            else if (*p == ']') token->type = TOK_RBRACK;
            else if (*p == ';') token->type = TOK_SEMICOLON;
            else if (*p == ':') token->type = TOK_COLON;
            else if (*p == ',') token->type = TOK_COMMA;
            else if (*p == '.') token->type = TOK_DOT;
            else token->type = TOK_ERROR;
            
            p++;
        }
    }
    
    tok->tokens[tok->count].type = TOK_EOF;
}

// ============= PARSER =============
Token* peek(Tokenizer *tok) {
    return &tok->tokens[tok->current];
}

Token* advance(Tokenizer *tok) {
    return &tok->tokens[tok->current++];
}

int match(Tokenizer *tok, TokenType type) {
    if (peek(tok)->type == type) {
        advance(tok);
        return 1;
    }
    return 0;
}

ASTNode* parse_expression(Tokenizer *tok);
ASTNode* parse_statement(Tokenizer *tok);

ASTNode* parse_primary(Tokenizer *tok) {
    Token *t = peek(tok);
    ASTNode *node = malloc(sizeof(ASTNode));
    
    if (t->type == TOK_NUMBER) {
        advance(tok);
        node->type = NODE_LITERAL;
        strcpy(node->data.literal.value, t->value);
        node->data.literal.is_number = 1;
        node->data.literal.is_string = 0;
        return node;
    }
    
    if (t->type == TOK_STRING_LIT) {
        advance(tok);
        node->type = NODE_LITERAL;
        strcpy(node->data.literal.value, t->value);
        node->data.literal.is_number = 0;
        node->data.literal.is_string = 1;
        return node;
    }
    
    if (t->type == TOK_TRUE || t->type == TOK_FALSE) {
        advance(tok);
        node->type = NODE_LITERAL;
        strcpy(node->data.literal.value, t->type == TOK_TRUE ? "1" : "0");
        node->data.literal.is_number = 1;
        node->data.literal.is_string = 0;
        return node;
    }
    
    if (t->type == TOK_IDENTIFIER) {
        advance(tok);
        
        // Function call
        if (peek(tok)->type == TOK_LPAREN) {
            advance(tok);
            node->type = NODE_CALL;
            strcpy(node->data.call.name, t->value);
            node->data.call.arg_count = 0;
            node->data.call.args = malloc(sizeof(ASTNode*) * 100);
            
            while (peek(tok)->type != TOK_RPAREN && peek(tok)->type != TOK_EOF) {
                node->data.call.args[node->data.call.arg_count++] = parse_expression(tok);
                if (peek(tok)->type == TOK_COMMA) advance(tok);
            }
            match(tok, TOK_RPAREN);
            return node;
        }
        
        // Array index
        if (peek(tok)->type == TOK_LBRACK) {
            advance(tok);
            node->type = NODE_INDEX;
            strcpy(node->data.index.name, t->value);
            node->data.index.index = parse_expression(tok);
            match(tok, TOK_RBRACK);
            return node;
        }
        
        // Identifier
        node->type = NODE_IDENTIFIER;
        strcpy(node->data.identifier.name, t->value);
        return node;
    }
    
    if (t->type == TOK_LBRACK) {
        advance(tok);
        node->type = NODE_ARRAY_LIT;
        node->data.array.element_count = 0;
        node->data.array.elements = malloc(sizeof(ASTNode*) * 100);
        
        while (peek(tok)->type != TOK_RBRACK && peek(tok)->type != TOK_EOF) {
            node->data.array.elements[node->data.array.element_count++] = parse_expression(tok);
            if (peek(tok)->type == TOK_COMMA) advance(tok);
        }
        match(tok, TOK_RBRACK);
        return node;
    }
    
    if (t->type == TOK_LPAREN) {
        advance(tok);
        node = parse_expression(tok);
        match(tok, TOK_RPAREN);
        return node;
    }
    
    error_at_token("Unexpected token in expression", t);
    return NULL;

}

ASTNode* parse_binary(Tokenizer *tok) {
    ASTNode *left = parse_primary(tok);
    
    while (peek(tok)->type == TOK_PLUS || peek(tok)->type == TOK_MINUS ||
           peek(tok)->type == TOK_MULT || peek(tok)->type == TOK_DIV ||
           peek(tok)->type == TOK_MOD || peek(tok)->type == TOK_EQ ||
           peek(tok)->type == TOK_NEQ || peek(tok)->type == TOK_LT ||
           peek(tok)->type == TOK_GT || peek(tok)->type == TOK_LTE ||
           peek(tok)->type == TOK_GTE || peek(tok)->type == TOK_AND ||
           peek(tok)->type == TOK_OR) {
        Token *op = advance(tok);
        ASTNode *right = parse_primary(tok);
        
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        strcpy(node->data.binary.op, op->value);
        node->data.binary.left = left;
        node->data.binary.right = right;
        left = node;
    }
    
    return left;
}

ASTNode* parse_expression(Tokenizer *tok) {
    return parse_binary(tok);
}

ASTNode* parse_statement(Tokenizer *tok) {
    Token *t = peek(tok);
    ASTNode *node = malloc(sizeof(ASTNode));

    // Break statement
    if (t->type == TOK_BREAK) {
        advance(tok);
        node->type = NODE_BREAK_STMT;
        if (peek(tok)->type == TOK_SEMICOLON) advance(tok);
        return node;
    }

    // Continue statement
    if (t->type == TOK_CONTINUE) {
        advance(tok);
        node->type = NODE_CONTINUE_STMT;
        if (peek(tok)->type == TOK_SEMICOLON) advance(tok);
        return node;
    }

    // While loop
    if (t->type == TOK_WHILE) {
        advance(tok);
        node->type = NODE_WHILE_STMT;

        match(tok, TOK_LPAREN);
        node->data.while_stmt.condition = parse_expression(tok);
        match(tok, TOK_RPAREN);

        match(tok, TOK_LBRACE);

        node->data.while_stmt.body = malloc(sizeof(ASTNode));
        node->data.while_stmt.body->type = NODE_BLOCK;
        node->data.while_stmt.body->data.block.stmt_count = 0;
        node->data.while_stmt.body->data.block.statements = malloc(sizeof(ASTNode*) * 1000);

        while (peek(tok)->type != TOK_RBRACE && peek(tok)->type != TOK_EOF) {
            node->data.while_stmt.body->data.block.statements[
                node->data.while_stmt.body->data.block.stmt_count++
            ] = parse_statement(tok);
        }

        match(tok, TOK_RBRACE);
        return node;
    }

    
    // Function declaration
    if (t->type == TOK_FUNC) {
        advance(tok);
        node->type = NODE_FUNC_DECL;
        Token *name = advance(tok);
        strcpy(node->data.func.name, name->value);
        
        match(tok, TOK_LPAREN);
        node->data.func.param_count = 0;
        node->data.func.params = malloc(sizeof(char*) * 100);
        
        while (peek(tok)->type != TOK_RPAREN && peek(tok)->type != TOK_EOF) {
            Token *param = advance(tok);
            node->data.func.params[node->data.func.param_count] = malloc(MAX_TOKEN_LEN);
            strcpy(node->data.func.params[node->data.func.param_count++], param->value);
            
            if (peek(tok)->type == TOK_COLON) {
                advance(tok);
                advance(tok); // Skip type
            }
            if (peek(tok)->type == TOK_COMMA) advance(tok);
        }
        match(tok, TOK_RPAREN);
        
        if (peek(tok)->type == TOK_ARROW) {
            advance(tok);
            Token *ret_type = advance(tok);
            strcpy(node->data.func.return_type, ret_type->value);
        }
        
        match(tok, TOK_LBRACE);
        node->data.func.body = malloc(sizeof(ASTNode));
        node->data.func.body->type = NODE_BLOCK;
        node->data.func.body->data.block.stmt_count = 0;
        node->data.func.body->data.block.statements = malloc(sizeof(ASTNode*) * 1000);
        
        while (peek(tok)->type != TOK_RBRACE && peek(tok)->type != TOK_EOF) {
            node->data.func.body->data.block.statements[node->data.func.body->data.block.stmt_count++] = parse_statement(tok);
        }
        match(tok, TOK_RBRACE);
        
        return node;
    }
    
    // Variable declaration
    if (t->type == TOK_LET || t->type == TOK_CONST) {
        advance(tok);
        node->type = NODE_VAR_DECL;
        node->data.var.is_const = (t->type == TOK_CONST);
        
        Token *name = advance(tok);
        strcpy(node->data.var.name, name->value);
        
        if (peek(tok)->type == TOK_COLON) {
            advance(tok);
            Token *type_tok = advance(tok);
            strcpy(node->data.var.var_type, type_tok->value);
        }
        
        match(tok, TOK_ASSIGN);
        node->data.var.init = parse_expression(tok);
        
        if (peek(tok)->type == TOK_SEMICOLON) advance(tok);
        return node;
    }
    
    // If statement
    if (t->type == TOK_IF) {
        advance(tok);
        node->type = NODE_IF_STMT;
        match(tok, TOK_LPAREN);
        node->data.if_stmt.condition = parse_expression(tok);
        match(tok, TOK_RPAREN);
        match(tok, TOK_LBRACE);
        
        node->data.if_stmt.then_branch = malloc(sizeof(ASTNode));
        node->data.if_stmt.then_branch->type = NODE_BLOCK;
        node->data.if_stmt.then_branch->data.block.stmt_count = 0;
        node->data.if_stmt.then_branch->data.block.statements = malloc(sizeof(ASTNode*) * 1000);
        
        while (peek(tok)->type != TOK_RBRACE && peek(tok)->type != TOK_EOF) {
            node->data.if_stmt.then_branch->data.block.statements[node->data.if_stmt.then_branch->data.block.stmt_count++] = parse_statement(tok);
        }
        match(tok, TOK_RBRACE);
        
        node->data.if_stmt.else_branch = NULL;
        if (peek(tok)->type == TOK_ELSE) {
            advance(tok);
            match(tok, TOK_LBRACE);
            
            node->data.if_stmt.else_branch = malloc(sizeof(ASTNode));
            node->data.if_stmt.else_branch->type = NODE_BLOCK;
            node->data.if_stmt.else_branch->data.block.stmt_count = 0;
            node->data.if_stmt.else_branch->data.block.statements = malloc(sizeof(ASTNode*) * 1000);
            
            while (peek(tok)->type != TOK_RBRACE && peek(tok)->type != TOK_EOF) {
                node->data.if_stmt.else_branch->data.block.statements[node->data.if_stmt.else_branch->data.block.stmt_count++] = parse_statement(tok);
            }
            match(tok, TOK_RBRACE);
        }
        
        return node;
    }
    
    // For loop
    if (t->type == TOK_FOR) {
        advance(tok);
        node->type = NODE_FOR_STMT;
        match(tok, TOK_LPAREN);
        Token *iter = advance(tok);
        strcpy(node->data.for_stmt.iterator, iter->value);
        match(tok, TOK_IN);
        node->data.for_stmt.iterable = parse_expression(tok);
        match(tok, TOK_RPAREN);
        match(tok, TOK_LBRACE);
        
        node->data.for_stmt.body = malloc(sizeof(ASTNode));
        node->data.for_stmt.body->type = NODE_BLOCK;
        node->data.for_stmt.body->data.block.stmt_count = 0;
        node->data.for_stmt.body->data.block.statements = malloc(sizeof(ASTNode*) * 1000);
        
        while (peek(tok)->type != TOK_RBRACE && peek(tok)->type != TOK_EOF) {
            node->data.for_stmt.body->data.block.statements[node->data.for_stmt.body->data.block.stmt_count++] = parse_statement(tok);
        }
        match(tok, TOK_RBRACE);
        
        return node;
    }
    
    // Return statement
    if (t->type == TOK_RETURN) {
        advance(tok);
        node->type = NODE_RETURN_STMT;
        node->data.return_stmt.value = parse_expression(tok);
        if (peek(tok)->type == TOK_SEMICOLON) advance(tok);
        return node;
    }
    
    // Assignment or expression statement
    if (t->type == TOK_IDENTIFIER) {
        Token *name = advance(tok);
        
        if (peek(tok)->type == TOK_ASSIGN || peek(tok)->type == TOK_PLUS_ASSIGN || 
            peek(tok)->type == TOK_MINUS_ASSIGN) {
            Token *op = advance(tok);
            node->type = NODE_ASSIGN;
            strcpy(node->data.binary.op, op->value);
            node->data.binary.left = malloc(sizeof(ASTNode));
            node->data.binary.left->type = NODE_IDENTIFIER;
            strcpy(node->data.binary.left->data.identifier.name, name->value);
            node->data.binary.right = parse_expression(tok);
            if (peek(tok)->type == TOK_SEMICOLON) advance(tok);
            return node;
        }
        
        // Must be function call
        if (peek(tok)->type == TOK_LPAREN) {
            advance(tok);
            node->type = NODE_EXPR_STMT;
            ASTNode *call = malloc(sizeof(ASTNode));
            call->type = NODE_CALL;
            strcpy(call->data.call.name, name->value);
            call->data.call.arg_count = 0;
            call->data.call.args = malloc(sizeof(ASTNode*) * 100);
            
            while (peek(tok)->type != TOK_RPAREN && peek(tok)->type != TOK_EOF) {
                call->data.call.args[call->data.call.arg_count++] = parse_expression(tok);
                if (peek(tok)->type == TOK_COMMA) advance(tok);
            }
            match(tok, TOK_RPAREN);
            if (peek(tok)->type == TOK_SEMICOLON) advance(tok);
            
            node->data.block.statements = malloc(sizeof(ASTNode*));
            node->data.block.statements[0] = call;
            node->data.block.stmt_count = 1;
            return node;
        }
    }
    
    free(node);
    return NULL;
}


ASTNode* parse_program(Tokenizer *tok) {
    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->data.block.stmt_count = 0;
    program->data.block.statements = malloc(sizeof(ASTNode*) * 1000);
    
    while (peek(tok)->type != TOK_EOF) {
        ASTNode *stmt = parse_statement(tok);
        if (stmt) {
            program->data.block.statements[program->data.block.stmt_count++] = stmt;
        }
    }
    
    return program;
}

// ============= INTERPRETER =============
Value create_int(int val) {
    Value v;
    v.type = VAL_INT;
    v.data.int_val = val;
    return v;
}

Value create_float(double val) {
    Value v;
    v.type = VAL_FLOAT;
    v.data.float_val = val;
    return v;
}

Value create_string(const char *val) {
    Value v;
    v.type = VAL_STRING;
    v.data.string_val = malloc(strlen(val) + 1);
    strcpy(v.data.string_val, val);
    return v;
}

Value create_bool(int val) {
    Value v;
    v.type = VAL_BOOL;
    v.data.bool_val = val;
    return v;
}

Value create_null() {
    Value v;
    v.type = VAL_NULL;
    return v;
}

Variable* find_var(Environment *env, const char *name) {
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->vars[i].name, name) == 0) {
            return &env->vars[i];
        }
    }
    return NULL;
}

Function* find_func(Environment *env, const char *name) {
    for (int i = 0; i < env->func_count; i++) {
        if (strcmp(env->funcs[i].name, name) == 0) {
            return &env->funcs[i];
        }
    }
    return NULL;
}

void set_var(Environment *env, const char *name, Value val) {
    Variable *var = find_var(env, name);
    if (var) {
        if (var->is_const) {
            fprintf(stderr, "Error: Cannot reassign const variable '%s'\n", name);
            exit(1);
        }
        var->value = val;
    } else {
        strcpy(env->vars[env->var_count].name, name);
        env->vars[env->var_count].value = val;
        env->vars[env->var_count].is_const = 0; // default
        env->var_count++;
    }
}


Value eval(ASTNode *node, Environment *env);

Value eval_binary(ASTNode *node, Environment *env) {
    Value left = eval(node->data.binary.left, env);
    Value right = eval(node->data.binary.right, env);
    const char *op = node->data.binary.op;
    
    if (strcmp(op, "+") == 0) {
        if (left.type == VAL_INT && right.type == VAL_INT) {
            return create_int(left.data.int_val + right.data.int_val);
        }
        if (left.type == VAL_STRING || right.type == VAL_STRING) {
            char result[1000];
            if (left.type == VAL_STRING && right.type == VAL_STRING) {
                sprintf(result, "%s%s", left.data.string_val, right.data.string_val);
            } else if (left.type == VAL_STRING && right.type == VAL_INT) {
                sprintf(result, "%s%d", left.data.string_val, right.data.int_val);
            } else if (left.type == VAL_INT && right.type == VAL_STRING) {
                sprintf(result, "%d%s", left.data.int_val, right.data.string_val);
            }
            return create_string(result);
        }
        return create_float(left.data.float_val + right.data.float_val);
    }
    if (strcmp(op, "-") == 0) {
        return create_int(left.data.int_val - right.data.int_val);
    }
    if (strcmp(op, "*") == 0) {
        return create_int(left.data.int_val * right.data.int_val);
    }
    if (strcmp(op, "/") == 0) {
        return create_int(left.data.int_val / right.data.int_val);
    }
    if (strcmp(op, "%") == 0) {
        return create_int(left.data.int_val % right.data.int_val);
    }
    if (strcmp(op, ">") == 0) {
        return create_bool(left.data.int_val > right.data.int_val);
    }
    if (strcmp(op, "<") == 0) {
        return create_bool(left.data.int_val < right.data.int_val);
    }
    if (strcmp(op, ">=") == 0) {
        return create_bool(left.data.int_val >= right.data.int_val);
    }
    if (strcmp(op, "<=") == 0) {
        return create_bool(left.data.int_val <= right.data.int_val);
    }
    if (strcmp(op, "==") == 0) {
        return create_bool(left.data.int_val == right.data.int_val);
    }
    if (strcmp(op, "!=") == 0) {
        return create_bool(left.data.int_val != right.data.int_val);
    }
    
    return create_null();
}

Value eval(ASTNode *node, Environment *env) {
    if (!node) return create_null();
    
    switch (node->type) {

        case NODE_BREAK_STMT:
            break_flag = 1;
            return create_null();

        case NODE_CONTINUE_STMT:
            continue_flag = 1;
            return create_null();

        case NODE_WHILE_STMT: {
            while (1) {
                Value cond = eval(node->data.while_stmt.condition, env);
                int is_true = (cond.type == VAL_BOOL && cond.data.bool_val) ||
                            (cond.type == VAL_INT && cond.data.int_val != 0);

                if (!is_true) break;

                eval(node->data.while_stmt.body, env);

                if (return_flag) break;

                if (break_flag) {
                    break_flag = 0;
                    break;
                }

                if (continue_flag) {
                    continue_flag = 0;
                    continue;
                }
            }
            return create_null();
        }

        case NODE_PROGRAM:
        case NODE_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                eval(node->data.block.statements[i], env);
                if (return_flag || break_flag || continue_flag) break;
            }
            return create_null();

            
        case NODE_FUNC_DECL: {
            Function *func = &env->funcs[env->func_count++];
            strcpy(func->name, node->data.func.name);
            func->params = node->data.func.params;
            func->param_count = node->data.func.param_count;
            func->body = node->data.func.body;
            return create_null();
        }
        
        case NODE_VAR_DECL: {
            Value val = eval(node->data.var.init, env);
            set_var(env, node->data.var.name, val);

            Variable *v = find_var(env, node->data.var.name);
            if (v) v->is_const = node->data.var.is_const;

            return create_null();
        }
        
        case NODE_ASSIGN: {
            Value val = eval(node->data.binary.right, env);
            const char *op = node->data.binary.op;
            const char *name = node->data.binary.left->data.identifier.name;
            
            if (strcmp(op, "+=") == 0) {
                Variable *var = find_var(env, name);
                if (var) {
                    val.data.int_val += var->value.data.int_val;
                }
            } else if (strcmp(op, "-=") == 0) {
                Variable *var = find_var(env, name);
                if (var) {
                    val.data.int_val = var->value.data.int_val - val.data.int_val;
                }
            }
            
            set_var(env, name, val);
            return create_null();
        }
        
        case NODE_IF_STMT: {
            Value cond = eval(node->data.if_stmt.condition, env);
            int is_true = (cond.type == VAL_BOOL && cond.data.bool_val) || 
                         (cond.type == VAL_INT && cond.data.int_val != 0);
            
            if (is_true) {
                eval(node->data.if_stmt.then_branch, env);
            } else if (node->data.if_stmt.else_branch) {
                eval(node->data.if_stmt.else_branch, env);
            }
            return create_null();
        }
        
        case NODE_FOR_STMT: {
            Value iterable = eval(node->data.for_stmt.iterable, env);
            if (iterable.type == VAL_ARRAY) {
                for (int i = 0; i < iterable.data.array_val.count; i++) {
                    set_var(env, node->data.for_stmt.iterator, *iterable.data.array_val.elements[i]);

                    eval(node->data.for_stmt.body, env);

                    if (return_flag) break;

                    if (break_flag) {
                        break_flag = 0;
                        break;
                    }

                    if (continue_flag) {
                        continue_flag = 0;
                        continue;
                    }
                }
            }
            return create_null();
        }

        
        case NODE_RETURN_STMT:
            return_value = eval(node->data.return_stmt.value, env);
            return_flag = 1;
            return return_value;
        
        case NODE_EXPR_STMT:
            return eval(node->data.block.statements[0], env);
        
        case NODE_BINARY_OP:
            return eval_binary(node, env);
        
        case NODE_CALL: {
            const char *name = node->data.call.name;
            

            if (strcmp(name, "input") == 0) {
                // Optional prompt: input("Enter: ")
                if (node->data.call.arg_count >= 1) {
                    Value prompt = eval(node->data.call.args[0], env);
                    if (prompt.type == VAL_STRING) {
                        printf("%s", prompt.data.string_val);
                        fflush(stdout);
                    }
                }

                char buffer[1024];
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    return create_string("");
                }

                // strip trailing newline
                size_t len = strlen(buffer);
                if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';

                return create_string(buffer);
            }


            // Built-in functions
            if (strcmp(name, "print") == 0) {
                for (int i = 0; i < node->data.call.arg_count; i++) {
                    Value arg = eval(node->data.call.args[i], env);
                    if (arg.type == VAL_INT) printf("%d", arg.data.int_val);
                    else if (arg.type == VAL_FLOAT) printf("%f", arg.data.float_val);
                    else if (arg.type == VAL_STRING) printf("%s", arg.data.string_val);
                    else if (arg.type == VAL_BOOL) printf("%s", arg.data.bool_val ? "true" : "false");
                }
                printf("\n");
                return create_null();
            }
            
            if (strcmp(name, "str") == 0) {
                Value arg = eval(node->data.call.args[0], env);
                char buf[100];
                if (arg.type == VAL_INT) sprintf(buf, "%d", arg.data.int_val);
                else if (arg.type == VAL_FLOAT) sprintf(buf, "%f", arg.data.float_val);
                else if (arg.type == VAL_STRING) return arg;
                return create_string(buf);
            }
            
            if (strcmp(name, "int") == 0) {
                Value arg = eval(node->data.call.args[0], env);
                if (arg.type == VAL_STRING) return create_int(atoi(arg.data.string_val));
                if (arg.type == VAL_FLOAT) return create_int((int)arg.data.float_val);
                return arg;
            }
            
            if (strcmp(name, "len") == 0) {
                Value arg = eval(node->data.call.args[0], env);
                if (arg.type == VAL_ARRAY) return create_int(arg.data.array_val.count);
                return create_int(0);
            }
            
            // User-defined functions
            Function *func = find_func(env, name);
            if (func) {
                Environment local_env = *env;
                for (int i = 0; i < func->param_count && i < node->data.call.arg_count; i++) {
                    Value arg = eval(node->data.call.args[i], env);
                    set_var(&local_env, func->params[i], arg);
                }
                
                return_flag = 0;
                eval(func->body, &local_env);
                Value ret = return_value;
                return_flag = 0;
                return ret;
            }
            
            return create_null();
        }
        
        case NODE_LITERAL: {
            if (node->data.literal.is_number) {
                if (strchr(node->data.literal.value, '.')) {
                    return create_float(atof(node->data.literal.value));
                }
                return create_int(atoi(node->data.literal.value));
            }
            return create_string(node->data.literal.value);
        }
        
        case NODE_IDENTIFIER: {
            Variable *var = find_var(env, node->data.identifier.name);
            if (var) return var->value;
            return create_null();
        }
        
        case NODE_ARRAY_LIT: {
            Value arr;
            arr.type = VAL_ARRAY;
            arr.data.array_val.count = node->data.array.element_count;
            arr.data.array_val.elements = malloc(sizeof(Value*) * arr.data.array_val.count);
            for (int i = 0; i < arr.data.array_val.count; i++) {
                arr.data.array_val.elements[i] = malloc(sizeof(Value));
                *arr.data.array_val.elements[i] = eval(node->data.array.elements[i], env);
            }
            return arr;
        }
        
        case NODE_INDEX: {
            Variable *var = find_var(env, node->data.index.name);
            if (var && var->value.type == VAL_ARRAY) {
                Value idx = eval(node->data.index.index, env);
                if (idx.type == VAL_INT && idx.data.int_val < var->value.data.array_val.count) {
                    return *var->value.data.array_val.elements[idx.data.int_val];
                }
            }
            return create_null();
        }
        
        default:
            return create_null();
    }
}

// ============= MAIN =============
int main(int argc, char *argv[]) {
    if (argc == 1) {
        show_logo();
        return 0;
    }
    
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("Foldr Programming Language v%s\n\n", VERSION);
        printf("Usage:\n");
        printf("  foldr              Show ASCII logo and version\n");
        printf("  foldr <file.fld>   Run a Foldr program\n");
        printf("  foldr --help       Show this help message\n");
        printf("  foldr --version    Show version information\n");
        return 0;
    }
    
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("Foldr v%s\n", VERSION);
        return 0;
    }
    
    const char *filename = argv[1];
    char *source = read_file(filename);
    
    // Tokenize
    Tokenizer tok;
    tokenize(source, &tok);
    
    // Parse
    ASTNode *program = parse_program(&tok);
    
    // Interpret
    global_env.var_count = 0;
    global_env.func_count = 0;
    eval(program, &global_env);
    
    free(source);
    return 0;
}
