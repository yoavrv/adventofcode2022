use std::env;
use std::fs;
use std::io::{self, BufRead, BufReader};
use std::usize;

fn parse_move_line(line: &str) -> Option<(usize, usize, usize)> {
    // line is format "move x from y to z"
    let v: Vec<usize> = line
        .splitn(6," ") // ['move' 'x' 'from' 'y' 'to' 'z']
        .skip(1)       // ['x' 'from' 'y' 'to' 'z']
        .step_by(2)    // ['x' 'y' 'z']
        .map(|x| x.parse::<usize>().unwrap()) //turbofish to turn to numbers
        .collect();
    match v.len() {
        3 => Some((v[0], v[1], v[2])),
        _ => None
    }
}

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
    let reader = stdin_or_first_reader();
    let string = reader.lines().nth(0).unwrap();
    let (mut a,mut b,mut c,mut d) = (' ',' ',' ',' ');
    println!("{:?}",string);
    let mut i:usize = 1;
    for new_char in string.unwrap().chars() {
        a=b;
        b=c;
        c=d;
        d=new_char;
        if a==b || a==c || a==d || b==c || b==d || c==d || a==' '{
            i+=1;
        }
        else {
            break;
        }
    }
    println!("we start at {i}");
}