1. group number
	Group 9 


2. partners
	Jing Xie
	Yaesop Lee


3. contribution list for each partner

Jing Xie : "1. Global variable, iterate instructions, find and clone functions needed to be cloned, store ret value in global, call pop_direct_br, loading ret from global, replace func call; 2. test and summary"

Yaesop Lee : 1. setting up the run code 2. building overall flow 3. finding the functions 4. summary and testings.


4. examples you have tested and result
Example codes in the test directories.


5. method/commands to run test your code

Set ; setenv PATH /afs/glue/class/old/enee/759c/llvm/llvm-3.4-install/opt/bin/:$PATH

go to obj folder and run "make install"

then, run " opt -load ../opt/lib/libHello.so -hello [.bc file to be run] -o [output .bc file]"


6. short summary of the project

Basically, we followed the instruction provided. Here are the steps that the program would actually do. 

Code summary:

1. The global variable is declared and initialized.

2. Iterations started from Module->function->basic block->instructions. The first function"pop_direct_branch" is skipped because it shouldn't be cloned. But a function pointer points to it to make it called later.

3. When find a called function start with p, return an integer and it was not cloned before (by searching a set holding the names of all cloned functions). Initialize a new function with void return type. Do function clone. 

4. Replace the old call site with new one (call cloned function). Modify the correspondent variables. Load return value from global variable. 

5. Iterate within the new cloned function, store the return value to global variable, call pop_direct_branch() and change the return instruction type from int to void. 

