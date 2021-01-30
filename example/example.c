#include "ufbgc.h"


/*
    Example test function shows assertion and likely macros
*/
ufbgc_return_t example_test1(ufbgc_test_parameters * parameters, void * uarg){

    int x = sizeof(double);         //sizeof double is 8

    ufbgc_likely(sizeof(12) == x);  //sizeof(int) may not be 8, likely warns but test continues


    //We know that this is not since 123.22 is a double number and sizeof(double) is equal to x
    //Assertion checks zero, this equality here is zero!(4 != 4 is zero) 
    ufbgc_assert_(sizeof(123.22) != x,
        "This is a very important mistake\n"
        "sticker note is left here to make sure we never got this error!\n"
        "which also shows how to put notes on assertion :)"
    );

    //Test will fail at above line, we couldn't even come to this point

    const char * str1 = "hEllo:)";
    const char * str2 = "hEllo:)";

    //Both strings are equal
    ufbgc_assert_eqstr(str1,str2);

    int n1[] = {[0] = 1, [1] = 2, [2] = 3};
    long n2[] = {10, 2 ,3};

    //Arrays are not the same, ufbgc assert this line!
    ufbgc_assert_eqmem(n1,n2,3);

    return UFBGC_OK;
}


/*
    UFBGC_TEST macro creates all the necessary fields and functions for the desired test
    UFBGC_TEST( Test2,              //Test function name
                NO_OPTION,          //Option of the test
                UFBGC_LOG_WARNING,  //Log level of the test
                NULL,               //Output file
                NULL,               //ufbgc_test_parameters
                {...},              //Setup function
                {...},              //Test function
                {...})              //Teardown functions
    
    This macro creates "const ufbgc_test_frame " variable with the given test name + _frame
    So for this case Test2_frame is created as const ufbgc_test_frame to provide the test-suite
*/
UFBGC_TEST(Test2,NO_OPTION,UFBGC_LOG_WARNING,NULL,NULL,
{
    printf("Test2 Setup function!\n");
    printf("Address of uarg %p\n",uarg);

    *uarg = strdup("hello");        //Setup function provides void ** uarg, so *uarg is used here
},
{
    printf("Test function of test2!\n");
    char ch = 'b';
    double pi = 3.141592654;

    ufbgc_assert(0 != 1);
    ufbgc_assert_op('a', ==, 'a');
    ufbgc_assert_op(ch, ==, 'b');

    ufbgc_assert_op(pi ,==, pi);
    printf("Address of uarg %p | message:%s\n",uarg, (char*)uarg); //Allocated user arg in the setup function is passed to test
},
{
    printf("Test2 teardown\n");
    printf("Address of uarg %p | message:%s\n",uarg, (char*)uarg); //Allocated user arg in the setup function is passed to teardown
    free(uarg); //We need to free uarg because we used strdup in the test function
})

/*
    This is the same function as above but only show the test function itself
*/
ufbgc_return_t test2_as_function(ufbgc_test_parameters * parameters, void * uarg){

    char ch = 'b';
    double pi = 3.141592654;

    ufbgc_assert(0 != 1);
    ufbgc_assert_op('a', ==, 'a');
    ufbgc_assert_op(ch, ==, 'b');

    ufbgc_assert_op(pi ,==, pi);

    return UFBGC_OK;
}

/* 
    test3 is used to show pass-test option
*/
ufbgc_return_t test3(ufbgc_test_parameters * parameters, void * uarg){

    //This will be asserted but this test will be added to test list with pass test option
    ufbgc_assert(0 == 0);
    
    return UFBGC_OK;
}


/*
    Creating parameter arrays for the test that user wants to iterate with the different parameters
    Below test function will use no_iteration field which is number of iterations
*/
const char * val1[] = {"a1","a2","a3"};
int  vali[] = {100,200,300,500};
double  valdb[] = {1.1,1.2,1.3};
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
        {
            .key = NULL, //Last parameter or its key must be NULL
            .value = NULL,
        }
    }
};
ufbgc_return_t parameter_test(ufbgc_test_parameters * parameters, void * uarg){

    const char * str;
    /*
        This macro needs variable, key and casting
        Casting is necessary, since "string-inputs" holds array of const char *, we cast it as const char **
    */
    ufbgc_get_param(str,"string-inputs",const char **);

    ufbgc_assert(str != NULL);

    printf("Got string:'%s'\n",str);

    /*
        "integer-inputs" holds integer array, so casting is int *, and so on  
    */
    int ia;
    ufbgc_get_param(ia,"integer-inputs",int *);
    printf("Got int:%d\n",ia);


    double db;
    ufbgc_get_param(db,"double-inputs",double *);
    printf("Got double:%g\n",db);


    /*
        Instead of taking the parameter automatically, user can also take the parameter value from its key
        Now below "dx" variable is equal to above created valdb array
    */
    double * dx = (double *)ufbgc_get_parameter("double-inputs");
    /*
        iterator of the current test can be taken with the function ufbgc_get_current_test_iterator
    */
    size_t it = 0;
    ufbgc_get_current_test_iterator(&it);
    printf("Double with iterator: %g\n",dx[it]);

    return UFBGC_OK;
}

/*
    This test is similar to "parameter_test"
    3 parameters is used to check addition operator, 
    expected array is the sum of two arrays but there is a miscalculation at the 4th expected value
    Test function must assert and shows that test it failed at the 3rd iteration
*/
int  operator_test_inputs1[]   = {100,    200,    300,   400,     500};
int  operator_test_inputs2[]   = {-50,    10,     3,     4,       23};
int  operator_test_expected[]  = {100-50, 200+10, 300+3, 400+4+2, 500+23};
ufbgc_test_parameters operator_test_param = {
    .no_iteration = 5,
    .parameters = {
        {
            .key = "input1",
            .value = operator_test_inputs1
        },
        {
            .key = "input2",
            .value = operator_test_inputs2
        },
        {
            .key = "expected",
            .value = operator_test_expected
        },
        {
            .key = NULL,
            .value = NULL,
        }
    }
};

ufbgc_return_t operator_test(ufbgc_test_parameters * parameters, void * uarg){

    int * a = (int *)ufbgc_get_parameter("input1");
    int * b = (int *)ufbgc_get_parameter("input2");
    int * c = (int *)ufbgc_get_parameter("expected");
    size_t it = 0;
    ufbgc_get_current_test_iterator(&it);

    //Instead of the macro we will take the values directly from its pointers

    int sum = a[it] + b[it];
    ufbgc_assert_eq(sum,c[it]);

    return UFBGC_OK;
}


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

int main(int argc, char const *argv[]){
    
    ufbgc_start_test(test_list, ufbgc_test_frame_array_length(test_list));

    return 0;
}
