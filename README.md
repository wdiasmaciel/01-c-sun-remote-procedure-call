# 01-c-sun-remote-procedure-call

# SUN RPC (Remote Procedure Call)

## Passo 0:  Preparar o Ambiente no Codespaces

O Codespaces roda em Ubuntu. 

Instalar a biblioteca de transporte independente libtirpc e o serviço rpcbind (o portmapper moderno).

1. Abra o terminal do Codespaces e instale as dependências:

```bashs
sudo apt-get update
```

```
sudo apt-get install -y rpcgen libtirpc-dev rpcbind
```

2. Inicie o serviço de mapeamento de portas (essencial para o cliente achar o servidor):

```bash
sudo service rpcbind start
```

# Passo 1: O Arquivo de Especificação XDR (External Data Representation)

Crie o arquivo `date.x` exatamente como especificado:

```text

/* arquivo date.x */
program DATE_PROG {
    version DATE_VERS {
        long bin_date(void) = 1;
        string str_date(long) = 2;
    } = 1;
} = 0x31234567;
```

## Passo 2: Geração dos Stubs

No terminal, execute o comando rpcgen. No Linux, usamos a flag -M (para tornar o código seguro para threads/multithread) ou compilamos com suporte a tirpc:

```bash
rpcgen -M -N date.x
```

**OBS**: a flag -N (Newstyle) facilita a passagem de argumentos, mas se quiser seguir o padrão clássico exato, use apenas rpcgen date.x.

## Passo 3: Implementação do Servidor (dateproc.c)

Crie o arquivo dateproc.c. 

Os procedimentos remotos precisam retornar ponteiros para variáveis estáticas, pois variáveis locais da pilha seriam destruídas ao sair da função.

```c
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
```

## Passo 4: Implementação do Cliente (client.c)

Crie o arquivo client.c. 

Ajuste os includes para o padrão Linux e adicie checagem de erros.

```c
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
```

## Passo 5: Compilação

Como estamos usando a biblioteca externa libtirpc, precisamos passar caminhos de include (-I) e a flag de linkagem (-ltirpc) ao compilador GCC.

1. **Compilar o Servidor**:

```bash
gcc -I/usr/include/tirpc -o server date_svc.c dateproc.c -ltirpc
```

2. **Compilar o Cliente**:

```bash
gcc -I/usr/include/tirpc -o client client.c date_clnt.c -ltirpc
```

## Passo 6: Execução

1. **Executar o Servidor** (em background):

```bash
sudo ./server &
```

2. **Executar o Cliente** (apontando para o **localhost** já que estamos na mesma máquina do Codespaces):

```bash
./client localhost
```

3. Resultado esperado no terminal:

```text
Data no servidor localhost (binário) = 1784841920
Data no servidor localhost (string) = Sat Aug 22 23:38:00 2026
```