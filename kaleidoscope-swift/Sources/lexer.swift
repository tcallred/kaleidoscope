enum Token : Equatable {
  case eof
  case def
  case extern
  case identifier(String)
  case number(Float64)
  case other(Character)
}

class Lexer {
  let input: [Character]
  var idx: Int = 0
  var tokens: [Token]

  init(_ input: String) {
    self.input = Array(input)
    self.tokens = []
  }

  func skipWhitespace() {
    while idx < input.count && input[idx].isWhitespace {
      idx += 1
    }
  }

  func lexKeywordOrId() {
    var idStr = String()
    while idx < input.count && (input[idx].isLetter || input[idx].isNumber) {
      idStr += "\(input[idx])"
      idx += 1
    }
    switch idStr {
    case "def":
      tokens.append(.def)
    case "extern":
      tokens.append(.extern)
    default:
      tokens.append(.identifier(idStr))
    }
  }

  func lexNumber() {
    var numStr = String()
    while input[idx].isNumber || input[idx] == "." {
      numStr += "\(input[idx])"
      idx += 1
    }
    tokens.append(.number(Float64(numStr)!))
  }

  func skipComment() {
    while input[idx] != "\n" && input[idx] != "\r" {
      idx += 1
    }
  }

  public func lex() -> [Token] {
    idx = 0
    tokens = []
    while idx < input.count {
      skipWhitespace()
      if idx >= input.count {
        break
      }
      if input[idx].isLetter {
        lexKeywordOrId()
      } else if input[idx].isNumber {
        lexNumber()
      } else if input[idx] == "#" {
        skipComment()
      } else {
        tokens.append(.other(input[idx]))
        idx += 1
      }
    }
    tokens.append(.eof)
    return tokens
  }
}
