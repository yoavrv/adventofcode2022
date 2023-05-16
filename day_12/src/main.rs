use std::fs;
use std::io::{self, BufRead, BufReader};
use std::env;
use std::collections::BinaryHeap;
use std::cmp::{Ordering};

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

// An item in the queue of positions as part of a path traversing the matrix:
// x, y are the location at the matrix
// prev_x, prev_y are the previous lcoation in the path
// dist is the (modified) distance so far: path length + taxicab distance from end
#[derive(Eq,Debug)]
struct QueueItem {
    dist:usize,
    dist_from:usize,
    x:usize,
    y:usize,
}


impl Ord for QueueItem {
    fn cmp(&self, other: &Self) -> Ordering {
        (other.dist).cmp(&self.dist) // notice the we reverse the comparison!
    }
}

impl PartialOrd for QueueItem {
    fn partial_cmp(&self, other: &Self) ->Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for QueueItem {
    fn eq(&self, other: &Self) -> bool {
        self.dist==other.dist
    }
}

fn print_matrix(v: &Vec<char>, stride: usize) {
    for line in v.chunks(stride) {
        println!("{}",String::from_iter(line));
    }
}

fn print_matrix2(v: &Vec<Option<(usize,usize)>>, stride: usize) {
    for line in v.chunks(stride) {
        println!("{:?}",line);
    }
}

fn matrix_traverse<T: Copy>(v: & Vec<T>, stride: usize, x_start: usize, y_start: usize,
                     traverserule : impl Fn(T,T)->bool, dist_heuristic : impl Fn(usize,usize)->usize, 
                     endrule : impl Fn(T,usize,usize)->bool) ->(Vec<Option<(usize,usize)>>, Option<usize>) {
    let height: usize = v.len()/stride;
    let mut queue_dist: BinaryHeap<QueueItem> = BinaryHeap::with_capacity(v.len()/2);
    let mut prev_xy : Vec<Option<(usize,usize)>> = vec![None; v.len()];

    // first item is special: rev_xy is None
    queue_dist.push(QueueItem { dist : dist_heuristic(x_start,y_start), dist_from: 0, x: x_start, y: y_start }) ;

    let push = |x:usize,y: usize, path: usize,prev_x: usize,prev_y: usize, queue_dist: &mut BinaryHeap<QueueItem>,prev_xy :  &mut Vec<Option<(usize,usize)>>| {
        queue_dist.push(QueueItem { dist : dist_heuristic(x, y)+path, dist_from: path, x, y}) ;
        prev_xy[x+stride*y] = Some((prev_x,prev_y));
    };

    let seen_xy = |x:usize,y:usize, prev_xy: &Vec<Option<(usize,usize)>>| {
        if x==x_start && y==y_start {return true;}; // special case: start doesn't have previous node but was seen
        match prev_xy[x+stride*y]{
            Some(_tup) => return true,
            None => return false,
        }
    };


    let mut dist=0;
    let mut success=false;
    while !queue_dist.is_empty() && queue_dist.len()<v.len() {
        if let Some(cur_item) = queue_dist.pop() {

            let (x,y, dist_from) : (usize,usize,usize)= (cur_item.x, cur_item.y, cur_item.dist_from+1);
            let index: usize = x+stride*y;
            let c: T = v[index];

            if endrule(c,x,y) {
                success=true;
                dist=cur_item.dist_from;
                break;
            }
            if 0<x {
                let c2: T = v[index-1];
                if traverserule(c,c2) && !seen_xy(x-1,y,&prev_xy) {
                    push(x-1, y, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
            if 0<y {
                let c2: T = v[index-stride];
                if traverserule(c,c2) && !seen_xy(x,y-1,&prev_xy) {
                    push(x, y-1, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
            if x<stride-1 {
                let c2: T = v[index+1];
                if traverserule(c,c2) && !seen_xy(x+1,y,&prev_xy) {
                    push(x+1, y, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
            if y<height-1 {
                let c2: T = v[index+stride];
                if traverserule(c,c2) && !seen_xy(x,y+1,&prev_xy) {
                    push(x, y+1, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
        }
    }


    if success {return (prev_xy, Some(dist));} else {return(prev_xy,None);}
}

fn main() {
    let mut v: Vec<char> = Vec::new();
    let mut stride: usize = 0;
    let mut height: usize = 0;
    for line in stdin_or_first_reader().lines() {
        if let Ok(line) = line {
            if line=="done" {
                break;
            }
            if stride==0 {
                stride=line.chars().count();
            } else if stride!=line.chars().count() {
                panic!("Read line {:?} of bad length (current stride: {:?})",line,stride);
            }

            v.extend(line.chars());
            height+=1;
        }

    }
    let (mut x_start, mut y_start, mut x_end, mut y_end):(usize,usize,usize,usize) = (0,0,0,0);
    for (y, l) in v.chunks_exact(stride).enumerate(){
        for (x, c) in l.iter().enumerate(){
            match c {
                'S'=> (x_start,y_start)=(x,y),
                'E'=> (x_end,y_end)=(x,y),
                _=>(),
            }
        }
        
    }
    v[x_start+stride*y_start]='a';
    v[x_end+stride*y_end]='z';
    println!("we start at {:?} and want to end at {:?}",(x_start,y_start),(x_end,y_end));
    print_matrix(&v,stride);

    // show the result is similar:
    let (solution, distance) = matrix_traverse(&v, stride, x_start, y_start,
                                         |c,c2| (c2 as u32)<=1+(c as u32),
                                         |x,y| x_end.abs_diff(x)+y_end.abs_diff(y), 
                                         |_c,x,y| x==x_end && y==y_end);
    if let Some(dist) = distance {
        println!("Success: distance {dist}");
    } else {
        println!{"Failure"};
    }
    if height<10 && stride<10 {print_matrix2(&solution, stride);}

    // these are no longer mut: is there a fixing?
    
    let mut queue_dist: BinaryHeap<QueueItem> = BinaryHeap::with_capacity(v.len()/2);
    let mut prev_xy : Vec<Option<(usize,usize)>> = vec![None; v.len()];

    // distance heuristic
    let d = |x:usize,y:usize| -> usize {
        x_end.abs_diff(x)+y_end.abs_diff(y)
    };
    

    // first item is special: rev_xy is None
    queue_dist.push(QueueItem { dist : d(x_start,y_start), dist_from: 0, x: x_start, y: y_start }) ;

    let push = |x:usize,y: usize, path: usize,prev_x: usize,prev_y: usize, queue_dist: &mut BinaryHeap<QueueItem>,prev_xy :  &mut Vec<Option<(usize,usize)>>| {
        queue_dist.push(QueueItem { dist : d(x, y)+path, dist_from: path, x, y}) ;
        prev_xy[x+stride*y] = Some((prev_x,prev_y));
    };

    let find_xy = |x:usize,y:usize, prev_xy: &Vec<Option<(usize,usize)>>| {
        if x==x_start && y==y_start {return true;};
        // for (y3, part) in prev_xy.chunks_exact(stride).enumerate() {
        //     for (x3, item) in part[x.saturating_sub(2)..stride.min(x+2)].iter().enumerate() {
        //         if let Some( (x2,y2) ) = item {
        //             if x==*x2 && y==*y2 {return true;};
        //             if x==x3 && y==y3 {return true;};
        //         };
        //     }
        // }
        match prev_xy[x+stride*y]{
            Some(_tup) => return true,
            None => return false,
        }
    };


    let mut dist=0;
    let mut success=false;
    while !queue_dist.is_empty() && queue_dist.len()<v.len() {
        if let Some(cur_item) = queue_dist.pop() {
            // println!("At {:?}",cur_item);
            if cur_item.x==x_end && cur_item.y==y_end {
                success=true;
                dist=cur_item.dist_from;
                break;
            }
            let (x,y, dist_from) : (usize,usize,usize)= (cur_item.x, cur_item.y, cur_item.dist_from+1);
            let index: usize = x+stride*y;
            let c: char = v[index];
            if 0<x {
                let c2: char = v[index-1];
                if ((c2 as u32)<=1+(c as u32)) && !find_xy(x-1,y,&prev_xy) {
                    push(x-1, y, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
            if 0<y {
                let c2 = v[index-stride];
                if ((c2 as u32)<=1+(c as u32)) && !find_xy(x,y-1,&prev_xy) {
                    push(x, y-1, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
            if x<stride-1 {
                let c2 = v[index+1];
                if ((c2 as u32)<=1+(c as u32)) && !find_xy(x+1,y,&prev_xy) {
                    push(x+1, y, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
            if y<height-1 {
                let c2 = v[index+stride];
                if ((c2 as u32)<=1+(c as u32)) && !find_xy(x,y+1,&prev_xy) {
                    push(x, y+1, dist_from,x, y, &mut queue_dist, &mut prev_xy);
                }
            };
        }
    }


    println!("Success: {}",success);
    println!("heap: {:?}", queue_dist);
    if height<10 && stride<10 {print_matrix2(&prev_xy, stride);}
    println!("distance: {}",dist);
    let mut cur_item = prev_xy[x_end+stride*y_end];
    let (mut x1,mut y1) = (x_end, y_end);
    while cur_item.is_some() {
        if let Some(item) = cur_item {
            let (x,y) = item;
            let index = x+stride*y;
            if x<x1 {v[index]='>'};
            if x1<x {v[index]='<'};
            if y<y1 {v[index]='v'};
            if y1<y {v[index]='^'};
            (x1,y1)=(x,y);
            cur_item = prev_xy[index];
        }
    }
    print_matrix(&v,stride);
}
