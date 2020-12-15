//#define UFBGC_LOG_LEVEL 2
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


ufbgc_return_t test1(ufbgc_test_parameters * parameters, ufbgc_args * uarg){


    //Lets check user args first
    int * iptr = (int *)ufbgc_get_arg("integer-arg");

    ufbgc_assert(iptr != NULL); //We know arg1 is in the list, lets check its value
    ufbgc_likely(*iptr == -520); //Warns the system but continues to test

    int x = sizeof(double);

    ufbgc_likely(sizeof(12.3) == x);

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

ufbgc_return_t test2(ufbgc_test_parameters * parameters, ufbgc_args * uarg){

    char ch = 'b';
    double pi = 3.141592654;

    ufbgc_assert(0 != 1);
    ufbgc_assert_op('a', ==, 'a');
    ufbgc_assert_op(ch, ==, 'b');

    ufbgc_assert_op(pi ,==, pi);

    return UFBGC_OK;
}


ufbgc_return_t test3(ufbgc_test_parameters * parameters, ufbgc_args * uarg){

    //This will be asserted but this test will be added to test list with pass test option
    ufbgc_assert(0 == 0);
    
    return UFBGC_OK;
}


const char * val1[] = {"a1","a2","a3",};

const int  vali[] = {100,200,300,500};

ufbgc_test_parameters paramtest_param = {
    .no_iteration = 3,
    .parameters = {
        {
            .key = "def",
            .value = val1
        },
        {
            .key = "foo",
            .value = vali
        },
        {
            .key = NULL,
            .value = NULL,
        }
    }
};



ufbgc_return_t parameter_test(ufbgc_test_parameters * parameters, ufbgc_args * uarg){

    const char ** strarray = (const char **) ufbgc_get_param("def");
    const char * str = (const char *)ufbgc_current_param(strarray,sizeof(const char *));

    ufbgc_assert(str != NULL);

    printf("Got string '%s' ",str);

    const int * ia = (const int *) ufbgc_get_param("foo");
    int * x = (int *)ufbgc_current_param(ia,sizeof(int));
    printf("Got int :%d\n",*x);

    return UFBGC_OK;
}

static const ufbgc_test_frame test_list[] = {
    {
        .fun_ptr = test2,
        .name = "test2",
        .option = PASS_TEST,
    },
    {
        .fun_ptr = test1,
        .name = "test1",
        .option = PASS_TEST,
        .parameters = NULL,
        .uarg = test1_args
    },
    {
        .fun_ptr = test3,
        .name = "test3",
        .option = PASS_TEST,
    },
    {
        .fun_ptr = parameter_test,
        .name = "parameter-test",
        .option = NO_OPTION,
        .parameters = &paramtest_param,
        .uarg = NULL
    },

    NULL,
};


int main(int argc, char const *argv[]){
    
    ufbgc_start_test(test_list);

    return 0;
}
