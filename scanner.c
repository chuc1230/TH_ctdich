/* 
 * Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;
extern CharCode charCodes[];

void skipBlank() {
  while (charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

void skipComment() {
  readChar(); // Skip the first '/'
  if (currentChar == '/') { // Single-line comment
    while (currentChar != '\n' && currentChar != EOF)
      readChar();
  } else if (currentChar == '*') { // Multi-line comment
    readChar();
    while (currentChar != '*' && currentChar != EOF) {
      if (currentChar == '\n')
        lineNo++;
      readChar();
    }
    if (currentChar == '*') {
      readChar();
      if (currentChar == '/')
        readChar();
    }
  }
}

Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int i = 0;
  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
    if (i < MAX_IDENT_LEN)
      token->string[i++] = currentChar;
    readChar();
  }
  token->string[i] = '\0';
  
  TokenType keywordType = checkKeyword(token->string);
  if (keywordType != TK_NONE)
    token->tokenType = keywordType;
  return token;
}

Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int value = 0;
  while (charCodes[currentChar] == CHAR_DIGIT) {
    value = value * 10 + (currentChar - '0');
    readChar();
  }
  token->value = value;
  return token;
}

Token* readString(void) { // Đọc chuỗi
  Token *token = makeToken(TK_STRING, lineNo, colNo);
  int i = 0;
  readChar(); // Bỏ qua dấu " đầu tiên
  while (currentChar != '"' && currentChar != EOF) { // Tìm dấu " kết thúc
    if (i < MAX_IDENT_LEN)
      token->string[i++] = currentChar;
    readChar();
  }
  token->string[i] = '\0';
  readChar(); // Bỏ qua dấu " cuối cùng
  return token;
}

Token* readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  readChar(); // Skip the opening single quote
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  token->string[0] = currentChar;
  token->string[1] = '\0';
  readChar();
  if (currentChar == EOF || charCodes[currentChar] != CHAR_SINGLEQUOTE) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  readChar(); // Skip the closing single quote
  return token;
}

Token* getToken(void) {
  Token *token;
  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_QUOTE: return readString(); // Thêm xử lý chuỗi
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_MINUS: 
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_TIMES: 
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_SLASH: 
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_PERCENT: // Thêm phép toán %
    token = makeToken(SB_MOD, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_LT: 
    token = makeToken(SB_LT, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_GT: 
    token = makeToken(SB_GT, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_EXCLAIMATION:
    token = makeToken(SB_EXCLAIMATION, lineNo, colNo); // Thêm dấu chấm than
    readChar();
    return token;
  case CHAR_SLASH: 
    readChar();
    if (currentChar == '/') { skipComment(); return getToken(); } 
    break;
  default: 
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    return token;
  }
  return NULL;
}


/******************************************************************/

// Hàm in token ra màn hình
void printToken(Token *token) {
  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%d)\n", token->value); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  default: break;
  }
}



int scan(char *fileName) {
  Token *token;


  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;


  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }


  free(token);
  closeInputStream();
  return IO_SUCCESS;
}


/******************************************************************/


int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }


  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}
