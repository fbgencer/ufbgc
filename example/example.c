#define UFBGC_LOG_LEVEL 3
#include "ufbgc.h"


int __test1int = -523;
double __test1db = 3.1415;
ufbgc_args test1_args[] = {
    {   .key = "integer-arg",
        .value = &__test1int
    },
    {
        .key = "arg2",
        .value = "ufbgc-string-example"
    },
    {
        .key = "double_arg",
        .value = &__test1db
    },
    {
        .key = NULL,
        .value = NULL
    }
};


ufbgc_return_t test1(ufbgc_test_parameters * parameters, void * uarg){


    int x = sizeof(double);

    ufbgc_likely(sizeof(12) == x);

    ufbgc_assert_(sizeof(123.22) != x,
        "This is a very important mistake\n"
        "sticker note is left here to make sure we never got this error!\n"
        "which also shows how to put notes on assertion :)"
    );

    const char * str1 = "hello:)";
    const char * str2 = "hello:)";

    ufbgc_assert_eqstr(str1,str2);

    int n1[] = {[0] = 1, [1] = 2, [2] = 3};
    long n2[] = { 10,2,3};

    ufbgc_assert_eqmem(n1,n2,3);

    return UFBGC_OK;
}


UFBGC_TEST(Testfoo,NO_OPTION,NULL,
{
    printf("Test2 Setup function!\n");
    printf("Address of uarg %p\n",uarg);

    *uarg = strdup("hello");
},
{
    printf("Test function of test2!\n");
    char ch = 'b';
    double pi = 3.141592654;

    ufbgc_assert(0 != 1);
    ufbgc_assert_op('a', ==, 'a');
    ufbgc_assert_op(ch, ==, 'b');

    ufbgc_assert_op(pi ,==, pi);
    printf("Address of uarg %p | message:%s\n",uarg, (char*)uarg);
},
{
    printf("Test2 teardown\n");
    printf("Address of uarg %p | message:%s\n",uarg, (char*)uarg);
    free(uarg);
})

ufbgc_return_t test2_as_function(ufbgc_test_parameters * parameters, void * uarg){

    char ch = 'b';
    double pi = 3.141592654;

    ufbgc_assert(0 != 1);
    ufbgc_assert_op('a', ==, 'a');
    ufbgc_assert_op(ch, ==, 'b');

    ufbgc_assert_op(pi ,==, pi);

    return UFBGC_OK;
}


ufbgc_return_t test3(ufbgc_test_parameters * parameters, void * uarg){

    //This will be asserted but this test will be added to test list with pass test option
    ufbgc_assert(0 == 0);
    
    return UFBGC_OK;
}


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
            .key = NULL,
            .value = NULL,
        }
    }
};

ufbgc_return_t parameter_test(ufbgc_test_parameters * parameters, void * uarg){

    const char * str;
    ufbgc_get_param(str,"string-inputs",const char **);

    ufbgc_assert(str != NULL);

    printf("Got string:'%s'\n",str);

    int ia;
    ufbgc_get_param(ia,"integer-inputs",int *);
    printf("Got int:%d\n",ia);


    double db;
    ufbgc_get_param(db,"double-inputs",double *);
    printf("Got double:%g\n",db);


    double * dx = (double *)ufbgc_get_parameter("double-inputs");
    size_t it = 0;
    ufbgc_get_current_test_iterator(&it);
    printf("Double with iterator: %g\n",dx[it]);


    return UFBGC_OK;
}



int  operator_test_inputs1[]    =   {100,200,300,400,500};
int  operator_test_inputs2[]    =   {-50,10,3,4,23};
int  operator_test_expected[]   =   {50,210,303,4042,523};
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

    int sum = a[it] + b[it];
    ufbgc_assert_eq(sum,c[it]);

    return UFBGC_OK;
}


ufbgc_test_frame test_list[] = {
        Testfoo_frame,
    {
        .test_f = test1,
        .name = "test1",
        .setup_f = NULL,
        .teardown_f = NULL,
        .parameters = NULL,
        .option = NO_OPTION,
        .log_level = UFBGC_LOG_LEVEL_WARNING,
    },
    {
        .test_f = test3,
        .name = "test3",
        .setup_f = NULL,
        .teardown_f = NULL,
        .parameters = NULL,
        .option = PASS_TEST,
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
        .log_level = UFBGC_LOG_LEVEL_INFO,
    },
    {
        NULL
    }
    ,
};


    



int main(int argc, char const *argv[]){
    
    ufbgc_start_test(test_list);

    return 0;
}
