package parser

type ExprAST interface {
	exprAST()
}

type Number struct {
	Value float64
}

type Variable struct {
	Value string
}

type Binop struct {
	Op  byte
	Lhs ExprAST
	Rhs ExprAST
}

type Call struct {
	Callee string
	Args   []ExprAST
}

type PrototypeAST struct {
	Name string
	Args []ExprAST
}

type FunctionAST struct {
	Proto PrototypeAST
	Body  ExprAST
}
