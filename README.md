# ufbgc - Unit Test Framework

`ufbgc` is a simple unit test framework with the following features

- Different assert macros for different situations
- Only requires standard C-libraries
- User arguments can be provided to test functions
- Iteration over test functions with different input parameters
- Verbosity level of outputs



## Getting started

- **Test function type**

```c

ufbgc_return_t test_function(ufbgc_test_parameters * parameters, ufbgc_args * uarg){

    char ch = 'x';
    double pi = 3.141592654;

    ufbgc_assert(0 != 1); 			//This condition is true
    ufbgc_assert_op('a', ==, 'a');	//This is also true!
    ufbgc_assert_op(ch, ==, 'b');  	//This condition is wrong, ch is 'x' ufbgc asserts this line and breaks the tests

    ufbgc_assert_op(pi ,==, pi); //We couldn't have chance to take this assert

    return UFBGC_OK;	//Each test function must return UFBGC_OK at the end, this will tell test function is passed its tests
}

```



- **Assertion macros** 

```C

//Assertions
ufbgc_assert(condition) 	  			//checks condition == true, fails if condition == false
ufbgc_assert_true(condition)  			//checks condition == true, fails if condition == false
ufbgc_assert_false(condition) 			//checks condition == false, fails if condition == true

//Assertions with user defined notes (these macros uses printf format)
ufbgc_assert_(condition,format, ...)
ufbgc_assert_true_(condition,format, ...) 
ufbgc_assert_false_(condition,format, ...) 

//likely and unlikely is used when error does not affect the rest of the test
ufbgc_likely(condition) 				//checks condition == true, fails if condition == false but test continues
ufbgc_likely_(condition,format, ...) 	//User defined message can be printed
ufbgc_unlikely(condition) 				//checks condition == false, fails if condition == true but test continues
ufbgc_unlikely_(condition,format, ...) 	//User defined message can be printed

ufbgc_assert_op(a,op,b) 		//Checks for condition a op b, e.g. a == b
ufbgc_assert_eq(a,b)			//Same as ufbgc_assert(a == b)
ufbgc_assert_eqstr(a,b) 		//String comparison, same as ufbgc_assert(!strcmp(a,b))
ufbgc_assert_eqmem(a,b,size) 	//Block of memory comparison, same as ufbgc_assert(!memcmp(a,b,size))
```

- **User parameters and iterations**

  `example/example.c` contains an example how to provide parameters, but basically

```c
const char * val1[] = {"a1","a2","a3"};
int  vali[] = {100,200,300,500};
double  valdb[] = {1.1,1.2,1.3};

//ufbgc test parameters contains number of iteration and parameters field
ufbgc_test_parameters paramtest_param = {
    .no_iteration = 3,
    .parameters = {
        {
            .key = "string-inputs",
            .value = val1
        },
        {
            .key = "integer-inputs",
            .value = vali
        },
        {
            .key = "double-inputs",
            .value = valdb
        },
        //Last parameter can be NULL, or its key must be NULL (either works to understand size of the parameters array)
        {
            .key = NULL,
            .value = NULL,
        }
    }
};

//In the test function there are two options to extract a parameter
ufbgc_return_t parameter_test(ufbgc_test_parameters * parameters, ufbgc_args * uarg){

    const char * str; //Pointer that holds our input string
    
    //This macro takes three arguments, variable to store, key of the parameter, and value type of the parameter
    //value type was array of char pointers so 'const char **' is used here 
    ufbgc_get_param(str,"string-inputs",const char **);
    ufbgc_assert(str != NULL);

    printf("Got string:'%s'\n",str); //This line prints extracted string

    //Instead of using ufbgc_get_param macro, this macro finds the parameter from its key
    double * dx = (double *)ufbgc_get_parameter("double-inputs"); //We cast it to double*
    size_t it = 0;	
    get_current_test_iterator(&it); //Current test iterator fills it variable to which iteration is it
    printf("Double with iterator: %g\n",dx[it]); //Printed double values depend on the iterator type

    return UFBGC_OK;
}
```

- **User arguments**

  `ufbgc_args` types has the following structure and its left as users wish how to provide data, it is called each time the test function is called

```c
typedef struct {
    const char * key;
    void * value;
} ufbgc_args;
```

- **Test suite**

  Test suite is called test lists in `ufbgc` and their type is `ufbgc_test_frame`, each element in `ufbgc_test_frame` takes function pointer of the test, name of the test, options of the tests and finally parameters and user arguments

```c
static const ufbgc_test_frame test_list[] = {
    {
        .fun_ptr = test2,
        .name = "test2",
        .option = NO_OPTION,
        .parameters = NULL,
        .uarg = NULL,
    },
    {
        .fun_ptr = test1,
        .name = "test1",
        .option = NO_OPTION,
        .parameters = NULL,
        .uarg = test1_args,
    },
    //Last value must be NULL
    {
        NULL
    }
    ,
};

//To run the test-suite call the function
ufbgc_start_test(test_list);
```



## Running the example

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./ufbgc_example
```



## How to include into a project

If you are using cmake, CMakeList.txt file is provided at the repository directory

- Compiling with the existing project

  src folder contains its own CMakeLists.txt file, just add the following lines into project cmake file (`ufbgc` exports itself as `ufbgc` so link the library with your target)

```cmake
add_subdirectory("ufbgc/src")
target_link_libraries(<target_name> ufbgc)
```

- Using as a third party library (Shared object file .so)

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install # Default it puts into /usr/local/lib and /usr/local/bin

To compile an example file run: 
$ gcc -L/usr/local/include/ -o <executable_name> example.c -lufbgc
$ ./<executable_name>

#If ./<executable_name> gives object file not found error, try this
$ LD_LIBRARY_PATH=/usr/local/lib
#This will add path of ufbgc.so file

```

- Taking the source code
  - License allows to copy `ufbgc` , so just copy the src folder which has only two files `ufbgc.c` and `ufbgc.h` and compile it with your project



# TO-DO List

- [ ] Assigning verbosity level of tests
- [ ] File output for test functions
- [ ] `Setup`(called before test) and `Teardown`(called after test) functions
- [ ] Time stamp on logs
- [ ] Different test options