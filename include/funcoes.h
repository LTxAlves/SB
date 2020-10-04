#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "instrucao.h"
#include "diretiva.h"

using std::string;

using std::vector;

using std::fstream;

using std::map;

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
bool acabaCom(string const &stringOrigem, string const &final);

/**
 * Inicializa vetor com instruções de pseudo assembly
 * @returns vetor ordenado de instruções
 * */
map<string, Instrucao*> inicializaInstrucoes();

/**
 * Inicializa vetor com diretivas de pseudo assembly
 * @returns vetor ordenado de diretivas
 * */
map<string, Diretiva*> inicializaDiretivas();

/**
 * Retorna uma linha do arquivo de entrada, parando em CR, LF ou CRLF
 * @param arquivo arquivo de entrada
 * @returns linha lida do arquivo
 * */
string getLineModificado(fstream& arquivo);

/**
 * Insere um vetor de strings no arquivo
 * @param arquivo arquivo de saída
 * @param toPut vetor de strings a escrever
 * */
void putLine(fstream& arquivo, vector<string>& toPut);

/**
 * Ignora chars passados como argumento
 * @param arquivo arquivo de entrada
 * @param char caracteres a serem ignorados
 * */
void ignoraChars(fstream& arquivo, vector<char> chars);

/**
 * Ignora espaços e tabulações
 * @param arquivo arquivo de entrada
 * */
void ignoraEspacoTab(fstream& arquivo);

/**
 * Ignora quebras de linha
 * @param arquivo arquivo de entrada
 * */
void ignoraQuebraLinha(fstream& arquivo);

/**
 * Ignora quebras de linha, espaços e tabulações
 * @param arquivo arquivo de entrada
 * */
void ignoraVazios(fstream& arquivo);

/**
 * Altera 3 últimos caracteres da extensao do arquivo para a extensao especificada
 * @param nomeArquivo nome do arqivo original com extensao
 * @param novaExtensao extensao a ser adicionada no nome
 * @returns nome do arquivo com extensão trocada
 * */
string alteraExtensaoNomeArquivo(string nomeArquivo, string novaExtensao);

/**
 * Busca string na linha atual do arquivo ignorando maiúsculas/minúsculas
 * @param arquivo arquivo de entrada onde buscar
 * @param procurado string a procurar
 * @returns posicao do primeiro caractere da string procurada, -1 se nao encontrou
 * */
bool encontrarNaLinha(fstream& arquivo, string procurado);

/**
 * Retorna a label em uma linha do arquivo (condicao de parada = ':')
 * @returns string com o nome da label em caixa alta "ERRO!" se encontrou erro
 * */
string getLabel(fstream& arquivo);

/**
 * Retorna a palavra a partir da posicao atual do arquivo (condicao de parada = ';', CR, LF, SPACE ou TAB)
 * @param arquivo arquivo de entrada
 * @returns string com o nome da label em caixa alta "ERRO!" se encontrou erro
 * */
string getWord(fstream& arquivo);

/**
 * Ignora caracteres até encontrar CR ou LF ou EOF
 * @param arquivo arquivo de entrada
 * */
void procuraFimLinha(fstream& arquivo);

/**
 * Separa a string em suas substrings já com letras maiúsculas
 * @param linha linha lida do arquivo de entrada
 * @returns vetor de substrings da linha (ignorando comentários)
 * */
vector<string> substrings(string linha);

/**
 * Retorna a string com caracteres em letras maiúsculas
 * @param str string para transformar
 * @returns string de entrada transformada em letras maiúsculas
 * */
string toupperStr(string str);

/**
 * Verifica se a string passada como argumento é uma diretiva
 * @param diretivas vetor de diretivas
 * @param str string a buscar
 * @returns booleano indicando se é uma diretiva
 * */
bool eDiretiva(string str);

/**
 * Verifica se a string passada como argumento é uma instrução
 * @param instrucoes vetor de intrucoes
 * @param str string a buscar
 * @returns booleano indicando se é uma diretiva
 * */
bool eInstrucao(string str);