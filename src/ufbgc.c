#include "ufbgc.h"

typedef struct{
    const ufbgc_test_frame * frame;
    size_t frame_iterator;
    bool frame_iterateable;
}internal_ufbgc_test_frame;

static internal_ufbgc_test_frame current_test_frame = {
    .frame = NULL,
    .frame_iterator = 0,
    .frame_iterateable = false,
};

ufbgc_return_t ufbgc_start_test(const ufbgc_test_frame * const test_list){

    ufbgc_print_magenta("ufbgc - starting tests\n\n");

    __ufbgc_internal_assert_(test_list != NULL,"Test frame pointer is NULL");

    const ufbgc_test_frame * tframe = test_list;

    while(tframe != NULL && tframe->fun_ptr != NULL){
        current_test_frame.frame = tframe;

        ufbgc_print_cyan("Starting test : '%s'\n",tframe->name);
        
        if(tframe->option == PASS_TEST){
            ufbgc_print_yellow("'%s'\t\t\t%-10s\n",tframe->name,"[PASS]");
        }
        else if(tframe->fun_ptr != NULL){
            if(tframe->parameters != NULL) current_test_frame.frame_iterateable = true;

            do{
                if(current_test_frame.frame_iterateable){
                    ufbgc_print_blue("Iteration : %lu\n",current_test_frame.frame_iterator);
                }

                if(tframe->fun_ptr(tframe->parameters,tframe->uarg) == UFBGC_OK){
                    ufbgc_print_green("'%s'\t\t\t%-10s\n",tframe->name,"[OK]");
                }
                else{
                    ufbgc_print_red("'%s'\t\t\t%-10s\n",tframe->name,"[FAILED]");
                }

                current_test_frame.frame_iterator++;
            }while(current_test_frame.frame_iterateable && current_test_frame.frame_iterator < tframe->parameters->no_iteration );

        }

        ufbgc_print_magenta("------------------------------------------------------------\n");
        tframe++;
        current_test_frame.frame = NULL;
        current_test_frame.frame_iterator = 0;
        current_test_frame.frame_iterateable = false;
    }


    ufbgc_print_magenta("ufbgc - tests completed\n\n");
    return UFBGC_OK;
}


void * ufbgc_get_arg(const char * key){
    if(current_test_frame.frame == NULL){
        return NULL;
    }

    //Use current tframe
    if(current_test_frame.frame->uarg != NULL){
        ufbgc_args * uarg = current_test_frame.frame->uarg;
        while(uarg != NULL && uarg->key != NULL){
            if(!strcmp(uarg->key,key)){
                return uarg->value;
            }
            uarg++;
        }
    }
    return NULL;
}

bool get_current_test_iterator(size_t * it){
    if(current_test_frame.frame == NULL || it == NULL){
        return false;
    }

    *it = current_test_frame.frame_iterator;
    return true;
}

void * ufbgc_get_parameter(const char * key){
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
