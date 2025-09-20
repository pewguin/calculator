#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

int main(int argc, char* argv[]) {
    char expr[256];
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            strcat(expr, argv[i]);
        }
    } else {
        if (fgets(expr, sizeof(expr), stdin) == NULL) {
            return 1;
        }
    }
    ASTNode* root = parse_expression(expr);
    printf("%g\n", evaluate(root));
}