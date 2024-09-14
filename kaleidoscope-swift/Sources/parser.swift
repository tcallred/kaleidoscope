indirect enum ExprAST {
  case number(Float64)
  case variable(String)
  case binop(op: Character, lhs: ExprAST, rhs: ExprAST)
  case call(callee: String, args: [ExprAST]) 
}

struct PrototypeAST {
  let name: String
  let args: [String]
}

struct FunctionAST {
  let proto: PrototypeAST 
  let body: ExprAST
}

struct ParseError: Error {
  let msg: String
}

class Parser {
  let tokens: [Token]
  var idx: Int = 0

  init(tokens: [Token]) {
    self.tokens = tokens
  }

  func expect(_ token: Token) throws {
    if tokens[idx] != token {
      throw ParseError(msg: "Expected \(token), got \(tokens[idx])")
    }
    idx += 1
  }

  func expectId() throws -> String {
    guard case let .identifier(s) = tokens[idx] else {
      throw ParseError(msg: "Expected identifier, got \(tokens[idx])")
    }
    idx += 1
    return s
  }

  func parseNumber() throws -> ExprAST {
    guard case let Token.number(n) = tokens[idx] else {
      throw ParseError(msg: "Expected number, got \(tokens[idx])")
    }
    idx += 1
    return ExprAST.number(n)
  }

  func parseParenExpr() throws -> ExprAST {
    try expect(.other("("))
    let expr = parseExpression()
    try expect(.other(")"))
    return expr
  }

  func parseIdentifierExpr() throws -> ExprAST {
    
  }
}