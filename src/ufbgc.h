#include "stdlib.h"
#include "stdarg.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "time.h"

#define ANSI_COLOR_BLACK    "\033[1;30m"
#define ANSI_COLOR_RED      "\033[1;31m"
#define ANSI_COLOR_GREEN    "\033[1;32m"
#define ANSI_COLOR_YELLOW   "\033[1;33m"
#define ANSI_COLOR_BLUE     "\033[1;34m"
#define ANSI_COLOR_MAGENTA  "\033[1;35m"
#define ANSI_COLOR_CYAN     "\033[1;36m"
#define ANSI_COLOR_WHITE    "\033[1;37m"
#define ANSI_COLOR_RESET    "\033[0m"

#define ANSI_COLOR_BLACK_UNDERLINE    "\e[4;30m"
#define ANSI_COLOR_RED_UNDERLINE      "\e[4;31m"
#define ANSI_COLOR_GREEN_UNDERLINE    "\e[4;32m"
#define ANSI_COLOR_YELLOW_UNDERLINE   "\e[4;33m"
#define ANSI_COLOR_BLUE_UNDERLINE     "\e[4;34m"
#define ANSI_COLOR_MAGENTA_UNDERLINE  "\e[4;35m"
#define ANSI_COLOR_CYAN_UNDERLINE     "\e[4;36m"
#define ANSI_COLOR_WHITE_UNDERLINE    "\e[4;37m"

#define UFBGC_LOG_LEVEL_ERROR   0
#define UFBGC_LOG_LEVEL_WARNING 1
#define UFBGC_LOG_LEVEL_INFO    2


#ifndef UFBGC_LOG_LEVEL
    #define UFBGC_LOG_LEVEL UFBGC_LOG_LEVEL_ERROR
#endif

typedef enum {
    UFBGC_OK,
    UFBGC_FAIL
}ufbgc_return_t;

typedef enum {
    NO_OPTION,
    PASS_TEST,
}ufbgc_option_t;

typedef enum {
    UFBGC_LOG_ERROR = UFBGC_LOG_LEVEL_ERROR,
    UFBGC_LOG_WARNING = UFBGC_LOG_LEVEL_WARNING,
    UFBGC_LOG_INFO = UFBGC_LOG_LEVEL_INFO,
}ufbgc_log_verbosity_t;


typedef struct {
    const char * key;
    const void * value;
} ufbgc_args;

typedef struct {
    size_t no_iteration;
    ufbgc_args parameters[];
} ufbgc_test_parameters;


typedef ufbgc_return_t(* ufbgc_setup_fun_t)(ufbgc_test_parameters * parameters, void ** uarg);
typedef ufbgc_return_t(* ufbgc_test_fun_t)(ufbgc_test_parameters * parameters, void * uarg);
typedef ufbgc_return_t(* ufbgc_teardown_fun_t)(ufbgc_test_parameters * parameters, void * uarg);

typedef struct{
    ufbgc_test_fun_t test_f;
    const char * name;
    ufbgc_setup_fun_t setup_f;
    ufbgc_teardown_fun_t teardown_f;
    ufbgc_test_parameters * parameters;
    ufbgc_option_t option;
    ufbgc_log_verbosity_t log_level;
}ufbgc_test_frame;

ufbgc_return_t ufbgc_start_test(const ufbgc_test_frame * test_list);
const void * ufbgc_get_parameter(const char * key);
bool ufbgc_get_current_test_iterator(size_t * it);

ufbgc_log_verbosity_t ufbgc_get_current_test_verbosity();
FILE * ufbgc_get_current_test_file();



#define UFBGC_COLORED_PRINT(color,format) color format ANSI_COLOR_RESET
#define UFBGC_COLORED_PRINT_DETAILED(color,format) color "%s/%s:%u" format ANSI_COLOR_RESET ,__FILE__,__FUNCTION__,  __LINE__


#define ufbgc_print_red(fl,format,...)     fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_RED,format),##__VA_ARGS__)
#define ufbgc_print_green(fl,format,...)   fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_GREEN,format),##__VA_ARGS__)
#define ufbgc_print_blue(fl,format,...)    fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_BLUE,format),##__VA_ARGS__)
#define ufbgc_print_yellow(fl,format,...)  fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_YELLOW,format),##__VA_ARGS__)
#define ufbgc_print_cyan(fl,format,...)    fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_CYAN,format),##__VA_ARGS__)
#define ufbgc_print_white(fl,format,...)   fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_WHITE,format),##__VA_ARGS__)
#define ufbgc_print_magenta(fl,format,...) fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_MAGENTA,format),##__VA_ARGS__)
#define ufbgc_print_black(fl,format,...)   fprintf(fl,UFBGC_COLORED_PRINT(ANSI_COLOR_BLACK,format),##__VA_ARGS__)
#define ufbgc_print_line(fl,color)         fprintf(fl,UFBGC_COLORED_PRINT_DETAILED(color,""))

