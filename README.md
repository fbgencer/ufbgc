# ufbgc - Unit Test Framework

`ufbgc` is a simple unit test framework with the following features

- Versatile assert macros for different situations
- Only requires standard C-libraries, no dependency
- User arguments can be provided to test functions
- Iterations over test functions with different input parameters
- Time stamps and execution time of the tests
- Assigning verbosity level of tests (error, warning and info levels)
- File output can be printed to a specific file (for each test)
- `Setup`(called before test) and `Teardown`(called after test) functions


## Getting started


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


- **Custom test function type**

```c

ufbgc_return_t test_function(ufbgc_test_parameters * parameters, void * uarg){
    double pi = 3.141592654;
    ufbgc_assert(0 != 1); 			//This condition is true
    ufbgc_assert_op(pi ,==, pi);    //This is also true
    return UFBGC_OK;	//Each test function must return UFBGC_OK at the end, this will tell test function is passed its tests
}

//Setup function type
ufbgc_return_t example_setup_function(ufbgc_test_parameters * parameters, void ** uarg);
//Teardown function type
ufbgc_return_t example_teardown_function(ufbgc_test_parameters * parameters, void * uarg);
```


- **UFBGC_TEST macro**

```c
/*
    UFBGC_TEST macro creates all the necessary fields and functions for the desired test
    UFBGC_TEST( Test2,              //Test function name
                NO_OPTION,          //Option of the test
                UFBGC_LOG_WARNING,  //Log level of the test
                NULL,               //Output file
                NULL,               //ufbgc_test_parameters
                {...},              //Setup function
                {...},              //Test function
                {...})              //Teardown function
    
    This macro creates "const ufbgc_test_frame " variable with the given test name + _frame
    So for this case Test2_frame is created as const ufbgc_test_frame to provide to the test-suite
*/
UFBGC_TEST(Test2,NO_OPTION,UFBGC_LOG_WARNING,NULL,NULL,
{
    printf("Test2 Setup function!\n");
    printf("Address of uarg %p\n",uarg); //uarg is not NULL but *uarg is
    *uarg = strdup("hello");        //Setup function provides void ** uarg, so *uarg is used here
},
{
    printf("Test function of test2!\n");
    char ch = 'b';

    ufbgc_assert(0 != 1);
    ufbgc_assert_op('a', ==, 'a');
    ufbgc_assert_op(ch, ==, 'b');
    printf("Message:%s\n",(char*)uarg); //Allocated user arg in the setup function is passed to test
},
{
    printf("Test2 teardown\n");
    printf("Address of uarg %p | message:%s\n",uarg, (char*)uarg); //Allocated user arg in the setup function is passed to teardown
    free(uarg); //We need to free uarg because we used strdup in the test function
})
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
    ufbgc_get_current_test_iterator(&it); //Current test iterator fills it variable to which iteration is it
    printf("Double with iterator: %g\n",dx[it]); //Printed double values depend on the iterator type

    return UFBGC_OK;
}
```
- **Test frame**

    Test frame contains necessary information about the test, it is used to run the test
    It has the following struct type
```c
typedef struct{
    ufbgc_test_fun_t test_f;                //<! Actual test function
    const char * name;                      //<! Test name
    ufbgc_setup_fun_t setup_f;              //<! Setup function
    ufbgc_teardown_fun_t teardown_f;        //<! Teardown function
    ufbgc_test_parameters * parameters;     //<! Test parameters
    ufbgc_option_t option;                  //<! Test options
    ufbgc_log_verbosity_t log_level;        //<! Log level of the test
    const char * output_file;               //<! Output file name
}ufbgc_test_frame;
```
Example of test frame can be created as following
```c
    .test_f = example_test1,            //Test function
    .name = "test1",                    //Name of the test function
    .setup_f = NULL,                    //Setup function
    .teardown_f = NULL,                 //Teardown function
    .parameters = NULL,                 //Parameters
    .option = NO_OPTION,                //Test option
    .log_level = UFBGC_LOG_WARNING,     //Test verbosity
    .output_file = "test1_output.txt",  //Instead of stdout, all test results will be printed into test1_output.txt
```

- **Test Options**
  
    There is only one test option at the current version, which is `PASS_TEST` macro, if test has this option is set then its test function is not gonna be called (This is helpful for running multiple tests and suppress the output of the test)

- **Test Log levels**

| Macro                       | Output                                                       | Color  |
| --------------------------- | ------------------------------------------------------------ | ------ |
| `UFBGC_LOG_ERROR` (default) | If `ufbgc_assert*`macros fail, results will be printed       | Red    |
| `UFBGC_LOG_WARNING`         | If `ufbgc_assert*` + `ufbgc_likely*`macros fail, results will be printed | Yellow |
| `UFBGC_LOG_INFO`            | Failure printing same as `UFBGC_LOG_WARNING`, but also prints all assert macro results | Green  |

- **Test suite**

    Test suite is called test lists in `ufbgc` and their type is `ufbgc_test_frame`.
    Example test suite from `example/example.c`
```c
ufbgc_test_frame test_list[] = {
    {
        .test_f = example_test1,            //Test function
        .name = "test1",                    //Name of the test function
        .setup_f = NULL,                    //Setup function
        .teardown_f = NULL,                 //Teardown function
        .parameters = NULL,                 //Parameters
        .option = NO_OPTION,                //Test option
        .log_level = UFBGC_LOG_WARNING,     //Test verbosity
        .output_file = "test1_output.txt",  //Instead of stdout, all test results will be printed into test1_output.txt
    },
            Test2_frame,                    //This variable is created with the UFBGC_TEST macro, hence needs to add into test list
    {
        .test_f = test3,
        .name = "test3",
        .setup_f = NULL,
        .teardown_f = NULL,
        .parameters = NULL,
        .option = PASS_TEST,                //ufbgc will pass this test and not run
    },
    {
        .test_f = parameter_test,
        .name = "parameter-test",
        .setup_f = NULL,
        .teardown_f = NULL,
        .parameters = &paramtest_param,
        .option = NO_OPTION,
    },
    {
        .test_f = operator_test,
        .name = "operator-test",
        .parameters = &operator_test_param,
        .option = NO_OPTION,
        .log_level = UFBGC_LOG_INFO,        //Verbosity level set to UFBGC_LOG_INFO, so every information about test will be printed(even if assertion not fails) 
    },
    {
        NULL //Last value of the test list must be NULL, so program will stop here
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
