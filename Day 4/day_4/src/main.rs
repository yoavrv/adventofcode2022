use std::env;
use std::fs;
use std::io::{self, BufRead, BufReader};

fn parse_line(line: &str) -> (u32, u32, u32, u32) {
    let v = line
        .splitn(4, |c| c == '-' || c == ',')
        .map(|x| x.parse::<u32>().unwrap()) //turbofish to turn to numbers
        .collect::<Vec<u32>>();
    (v[0], v[1], v[2], v[3])
}

// stolen from stackoverflow https://stackoverflow.com/a/49964042
// Reads from file in first argument or stdin
fn main() {
    let input = env::args().nth(1);
    let reader: Box<dyn BufRead> = match input {
        None => Box::new(BufReader::new(io::stdin())),
        Some(filename) => Box::new(BufReader::new(fs::File::open(filename).unwrap())),
    };

    let mut containing: u32 = 0;
    let mut overlapping: u32 = 0;
    let max_debug = 20;
    for (i, line) in reader.lines().enumerate() {
        if i < max_debug {
            println!("Line: {:?}, number of containing {containing}, number of overlapping {overlapping}", line);
        }
        let (low1, high1, low2, high2) = parse_line(&line.unwrap());

        // overlapp
        if low1 <= low2 && low2 <= high1 {
            overlapping += 1;
            if i < max_debug {
                println!("* overlapp in {low1}, {high1} with {low2},{high2} ");
            }
        } else if low2 <= low1 && low1 <= high2 {
            overlapping += 1;
            if i < max_debug {
                println!("* overlapp in {low1}, {high1} with {low2},{high2} ");
            }
        } else {
            if i < max_debug {
                println!("* no overlap in {low1}, {high1}, {low2},{high2} ");
            }
        }

        //contain
        if low1 <= low2 && high1 >= high2 {
            containing += 1;
            if i < max_debug {
                println!("** First {low1}, {high1} contains {low2},{high2} ");
            }
        } else if low1 >= low2 && high1 <= high2 {
            containing += 1;
            if i < max_debug {
                println!("** First {low1}, {high1} is contained in {low2},{high2}");
            }
        } else {
            if i < max_debug {
                println!("** No pair contains the other");
            }
        }
    }
    println!("Final containing count: {containing}");
    println!("Final overlapping count: {overlapping}");
}
