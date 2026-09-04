# 01-c-sun-remote-procedure-call

# SUN RPC (Remote Procedure Call)

## Passo 0:  Preparar o Ambiente no Codespaces

O Codespaces roda em Ubuntu. 

Instalar a biblioteca de transporte independente libtirpc e o serviço rpcbind (o portmapper moderno).

1. Abra o terminal do Codespaces e instale as dependências:

```bashs
udo apt-get update
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

