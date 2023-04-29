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


#[derive(Debug)]
enum CommandDir {
    Up,
    Down,
    Left,
    Right,
}

fn parse_move_line(line: &str) -> Option<(CommandDir, i32)> {
    // line is format "R number"
    let mut it = line.split_whitespace();
    let mut com: Option<CommandDir> = None;
    match it.next()? {
        "U" => com = Some(CommandDir::Up),
        "D" => com = Some(CommandDir::Down),
        "L" => com = Some(CommandDir::Left),
        "R" => com = Some(CommandDir::Right),
        _ => com = None,
    };
    if let Some(cm) = com { 
        return Some((cm,it.next()?.parse::<i32>().ok()?));
    } else {
        return None;
    }
}


fn main() {
    let mut visits : HashMap<(i32,i32),usize>= HashMap::new();
    let add1_at = |x: i32, y: i32| {
        visits.entry((x,y)).and_modify(|e| {*e+=1}).or_insert(1);
    };
    
    for line in stdin_or_first_reader().lines() {
        println!("{:?}",parse_move_line(&line.unwrap()));
    }

    println!("{:?}",visits);
}
