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
    let command: Option<CommandDir>;
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


fn tail(rope: &mut Vec<(i32,i32)>, head_move: Option<(CommandDir, i32)>, visits: &mut HashSet<(i32,i32)>){
    if let Some((command, len)) = head_move {
        for _i in 0..len {
            match command {
                CommandDir::Up => {
                    rope[0].1+=1;
                },
                CommandDir::Down => {
                    rope[0].1-=1;
                },
                CommandDir::Left => {
                    rope[0].0-=1;
                },
                CommandDir::Right => {
                    rope[0].0+=1;
                },
            };
            let (mut x, mut y) = rope[0];
            // [X # $ # X]
            // [# * * * #]
            // [$ * O * $]
            // [# * * * #]
            // [X # $ # X]
            for (xt,yt) in rope.iter_mut().skip(1) {
                let dx = x-*xt;
                let dy = y-*yt;
                match (dx,dy) {
                    (-2,-2) => {*xt-=1;*yt-=1},
                    (-2,2) => {*xt-=1;*yt+=1},
                    (2,-2) => {*xt+=1;*yt-=1},
                    (2,2) => {*xt+=1;*yt+=1},
                    (-2,dy) => {*xt-=1;*yt+=dy},
                    (2,dy) => {*xt+=1;*yt+=dy},
                    (dx, -2) => {*xt+=dx;*yt-=1},
                    (dx, 2) => {*xt+=dx;*yt+=1},
                    _=> {break;},
                }
                x = *xt;
                y = *yt;
            }
            visits.insert(*rope.last().unwrap());
        }
    }
}

fn main() {
    let mut visits : HashSet<(i32,i32)>= HashSet::new();
    
    let mut rope = vec![(0,0);10];
    visits.insert(*rope.last().unwrap());


    for line in stdin_or_first_reader().lines() {
        if let Ok(lb) = line {
            // println!("{}",lb);
            // print!("{:?}->",rope);
            match lb.as_str(){
                "quit" => break,
                _ => {}
            }
            tail(&mut rope, parse_move_line(&lb), &mut visits);
            // println!("{:?}",rope);
            
        }
    }

    println!("Visited {:?} positions",visits.len());
}
