# CUDA
A repository for practice CUDA programming

## Features
* [Use of cudamalloc(). Why the double pointer?](https://stackoverflow.com/questions/7989039/use-of-cudamalloc-why-the-double-pointer)
>   All CUDA API functions return an error code (or cudaSuccess if no error occured). All other parameters are passed by reference. However, in plain C you cannot have references, that's why you have to pass an address of the variable that you want the return information to be stored. Since you are returning a pointer, you need to pass a double-pointer.<br><br>
