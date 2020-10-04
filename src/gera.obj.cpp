#include "gera.obj.h"

#include <iostream>
#include <map>
#include <vector>

#include "funcoes.h"

using std::string;
using std::stoi;

using std::fstream;

using std::cout;
using std::cerr;
using std::endl;

using std::map;

using std::vector;

map<string, int> tabelaLabels;
map<string, Diretiva*> dirs     = inicializaDiretivas();
map<string, Instrucao*> insts   = inicializaInstrucoes();

int mainObj(string nomeArquivoEntrada, fstream& arquivoEntrada) {

    string nomeArquivoSaida = alteraExtensaoNomeArquivo(nomeArquivoEntrada, "obj");

    fstream arquivoSaida;
    arquivoSaida.open(nomeArquivoSaida, fstream::in | fstream::out | fstream::trunc);

    if(!arquivoSaida.is_open() || arquivoSaida.bad())  {
        cerr << "Erro ao criar arquivo " << nomeArquivoSaida << endl;
        return -1;
    }

    cout << "Gerando arquivo " << nomeArquivoSaida << endl;

    cout << "Primeira passagem..." << endl;

    if(primeiraPassagem(arquivoEntrada) != 0) {
        cout << "Erro ao executar primeira passagem!" << endl;
        return -1;
    }

    cout << "Primeira passagem finalizada!" << endl;

    #if DEBUG
    cout << "Labels encontradas:" << endl;
    for(auto it = tabelaLabels.begin(); it != tabelaLabels.end(); it++) {
        cout << it->first << ": " << it->second << endl;
    }
    #endif

    arquivoEntrada.close();
    arquivoEntrada.open(nomeArquivoEntrada, fstream::in | fstream::out);
    
    cout << "Segunda passagem..." << endl;
    
    if(segundaPassagem(arquivoEntrada, arquivoSaida) != 0) {
        cout << "Erro ao executar segunda passagem!" << endl;
        return -1;
    }

    cout << "Segunda passagem finalizada!" << endl;

    arquivoSaida.close();

    return 0;
}

int primeiraPassagem(fstream& arquivoEntrada) {

    string linhaEntrada;
    vector<string> entradaSubstrings;
    string str;
    int PC = 0;

    do {
        linhaEntrada = getLineModificado(arquivoEntrada);
        entradaSubstrings = substrings(linhaEntrada);
        int tam = entradaSubstrings.size();
        
        if(tam > 0) {
            str = entradaSubstrings[0];

            if(str.back() == ':') {
                str.pop_back();

                tabelaLabels[str] = PC;
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            
            if(tam > 0) {
                str = entradaSubstrings[0];
                if (eInstrucao(str)) {
                    PC += insts[str]->getTam();
                } else if(eDiretiva(str)) {
                    PC += dirs[str]->getTam();
                }
            }
        }

    } while (!(arquivoEntrada.eof() || arquivoEntrada.bad()));

    return 0;
}

int segundaPassagem(fstream& arquivoEntrada, fstream& arquivoSaida) {

    string linhaEntrada;
    vector<string> entradaSubstrings;
    vector<int> toWrite;
    string str;


    do {
        linhaEntrada = getLineModificado(arquivoEntrada);
        entradaSubstrings = substrings(linhaEntrada);
        int tam = entradaSubstrings.size();
        
        if(tam > 0) {
            str = entradaSubstrings[0];

            if(str.back() == ':') {
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            
            if(tam > 0) {
                str = entradaSubstrings[0];

                if (eInstrucao(str)) {
                    int tamInst = insts[str]->getTam();
                    toWrite.push_back(insts[str]->getCod());
                    for(int i = 1; i < tamInst; i++) {

                        string operando = entradaSubstrings[i];

                        if(operando.back() == ',') 
                            operando.pop_back();

                        if(tabelaLabels.find(operando) == tabelaLabels.end()) {
                            cout << "Erro! Operando " << operando << " não encontrado!" << endl;
                            return -1;
                        }

                        toWrite.push_back(tabelaLabels[operando]);
                    }
                } else if(eDiretiva(str)) {
                    if(str.compare("SPACE") == 0) {
                        toWrite.push_back(0);
                    } else if(str.compare("CONST") == 0) {
                        toWrite.push_back(stoi(entradaSubstrings[1]));
                    }
                }
            }
        }

    } while (!(arquivoEntrada.eof() || arquivoEntrada.bad()));

    putVectorInt(arquivoSaida, toWrite);

    return 0;
}
