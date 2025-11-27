/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) { //Khai báo hằng
  if(lookAhead->tokenType == TK_IDENT){
    compileConstDecl();
    compileConstDecls();
  }
  //
}

void compileConstDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) { //Khai báo kiểu
  if(lookAhead->tokenType == TK_IDENT){
    compileTypeDecl();
    compileTypeDecls();
  }
}

void compileTypeDecl(void) { //Khai báo kiểu
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) { //Khai báo biến
  if (lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
    compileVarDecls(); // Đệ quy nếu còn khai báo biến tiếp theo
  }
}

void compileVarDecl(void) { //Khai báo biến bao gồm tên và kiểu
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) { //Xử lý Khai báo con
  assert("Parsing subtoutines ....");
  if (lookAhead->tokenType == KW_FUNCTION) {
      compileFuncDecl();
      compileSubDecls();
  } else if (lookAhead->tokenType == KW_PROCEDURE) {
      compileProcDecl();
      compileSubDecls();
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {  // FUNCTION Ident Params : BasicType ; Block ;
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void) {  // PROCEDURE Ident Params ; Block ;
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) { // Xử lý số không dấu
  switch(lookAhead->tokenType) {
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    case TK_IDENT:
      eat(TK_IDENT);
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    default:
      error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileConstant(void) { // +, -, hằng ký tự
  switch(lookAhead->tokenType) {
    case SB_PLUS:
      eat(SB_PLUS);
      compileConstant2();
      break;
    case SB_MINUS:
      eat(SB_MINUS);
      compileConstant2();
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    default:
      compileConstant2();
      break;
  }
}

void compileConstant2(void) { //Định danh hằng hoặc số nguyên
  switch(lookAhead->tokenType) {
    case TK_IDENT:
      eat(TK_IDENT);
      break;
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    default:
      error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileType(void) { //INTEGER, CHAR, ARR, 
  switch(lookAhead->tokenType) {
    case KW_INTEGER:
      eat(KW_INTEGER);
      break;
    case KW_CHAR:
      eat(KW_CHAR);
      break;
    case KW_ARRAY:
      eat(KW_ARRAY);
      eat(SB_LSEL);
      eat(TK_NUMBER);
      eat(SB_RSEL);
      eat(KW_OF);
      compileType();
      break;
    case TK_IDENT:
      eat(TK_IDENT);
      break;
    default:
      error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileBasicType(void) { //INTEGER, CHAR (dùng cho tham số hàm)
  switch(lookAhead->tokenType) {
    case KW_INTEGER:
      eat(KW_INTEGER);
      break;
    case KW_CHAR:
      eat(KW_CHAR);
      break;
    default:
      error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileParams(void) { // Tổng quát danh sách tham số: ( ... ) hoặc Rỗng
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}

void compileParams2(void) { // Các tham số tiếp theo (phân cách bởi dấu chấm phẩy)
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
}

void compileParam(void) { // tham số đơn lẻ: [VAR] Ident : BasicType
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
  }
  eat(TK_IDENT);
  eat(SB_COLON);
  compileBasicType();
}

void compileStatements(void) {  // Statements ::= Statement Statements2
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {  // Lệnh gán: Variable := Expression
  // Tên biến đã được ăn ở hàm compileStatement trước khi gọi hàm này
  // Bắt đầu xử lý phần Index (nếu có) và dấu :=
  assert("Parsing an assign statement ....");
  eat(TK_IDENT);
  compileIndexes(); // Xử lý nếu là mảng A(.1.)
  eat(SB_ASSIGN);
  compileExpression();
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {  // Lệnh gọi thủ tục: Call Proc(...)
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) { // Khối lệnh Group: BEGIN ... END
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements(); // <--- SỬA THÀNH CÁI NÀY
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) { // WHILE ... DO ...
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {  // Lệnh FOR ... TO ... DO ...
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  eat(TK_IDENT); // Biến chạy
  eat(SB_ASSIGN);
  compileExpression(); // Giá trị đầu
  eat(KW_TO);
  compileExpression(); // Giá trị cuối
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileArguments(void) {  // Danh sách tham số truyền vào: (a, b, c)
  switch(lookAhead->tokenType) {
    case SB_LPAR:
      eat(SB_LPAR);
      compileExpression();
      compileArguments2();
      eat(SB_RPAR);
      break;
    // Follow set of Arguments (nếu không có tham số)
    case SB_SEMICOLON:
    case KW_END:
    case KW_ELSE:
    case SB_TIMES:
    case SB_SLASH:
    case SB_PLUS:
    case SB_MINUS:
    case SB_RPAR:
    case SB_COMMA:
      break;
    default:
      break;
  }
}

void compileArguments2(void) {
  if (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  }
}

void compileCondition(void) {  // Condition ::= Expression Condition2
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {  // Condition2 ::= [=, <, >, <=, >=, !=] Expression
  switch(lookAhead->tokenType) {
    case SB_EQ:
      eat(SB_EQ);
      compileExpression();
      break;
    case SB_NEQ:
      eat(SB_NEQ);
      compileExpression();
      break;
    case SB_LE:
      eat(SB_LE);
      compileExpression();
      break;
    case SB_LT:
      eat(SB_LT);
      compileExpression();
      break;
    case SB_GE:
      eat(SB_GE);
      compileExpression();
      break;
    case SB_GT:
      eat(SB_GT);
      compileExpression();
      break;
    default:
      break;
  }
}

void compileExpression(void) { //Xử lí biểu thức + -     (Expression ::= [+-] Term Expression2)
  assert("Parsing an expression");
  switch(lookAhead->tokenType) {
    case SB_PLUS:
      eat(SB_PLUS);
      compileTerm();
      compileExpression2();
      break;
    case SB_MINUS:
      eat(SB_MINUS);
      compileTerm();
      compileExpression2();
      break;
    default:
      compileTerm();
      compileExpression2();
      break;
  }
  assert("Expression parsed");
}

void compileExpression2(void) {  // Expression2 ::= { [+-] Term }
  switch(lookAhead->tokenType) {
    case SB_PLUS:
      eat(SB_PLUS);
      compileTerm();
      compileExpression2();
      break;
    case SB_MINUS:
      eat(SB_MINUS);
      compileTerm();
      compileExpression2();
      break;
    default:
      break;
  }
}


void compileExpression3(void) {
  // TODO
}

void compileTerm(void) { //  Xử lý Số hạng (Nhân/Chia)   Term ::= Factor Term2
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {  // Term2 ::= { [*/] Factor }
  switch(lookAhead->tokenType) {
    case SB_TIMES:
      eat(SB_TIMES);
      compileFactor();
      compileTerm2();
      break;
    case SB_SLASH:
      eat(SB_SLASH);
      compileFactor();
      compileTerm2();
      break;
    default:
      break;
  }
}

void compileFactor(void) {  //  Xử lý Nhân tử (Số, Biến, Hàm, Ngoặc)
  switch(lookAhead->tokenType) {
    case TK_NUMBER:
    case TK_CHAR:
      compileUnsignedConstant();
      break;
    case SB_LPAR: // Biểu thức trong ngoặc
      eat(SB_LPAR);
      compileExpression();
      eat(SB_RPAR);
      break;
    case TK_IDENT: //Là tên biến hoặc hàm
      eat(TK_IDENT);
      // Kiểm tra xem có phải chỉ số mảng (. .), hoặc tham số hàm ( ... )
      switch(lookAhead->tokenType) {
        case SB_LSEL:
          compileIndexes();
          break;
        case SB_LPAR: //Hàm: Sum(a,b)
          compileArguments();
          break;
        default:
          break;
      }
      break;
    default:
      error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileIndexes(void) {  // Xử lý chỉ số mảng (. . .)
  while(lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
