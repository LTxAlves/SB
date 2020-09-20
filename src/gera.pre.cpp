#include "gera.pre.h"

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "funcoes.h"
#include "instrucao.h"
#include "diretiva.h"

using std::map;

using std::vector;

using std::cout;
using std::cerr;

using std::find;

map<string, string> equs;
map<string, string> labels;

vector<Diretiva> diretivas      = inicializaDiretivas();
vector<Instrucao> instrucoes    = inicializaInstrucoes();

int mainPre(string nomeArquivoEntrada, fstream& arquivoEntrada) {

    string nomeArquivoSaida = alteraExtensaoNomeArquivo(nomeArquivoEntrada, "pre");

    fstream arquivoSaida;
    arquivoSaida.open(nomeArquivoSaida, fstream::in | fstream::out | fstream::trunc);

    if(!arquivoSaida.is_open() || arquivoSaida.bad())  {
        cerr << "Erro ao criar arquivo " << nomeArquivoSaida << endl;
        return -1;
    }

    cout << "Gerando arquivo " << nomeArquivoSaida << "..." << endl;

    mapeiaEqus(arquivoEntrada);

    for(map<string, string>::iterator it = equs.begin(); it != equs.end(); it++) {
        cout << it->first << " = " << it->second << endl;
    }

    arquivoSaida.close();

    return 0;
}

int mapeiaEqus(fstream& arquivoEntrada) {

    string linhaEntrada = getLineModificado(arquivoEntrada);
    vector<string> entradaSubstrings = substrings(linhaEntrada);
    vector<string>::iterator it;

    for(it = entradaSubstrings.begin(); it != entradaSubstrings.end(); it++) {
        string str = *it;
        if(str.find(':')){
            labels[str.erase(str.find(':'))] = "lixo";
            cout << str << endl;
        }
    }
    

    return 0;
}
