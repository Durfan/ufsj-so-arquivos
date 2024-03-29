# Sistemas Operacionais <img width="100" height="100" src="https://github.com/Durfan/ufsj-so-arquivos/blob/master/docs/logo_ufsj.png" align="right">

## Trabalho Prático 3

O terceiro trabalho prático da disciplina de Sistemas Operacionais consiste na implementação de um simulador de um sistema de arquivos simples baseado em tabela de alocação de 16 bits (FAT) e um shell usado para realizar operações sobre este sistema de arquivos. O sistema de arquivos virtual deverá ser armazenado em uma partição virtual e suas estruturas de dados mantidas em um único arquivo nomeado `fat.part`.

![Captura](https://github.com/Durfan/ufsj-so-arquivos/blob/master/docs/fat.part.PPM.png)

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

``` text
      inicializar o drive:   init
         carregar o drive:   load
         listar diretorio:     ls [/caminho/diretorio]
          criar diretorio:  mkdir [/caminho/diretorio]
            criar arquivo: create [/caminho/arquivo]
excluir arquivo/diretorio: unlink [/caminho/arquivo]
      escrever no arquivo:  write "string" [/caminho/arquivo]
        anexar em arquivo: append "string" [/caminho/arquivo]
           ler um arquivo:   read [/caminho/arquivo]
            exibe a ajuda:   help
                     sair:   exit
```

## Acknowledgments

* [djb2](http://www.cse.yorku.ca/~oz/hash.html) - _the magic of number 33  
(why it works better than many other constants, prime or not) has never been adequately explained._
* [GenericMakefile](https://github.com/mbcrawfo/GenericMakefile) - versionamento e debug definidos em tempo de compilação.
