# Sistemas Operacionais

## Trabalho Prático 3

O terceiro trabalho prático da disciplina de Sistemas Operacionais consiste na implementação de um simulador de um sistema de arquivos simples baseado em tabela de alocação de 16 bits (FAT) e um shell usado para realizar operações sobre este sistema de arquivos. O sistema de arquivos virtual deverá ser armazenado em uma partição virtual e suas estruturas de dados mantidas em um único arquivo nomeado `fat.part`.

### Informações sobre o valor das entradas na FAT de 16 bits:

``` text
0x0000              -> cluster livre
0x0001 - 0xfffc     -> arquivo (ponteiro p/ proximo cluster)
0xfffd              -> boot block
0xfffe              -> FAT
0xffff              -> fim do arquivo
```

### Informações sobre a estrutura das entradas de diretório:

``` text
18 bytes            -> nome do arquivo
1 byte              -> atributo do arquivo
7 bytes             -> reservado
2 bytes             -> numero do primeiro cluster ocupado
4 bytes             -> tamanho do arquivo
Byte de atributo do arquivo - valor: 0 - arquivo, 1 - diretório
```

## Instalação e uso

``` bash
make
make debug # binario com debug
make clean # remover binario
```

### Uso: Detalhes do shell

``` console
init                              # inicializar o sistema de arquivos
load                              # carregar o sistema de arquivos
ls [/caminho/diretorio]           # listar diretório
mkdir [/caminho/diretorio]        # criar diretório
create [/caminho/arquivo]         # criar arquivo
unlink [/caminho/arquivo]         # excluir arquivo ou diretório
write "string"[/caminho/arquivo]  # escrever dados em um arquivo
append "string"[/caminho/arquivo] # anexar dados em um arquivo
read [/caminho/arquivo]           # ler o conteúdo de um arquivo
```

## Acknowledgments

*   [GenericMakefile](https://github.com/mbcrawfo/GenericMakefile) - versionamento e debug definidos em tempo de compilação.
