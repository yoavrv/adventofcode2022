use std::env;
use std::fs;
use std::io::{self, BufReader, BufRead};


fn parse_line(line: &str) -> (u32,u32,u32,u32){
    let v = line.splitn(4, |c| c == '-' || c == ',')
                .map(|x| x.parse::<u32>().unwrap()) //turbofish to turn to numbers
                .collect::<Vec<u32>>();
    (v[0],v[1],v[2],v[3])
}

// stolen from stackoverflow https://stackoverflow.com/a/49964042
// Reads from file in first argument or stdin
fn main() {
    let input = env::args().nth(1);
    let reader: Box<dyn BufRead> = match input {
        None => Box::new(BufReader::new(io::stdin())),
        Some(filename) => Box::new(BufReader::new(fs::File::open(filename).unwrap()))
    };
    for line in reader.lines() {
        println!("{:?}", line);
        println!("{:?}", parse_line(&line.unwrap()));
    }
}

