#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

struct tokens {
  size_t tokens_length;
  char **tokens;
  size_t buffers_length;
  char **buffers;
};

static void *vector_push(char ***pointer, size_t *size, void *elem) {
  *pointer = (char**) realloc(*pointer, sizeof(char *) * (*size + 1));
  (*pointer)[*size] = elem;
  *size += 1;
  return elem;
}

static void *copy_word(char *source, size_t n) {
  source[n] = '\0';
  char *word = (char *) malloc(n + 1);
  strncpy(word, source, n + 1);
  return word;
}

struct tokens *tokenize(const char *line) {
  if (line == NULL) {
    return NULL;
  }

  static char token[4096];
  size_t n = 0, n_max = 4096;
  struct tokens *tokens;
  size_t line_length = strlen(line);

  tokens = (struct tokens *) malloc(sizeof(struct tokens));
  tokens->tokens_length = 0;
  tokens->tokens = NULL;
  tokens->buffers_length = 0;
  tokens->buffers = NULL;

  const int MODE_NORMAL = 0,
        MODE_SQUOTE = 1,
        MODE_DQUOTE = 2;
  int mode = MODE_NORMAL;

  for (unsigned int i = 0; i < line_length; i++) {
    char c = line[i];
    if (mode == MODE_NORMAL) {
      if (c == '\'') {
        mode = MODE_SQUOTE;
      } else if (c == '"') {
        mode = MODE_DQUOTE;
      } else if (c == '\\') {
        if (i + 1 < line_length) {
          token[n++] = line[++i];
        }
      } else if (isspace(c)) {
        if (n > 0) {
          void *word = copy_word(token, n);
          vector_push(&tokens->tokens, &tokens->tokens_length, word);
          n = 0;
        }
      } else {
        token[n++] = c;
      }
    } else if (mode == MODE_SQUOTE) {
      if (c == '\'') {
        mode = MODE_NORMAL;
      } else if (c == '\\') {
        if (i + 1 < line_length) {
          token[n++] = line[++i];
        }
      } else {
        token[n++] = c;
      }
    } else if (mode == MODE_DQUOTE) {
      if (c == '"') {
        mode = MODE_NORMAL;
      } else if (c == '\\') {
        if (i + 1 < line_length) {
          token[n++] = line[++i];
        }
      } else {
        token[n++] = c;
      }
    }
    if (n + 1 >= n_max) abort();
  }

  if (n > 0) {
    void *word = copy_word(token, n);
    vector_push(&tokens->tokens, &tokens->tokens_length, word);
    n = 0;
  }
  return tokens;
}

size_t tokens_get_length(struct tokens *tokens) {
  if (tokens == NULL) {
    return 0;
  } else {
    return tokens->tokens_length;
  }
}

char *tokens_get_token(struct tokens *tokens, size_t n) {
  if (tokens == NULL || n >= tokens->tokens_length) {
    return NULL;
  } else {
    return tokens->tokens[n];
  }
}

void tokens_destroy(struct tokens *tokens) {
  if (tokens == NULL) {
    return;
  }
  for (int i = 0; i < tokens->tokens_length; i++) {
    free(tokens->tokens[i]);
  }
  for (int i = 0; i < tokens->buffers_length; i++) {
    free(tokens->buffers[i]);
  }
  if (tokens->tokens) {
    free(tokens->tokens);
  }
  free(tokens);
}
void copy_tokens(char ***arr_tokens,int start_position, struct tokens *tokens)
{
        //Get the length
        int length = 0;
        int i;
        for ( i = start_position ; i < tokens_get_length(tokens) ; i++ )
        {
                if ( strcmp(tokens_get_token(tokens,i),"<") == 0) 
                {
                        i += 1;
                        continue;
                }
                else if ( strcmp(tokens_get_token(tokens,i),">") == 0)
                {
                        i += 1;
                        continue;
                }
                if ( strcmp(tokens_get_token(tokens,i),"|") == 0 )
                {
                        break;
                }
                length = length + 1;
        }

        //printf("Length of token is : %d\n",length);
        int len = (length) + 1;
        *arr_tokens = malloc( len *sizeof(char*));
        arr_tokens[0][len - 1] = NULL;
        int j;
        for ( i = start_position , j = 0 ; j < length ; i++ )
        {
                if ( strcmp(tokens_get_token(tokens,i),"<") == 0) 
                {
                        i += 1;
                        continue;
                }
                else if ( strcmp(tokens_get_token(tokens,i),">") == 0)
                {
                        i += 1;
                        continue;
                }
                arr_tokens[0][j] = tokens_get_token(tokens,i);
                j++;
        }
        //return i;
}
int find_pipe_delimiter(struct tokens *tokens, int current_position)
{
        for ( int i = current_position ; i < tokens_get_length(tokens) ; i++ )
        {
                if ( strcmp(tokens_get_token(tokens,i),"|") == 0 )
                {
                        return i;
                }
        }
        return -1;
}

char *str_tok(char *_main_string, char _delimiter, int *_start_position)
{
        if ( _main_string[*_start_position] == _delimiter )
        {
                *_start_position += 1;
        }
        int count = 0;
        for ( int i = *_start_position ; _main_string[i] != '\0' ; i++ )
        {
                if ( _main_string[i] == _delimiter )
                {
                        break;
                }
                count++;
        }
        if ( count == 0 )
        {
                return NULL;
        }
        char *_new_string = (char*)malloc(count * sizeof(char));
        for ( int j = 0 ; j < count ; *_start_position += 1  , j++ )
        {
                _new_string[j] = _main_string[*_start_position];
        }

        char *_return_string = _new_string;
        return _return_string;
}
