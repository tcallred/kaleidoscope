mod lexer;
mod parser;

use std::io::Read;

fn main() {
    let mut buf: Vec<u8> = vec![];
    std::io::stdin().read_to_end(&mut buf).unwrap();
    let chars: Vec<char> = buf.iter().map(|byte| *byte as char).collect();
    let tokens = lexer::lex(&chars);
    println!("Tokens: {:?}", tokens);
    let ast_res = parser::parse(&tokens);
    match ast_res {
        Ok(ast) => { println!("Ast: {:?}", ast); }
        Err(parse_err) => { eprint!("{:?}\n", parse_err); }
    }
}
