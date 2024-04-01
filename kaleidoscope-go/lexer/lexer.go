package lexer

import (
	"strconv"
	"strings"
	"unicode"
)

func skipWhitespace(input []byte, idx int) int {
	for idx < len(input) && unicode.IsSpace(rune(input[idx])) {
		idx++
	}
	return idx
}

func isAlphaNumeric(c byte) bool {
	return unicode.IsLetter(rune(c)) || unicode.IsDigit(rune(c))
}

func lexKeywordOrIdentifier(input []byte, tokens []Token, idx int) ([]Token, int) {
	var sb strings.Builder
	for isAlphaNumeric(input[idx]) {
		sb.WriteByte(input[idx])
		idx++
	}
	idStr := sb.String()
	if idStr == "def" {
		tokens = append(tokens, &TokDef{})
	} else if idStr == "extern" {
		tokens = append(tokens, &TokExtern{})
	} else {
		tokens = append(tokens, &TokIdentifier{Value: idStr})
	}
	return tokens, idx
}

func lexNumber(input []byte, tokens []Token, idx int) ([]Token, int) {
	var sb strings.Builder
	for unicode.IsDigit(rune(input[idx])) || input[idx] == '.' {
		sb.WriteByte(input[idx])
		idx++
	}
	num, _ := strconv.ParseFloat(sb.String(), 64)
	tokens = append(tokens, &TokNumber{Value: num})
	return tokens, idx
}

func skipComment(input []byte, idx int) int {
	for input[idx] != '\n' && input[idx] != '\r' {
		idx++
	}
	return idx
}

func Lex(input []byte) []Token {
	idx := 0
	tokens := make([]Token, 0, 128)
	for idx < len(input) {
		idx = skipWhitespace(input, idx)
		if idx >= len(input) {
			break
		}
		if unicode.IsLetter(rune(input[idx])) {
			tokens, idx = lexKeywordOrIdentifier(input, tokens, idx)
		} else if unicode.IsDigit(rune(input[idx])) || input[idx] == '.' {
			tokens, idx = lexNumber(input, tokens, idx)
		} else if input[idx] == '#' {
			idx = skipComment(input, idx)
		} else {
			tokens = append(tokens, &TokOther{Value: input[idx]})
			idx++
		}
	}
	tokens = append(tokens, &TokEof{})

	return tokens
}
