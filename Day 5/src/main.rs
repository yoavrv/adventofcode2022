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

fn vecs_to_str(vecs: &Vec<Vec<char>>) -> String {
    let max_height: usize = vecs.iter().map(|v| v.len()).max().unwrap();
    println!("max height: {max_height}");
    let mut s = String::new();
    for i in (0..max_height).rev() {
        for (j, vec) in vecs.iter().enumerate() {
            if j!=0 {
                s.push(' ');
            }
            if i<vec.len() {
                s.push('[');
                s.push(vec[i]);
                s.push(']');
            }
            else {
                s.push_str("   ");
            }
        }
        s.push('\n');
    }
    for j in 0..vecs.len() {
        if j!=0 {
            s.push(' ');
        }
        s.push(' ');
        s.push(char::from_digit((j+1).try_into().unwrap(),10).unwrap());
        s.push(' ');
    }
    return s
}

fn parse_initial_lines(line_iterator: &mut impl Iterator<Item=io::Result<String>>) -> Vec<Vec<char>> {
    let line = line_iterator.next().unwrap().unwrap();
    let num_stacks: usize = (line.len()-1)/4+1;
    let mut vecs: Vec<Vec<char>> = Vec::new();
    for _i in 0..num_stacks {
       vecs.push(Vec::new());
    }
    let line_chars: Vec<char> = line.chars().skip(1).step_by(4).collect();
    for i in 0..num_stacks {
        if line_chars[i] != ' ' {
            vecs[i].push(line_chars[i]);
        }
    }
    for line in line_iterator {
        let line_chars: Vec<char> = line.unwrap().chars().skip(1).step_by(4).collect();
        if line_chars.len()!=num_stacks {
            break;
        }
        if line_chars.iter().all(|c| c.is_digit(10)) {
            break;
        }
        else {
            for i in 0..num_stacks {
                if line_chars[i] != ' ' {
                    vecs[i].push(line_chars[i]);
                }
            }
        }
    }
    for i in 0..num_stacks {
        vecs[i].reverse();
    }
   vecs
}

// stolen from stackoverflow https://stackoverflow.com/a/49964042
// Reads from file in first argument or stdin
fn main() {
    let input = env::args().nth(1);
    let reader: Box<dyn BufRead> = match input {
        None => Box::new(BufReader::new(io::stdin())),
        Some(filename) => Box::new(BufReader::new(fs::File::open(filename).unwrap())),
    };

    let mut iter_lines = reader.lines();
    let mut vecs = parse_initial_lines(&mut iter_lines);
    let s = vecs_to_str(&vecs);
    println!("pretty please print\n{}",s);
    let (mut num, mut from ,mut to): (usize,usize,usize);
    let mut steps =0;
    for (i, line) in iter_lines.enumerate() {
        // do a move
        if i<10 {
            println!("{:?}",line);
        }
        match parse_move_line(&line.unwrap()) {
            Some(tup) => (num, from, to) = tup,
            None => continue,
        }
        if i<10 {
            println!("{:?}",(num, from, to));
        }
        let l = vecs[from-1].len()-num;
        let v: Vec<char> = vecs[from-1].drain(l..).collect();
        vecs[to-1].extend(v);
        // for _i in 0..num {
        //     let c = vecs[from-1].pop().unwrap();
        //     vecs[to-1].push(c);
        // }
        if i<10 {
            println!("{num} {from} {to}");
            let s = vecs_to_str(&vecs);
            println!("{}",s);
        }
        steps+=1;
    }
    println!("done after {} steps",steps);
    let s = vecs_to_str(&vecs);
    println!("{}",s);
    println!("finals: {:?}",vecs.iter().map(|v| v.last().unwrap()).collect::<Vec<&char>>());
}