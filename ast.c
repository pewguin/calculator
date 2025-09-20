#include <stdlib.h>
#include "ast.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    union {
        double number;
        struct {
            ASTNode* lhs;
            ASTNode* rhs;
            BinaryOperator oper;
        } binary;
        struct {
            ASTNode* val;
            UnaryOperator oper;
        } unary;
    } content;
};

typedef struct NodeStack {
    ASTNode* tokens[128];
    int top;
} NodeStack;

void initialize_stack_node(NodeStack* stack) {
    stack->top = -1;
}
void push_node(NodeStack* stack, ASTNode* token) {
    stack->tokens[++stack->top] = token;
}
bool is_empty_node(NodeStack* stack) {
    return stack->top <= -1;
}
ASTNode* pop_node(NodeStack* stack) {
    if (is_empty_node(stack)) { printf("empty node stack\n"); return NULL; }
    return stack->tokens[stack->top--];
}
ASTNode* peek_node(NodeStack* stack) {
    if (is_empty_node(stack)) { printf("empty node stack\n"); return NULL; }
    return stack->tokens[stack->top];
}

typedef struct Token {
    TokenType type;
    union {
        struct {
            TokenOperator oper;
            int prec;
        };
        double number;
    };
} Token;

typedef struct TokenStack {
    Token* tokens[128];
    int top;
} TokenStack;

void initialize_stack_token(TokenStack* stack) {
    stack->top = -1;
}
void push_token(TokenStack* stack, Token* token) {
    stack->tokens[++stack->top] = token;
}
bool is_empty_token(TokenStack* stack) {
    return stack->top <= -1;
}
Token* pop_token(TokenStack* stack) {
    if (is_empty_token(stack)) { printf("empty token stack\n"); return NULL; }
    return stack->tokens[stack->top--];
}
Token* peek_token(TokenStack* stack) {
    if (is_empty_token(stack)) { printf("empty token stack\n"); return NULL; }
    return stack->tokens[stack->top];
}

ASTNode* make_ast_number(const double number) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->content.number = number;
    return node;
}
ASTNode* make_ast_binary(ASTNode* lhs, ASTNode* rhs, const BinaryOperator oper) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY;
    node->content.binary.lhs = lhs;
    node->content.binary.rhs = rhs;
    node->content.binary.oper = oper;
    return node;
}
ASTNode* make_ast_unary(ASTNode* val, UnaryOperator oper) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_UNARY;
    node->content.unary.val = val;
    return node;
}

Token* make_token_number(const double number) {
    Token* token = malloc(sizeof(Token));
    token->type = TOK_NUMBER;
    token->number = number;
    return token;
}
Token* make_token_operator(const TokenOperator oper) {
    Token* token = malloc(sizeof(Token));
    token->type = TOK_OPERATOR;
    token->oper = oper;
    switch (oper) {
        case TOK_ADD:
        case TOK_SUB: token->prec = 0; break;
        case TOK_MUL:
        case TOK_DIV: token->prec = 1; break;
        case TOK_NEG: token->prec = 2; break;
    }
    return token;
}
Token* make_token_paren(const TokenType paren_type) {
    Token* token = malloc(sizeof(Token));
    token->type = paren_type;
    return token;
}
Token* make_token_end() {
    Token* token = malloc(sizeof(Token));
    token->type = TOK_END;
    return token;
}

bool is_operator(char c) {
   return c == '+' || c == '-' || c == '*' || c == 'x' || c == '/' || c == '(' || c == ')';
}

double evaluate(const ASTNode* expr) {
    switch (expr->type) {
        case AST_NUMBER:
            return expr->content.number;
        case AST_BINARY: {
            const double lhs = evaluate(expr->content.binary.lhs);
            const double rhs = evaluate(expr->content.binary.rhs);
            switch (expr->content.binary.oper) {
                case AST_B_ADD:
                    return lhs + rhs;
                case AST_B_SUBTRACT:
                    return lhs - rhs;
                case AST_B_MULTIPLY:
                    return lhs * rhs;
                case AST_B_DIVIDE:
                    return lhs / rhs;
            }
            case AST_UNARY: {
                const double val = evaluate(expr->content.unary.val);
                switch (expr->content.unary.oper) {
                    case AST_U_NEGATE:
                        return -val;
                }
            }
        }
    }
    return 0;
}

Token** tokenize(char* expr) {
    Token** arr = malloc(sizeof(Token*) * 128);
    int token_index = 0;
    char* front = expr;

    while (*front != '\0') {
        char* new_front;
        double num = strtod(front, &new_front);
        if (is_operator(*front)) {
            TokenOperator oper;
            switch (*front) {
                case '+': oper = TOK_ADD; break;
                case '-': {
                    if (token_index == 0 ||
                        arr[token_index - 1]->type == TOK_OPERATOR ||
                        arr[token_index - 1]->type == TOK_LPAREN) {
                        oper = TOK_NEG;
                    } else {
                        oper = TOK_SUB;
                    }
                    break;
                }
                case 'x':
                case '*': oper = TOK_MUL; break;
                case '/': oper = TOK_DIV; break;
                default:
                    printf("Undefined operator: %c\nDefaulting to addition\n", *front);
                    oper = TOK_ADD;
                    break;
            }
            arr[token_index] = make_token_operator(oper);
            ++token_index;
            ++front;
        } else if (front != new_front) {
            front = new_front;
            arr[token_index] = make_token_number(num);
            ++token_index;
        } else {
            ++front;
        }
    }
    arr[token_index] = make_token_end();
    return arr;
}