#define ufbgc_print_fail(name,format,...)                       \
    do{                                                         \
        ufbgc_print_black(stdout,format,##__VA_ARGS__);         \
        ufbgc_print_black(stdout,"  -->  %s failed @[",name);   \
        ufbgc_print_line(stdout,ANSI_COLOR_RED_UNDERLINE);      \
        ufbgc_print_black(stdout,"]\n");                        \
    }while(0)


#define ufbgc_print_pass(name,format,...)                       \
    do{                                                         \
        ufbgc_print_green(stdout,format,##__VA_ARGS__);         \
        ufbgc_print_green(stdout,"  -->  %s passed @[",name);   \
        ufbgc_print_line(stdout,ANSI_COLOR_GREEN_UNDERLINE);    \
        ufbgc_print_green(stdout,"]\n");                        \
    }while(0)


#define ufbgc_assert_full(log_level,type,condition,expected,should_return,format,...)                       \
    do{                                                                                                     \
        if(ufbgc_get_current_test_verbosity() >= log_level && __builtin_expect(condition,1) == expected){   \
            ufbgc_print_fail(type,"%s",#condition);                                                         \
            if(strcmp(format,"")){                                                                          \
                ufbgc_print_yellow(stdout,"\nNote:{");                                                      \
                ufbgc_print_blue(stdout,format,##__VA_ARGS__);                                              \
                ufbgc_print_yellow(stdout,"}\n\n");                                                         \
            }                                                                                               \
            if(should_return){return UFBGC_FAIL;}                                                           \
        }                                                                                                   \
        else{                                                                                               \
            if(ufbgc_get_current_test_verbosity() >= UFBGC_LOG_LEVEL_INFO){                                 \
                ufbgc_print_pass(type,"%s",#condition);                                                     \
            }                                                                                               \
        }                                                                                                   \
    }while(0)




#define __ufbgc_internal_assert_full(type,condition,expected,should_return,format,...)          \
    do{                                                                                         \
        if(__builtin_expect(condition,1) == expected){                                          \
            ufbgc_print_fail(type,"%s [" format "]",#condition,##__VA_ARGS__);                  \
            if(should_return){return UFBGC_FAIL;}                                               \
        }                                                                                       \
    }while(0)



#define ufbgc_assert(condition) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert",condition,false,true,"")
#define ufbgc_assert_true(condition) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert",condition,false,true,"")
#define ufbgc_assert_false(condition) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert",condition,true,true,"")
#define ufbgc_assert_(condition,format, ...) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert",condition,false,true,format,##__VA_ARGS__)
#define ufbgc_assert_true_(condition,format, ...) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert",condition,false,true,format,##__VA_ARGS__)
#define ufbgc_assert_false_(condition,format, ...) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert",condition,true,true,format,##__VA_ARGS__)


#define ufbgc_likely(condition) ufbgc_assert_full(UFBGC_LOG_WARNING,"likely",condition,false,false,"");
#define ufbgc_likely_(condition,format, ...) ufbgc_assert_full(UFBGC_LOG_WARNING,"likely",condition,false,false,format,##__VA_ARGS__)
#define ufbgc_unlikely(condition) ufbgc_assert_full(UFBGC_LOG_WARNING,"unlikely",condition,true,false,"")
#define ufbgc_unlikely_(condition,format, ...) ufbgc_assert_full(UFBGC_LOG_WARNING,"unlikely",condition,true,false,format,##__VA_ARGS__)

#define ufbgc_assert_op(a,op,b) ufbgc_assert(a op b)
#define ufbgc_assert_eq(a,b) ufbgc_assert(a == b)
#define ufbgc_assert_eqstr(a,b) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert_eqstr",!strcmp(a,b),false,true,"")
#define ufbgc_assert_eqmem(a,b,size) ufbgc_assert_full(UFBGC_LOG_ERROR,"assert_eqmem",!memcmp(a,b,size),false,true,"")


#define __ufbgc_internal_assert(condition) __ufbgc_internal_assert_full("internal-assert",condition,false,true,"");
#define __ufbgc_internal_assert_(condition,format, ...) __ufbgc_internal_assert_full("internal-assert",condition,false,true,format,##__VA_ARGS__);

#define ufbgc_get_param(buf, key, cast)                         \
    do{                                                         \
        size_t it = 0;                                          \
        ufbgc_get_current_test_iterator(&it);                   \
        const void * p = ufbgc_get_parameter(key);              \
        __ufbgc_internal_assert_(p != NULL,"No key found!");    \
        buf = ((cast) p)[it];                                   \
    }while(0)




//Time related
void ufbgc_get_current_time(struct tm * dest);
double ufbgc_get_execution_time(clock_t start);
double ufbgc_get_execution_time_ms(clock_t start);
double ufbgc_get_execution_time_us(clock_t start);



#define UFBGC_TEST(test_function_name, _opt, _param,sf,tf,tdf)                                          \
    ufbgc_return_t test_function_name##_setup(ufbgc_test_parameters * parameters, void ** uarg)         \
        {sf return UFBGC_OK;}                                                                           \
    ufbgc_return_t test_function_name(ufbgc_test_parameters * parameters, void * uarg)                  \
        {tf return UFBGC_OK;}                                                                           \
    ufbgc_return_t test_function_name##_teardown(ufbgc_test_parameters * parameters, void * uarg)       \
        {tdf return UFBGC_OK;}                                                                          \
        const ufbgc_test_frame test_function_name##_frame = {                                           \
        .test_f = test_function_name,                                                                   \
        .name = #test_function_name,                                                                    \
        .setup_f = test_function_name##_setup,                                                          \
        .teardown_f = test_function_name##_teardown,                                                    \
        .parameters = _param,                                                                           \
        .option = _opt};

//Put a newline here to make compiler happy
