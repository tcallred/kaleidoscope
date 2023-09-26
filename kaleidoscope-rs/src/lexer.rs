#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Eof,
    Def,
    Extern,
    Identifier(String),
    Number(f64),
    Other(char),
}

fn skip_whitespace(input: &[char], mut idx: usize) -> usize {
    while idx < input.len() && input[idx].is_whitespace() {
        idx += 1;
    }
    idx
}

fn lex_keyword_or_identifier(input: &[char], mut idx: usize, tokens: &mut Vec<Token>) -> usize {
    let mut identifier_str = String::new();
    while input[idx].is_alphanumeric() {
        identifier_str.push(input[idx]);
        idx += 1;
    }
    if identifier_str == "def" {
        tokens.push(Token::Def);
    } else if identifier_str == "extern" {
        tokens.push(Token::Extern);
    } else {
        tokens.push(Token::Identifier(identifier_str));
    }
    idx
}

fn lex_number(input: &[char], mut idx: usize, tokens: &mut Vec<Token>) -> usize {
    let mut num_str = String::new();
    while input[idx].is_numeric() || input[idx] == '.' {
        num_str.push(input[idx]);
        idx += 1;
    }
    tokens.push(Token::Number(num_str.parse().unwrap()));
    idx
}

fn skip_comment(input: &[char], mut idx: usize) -> usize {
    while input[idx] != '\n' && input[idx] != '\r' {
        idx += 1;
    }
    idx
}

pub fn lex(input: &[char]) -> Vec<Token> {
    let mut idx = 0;
    let mut tokens: Vec<Token> = vec![];
    while idx < input.len() {
        idx = skip_whitespace(input, idx);
        if idx >= input.len() {
            break;
        }
        if input[idx].is_alphabetic() {
            idx = lex_keyword_or_identifier(input, idx, &mut tokens)
        } else if input[idx].is_numeric() || input[idx] == '.' {
            idx = lex_number(input, idx, &mut tokens)
        } else if input[idx] == '#' {
            idx = skip_comment(input, idx)
        } else {
            tokens.push(Token::Other(input[idx]));
            idx += 1;
        }
    }

    tokens.push(Token::Eof);
    tokens
}