ASTNode* token_into_node(const Token* token) {
    switch (token->type) {
        case TOK_NUMBER: return make_ast_number(token->number);
        case TOK_OPERATOR:
            switch (token->oper) {
                case TOK_ADD: return make_ast_binary(NULL, NULL, AST_B_ADD);
                case TOK_SUB: return make_ast_binary(NULL, NULL, AST_B_SUBTRACT);
                case TOK_MUL: return make_ast_binary(NULL, NULL, AST_B_MULTIPLY);
                case TOK_DIV: return make_ast_binary(NULL, NULL, AST_B_DIVIDE);
                case TOK_NEG: return make_ast_unary(NULL, AST_U_NEGATE);
            }
            break;
        default: return NULL;
    }
}

void free_token_array(Token** tokens) {
    for (int i = 0; tokens[i]->type != TOK_END; ++i) {
        free(tokens[i]);
    }
    free(tokens);
}

ASTNode* create_ast(Token** tokens) {
    NodeStack output_stack;
    TokenStack operator_stack;
    initialize_stack_node(&output_stack);
    initialize_stack_token(&operator_stack);
    int token_index = 0;

    while (tokens[token_index]->type != TOK_END) {
        Token* token = tokens[token_index];
        switch (token->type) {
            case TOK_NUMBER:
                push_node(&output_stack, make_ast_number(token->number));
                break;
            case TOK_OPERATOR:
                while (!is_empty_token(&operator_stack) && peek_token(&operator_stack)->prec >= token->prec) {
                    ASTNode* old = token_into_node(pop_token(&operator_stack));
                    old->content.binary.rhs = pop_node(&output_stack);
                    old->content.binary.lhs = pop_node(&output_stack);
                    push_node(&output_stack, old);
                }
                push_token(&operator_stack, token);
                break;
            case TOK_LPAREN:
                push_token(&operator_stack, make_token_paren(TOK_LPAREN));
                break;
            case TOK_RPAREN:
                while (peek_token(&operator_stack)->type != TOK_LPAREN) {
                    Token* old_token = pop_token(&operator_stack);
                    if (old_token->type == TOK_END) { return make_ast_number(-1); }

                    ASTNode* old = token_into_node(old_token);
                    old->content.binary.rhs = pop_node(&output_stack);
                    old->content.binary.lhs = pop_node(&output_stack);
                    push_node(&output_stack, old);
                }
                free(pop_token(&operator_stack));
                break;
            case TOK_END:
                break;
        }
        ++token_index;
    }
    while (!is_empty_token(&operator_stack)) {
        ASTNode* node = token_into_node(pop_token(&operator_stack));
        node->content.binary.rhs = pop_node(&output_stack);
        node->content.binary.lhs = pop_node(&output_stack);
        push_node(&output_stack, node);
    }
    free_token_array(tokens);
    return pop_node(&output_stack);
}

char* get_tabs(int count) {
    char* tabs = malloc(sizeof(char) * (count + 1));
    memset(tabs, ' ', sizeof(char) * count);
    tabs[count] = '\0';
    return tabs;
}

void print_ast_recur(ASTNode* node, int depth) {
    char* tabs = get_tabs(depth);
    if (node == NULL) {
        printf("%sNULL\n", tabs);
        free(tabs);
        return;
    }
    switch (node->type) {
        case AST_NUMBER:
            printf("%sNUMBER: %lf\n", tabs, node->content.number);
            break;
        case AST_BINARY:
            printf("%sB OPERATOR: %d\n", tabs, node->content.binary.oper);
            print_ast_recur(node->content.binary.lhs, depth + 4);
            print_ast_recur(node->content.binary.rhs, depth + 4);
            break;
        case AST_UNARY:
            printf("%sU OPERATOR: %d\n", tabs, node->content.unary.oper);
            print_ast_recur(node->content.unary.val, depth + 4);
            break;
    }
    free(tabs);
}

void print_ast(ASTNode* root) {
    print_ast_recur(root, 0);
}

void free_ast(ASTNode* node) {
    switch (node->type) {
        case AST_BINARY: {
            free_ast(node->content.binary.lhs);
            free_ast(node->content.binary.rhs);
            break;
        }
        case AST_UNARY: {
            free_ast(node->content.unary.val);
            break;
        }
        case AST_NUMBER:
            break;
    }
    free(node);
}

ASTNode* parse_expression(char* expr) {
    return create_ast(tokenize(expr));
}