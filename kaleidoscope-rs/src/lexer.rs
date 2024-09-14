#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Eof,
    Def,
    Extern,
    Identifier(String),
    Number(f64),
    LParen,
    RParen,
    Comma,
    Semicolon,
    Other(char),
}

pub fn lex(input: &[char]) -> Vec<Token> {
    let mut idx = 0;
    let mut tokens: Vec<Token> = vec![];
    while idx < input.len() {
        while idx < input.len() && input[idx].is_whitespace() {
            idx += 1;
        }
        if idx >= input.len() {
            break;
        }
        if input[idx].is_alphabetic() {
            let tokens: &mut Vec<Token> = &mut tokens;
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
        } else if input[idx].is_numeric() || input[idx] == '.' {
            let tokens: &mut Vec<Token> = &mut tokens;
            let mut num_str = String::new();
            while input[idx].is_numeric() || input[idx] == '.' {
                num_str.push(input[idx]);
                idx += 1;
            }
            tokens.push(Token::Number(num_str.parse().unwrap()));
        } else if input[idx] == '#' {
            while input[idx] != '\n' && input[idx] != '\r' {
                idx += 1;
            }
        } else {
            let tok = match input[idx] {
                '(' => Token::LParen,
                ')' => Token::RParen,
                ',' => Token::Comma,
                ';' => Token::Semicolon,
                _ => Token::Other(input[idx]),
            };
            tokens.push(tok);
            idx += 1;
        }
    }

    tokens.push(Token::Eof);
    tokens
}
