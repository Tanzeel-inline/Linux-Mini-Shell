#pragma once
#include <stdio.h>
/* A struct that represents a list of words. */
struct tokens;

/* Turn a string into a list of words. */
struct tokens *tokenize(const char *line);

/* How many words are there? */
size_t tokens_get_length(struct tokens *tokens);

/* Get me the Nth word (zero-indexed) */
char *tokens_get_token(struct tokens *tokens, size_t n);

/* Free the memory */
void tokens_destroy(struct tokens *tokens);

int find_pipe_delimiter(struct tokens *tokens, int current_position);

void copy_tokens(char ***arr_tokens,int start_position, struct tokens *tokens);


char *str_tok(char *_main_string, char _delimiter, int *_start_position);
