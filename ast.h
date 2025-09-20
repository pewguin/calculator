#ifndef CALCULATOR_AST_H
#define CALCULATOR_AST_H

typedef enum {
    AST_NUMBER,
    AST_BINARY,
    AST_UNARY,
} NodeType;

typedef enum {
    AST_B_ADD,
    AST_B_SUBTRACT,
    AST_B_MULTIPLY,
    AST_B_DIVIDE,
} BinaryOperator;

typedef enum {
    AST_U_NEGATE,
} UnaryOperator;

typedef enum {
    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,
    TOK_NEG,
} TokenOperator;

typedef enum {
    TOK_NUMBER,
    TOK_OPERATOR,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_END,
} TokenType;

typedef struct ASTNode ASTNode;

ASTNode* parse_expression(char* expr);
double evaluate(const ASTNode* expr);

void print_ast(ASTNode* root);

#endif //CALCULATOR_AST_H