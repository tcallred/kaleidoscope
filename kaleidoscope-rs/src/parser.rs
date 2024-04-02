use crate::lexer::Token;

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

    Ok(ExprAST::Number(n))
}

fn parse_paren_expr(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    expect(tokens, idx, Token::Other('('))?;
    let expr = parse_expression(tokens, idx)?;
    expect(tokens, idx, Token::Other(')'))?;
    Ok(expr)
}

fn parse_identifier_expr(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    let id = expect_id(tokens, idx)?;
    if tokens[*idx] != Token::Other('(') {
        return Ok(ExprAST::Variable(id));
    }
    expect(tokens, idx, Token::Other('('))?;
    let mut args: Vec<ExprAST> = Vec::new();
    if tokens[*idx] == Token::Other(')') {
        expect(tokens, idx, Token::Other(')'))?;
        return Ok(ExprAST::Call { callee: id, args });
    }
    loop {
        let expr = parse_expression(tokens, idx)?;
        args.push(expr);
        if tokens[*idx] == Token::Other(',') {
            expect(tokens, idx, Token::Other(','))?;
        }
        if tokens[*idx] == Token::Other(')') {
            expect(tokens, idx, Token::Other(')'))?;
            break;
        }
    }
    return Ok(ExprAST::Call { callee: id, args });
}

fn parse_primary(tokens: &[Token], idx: &mut usize) -> Result<ExprAST, ParseError> {
    match tokens[*idx] {
        Token::Identifier(_) => parse_identifier_expr(tokens, idx),
        Token::Number(_) => parse_number(tokens, idx),
        Token::Other('(') => parse_paren_expr(tokens, idx),
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

fn get_tok_precedence(token: Token) -> i32 {
    match token {
        Token::Other(c) => binop_precedence(c),
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
        let tok_prec = get_tok_precedence(tokens[*idx].clone());
        if tok_prec < expr_prec {
            return Ok(lhs);
        }
        let Token::Other(binop) = tokens[*idx] else {
            return Err(ParseError("Expected binop".to_string()));
        };
        *idx += 1;
        let mut rhs = parse_primary(tokens, idx)?;
        let next_prec = get_tok_precedence(tokens[*idx].clone());
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
