# Trabalho 1 - Software Básico

## Índice
- [Trabalho 1 - Software Básico](#trabalho-1---software-básico)
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

* Entre as opções de projeto disponíveis (detecção de erros ou tratamento de macros), foi optado realizar o tratamento de macros.
* Não há limitação na quantidade de macros ou de argumentos para cada macro do arquivo de entrada.
* Ao criar rótulos dentro de uma macro, estes serão substituídos a cada chamda da macro
  * A substituição ocorre da seguinte forma: Supondo um rótulo `rot:` dentro da macro `umamacro: macro &arg`, o pré-processamento irá gerar o rótulo `ROT_UMAMACRO#` onde `#` é o número correspondente ao contador de chamadas de macro, permitindo que uma mesma macro seja chamada diversas vezes

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
  * [Estes links](#feito-com) podem ser úteis.

### Instalando pré-requisitos em sistema operacional Windows ou macOS

* Programa ainda não testado em/produzido para Windows ou macOS

## Executando o programa

1. Faça o download do código fonte
2. Coloque o(s) arquivo(s) de entrada (extensão .asm) no mesmo diretório do arquivo `Makefile` baixado (opcional)
3. Via terminal/linha de comando:
   1. Navegue até o a pasta com o arquivo `Makefile`
   2. Digite o comando `make release`
   3. Aguarde a compilação do código
   4. Utilize o executável `./montador -p arquivo_de_entrada.asm` mudando o nome do arquivo de entrada para o nome utilizado por você (podendo utilizar caminho relativo a ele, isto é, `../arquivo.asm` levaria ao diretório superior, ou o caminho absoluto, como `/home/username/Documents/arquivo.asm`)
       - Ao finalizar o pré-processamento, será gerado um arquivo com nome igual no mesmo diretório do arquivo de entrada, desta vez com extensão `.pre`
   5. Para realizar a montagem, utilize o comando `./montador -o arquivo_de_entrada.pre`, sendo `arquivo_de_entrada.pre` o arquivo pré-processado gerado pelo passo 4
       - Ao finalizar a montagem, será gerado um arquivo de mesmo nome e no mesmo diretório que o arquivo pré-processado, desta vez com extensão `.obj`

### Limitações

1. Como foi escolhido o tratamento de macros, o programa não foi devidamente preparado para tratar erros léxicos ou de sintaxe/semântica
   - Alguns erros (relativos a diretivas MACRO, EQU e IF) são detectados durante o pré-processamento, impedindo a continuação do programa
   - Alguns erros são identificados durante a montagem, porém o programa para a execução caso encontre algum erro, mostrando apenas o primeiro erro encontrado.
2. Instruções e diretivas com recebimento de argumentos no arquivo em pseudo assembly de entrada, conforme definido nas instruções para o trabalho, devem vir devidamente separadas (isto é, ao menos um espaço ou tabulação entre elas e sem espaços em branco antes da vírgula)
   - Exemplo 1: `COPY L1,L2` ou `COPY L1 ,L2` estão incorretos, enquanto `COPY L1, L2` ou `COPY L1,`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`L2` estão corretos.
   - Exemplo 2: `LABEL: MACRO &A,&B` ou `LABEL: MACRO &A ,&B` estão incorretos, enquanto `LABEL: MACRO &A, &B` ou `LABEL: MACRO &A,`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`&B` estão corretos.
     - O mesmo vale para a chamada da macro: `LABEL A, B` é aceito, mas `LABEL A,B` não
3. Conforme na definição do trabalho, o programa não diferencia maiúsculas de minúsculas, isto é, `Label:`, `LABEL:`, `label:`, `LaBeL:` ou variações são considerados rótulos iguais. O mesmo vale para instruções e diretivas.
4. Alguma lmitações se devem às especificações do trabalho ou à escolha devido à falta de especificações a respeito das questões:
    - Diretivas do tipo IF e a linha seguinte as diretivas IF não podem possuir rótulos, de forma a não haver referências à linhas possivelmente deletadas durante pré-processamento
    - A quebra de linha (LF, CR ou CRLF) é considerada como o separador de instruções. Sendo assim, cada instrução/diretiva deve vir em uma linha sem outras instruções/diretivas e com todos os seus operandos
    - Macros não podem conter chamdas a outras macros dentro do seu corpo
    - Macros não podem conter definições de outras macros dentro do seu corpo

## Feito com

* [C++](https://www.cplusplus.com/)
* [gcc](https://gcc.gnu.org/)
* [make](https://www.gnu.org/software/make/manual/make.html)

## Reconhecimentos

* *Billie Thompson's README template* - disponível [aqui](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md)
