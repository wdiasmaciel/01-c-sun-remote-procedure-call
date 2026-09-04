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