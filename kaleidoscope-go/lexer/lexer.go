package lexer

import "unicode"

func skipWhitespace(input []byte, idx int) int {
	for idx < len(input) && unicode.IsSpace(rune(input[idx])) {
		idx += 1
	}
	return idx
}
