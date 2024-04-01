use crate::lexer::Token;
use std::iter::{Iterator, Peekable};
use std::slice::Iter;

enum ExprAST {
    Number(f64),
    Variable(String),
    Binop {
        op: char,
        lhs: Box<ExprAST>,
        rhs: Box<ExprAST>,
    },
    Call {
        callee: String,
        args: Vec<ExprAST>,
    },
}

struct PrototypeAST {
    name: String,
    args: Vec<ExprAST>,
}

struct FunctionAST {
    proto: PrototypeAST,
    body: ExprAST,
}

struct ParseError(String);

fn expect(tokens: &mut Peekable<Iter<Token>>, token: Token) -> Result<(), ParseError> {
    match tokens.next() {
        Some(t) if *t == token => Ok(()),
        _ => Err(ParseError(format!("Expected {:?}", token))),
    }
}

fn expect_id(tokens: &mut Peekable<Iter<Token>>) -> Result<String, ParseError> {
    match tokens.next() {
        Some(t) => match t {
            Token::Identifier(s) => Ok(s.to_owned()),
            _ => Err(ParseError(format!("Exected identifier got {:?}", t))),
        },
        _ => Err(ParseError("Expected identifier".to_string())),
    }
}

fn parse_expression(tokens: &mut Peekable<Iter<Token>>) -> Result<ExprAST, ParseError> {
    todo!()
}

fn parse_number(tokens: &mut Peekable<Iter<Token>>) -> Result<ExprAST, ParseError> {
    if let Some(Token::Number(n)) = tokens.next() {
        Ok(ExprAST::Number(*n))
    } else {
        Err(ParseError("Expected number".to_string()))
    }
}

fn parse_paren_expr(tokens: &mut Peekable<Iter<Token>>) -> Result<ExprAST, ParseError> {
    expect(tokens, Token::Other('('))?;
    let expr = parse_expression(tokens)?;
    expect(tokens, Token::Other(')'))?;
    Ok(expr)
}

fn parse_identifier_expr(tokens: &mut Peekable<Iter<Token>>) -> Result<ExprAST, ParseError> {
    let id = expect_id(tokens)?;
    if let Some(Token::Other('(')) = tokens.peek() {
        expect(tokens, Token::Other('('))?;
        let mut args: Vec<ExprAST> = Vec::new();
        if let Some(Token::Other(')')) = tokens.peek() {
            expect(tokens, Token::Other(')'))?;
        } else {
            loop {
                let expr = parse_expression(tokens)?;
                args.push(expr);
                match tokens.peek() {
                    Some(Token::Other(',')) => {
                        expect(tokens, Token::Other(','))?;
                    }
                    Some(Token::Other(')')) => {
                        expect(tokens, Token::Other(')'))?;
                        break;
                    }
                    _ => {
                        return Err(ParseError(
                            "Unexpected token or end of token stream".to_string(),
                        ));
                    }
                }
            }
        }
        Ok(ExprAST::Call { callee: id, args })
    } else {
        Ok(ExprAST::Variable(id))
    }
}

fn parse_primary(tokens: &mut Peekable<Iter<Token>>) -> Result<ExprAST, ParseError> {
    if let Some(token) = tokens.peek() {
        match token {
            Token::Identifier(_) => parse_identifier_expr(tokens),
            Token::Number(_) => parse_number(tokens),
            Token::Other('(') => parse_paren_expr(tokens),
            _ => Err(ParseError(format!("Unexpected token {:?}", token))),
        }
    } else {
        Err(ParseError("Unexpected end of stream".to_string()))
    }
}

fn get_tok_precedence(token: char) -> i32 {
    match token {
        '<' => 10,
        '+' => 20,
        '-' => 20,
        '*' => 30,
        _ => -1,
    }
}
