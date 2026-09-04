#include <rpc/rpc.h>
#include <time.h>
#include "date.h"

/* Implementação do bin_date: */
long * bin_date_1_svc(void *argp, struct svc_req *rqstp) {
    static long result;
    result = (long)time(NULL);
    return &result;
}

/* Implementação do str_date: */
char ** str_date_1_svc(long *argp, struct svc_req *rqstp) {
    static char *result;
    time_t time_val = (time_t)*argp;
    
    // ctime retorna uma string terminada em \n:
    result = ctime(&time_val); 
    return &result;
}