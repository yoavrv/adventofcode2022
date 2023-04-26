# Advent of Code 2022  
A well known ~setting up things~ coding challenge.  

c programs are compiled from the .c and .h files in their day  
rust programs used `cargo run` on the day_ folder to run.  

Besides day_1 (which runs the input file in its directory), all other executables takes the stdin or the first argument as the input  
`out.exe input`  
`cargo run .\input`  

* challenge 0: get git to work directly on windows instead of WSL.  
* challenge 0.5: get c to work directly on windows instead of WSL.  
* **Day 1**: Simpler `getc()` io with i=digit+10*i to parse the chars to int.  
* challenge 1.5: understand why c file handling in windows acts wierd.  
* **Day 2**: Better c io `fscanf(%1s %1s,A,X)` to parse line, and using a `scoretable[A-'A'][X-'X']` to score game.  
* **Day 3**: Bunch of string traversal to find duplicate chars using pointers (N^2 but these aren't long strings).  
* challenge 3.5: get rust to work directly on windows instead of WSL.  
* **Day 4**: Parsing a string in rust with splitn to 4 numbers, using a large if block on the 4 numbers.  
* **Day 5**: Some more parsing of strings in rust, neat printing, and slight game of draining a vector to another.  
* **Day 6**: Cheeky `HashSet::from(array).len()<N` O(N) solution: A counter solution would probably be better (counter[in]++; if counter[out]==2 counter.values().map(|x|==1).all(); counter[out]--).  
* **Day 7**: Simulating a rudimentary shell in c with a simple tree with files as nodes.
* challenge 7.01: stop having fun simulating a rudimentary shell.  
* **Day 8**: Rust solution, traversing `Vec<Vec<>>` in the fast (row-major) order while keeping track of the latest index of the digits encountered.
* challenge ?: get cargo to work correctly on existing git project
