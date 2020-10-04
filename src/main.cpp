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
#include "gera.pre.h"
#include "gera.obj.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

using std::fstream;

using std::string;

using std::vector;

const int       NUM_ARGS = 3;       //Número exato de argumentos via terminal
const string    FLAG_PRE = "-p";    //flag para gerar arquivo .pre
const string    FLAG_OBJ = "-o";    //flag para gerar arquivo .obj

int main(int argc, char** argv) {

    if(argc != NUM_ARGS) {
        cerr << "Número de argumentos incorreto!" << endl
        << "Modo de uso: ./montador -p/-o arquivo.asm " << endl;
        return -1;
    }

    string flag = string(argv[1]);

    bool preProc = FLAG_PRE.compare(flag) == 0;
    bool montar = FLAG_OBJ.compare(flag) == 0; 

    if( !preProc && !montar ) {
        cerr << "Uso de argumento incorreto!" << endl
        << "Modo de uso: ./montador -p arquivo.asm" << endl
        << "ou: ./montador -o arquivo.pre" << endl;

        cout << flag << endl;
        return -2;
    }

    string nomeArquivo = string(argv[2]);

    if(preProc && !acabaCom(nomeArquivo, ".asm")) {
        cerr << "Arquivo de entrada para pré processamento deve ter extensão .asm" << endl;
        return -3;
    } else if(montar && !acabaCom(nomeArquivo, ".pre") ){
        cerr << "Arquivo de entrada para montar deve ter extensão .pre" << endl;
        return -3;
    }

    fstream arquivoEntrada;
    arquivoEntrada.open(nomeArquivo, fstream::in | fstream::out);

    if(!arquivoEntrada.is_open() || arquivoEntrada.bad()) {
        cerr << "Erro ao abrir arquivo!" << endl;
        return -3;
    }
    
    cout << "Arquivo aberto com sucesso! Iniciando execução..." << endl;
    
    if(preProc) {
        mainPre(nomeArquivo, arquivoEntrada);
    } else if(montar) {
        mainObj(nomeArquivo, arquivoEntrada);
    }
    
    arquivoEntrada.close();

    cout << "Encerrando programa..." << endl;
    return 0;
}