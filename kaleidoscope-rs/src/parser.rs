#![allow(dead_code)]

use crate::lexer::Token;

#[derive(Debug)]
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

#[derive(Debug)]
struct PrototypeAST {
    name: String,
    args: Vec<String>,
}

#[derive(Debug)]
pub struct FunctionAST {
    proto: PrototypeAST,
    body: ExprAST,
}

#[derive(Debug)]
pub struct ParseError(String);

fn expect(tokens: &[Token], idx: &mut usize, token: Token) -> Result<(), ParseError> {
    if tokens[*idx] != token {
        return Err(ParseError(format!("Expected {:?}", token)));
    }
    *idx += 1;
    Ok(())
}

fn expect_id(tokens: &[Token], idx: &mut usize) -> Result<String, ParseError> {
    let Token::Identifier(ref s) = tokens[*idx] else {
        return Err(ParseError(format!(
            "Expected identifier got {:?}",
            tokens[*idx]
        )));
    };
    *idx += 1;
    Ok(s.clone())
}

fn parse_number(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    let Token::Number(n) = tokens[*idx] else {
        return Err(ParseError("Expected number".to_string()));
    };
    *idx += 1;
    Ok(ExprAST::Number(n))
}

fn parse_paren_expr(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    expect(tokens, idx, Token::LParen)?;
    let expr = parse_expression(tokens, idx)?;
    expect(tokens, idx, Token::RParen)?;
    Ok(expr)
}

fn parse_identifier_expr(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    let id = expect_id(tokens, idx)?;
    if tokens[*idx] != Token::LParen {
        return Ok(ExprAST::Variable(id));
    }
    expect(tokens, idx, Token::LParen)?;
    let mut args: Vec<ExprAST> = Vec::new();
    if tokens[*idx] == Token::RParen {
        expect(tokens, idx, Token::RParen)?;
        return Ok(ExprAST::Call { callee: id, args });
    }
    loop {
        let expr = parse_expression(tokens, idx)?;
        args.push(expr);
        if tokens[*idx] == Token::Comma {
            expect(tokens, idx, Token::Comma)?;
        }
        if tokens[*idx] == Token::RParen {
            expect(tokens, idx, Token::RParen)?;
            break;
        }
    }
    return Ok(ExprAST::Call { callee: id, args });
}

fn parse_primary(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    match tokens[*idx] {
        Token::Identifier(_) => parse_identifier_expr(tokens, idx),
        Token::Number(_) => parse_number(tokens, idx),
        Token::LParen => parse_paren_expr(tokens, idx),
        _ => Err(ParseError(format!("Unexpected token {:?}", tokens[*idx]))),
    }
}

fn binop_precedence(token: char) -> i32 {
    match token {
        '<' => 10,
        '+' => 20,
        '-' => 20,
        '*' => 30,
        _ => -1,
    }
}

fn get_tok_precedence(token: &Token) -> i32 {
    match token {
        Token::Other(c) => binop_precedence(*c),
        _ => -1,
    }
}

fn parse_binop_rhs(
    tokens: &[Token],
    idx: &mut usize,
    expr_prec: i32,
    mut lhs: ExprAST,
) -> Result<ExprAST, ParseError> {
    loop {
        let tok_prec = get_tok_precedence(&tokens[*idx]);
        if tok_prec < expr_prec {
            return Ok(lhs);
        }
        let Token::Other(binop) = tokens[*idx] else {
            return Err(ParseError("Expected binop".to_string()));
        };
        *idx += 1;
        let mut rhs = parse_primary(tokens, idx)?;
        let next_prec = get_tok_precedence(&tokens[*idx]);
        if tok_prec < next_prec {
            rhs = parse_binop_rhs(tokens, idx, tok_prec + 1, rhs)?;
        }
        lhs = ExprAST::Binop {
            op: binop,
            lhs: Box::new(lhs),
            rhs: Box::new(rhs),
        }
    }
}

fn parse_expression(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    let lhs = parse_primary(tokens, idx)?;
    return parse_binop_rhs(tokens, idx, 0, lhs);
}

fn parse_prototype(tokens: &[Token], idx: &mut usize) -> Result<PrototypeAST, ParseError> {
    let fn_name = expect_id(tokens, idx)?;
    expect(tokens, idx, Token::LParen)?;
    let mut arg_names = Vec::new();
    loop {
        if tokens[*idx] == Token::RParen {
            expect(tokens, idx, Token::RParen)?;
            break;
        }
        let arg_name = expect_id(tokens, idx)?;
        arg_names.push(arg_name);
    }
    Ok(PrototypeAST {
        name: fn_name,
        args: arg_names,
    })
}

fn parse_definition(tokens: &[Token], idx: &mut usize) -> Result<FunctionAST, ParseError> {
    expect(tokens, idx, Token::Def)?;
    let proto = parse_prototype(tokens, idx)?;
    let e = parse_expression(tokens, idx)?;
    Ok(FunctionAST { proto, body: e })
}

fn parse_extern(tokens: &[Token], idx: &mut usize) -> Result<PrototypeAST, ParseError> {
    expect(tokens, idx, Token::Extern)?;
    parse_prototype(tokens, idx)
}

fn parse_top_level_expr(tokens: &[Token], idx: &mut usize) -> Result<FunctionAST, ParseError> {
    let e = parse_expression(tokens, idx)?;
    let proto = PrototypeAST {
        name: "__anon_expr".to_string(),
        args: vec![],
    };
    Ok(FunctionAST { proto, body: e })
}

pub fn parse(tokens: &[Token]) -> Result<Vec<FunctionAST>, ParseError> {
    let mut idx = 0;
    let mut fns = Vec::new();
    loop {
        match tokens[idx] {
            Token::Eof => {
                return Ok(fns);
            }
            Token::Semicolon => idx += 1,
            Token::Def => {
                let d = parse_definition(tokens, &mut idx)?;
                fns.push(d);
            }
            Token::Extern => {
                let ext = parse_extern(tokens, &mut idx)?;
                fns.push(FunctionAST {
                    proto: ext,
                    body: ExprAST::Number(0.0),
                });
            }
            _ => {
                let top_level = parse_top_level_expr(tokens, &mut idx)?;
                fns.push(top_level);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::lex;

    #[test]
    fn test_parse_exp() {
        let program: Vec<char> = "x+(1+2)*3-f(x)\n".chars().collect();
        let tokens = lex(&program);
        let mut idx = 0;
        let expr_res = parse_expression(&tokens, &mut idx);
        println!("{:?}", expr_res);
    }
}
