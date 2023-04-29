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

type Counter<K> = HashMap<K,usize>;
type SparseGridCounter<K> = Counter<(K,K)>;

fn add1<K: Eq+ PartialEq + std::hash::Hash>(grid: &mut SparseGridCounter<K>, x: K, y: K){
    grid.entry((x,y)).and_modify(|e| {*e+=1}).or_insert(1);
}

fn main() {
    let mut visits : SparseGridCounter<i32>= SparseGridCounter::new();
    add1(&mut visits,0,0);
    println!("{:?}",visits);
}
