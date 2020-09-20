# Trabalho 1 - Software Básico

## Índice
- [Trabalho 1 - Software Básico](#trabalho-1---software-básico)
  - [Índice](#índice)
  - [Autor](#autor)
  - [Começando](#começando)
    - [Pré-requisitos](#pré-requisitos)
    - [Instalando](#instalando)
    - [Instalando pré-requisitos em sistema operacional Linux](#instalando-pré-requisitos-em-sistema-operacional-linux)
    - [Instalando pré-requisitos em sistema operacional Windows ou macOS](#instalando-pré-requisitos-em-sistema-operacional-windows-ou-macos)
  - [Executando o programa](#executando-o-programa)
    - [Limitações](#limitações)
  - [Feito com](#feito-com)
  - [Reconhecimentos](#reconhecimentos)

## Autor

* **Leonardo Alves** - *16/0012007* - [LTxAlves](https://github.com/LTxAlves "GitHub de Leonardo")

## Começando

Estas instruções ensinarão como utilizar o código para desenvolvimento e testes em sua máquina.

### Pré-requisitos

Para utilização do projeto, você vai precisar dos seguintes:

* c++ (versão 11 ou superior)
* g++
* make

### Instalando

### Instalando pré-requisitos em sistema operacional Linux

* Consulte sua distribuição (Ubuntu, Mint, Manjaro etc.)! Distribuições debian e arch, por exemplo, possuem modos de instalação/atualização diferentes!
  * Google é seu amigo!

### Instalando pré-requisitos em sistema operacional Windows ou macOS
* Programa ainda não testado em/produzido para Windows ou macOS

## Executando o programa

1. Faça o download do código fonte
2. Coloque o(s) arquivo(s) de entrada (extensão .asm) no mesmo diretório do arquivo `Makefile` baixado (opcional)
3. Via terminal/linha de comando:
   1. Navegue até o a pasta com o arquivo `Makefile`
   2. Digite o comando `make`
   3. Aguarde a compilação do código
   4. Utilize o executável `./montador [-p -o] arquivo_de_entrada.asm` mudando o nome do arquivo de entrada para o nome utilizado por você (podendo utilizar caminho relativo a ele, isto é, `../arquivo.asm` levaria ao diretório superior, ou o caminho absoluto, como `/home/username/Documents/arquivo.asm`)
      1. O uso de um, e somente um, dos argumentos -p ou -o é obrigatório.
      2. Ao usar o argumento -p, o programa irá pré-processar o arquivo de entrada e gerar o arquivo de extensão `.pre` com o código realmente executado pelo programa de entrada
      3. Ao usar o argumento -o, o programa irá pré-processar o arquivo de entrada e gerar o aqruivo de extensão `.obj` com o código decimal das instruções executadas pelo programa de entrada

### Limitações

1. Ainda não definidas

## Feito com

* [C++](https://www.cplusplus.com/)
* [make](https://www.gnu.org/software/make/manual/make.html)

## Reconhecimentos

* *Billie Thompson's README template* - disponível [aqui](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md)