#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

/**
 * Função principal para geração do arquivo .pre
 * @param nomeArquivoEntrada nome do arquivo de entrada .asm
 * @param arquivoEntrada referência ao arquivo de entrada
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int mainPre(std::string nomeArquivoEntrada, std::fstream& arquivoEntrada);

/**
 * Cria map das diretivas EQU
 * @returns -1 se erro, 0 caso contrário
 * */
int mapeiaEqus(std::fstream& arquivoEntrada);

/**
 * Geração do arquivo .pre em si
 * @param arquivoEntrada arquivo em pseudo assembly de entrada
 * @param arquivoSaida arquivo de saída
 * @returns -1 se erro, 0 caso contrário
 * */
int geraPre(std::fstream& arquivoEntrada, std::fstream& arquivoSaida);

/**
 * Cria a tabela de definição de macro
 * @param arquivoEntrada arquivo em pseudo assembly de entrada
 * @param macroCorpo corpo da macro
 * @returns -1 se erro, 0 caso contrário
 * */
int mapeiaMacro(std::fstream& arquivoEntrada, std::vector<std::string>& macroCorpo);