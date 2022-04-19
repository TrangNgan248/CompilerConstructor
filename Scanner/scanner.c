/* Scanner
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
FILE* f;

/***************************************************************/

void skipBlank()
{
  // TODO
  while ((currentChar != EOF) &&
         (charCodes[currentChar] == CHAR_SPACE))
    readChar();
}

void skipComment()
{
  // TODO
  int state = 0;
  readChar();
  while (currentChar != EOF && state < 2)
  {
    if (charCodes[currentChar] == CHAR_TIMES)
    {
      state = 1;
    }
    else if (charCodes[currentChar] == CHAR_RPAR)
    {
      if (state == 1)
      {
        state = 2;
      }
      else
      {
        state = 0;
      }
    }
    readChar();
  }
  if (state != 2)
  {
    error(f,ERR_ENDOFCOMMENT, lineNo, colNo);
  }
}

Token *readIdentKeyword(void)
{
  // TODO
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int i = 0;
  while (currentChar != NULL && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT))
  {
    token->string[i] = (char)currentChar;
    i++;
    readChar();
  }
  if (i > MAX_IDENT_LEN)
  {
    error(f,ERR_IDENTTOOLONG, lineNo, colNo);
  }
  else
  {
    TokenType type = checkKeyword(token->string);

    if (type != TK_NONE)
    {
      token->tokenType = type;
    }
  }
  return token;
}

Token *readNumber(void)
{
  // TODO
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int i = 0;
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT))
  {
    token->string[i] = (char)currentChar;
    i++;
    readChar();
  }
  if (i > MAX_NUM_LEN)
  {
    error(f,ERR_NUMBERTOOLONG, lineNo, colNo);
    return token;
  }
  token->string[i] = '\0';
  token->value = atoi(token->string);
  return token;
}

Token *readConstChar(void)
{
  // TODO
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  readChar();

  if (currentChar == EOF)
  {
    token->tokenType = TK_NONE;
    error(f,ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  token->string[0] = currentChar;
  token->string[1] = '\0';
  readChar();

  if (charCodes[currentChar] != CHAR_SINGLEQUOTE)
  {
    token->tokenType = TK_NONE;
    error(f,ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  else
  {
    readChar();
    return token;
  }
}

Token *getToken(void)
{
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();
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
  case CHAR_LT:
    token = makeToken(SB_LT, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_LE;
      readChar();
    }
    return token;
  case CHAR_GT:
    token = makeToken(SB_GT, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_GE;
      readChar();
    }
    return token;
  case CHAR_EXCLAIMATION:
    token = makeToken(SB_NEQ, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      return token;
    }
    else
    {
      error(f,ERR_INVALIDSYMBOL, lineNo, colNo);
    }
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    if (currentChar == ')')
    {
      token->tokenType = SB_RSEL;
      readChar();
    }
    return token;
  case CHAR_COLON:
    token = makeToken(SB_COLON, lineNo, colNo);
    readChar();
    if (currentChar == '=')
    {
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LPAR:
    token = makeToken(SB_LPAR, lineNo, colNo);
    readChar();
    switch (currentChar)
    {
    case '.':
      token->tokenType = SB_LSEL;
      readChar();
      break;
    case '*':
      skipComment();
      getToken();
    }
    return token;
  case CHAR_RPAR:
    token = makeToken(SB_LPAR, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(f,ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

/******************************************************************/

void printToken(Token *token, FILE* f)
{

  fprintf(f, "%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    fprintf(f,"TK_NONE\n");
    break;
  case TK_IDENT:
    fprintf(f,"TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    fprintf(f,"TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    fprintf(f,"TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    fprintf(f,"TK_EOF\n");
    break;

  case KW_PROGRAM:
    fprintf(f,"KW_PROGRAM\n");
    break;
  case KW_CONST:
    fprintf(f,"KW_CONST\n");
    break;
  case KW_TYPE:
    fprintf(f,"KW_TYPE\n");
    break;
  case KW_VAR:
    fprintf(f,"KW_VAR\n");
    break;
  case KW_INTEGER:
    fprintf(f,"KW_INTEGER\n");
    break;
  case KW_CHAR:
    fprintf(f,"KW_CHAR\n");
    break;
  case KW_ARRAY:
    fprintf(f,"KW_ARRAY\n");
    break;
  case KW_OF:
    fprintf(f,"KW_OF\n");
    break;
  case KW_FUNCTION:
    fprintf(f,"KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    fprintf(f,"KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    fprintf(f,"KW_BEGIN\n");
    break;
  case KW_END:
    fprintf(f,"KW_END\n");
    break;
  case KW_CALL:
    fprintf(f,"KW_CALL\n");
    break;
  case KW_IF:
    fprintf(f,"KW_IF\n");
    break;
  case KW_THEN:
    fprintf(f,"KW_THEN\n");
    break;
  case KW_ELSE:
    fprintf(f,"KW_ELSE\n");
    break;
  case KW_WHILE:
    fprintf(f,"KW_WHILE\n");
    break;
  case KW_DO:
    fprintf(f,"KW_DO\n");
    break;
  case KW_FOR:
    fprintf(f,"KW_FOR\n");
    break;
  case KW_TO:
    fprintf(f,"KW_TO\n");
    break;

  case SB_SEMICOLON:
    fprintf(f,"SB_SEMICOLON\n");
    break;
  case SB_COLON:
    fprintf(f,"SB_COLON\n");
    break;
  case SB_PERIOD:
    fprintf(f,"SB_PERIOD\n");
    break;
  case SB_COMMA:
    fprintf(f,"SB_COMMA\n");
    break;
  case SB_ASSIGN:
    fprintf(f,"SB_ASSIGN\n");
    break;
  case SB_EQ:
    fprintf(f,"SB_EQ\n");
    break;
  case SB_NEQ:
    fprintf(f,"SB_NEQ\n");
    break;
  case SB_LT:
    fprintf(f,"SB_LT\n");
    break;
  case SB_LE:
    fprintf(f,"SB_LE\n");
    break;
  case SB_GT:
    fprintf(f,"SB_GT\n");
    break;
  case SB_GE:
    fprintf(f,"SB_GE\n");
    break;
  case SB_PLUS:
    fprintf(f,"SB_PLUS\n");
    break;
  case SB_MINUS:
    fprintf(f,"SB_MINUS\n");
    break;
  case SB_TIMES:
    fprintf(f,"SB_TIMES\n");
    break;
  case SB_SLASH:
    fprintf(f,"SB_SLASH\n");
    break;
  case SB_LPAR:
    fprintf(f,"SB_LPAR\n");
    break;
  case SB_RPAR:
    fprintf(f,"SB_RPAR\n");
    break;
  case SB_LSEL:
    fprintf(f,"SB_LSEL\n");
    break;
  case SB_RSEL:
    fprintf(f,"SB_RSEL\n");
    break;
  }
}

int scan(char *fileName)
{
  Token *token;
  FILE* f = fopen("result.txt", "w+");

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token, f);
    free(token);
    token = getToken();
  }

  free(token);
  fclose(f);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

// int main(int argc, char *argv[])
// {
//   if (argc <= 1)
//   {
//     printf("scanner: no input file.\n");
//     return -1;
//   }

//   if (scan(argv[1]) == IO_ERROR)
//   {
//     printf("Can\'t read input file!\n");
//     return -1;
//   }

//   return 0;
// }

int main()
{

  if (scan("example2.kpl") == IO_ERROR)
  {
    printf("Can\'t read input file!\n");
  }
  return 0;
}
