#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include "date.h"

int main(int argc, char *argv[]) {
    CLIENT *cl;
    char *server;
    long *lresult;
    char **sresult;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <hostname>\n", argv[0]);
        exit(1);
    }
    server = argv[1];

    /* Cria a associação com o servidor remoto usando UDP: */
    cl = clnt_create(server, DATE_PROG, DATE_VERS, "udp");
    if (cl == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    /* Chamada 1: bin_date */
    lresult = bin_date_1(NULL, cl);
    if (lresult == NULL) {
        clnt_perror(cl, "Falha na chamada bin_date");
        exit(1);
    }
    printf("Data no servidor %s (binário) = %ld\n", server, *lresult);

    /* Chamada 2: str_date */
    sresult = str_date_1(lresult, cl);
    if (sresult == NULL) {
        clnt_perror(cl, "Falha na chamada str_date");
        exit(1);
    }
    printf("Data no servidor %s (string) = %s", server, *sresult);

    clnt_destroy(cl);
    return 0;
}