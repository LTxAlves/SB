#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "instrucao.h"
#include "diretiva.h"

/**
 * Função principal para geração do arquivo .pre
 * @param nomeArquivoEntrada nome do arquivo de entrada .asm
 * @param arquivoEntrada referência ao arquivo de entrada
 * @returns 0 se executado corretamente, outro número se houve erro
 * */
int mainConverte(std::string nomeArquivoEntrada, std::fstream& arquivoEntrada);

/**
 * Cria map das diretivas EQU
 * @returns -1 se erro, 0 caso contrário
 * */
int mapeiaEqus(std::fstream& arquivoEntrada);

/**
 * Geração do arquivo .pre em si
 * @param arquivoEntrada arquivo em pseudo assembly de entrada
 * @param arquivoSaida arquivo de saída
 * @param instrucoes mapa de instruções
 * @param diretivas mapa de diretivas
 * @returns -1 se erro, 0 caso contrário
 * */
int geraConvertido(std::fstream& arquivoEntrada, std::fstream& arquivoSaida, std::unordered_map<std::string, Instrucao*>& instrucoes, std::unordered_map<std::string, Diretiva*>& diretivas);

/**
 * Cria a tabela de definição de macro
 * @param arquivoEntrada arquivo em pseudo assembly de entrada
 * @param macroCorpo corpo da macro
 * @param macroLbls vetor de labels da macro
 * @param instrucoes mapa de instruções
 * @param diretivas mapa de diretivas
 * @returns -1 se erro, 0 caso contrário
 * */
int mapeiaMacro(std::fstream& arquivoEntrada, std::vector<std::string>& macroCorpo, std::vector<std::string>& macroLbls, std::unordered_map<std::string, Instrucao*>& instrucoes, std::unordered_map<std::string, Diretiva*>& diretivas);