#include "ufbgc.h"

typedef struct{
    const ufbgc_test_frame * frame;
    size_t frame_iterator;
    bool frame_iterateable;
    clock_t test_start;
    FILE * output_file;
}internal_ufbgc_test_frame;

static internal_ufbgc_test_frame current_test_frame = {
    .frame = NULL,
    .frame_iterator = 0,
    .frame_iterateable = false,
    .output_file = NULL,
};

ufbgc_return_t ufbgc_start_test(const ufbgc_test_frame * test_list, size_t list_len){

    ufbgc_print_magenta(stdout,"ufbgc - starting tests\n\n");

    __ufbgc_internal_assert_(test_list != NULL,"Test frame pointer is NULL");
    __ufbgc_internal_assert_(list_len > 0,"Test list length must be bigger than zero");

    const ufbgc_test_frame * tframe = test_list;

    #ifdef UFBGC_PRINT_SUMMARY
        typedef struct{
            const ufbgc_test_frame * tframe;
            ufbgc_return_t test_result;
            double execution_time;
        }test_summary;
        test_summary * summary_list = (test_summary*) malloc(sizeof(test_summary) * list_len);
    #endif
    
    for(size_t iter = 0; iter < list_len && tframe != NULL && tframe->test_f != NULL; ++iter){

        #ifdef UFBGC_PRINT_SUMMARY
            summary_list[iter].tframe = tframe;
            summary_list[iter].test_result = UFBGC_FAIL;
        #endif        
        

        current_test_frame.frame = tframe;

        struct tm current_time;
        ufbgc_get_current_time(&current_time);

        current_test_frame.output_file = stdout;

        if(tframe->output_file != NULL){
            current_test_frame.output_file = fopen(tframe->output_file,"a+");
            __ufbgc_internal_assert_(current_test_frame.output_file != NULL,"Can't open file:%s",tframe->output_file);
        }
        

        ufbgc_print_cyan(current_test_frame.output_file,"Starting test : '%s' @ %s",tframe->name ? tframe->name : "NULL" ,asctime(&current_time));


        if(tframe->option == PASS_TEST){
            ufbgc_print_yellow(current_test_frame.output_file,"'%s'\t\t\t%-10s\n",tframe->name,"[PASS]");
        }
        else if(tframe->test_f != NULL){
            if(tframe->parameters != NULL) current_test_frame.frame_iterateable = true;

            do{
                if(current_test_frame.frame_iterateable){
                    ufbgc_print_blue(current_test_frame.output_file,"Iteration : %lu\n",current_test_frame.frame_iterator);
                }
                
                void * user_arg = NULL;

                if(tframe->setup_f != NULL){
                    tframe->setup_f(tframe->parameters, &user_arg);
                }

                current_test_frame.test_start = clock();
                ufbgc_return_t test_result = tframe->test_f(tframe->parameters,user_arg);
                double execution_time = ufbgc_get_execution_time_ms(current_test_frame.test_start);
                if(test_result == UFBGC_OK){

                    ufbgc_print_green(current_test_frame.output_file,"'%s'\t\t\t%-10s %gms\n",tframe->name,"[OK]",execution_time);
                }
                else{
                    ufbgc_print_red(current_test_frame.output_file,"'%s'\t\t\t%-10s\n",tframe->name,"[FAILED]");
                }

                #ifdef UFBGC_PRINT_SUMMARY
                    summary_list[iter].test_result = test_result;
                    summary_list[iter].execution_time = execution_time;
                #endif
                
                if(tframe->teardown_f != NULL){
                    tframe->teardown_f(tframe->parameters, user_arg);   
                }

                current_test_frame.frame_iterator++;
            }while(current_test_frame.frame_iterateable && current_test_frame.frame_iterator < tframe->parameters->no_iteration );
        }

        ufbgc_print_magenta(current_test_frame.output_file,"------------------------------------------------------------\n");
        tframe++;
        current_test_frame.frame = NULL;
        current_test_frame.frame_iterator = 0;
        current_test_frame.frame_iterateable = false;

        if(current_test_frame.output_file != stdout){
            fclose(current_test_frame.output_file);
        }
    }


    ufbgc_print_magenta(stdout,"ufbgc - tests completed\n\n");

    #ifdef UFBGC_PRINT_SUMMARY
    ufbgc_print_magenta(stdout,"ufbgc - tests summary:\n");
    for(size_t i = 0; i<list_len; ++i){
        const ufbgc_test_frame * tframe = summary_list[i].tframe;
        __ufbgc_internal_assert_(tframe != NULL,"Can't put summary! Test frame pointer is NULL");
        size_t test_name_len = strlen(tframe->name);
        size_t right_row = 50;
        
        if(tframe->option == PASS_TEST){
            ufbgc_print_yellow(stdout,"'%s'%*s\n",tframe->name,right_row-test_name_len,"[PASS]");
            continue;
        }
        if(summary_list[i].test_result == UFBGC_OK){
            ufbgc_print_green(stdout,"'%s'%*s (%g ms)\n",tframe->name,right_row-test_name_len,"[OK]",summary_list[i].execution_time);
        }
        else{
            ufbgc_print_red(stdout,"'%s'%*s\n",tframe->name,right_row-test_name_len,"[FAILED]");
        }
    }
    free(summary_list);
    #endif


    return UFBGC_OK;
}

ufbgc_log_verbosity_t ufbgc_get_current_test_verbosity(){
    if(current_test_frame.frame != NULL){
        return current_test_frame.frame->log_level;
    }
    return UFBGC_LOG_ERROR;
}

FILE * ufbgc_get_current_test_file(){
    if(current_test_frame.frame != NULL && current_test_frame.frame->output_file != NULL){
        return current_test_frame.output_file;
    }
    return stdout;
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


int ufbgc_randint(int min, int max){
	srand((int) clock() );
    return rand()%(max-min + 1) + min;
}