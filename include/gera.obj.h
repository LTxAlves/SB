#pragma once

#include <string>
#include <fstream>

/**
 * Função principal para geração do arquivo .pre
 * @param nomeArquivoEntrada nome do arquivo de entrada .pre
 * @param arquivoEntrada referência ao arquivo de entrada
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int mainObj(std::string nomeArquivoEntrada, std::fstream& arquivoEntrada);

/**
 * Função principal para geração do arquivo .pre
 * @param arquivoEntrada referência ao arquivo de entrada
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int primeiraPassagem(std::fstream& arquivoEntrada);

/**
 * Função principal para geração do arquivo .pre
 * @param arquivoEntrada referência ao arquivo de entrada
 * @param arquivoSaida referência ao arquivo de saída .obj
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int segundaPassagem(std::fstream& arquivoEntrada, std::fstream& arquivoSaida);