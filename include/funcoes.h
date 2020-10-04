#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "instrucao.h"
#include "diretiva.h"

const char SPACE = ' ';
const char TAB = '\t';
const char LF = '\n';
const char CR = '\r';

/**
 * Checa se uma string acaba com uma substring
 * @param stringOrigem string onde buscar
 * @param final substring a ser buscada no final
 * @returns true se stringOrigem acaba com final, falso caso contrário
 * */
bool acabaCom(std::string const &stringOrigem, std::string const &final);

/**
 * Inicializa vetor com instruções de pseudo assembly
 * @returns vetor ordenado de instruções
 * */
std::map<std::string, Instrucao*> inicializaInstrucoes();

/**
 * Inicializa vetor com diretivas de pseudo assembly
 * @returns vetor ordenado de diretivas
 * */
std::map<std::string, Diretiva*> inicializaDiretivas();

/**
 * Retorna uma linha do arquivo de entrada, parando em CR, LF ou CRLF
 * @param arquivo arquivo de entrada
 * @returns linha lida do arquivo
 * */
std::string getLineModificado(std::fstream& arquivo);

/**
 * Insere um vetor de strings no arquivo
 * @param arquivo arquivo de saída
 * @param toPut vetor de strings a escrever
 * */
void putLine(std::fstream& arquivo, std::vector<std::string>& toPut);

/**
 * Altera 3 últimos caracteres da extensao do arquivo para a extensao especificada
 * @param nomeArquivo nome do arqivo original com extensao
 * @param novaExtensao extensao a ser adicionada no nome
 * @returns nome do arquivo com extensão trocada
 * */
std::string alteraExtensaoNomeArquivo(std::string nomeArquivo, std::string novaExtensao);

/**
 * Separa a string em suas substrings já com letras maiúsculas
 * @param linha linha lida do arquivo de entrada
 * @returns vetor de substrings da linha (ignorando comentários)
 * */
std::vector<std::string> substrings(std::string linha);

/**
 * Retorna a string com caracteres em letras maiúsculas
 * @param str string para transformar
 * @returns string de entrada transformada em letras maiúsculas
 * */
std::string toupperStr(std::string str);

/**
 * Verifica se a string passada como argumento é uma diretiva
 * @param diretivas vetor de diretivas
 * @param str string a buscar
 * @returns booleano indicando se é uma diretiva
 * */
bool eDiretiva(std::string str);

/**
 * Verifica se a string passada como argumento é uma instrução
 * @param instrucoes vetor de intrucoes
 * @param str string a buscar
 * @returns booleano indicando se é uma diretiva
 * */
bool eInstrucao(std::string str);