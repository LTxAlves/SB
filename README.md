# Trabalho 2 - Software Básico

## Índice
- [Trabalho 2 - Software Básico](#trabalho-2---software-básico)
  - [Índice](#índice)
  - [Autor](#autor)
  - [Antes de iniciar](#antes-de-iniciar)
  - [Escolha de projeto](#escolha-de-projeto)
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

## Antes de iniciar

Recomenda-se o uso de um visualizador de markdown para a leitura deste documento. Exemplo: [Markdown Live Preview](https://markdownlivepreview.com/)

## Escolha de projeto

* Foi extendido a 10 caracteres o máximo de caracteres de entrada da função INPUT para comportar números maiores

## Começando

Estas instruções ensinarão como utilizar o código para desenvolvimento e testes em sua máquina.

### Pré-requisitos

Para utilização do projeto, você vai precisar dos seguintes:

* c++ (versão 11 ou superior)
* g++
* make
* nasm
* ld

### Instalando

### Instalando pré-requisitos em sistema operacional Linux

* Consulte sua distribuição (Ubuntu, Mint, Manjaro etc.)! Distribuições debian e arch, por exemplo, possuem modos de instalação/atualização diferentes!
  * Google é seu amigo!
  * [Estes links](#feito-com) podem ser úteis.

### Instalando pré-requisitos em sistema operacional Windows ou macOS

* Programa ainda não testado em/produzido para Windows ou macOS
  * A tradução foi feita de maneira em que interrupções em IA-32 são voltadas a sistemas operacionais Linux, sem garantia de funcionamento correto em Windows ou MacOS

## Executando o programa

1. Faça o download do código fonte
2. Coloque o(s) arquivo(s) de entrada (extensão .asm) no mesmo diretório do arquivo `Makefile` baixado (opcional)
3. Via terminal/linha de comando:
   1. Navegue até o a pasta com o arquivo `Makefile`
   2. Digite o comando `make release`
   3. Aguarde a compilação do código
   4. Utilize o executável `./tradutor arquivo_de_entrada.asm` mudando o nome do arquivo de entrada para o nome utilizado por você (podendo utilizar caminho relativo a ele, isto é, `../arquivo.asm` levaria ao diretório superior, ou o caminho absoluto, como `/home/username/Documents/arquivo.asm`)
       - Ao finalizar, será gerado um arquivo em Assembly IA-32 com o mesmo nome do arquivo de entrada e extensão `.s`
   5. Para executar o programa traduzido, navegue até seu local de criação e utilize os comandos
       - `nasm -f elf arquivo.s -o arquivo.o`
       - `ld [-m elf_i386] arquivo.o -o executavel` (a parte entre colchetes deve ser utilizada apenas se seu computador tiver arquitetura de 64 bits)

### Limitações

1. Instruções e diretivas com recebimento de argumentos no arquivo em pseudo assembly de entrada, conforme definido nas instruções para o trabalho, devem vir devidamente separadas (isto é, ao menos um espaço ou tabulação entre elas e sem espaços em branco antes da vírgula)
   - Exemplo 1: `COPY L1,L2` e `COPY L1 ,L2` estão incorretos, enquanto `COPY L1, L2` e `COPY L1,`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`L2` estão corretos.
   - Exemplo 2: `S_INPUT LABEL,100` e `S_INPUT LABEL ,100` estão incorretos, enquanto `S_INPUT LABEL, 100` e `S_INPUT LABEL,`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`100` estão corretos
2. Conforme na definição do trabalho, o programa não diferencia maiúsculas de minúsculas, isto é, `Label:`, `LABEL:`, `label:`, `LaBeL:` ou variações são considerados rótulos iguais. O mesmo vale para instruções e diretivas.
3. Alguma lmitações se devem às especificações do trabalho ou à escolha devido à falta de especificações a respeito das questões:
    - Diretivas do tipo IF e a linha seguinte as diretivas IF não podem possuir rótulos, de forma a não haver referências à linhas possivelmente deletadas durante pré-processamento
    - A quebra de linha (LF, CR ou CRLF) é considerada como o separador de instruções. Sendo assim, cada instrução/diretiva deve vir em uma linha sem outras instruções/diretivas e com todos os seus operandos
4. Conforme definição do trabalho, espera-se que não haja erros ou macros no arquivo de entrada

## Feito com

* [C++](https://www.cplusplus.com/)
* [gcc](https://gcc.gnu.org/)
* [make](https://www.gnu.org/software/make/manual/make.html)
* [nasm](https://www.nasm.us/)

## Reconhecimentos

* *Billie Thompson's README template* - disponível [aqui](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md)
