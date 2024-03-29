package lexer

type Token interface {
	token()
}

type TokEof struct{}

func (t *TokEof) token() {}

type TokDef struct{}

func (t *TokDef) token() {}

type TokExtern struct{}

func (t *TokExtern) token() {}

type TokIdentifier struct {
	Value string
}

func (t *TokIdentifier) token() {}

type TokNumber struct {
	Value float64
}

func (t *TokNumber) token() {}

type TokOther struct {
	Value byte
}

func (t *TokOther) token() {}
