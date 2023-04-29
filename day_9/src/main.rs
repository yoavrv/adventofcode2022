use std::fs;
use std::io::{self, BufRead, BufReader};
use std::env;
use std::collections::HashMap;

// stolen from stackoverflow https://stackoverflow.com/a/49964042
// Reads from file in first argument or stdin
fn stdin_or_first_reader() -> Box<dyn BufRead> {
    let input = env::args().nth(1);
    let reader: Box<dyn BufRead> = match input {
        None => Box::new(BufReader::new(io::stdin())),
        Some(filename) => Box::new(BufReader::new(fs::File::open(filename).unwrap())),
    };
    return reader
}


fn main() {
    let mut visits : HashMap<(i32,i32),usize>= HashMap::new();
    let mut add1_at = |x: i32, y: i32| {
        visits.entry((x,y)).and_modify(|e| {*e+=1}).or_insert(1);
    };
    add1_at(0,0);
    add1_at(-1,-1);
    add1_at(3,-5);
    println!("{:?}",visits);
}
