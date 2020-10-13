#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

#include "instrucao.h"
#include "diretiva.h"

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
 * @param instrucoes mapa de instruções
 * @param diretivas mapa de diretivas
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int primeiraPassagem(std::fstream& arquivoEntrada, std::unordered_map<std::string, Instrucao*>& instrucoes, std::unordered_map<std::string, Diretiva*>& diretivas);

/**
 * Função principal para geração do arquivo .pre
 * @param arquivoEntrada referência ao arquivo de entrada
 * @param arquivoSaida referência ao arquivo de saída .obj
 * @param instrucoes mapa de instruções
 * @param diretivas mapa de diretivas
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int segundaPassagem(std::fstream& arquivoEntrada, std::fstream& arquivoSaida, std::unordered_map<std::string, Instrucao*>& instrucoes, std::unordered_map<std::string, Diretiva*>& diretivas);