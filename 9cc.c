#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED,    // special character
    TK_NUM,         // integer
    TK_EOF,         // end of input
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

// input program
char *user_input;

// current token
Token *token;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // output space at "pos" times
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "expected '%c'.", op);
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "expected a number.");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // skip spaces
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "can not tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("invalid number of arguments", argv[0]);
    return 1;
  }

  user_input = argv[1];
  token = tokenize();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // first token of expression must be digit.
  // check and output 'mov' assembly.
  printf("  mov rax, %d\n", expect_number());

  while (!at_eof()) {
      if (consume('+')) {
          printf("  add rax, %ld\n", expect_number());
          continue;
      }

      expect('-');
      printf("  sub rax, %ld\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}
