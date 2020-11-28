/**
 * 
 * Programa para montagem de arquivos em pseudo assembly
 * para a disciplina CIC0104 - Software Básico
 * 
 * @professor: Bruno Luiggi Macchiavello Espinoza
 * 
 * Trabalho 1
 * 
 * @author: Leonardo Teixeira Alves
 * @matricula: 160012007
 * @version: 1.0.0
 * 
 * */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

#include "funcoes.h"
#include "instrucao.h"
#include "diretiva.h"
#include "converte_nasm.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

using std::fstream;

using std::string;

using std::vector;

const int       NUM_ARGS = 2;       //Número exato de argumentos via terminal

int main(int argc, char** argv) {

    if(argc != NUM_ARGS) {
        cerr << "Número de argumentos incorreto!" << endl
        << "Modo de uso: ./montador arquivo.asm " << endl;
        return -1;
    }

    string nomeArquivo = string(argv[1]);

    fstream arquivoEntrada;
    arquivoEntrada.open(nomeArquivo, fstream::in | fstream::out);

    if(!arquivoEntrada.is_open() || arquivoEntrada.bad()) {
        cerr << "Erro ao abrir arquivo!" << endl;
        return -2;
    }
    cout << "Arquivo aberto com sucesso! Iniciando execução..." << endl;

    if(mainConverte(nomeArquivo, arquivoEntrada) != 0)
        return -3;
    
    arquivoEntrada.close();

    cout << "Encerrando programa..." << endl;
    return 0;
}