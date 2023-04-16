use std::env;
use std::fs;
use std::io::{self, BufRead, BufReader};
use std::usize;
use std::collections::HashSet;

fn update_array<const N:usize>(array: &mut [char; N],c: char){
    for i in 0..(array.len()-1){
        array[i] = array[i+1];
    }
    array[array.len()-1]=c;
}

// Well, it almost certainly not faster than the O(N^2) solution (direct comparisons)
// or O(Nlog(N)) solution (sorting) for our N, but it is O(N)
fn array_has_duplicates<const N:usize>(array: [char; N]) -> bool{
    HashSet::from(array).len()<N
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
    let mut past_chars: [char; 14] = [' '; 14] ;
    println!("{:?}",string);
    let mut start:usize = 0;
    let mut mess:usize = 0;
    for (i, new_char) in string.unwrap().chars().enumerate() {
        a=b;
        b=c;
        c=d;
        d=new_char;
        if 0!=start || a==b || a==c || a==d || b==c || b==d || c==d || a==' '{
        }
        else {
           start=i+1;
        }
        update_array(&mut past_chars, new_char);
        if mess!=0 || i<=past_chars.len() || array_has_duplicates(past_chars){

        }
        else {
            mess=i+1;
        }
    }
    println!("Transmission starts at {start}; message starts as {mess}");
}