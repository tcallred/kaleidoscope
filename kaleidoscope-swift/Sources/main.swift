
let lexer = Lexer("def fib(): 1 + x")

let tokens = lexer.lex()

print(tokens)