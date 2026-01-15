/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"
#include "codegen.h"  

#define CODE_SIZE 10000
extern SymTab* symtab;

extern Object* readiFunction;
extern Object* readcFunction;
extern Object* writeiProcedure;
extern Object* writecProcedure;
extern Object* writelnProcedure;

CodeBlock* codeBlock;

void genVariableAddress(Object* var) {
  int level = 0;
  int offset = VARIABLE_OFFSET(var);
  genLA(level, offset);
}

void genVariableValue(Object* var) {
  int level = 0;
  int offset = VARIABLE_OFFSET(var);
  genLV(level, offset);
}

void genParameterValue(Object* obj) {
  // 1. Tính Level: Khoảng cách từ Scope hiện tại đến Scope chứa tham số
  int level = 0;
  Scope* scope = symtab->currentScope;
  
  // Duyệt ngược lên các scope cha cho đến khi tìm thấy scope khai báo tham số
  while (scope != obj->paramAttrs->scope) {
    level++;
    scope = scope->outer;
  }

  // 2. Sinh lệnh LV (Load Value)
  // Lấy giá trị tại: Stack Frame của cha (cách 'level' bước) + Offset của tham số
  genLV(level, obj->paramAttrs->localOffset);
}

void genFunctionCall(Object* obj) {
  // 1. Tính Level: Để truyền Static Link (SL) cho hàm được gọi
  // Ta cần tìm scope nơi hàm này được KHAI BÁO (Defined), tức là scope cha của nó
  int level = 0;
  Scope* scope = symtab->currentScope;
  Scope* definedScope = obj->funcAttrs->scope->outer;

  while (scope != definedScope) {
    level++;
    scope = scope->outer;
  }

  // 2. Sinh lệnh CALL
  // Nhảy đến địa chỉ codeAddress của hàm, mang theo Static Link tương ứng với level
  genCALL(level, obj->funcAttrs->codeAddress);
}

int isPredefinedFunction(Object* func) {
  return ((func == readiFunction) || (func == readcFunction));
}

int isPredefinedProcedure(Object* proc) {
  return ((proc == writeiProcedure) || (proc == writecProcedure) || (proc == writelnProcedure));
}

void genPredefinedProcedureCall(Object* proc) {
  if (proc == writeiProcedure)
    genWRI();
  else if (proc == writecProcedure)
    genWRC();
  else if (proc == writelnProcedure)
    genWLN();
}

void genPredefinedFunctionCall(Object* func) {
  if (func == readiFunction)
    genRI();
  else if (func == readcFunction)
    genRC();
}

void genLA(int level, int offset) {
  emitLA(codeBlock, level, offset);
}

void genLV(int level, int offset) {
  emitLV(codeBlock, level, offset);
}

void genLC(WORD constant) {
  emitLC(codeBlock, constant);
}

void genLI(void) {
  emitLI(codeBlock);
}

void genINT(int delta) {
  emitINT(codeBlock,delta);
}

void genDCT(int delta) {
  emitDCT(codeBlock,delta);
}

Instruction* genJ(CodeAddress label) {
  Instruction* inst = codeBlock->code + codeBlock->codeSize;
  emitJ(codeBlock,label);
  return inst;
}

Instruction* genFJ(CodeAddress label) {
  Instruction* inst = codeBlock->code + codeBlock->codeSize;
  emitFJ(codeBlock, label);
  return inst;
}

void genHL(void) {
  emitHL(codeBlock);
}

void genST(void) {
  emitST(codeBlock);
}

void genCALL(int level, CodeAddress label) {
  emitCALL(codeBlock, level, label);
}

void genEP(void) {
  emitEP(codeBlock);
}

void genEF(void) {
  emitEF(codeBlock);
}

void genRC(void) {
  emitRC(codeBlock);
}

void genRI(void) {
  emitRI(codeBlock);
}

void genWRC(void) {
  emitWRC(codeBlock);
}

void genWRI(void) {
  emitWRI(codeBlock);
}

void genWLN(void) {
  emitWLN(codeBlock);
}

void genAD(void) {
  emitAD(codeBlock);
}

void genSB(void) {
  emitSB(codeBlock);
}

void genML(void) {
  emitML(codeBlock);
}

void genDV(void) {
  emitDV(codeBlock);
}

void genNEG(void) {
  emitNEG(codeBlock);
}

void genCV(void) {
  emitCV(codeBlock);
}

void genEQ(void) {
  emitEQ(codeBlock);
}

void genNE(void) {
  emitNE(codeBlock);
}

void genGT(void) {
  emitGT(codeBlock);
}

void genGE(void) {
  emitGE(codeBlock);
}

void genLT(void) {
  emitLT(codeBlock);
}

void genLE(void) {
  emitLE(codeBlock);
}

void updateJ(Instruction* jmp, CodeAddress label) {
  jmp->q = label;
}

void updateFJ(Instruction* jmp, CodeAddress label) {
  jmp->q = label;
}

CodeAddress getCurrentCodeAddress(void) {
  return codeBlock->codeSize;
}


void initCodeBuffer(void) {
  codeBlock = createCodeBlock(CODE_SIZE);
}

void printCodeBuffer(void) {
  printCodeBlock(codeBlock);
}

void cleanCodeBuffer(void) {
  freeCodeBlock(codeBlock);
}

int serialize(char* fileName) {
  FILE* f;

  f = fopen(fileName, "wb");
  if (f == NULL) return IO_ERROR;
  saveCode(codeBlock, f);
  fclose(f);
  return IO_SUCCESS;
}