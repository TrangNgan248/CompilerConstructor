/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void error(FILE* f,ErrorCode err, int lineNo, int colNo)
{
  switch (err)
  {
  case ERR_ENDOFCOMMENT:
    fprintf(f,"%d-%d:%s\n", lineNo, colNo, ERM_ENDOFCOMMENT);
    break;
  case ERR_IDENTTOOLONG:
    fprintf(f,"%d-%d:%s\n", lineNo, colNo, ERM_IDENTTOOLONG);
    break;
  case ERR_INVALIDCHARCONSTANT:
    fprintf(f,"%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCHARCONSTANT);
    break;
  case ERR_INVALIDSYMBOL:
    fprintf(f,"%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSYMBOL);
    break;
  case ERR_NUMBERTOOLONG:
    fprintf(f,"%d-%d:%s\n", lineNo, colNo, ERM_NUMBERTOOLONG);
    break;
  }
  exit(-1);
}
