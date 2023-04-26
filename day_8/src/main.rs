use std::fs;
use std::io::{self, BufRead, BufReader};
use std::env;

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

// get an array of numbers in 0-9 and return the number of items
// visible from the edge (that are the highest from all 4 directions)
fn edge_visible(heights: &Vec<Vec<u32>>) -> (usize, Vec<Vec<bool>>) {
    let mut num_visible: usize=0;
    let mut visible: Vec<Vec<bool>>= heights.iter().map(|x| vec![false; x.len()]).collect();
    let max_row_len: usize = visible.iter().map(|x| x.len()).max().unwrap();
    let mut leftright_max: u32 = 0;
    let mut row_max: Vec<u32> = vec![0; max_row_len];
    // visible from above and left
    for (i,row) in heights.iter().enumerate() {
        for (j, item) in row.iter().enumerate(){
            if j==0 || leftright_max<*item {
                leftright_max=*item;
                if !visible[i][j] {
                    num_visible+=1;
                    visible[i][j]=true;
                }
            }
            if i==0 || row_max[j]<*item{
                row_max[j]=*item;
                if !visible[i][j] {
                    num_visible+=1;
                    visible[i][j]=true;
                }
            }
        }
    }
    // rest rowmax
    for i in 0..max_row_len {
        row_max[i]=0;
    }
    for (i,row) in heights.iter().enumerate().rev() {
        for (j, item) in row.iter().enumerate().rev(){
            if j==row.len()-1 || leftright_max<*item {
                leftright_max=*item;
                if !visible[i][j] {
                    num_visible+=1;
                    visible[i][j]=true;
                }
            }
            if i==heights.len()-1 || row_max[j]<*item{
                row_max[j]=*item;
                if !visible[i][j] {
                    num_visible+=1;
                    visible[i][j]=true;
                }
            }
        }
    }
    (num_visible, visible)
}

// This will be easier if we utilize the limited range of digits 0-9
// We save the latest index of each tree size
// So if we have item 7 we can look at the position of the last 7,8, or 9
//  [2   4   3   2]
//>[0,0,0,0,0,0]
//   > [0,0,1,0,0]               dist=0-max(0,0,0)+1
//       > [0,0,1,0,2]           dist=1-max(0,0)+1
//           > [0,0,1,3,2]       dist=2-max(0,2)+1
//                > [0,0,4,3,2]  dist=3-max(1,3,2)+1
fn inner_visible(heights: &Vec<Vec<u32>>) -> (u32, Vec<Vec<u32>>) {
    let mut score: Vec<Vec<u32>>= heights.iter().map(|x| vec![1;x.len()] ).collect();
    let max_row_len: usize = score.iter().map(|x| x.len()).max().unwrap();
    let mut leftright_idx: [usize; 10] = [0;10];
    let mut row_updown_idx: Vec<[usize; 10]> = vec![[0;10]; max_row_len]; // index+1 of latest 0-9 for each column

    // visible from above and left
    for (i,row) in heights.iter().enumerate() {
        for j in 0..10{
            leftright_idx[j]=0;
        }
        for (j, item) in row.iter().enumerate(){
            let height: usize = (*item) as usize;
            if 9<height {
                panic!("invalid height>9");
            }
            if j==0 {
                score[i][j]=0;
            } else {
                score[i][j] *= (j-leftright_idx.iter().skip(height).max().unwrap()) as u32;
            }

            if i==0{
                score[i][j] *= 0;
            } else {
                score[i][j] *= (i-row_updown_idx[j].iter().skip(height).max().unwrap()) as u32;
            }

            leftright_idx[height]=j;
            row_updown_idx[j][height]=i;
        }
    }

    // rest the indices
    
    for j in 0..max_row_len {
        for k in 0..10{
            row_updown_idx[j][k]=score.len()-1;
        }
    }
    // visible from below and right
    for (i,row) in heights.iter().enumerate().rev() {
        for j in 0..10{
            leftright_idx[j]=row.len()-1;
        }
        for (j, item) in row.iter().enumerate().rev(){
            let height: usize = (*item) as usize;
            if 9<height {
                panic!("invalid height>9");
            }
            if j==row.len()-1 {
                score[i][j] *= 0;
            } else {
                score[i][j] *= (leftright_idx.iter().skip(height).min().unwrap()-j) as u32;
            }

            if i==score.len()-1 {
                score[i][j] *= 0;
            } else {
                score[i][j] *= (row_updown_idx[j].iter().skip(height).min().unwrap()-i) as u32;
            }

            leftright_idx[height]=j;
            row_updown_idx[j][height]=i;
        }
    }
    return (*score.iter().map(|x| x.iter().max().unwrap()).max().unwrap(), score);
}




fn main() {
    let reader = stdin_or_first_reader();
    let vec: Vec<Vec<u32>> = reader.lines().map(|x| 
                                x.unwrap().chars().map(|x| x.to_digit(10).unwrap()).collect())
                            .collect();
    // println!("vec is {:?}",vec);
    // for row in &vec{
    //     println!("{:?}",row);
    // }

    let res_edge: (usize, Vec<Vec<bool>>) = edge_visible(&vec);
    println!("num visible is {:?}",res_edge.0);
    // for row in (&res.1){
    //     println!("{:?}",row);
    // }
    let res_in: (u32, Vec<Vec<u32>>) = inner_visible(&vec);
    println!("max score is {:?}",res_in.0);

    if vec.len()<10 {
        for row in &res_edge.1{
            println!("{:?}",row);
        }
        for row in &res_in.1{
            println!("{:?}",row);
        }
    }
}
