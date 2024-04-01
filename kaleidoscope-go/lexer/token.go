package lexer

import "fmt"

type Token interface {
	fmt.Stringer
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

func (t *TokEof) String() string {
	return "Eof"
}
func (t *TokDef) String() string {
	return "Def"
}
func (t *TokExtern) String() string {
	return "Extern"
}
func (t *TokIdentifier) String() string {
	return fmt.Sprintf("Identifier{ %q }", t.Value)
}
func (t *TokNumber) String() string {
	return fmt.Sprintf("Number{ %f }", t.Value)
}
func (t *TokOther) String() string {
	return fmt.Sprintf("Other{ %q }", string(t.Value))
}
