# rmfast
`rmfast` is a tool that is conceived to remove a lot of files as fast as possible.  
It is the equivalent of `rd /Q /S` on Windows. And `rm -rf` on Linux.  
It is only implemented for Windows for now.  

# Results
Removing 50,000 files in 150 folders:  
`Explorer` : 60.0 seconds  
`rd /Q /S` : 23.5 seconds  
`rmfast`   : 19.0 seconds  

`rmfast` is 19% faster than `rd` and 32% faster than `Windows Explorer`.


# TODO
* Get faster  
* Try some multithreading to see if it can be faster  
* Remove stdlib  
