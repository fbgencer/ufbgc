# ufbgc - Unit Test Framework



ufbgc is a simple unit test framework with the following features

- Different assert macros for different situations
- Only requires standard C-libraries
- User arguments can be provided to test functions
- Iteration over test functions with different input parameters
- Verbosity level of outputs



## Getting started

```C


```





### Running the example

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./ufbgc_example
```



### How to include into a project

If you are using CMake, CMakeList.txt file is provided at the repository directory

- Using as a third party library (Shared object file)

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install # Default it puts into /usr/local/lib and /usr/local/bin

To compile an example file run: 
$ gcc -L/usr/local/include/ -o ufbgcx example.c -lufbgc
$ ./ufbgcx

#If ./ufbgcx gives object file not found error, try this
$ LD_LIBRARY_PATH=/usr/local/lib

```

- Compiling with the project
  - ufbgc has only two files `ufbgc.c` and `ufbgc.h` so just copy it into project directory



#