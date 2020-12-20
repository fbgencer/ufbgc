#include "ufbgc.h"

typedef struct{
    const ufbgc_test_frame * frame;
    size_t frame_iterator;
    bool frame_iterateable;
    clock_t test_start;
}internal_ufbgc_test_frame;

static internal_ufbgc_test_frame current_test_frame = {
    .frame = NULL,
    .frame_iterator = 0,
    .frame_iterateable = false,
};

ufbgc_return_t ufbgc_start_test(const ufbgc_test_frame * test_list){

    ufbgc_print_magenta(stdout,"ufbgc - starting tests\n\n");

    __ufbgc_internal_assert_(test_list != NULL,"Test frame pointer is NULL");

    const ufbgc_test_frame * tframe = test_list;

    while(tframe != NULL && tframe->test_f != NULL){
        current_test_frame.frame = tframe;

        struct tm current_time;
        ufbgc_get_current_time(&current_time);

        ufbgc_print_cyan(stdout,"Starting test : '%s' @ %s",tframe->name ? tframe->name : "NULL" ,asctime(&current_time));
        
        if(tframe->option == PASS_TEST){
            ufbgc_print_yellow(stdout,"'%s'\t\t\t%-10s\n",tframe->name,"[PASS]");
        }
        else if(tframe->test_f != NULL){
            if(tframe->parameters != NULL) current_test_frame.frame_iterateable = true;

            do{
                if(current_test_frame.frame_iterateable){
                    ufbgc_print_blue(stdout,"Iteration : %lu\n",current_test_frame.frame_iterator);
                }
                
                void * user_arg = NULL;

                if(tframe->setup_f != NULL){
                    tframe->setup_f(tframe->parameters, &user_arg);
                }

                current_test_frame.test_start = clock();
                if(tframe->test_f(tframe->parameters,user_arg) == UFBGC_OK){
                    ufbgc_print_green(stdout,"'%s'\t\t\t%-10s %gms\n",tframe->name,"[OK]",ufbgc_get_execution_time_ms(current_test_frame.test_start));
                }
                else{
                    ufbgc_print_red(stdout,"'%s'\t\t\t%-10s\n",tframe->name,"[FAILED]");
                }
                
                if(tframe->teardown_f != NULL){
                    tframe->teardown_f(tframe->parameters, user_arg);   
                }

                current_test_frame.frame_iterator++;
            }while(current_test_frame.frame_iterateable && current_test_frame.frame_iterator < tframe->parameters->no_iteration );

        }

        ufbgc_print_magenta(stdout,"------------------------------------------------------------\n");
        tframe++;
        current_test_frame.frame = NULL;
        current_test_frame.frame_iterator = 0;
        current_test_frame.frame_iterateable = false;
    }


    ufbgc_print_magenta(stdout,"ufbgc - tests completed\n\n");
    return UFBGC_OK;
}

ufbgc_log_verbosity_t ufbgc_get_current_test_verbosity(){
    if(current_test_frame.frame != NULL){
        return current_test_frame.frame->log_level;
    }
    return UFBGC_LOG_ERROR;
}


bool ufbgc_get_current_test_iterator(size_t * it){
    if(current_test_frame.frame == NULL || it == NULL){
        return false;
    }

    *it = current_test_frame.frame_iterator;
    return true;
}

const void * ufbgc_get_parameter(const char * key){
    if(current_test_frame.frame == NULL){
        return NULL;
    }
    //Use current tframe
    if(current_test_frame.frame->parameters != NULL){
        ufbgc_test_parameters * params = current_test_frame.frame->parameters;
        const ufbgc_args * uarg = params->parameters;
        while(uarg != NULL && uarg->key != NULL){
            if(!strcmp(uarg->key,key)){
                return uarg->value;
            }
            uarg++;
        }
    }

    current_test_frame.frame_iterateable = false;

    return NULL;
}


void ufbgc_get_current_time(struct tm * dest){
    time_t rawtime;
    time ( &rawtime );
    struct tm * timeinfo = localtime ( &rawtime );
    memcpy(dest,timeinfo,sizeof(struct tm));
}


double ufbgc_get_execution_time(clock_t start){
    return (double)(clock() - start) / CLOCKS_PER_SEC; 
}
double ufbgc_get_execution_time_ms(clock_t start){
    return (double)(clock() - start) / (CLOCKS_PER_SEC/1000); 
}
double ufbgc_get_execution_time_us(clock_t start){
    return (double)(clock() - start) / (CLOCKS_PER_SEC/1000000); 
}