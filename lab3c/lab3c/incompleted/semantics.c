/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  Scope* scope = symtab->currentScope;
  Object* obj;

  while (scope != NULL) {
    obj = findObject(scope->objList, name);
    if (obj != NULL) return obj;
    scope = scope->outer;
  }
  obj = findObject(symtab->globalObjectList, name);
  return obj;
}

// Kiểm tra trùng tên khi khai báo (Chỉ kiểm tra scope hiện tại)
void checkFreshIdent(char *name) {
  Object* obj = findObject(symtab->currentScope->objList, name);
  if (obj != NULL) {
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
  }
}

// Kiểm tra định danh đã khai báo chưa (Chung)
Object* checkDeclaredIdent(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

// Kiểm tra xem có phải là Hằng số không
Object* checkDeclaredConstant(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }
  if (obj->kind != OBJ_CONSTANT) {
    error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

// Kiểm tra xem có phải là Kiểu dữ liệu không
Object* checkDeclaredType(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  }
  if (obj->kind != OBJ_TYPE) {
    error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

// Kiểm tra xem có phải là Biến (hoặc Tham số) không
Object* checkDeclaredVariable(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
  if (obj->kind != OBJ_VARIABLE && obj->kind != OBJ_PARAMETER) {
    error(ERR_INVALID_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

// Kiểm tra xem có phải là Hàm không
Object* checkDeclaredFunction(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }
  if (obj->kind != OBJ_FUNCTION) {
    error(ERR_INVALID_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

// Kiểm tra xem có phải là Thủ tục không
Object* checkDeclaredProcedure(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }
  if (obj->kind != OBJ_PROCEDURE) {
    error(ERR_INVALID_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

// Kiểm tra định danh ở vế trái lệnh gán (L-Value)
Object* checkDeclaredLValueIdent(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }

  switch (obj->kind) {
  case OBJ_VARIABLE:
  case OBJ_PARAMETER:
    break;
  case OBJ_FUNCTION:
    // Nếu gán cho tên hàm, thì phải đang ở trong chính hàm đó (giá trị trả về)
    if (obj != symtab->currentScope->owner) {
      error(ERR_INVALID_IDENT, currentToken->lineNo, currentToken->colNo);
    }
    break;
  default:
    error(ERR_INVALID_IDENT, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

