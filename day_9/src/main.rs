use std::fs;
use std::io::{self, BufRead, BufReader};
use std::env;
use std::collections::HashSet;

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
    let mut command: Option<CommandDir>;
    match it.next()? {
        "U" => command = Some(CommandDir::Up),
        "D" => command = Some(CommandDir::Down),
        "L" => command = Some(CommandDir::Left),
        "R" => command = Some(CommandDir::Right),
        _ => command = None,
    };
    if let Some(cm) = command { 
        return Some((cm,it.next()?.parse::<i32>().ok()?));
    } else {
        return None;
    }
}


fn main() {
    let mut visits : HashSet<(i32,i32)>= HashSet::new();
    let mut visit = |x: i32, y: i32| {
        visits.insert((x,y));
    };
    
    let mut dx_head: i32=0;
    let mut x_tail: i32=0;
    let mut dy_head: i32=0;
    let mut y_tail: i32=0;
    visit(x_tail,y_tail);
    for line in stdin_or_first_reader().lines() {
        if let Ok(lb) = line {
            match lb.as_str(){
                "quit" => break,
                _ => {}
            }
            if let Some((command, len)) = parse_move_line(&lb) {
                for _i in 0..len {
                    match command {
                        CommandDir::Up => {
                            if dy_head==1 {
                                x_tail+=dx_head;
                                y_tail+=dy_head;
                                dx_head=0;
                                visit(x_tail,y_tail);
                            } else {
                                dy_head+=1;
                            }
                        },
                        CommandDir::Down => {
                            if dy_head==-1 {
                                x_tail+=dx_head;
                                y_tail+=dy_head;
                                dx_head=0;
                                visit(x_tail,y_tail);
                            } else {
                                dy_head-=1;
                            }
                        },
                        CommandDir::Left => {
                            if dx_head==-1 {
                                x_tail+=dx_head;
                                y_tail+=dy_head;
                                dy_head=0;
                                visit(x_tail,y_tail);
                            } else {
                                dx_head-=1;
                            }
                        },
                        CommandDir::Right => {
                            if dx_head==1 {
                                x_tail+=dx_head;
                                y_tail+=dy_head;
                                dy_head=0;
                                visit(x_tail,y_tail);
                            } else {
                                dx_head+=1;
                            }
                        },
                    };
            }
            }
        }
    }

    println!("Visited {:?} positions",visits.len());
}
