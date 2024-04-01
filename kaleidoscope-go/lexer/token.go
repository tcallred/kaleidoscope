package lexer

type Token interface {
	token()
}

type TokEof struct{}

type TokDef struct{}

type TokExtern struct{}

type TokIdentifier struct {
	Value string
}

type TokNumber struct {
	Value float64
}

type TokOther struct {
	Value byte
}

func (t *TokEof) token()        {}
func (t *TokDef) token()        {}
func (t *TokExtern) token()     {}
func (t *TokIdentifier) token() {}
func (t *TokNumber) token()     {}
func (t *TokOther) token()      {}
