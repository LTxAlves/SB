#pragma once

#include <iostream>
#include <string>
#include <fstream>

using std::cerr;
using std::cout;
using std::endl;

using std::string;

using std::fstream;

/**
 * Função principal para geração do arquivo .pre
 * @param nomeArquivoEntrada nome do arquivo de entrada .asm
 * @param arquivoEntrada referência ao arquivo de entrada
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int mainPre(string nomeArquivoEntrada, fstream& arquivoEntrada);

/**
 * Cria map das diretivas EQU
 * @returns -1 se erro, 0 caso contrário
 * */
int mapeiaEqus(fstream& arquivoEntrada);