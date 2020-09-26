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

using std::to_string;

map<string, string> equs;
map<string, string> labels;
map<string, string> macros;

map<string, Diretiva> diretivas      = inicializaDiretivas();
map<string, Instrucao> instrucoes    = inicializaInstrucoes();

int mainPre(string nomeArquivoEntrada, fstream& arquivoEntrada) {

    string nomeArquivoSaida = alteraExtensaoNomeArquivo(nomeArquivoEntrada, "pre");

    fstream arquivoSaida;
    arquivoSaida.open(nomeArquivoSaida, fstream::in | fstream::out | fstream::trunc);

    if(!arquivoSaida.is_open() || arquivoSaida.bad())  {
        cerr << "Erro ao criar arquivo " << nomeArquivoSaida << endl;
        return -1;
    }

    cout << "Gerando arquivo " << nomeArquivoSaida << endl;

    if( mapeiaEqus(arquivoEntrada) != 0 ) {
        return -1;
    }

    cout << "Diretivas EQU encontradas:" << endl;

    for(auto it = equs.begin(); it != equs.end(); it++) {
        cout << it->first << " = " << it->second << endl;
    }

    geraPre(arquivoEntrada, arquivoSaida);

    arquivoSaida.close();

    return 0;
}

int mapeiaEqus(fstream& arquivoEntrada) {

    int pos = arquivoEntrada.tellg();
    string linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
    vector<string> entradaSubstrings = substrings(linhaEntrada);
    bool encontrouLabel = false;
    bool encontrouEQU = false;
    string ultimaLabel;

    while(linhaEntrada.find("SECTION") == string::npos && !arquivoEntrada.eof() && !arquivoEntrada.bad()) {
        for(string str : entradaSubstrings) {

            if(str.find(':') != string::npos) {

                if(encontrouLabel) {
                    cout << "Erro! Dois rótulos seguidos encontrados!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                if(str[0] >= '0' && str[0] <= '9') {
                    cout << "Erro! Rótulo iniciado por dígito numérico." << endl;
                    return -1;
                }

                for(char c : str) {
                    if(!(c >= 'A' && c <= 'Z') && !( c>= '0' && c <= '9') && c != '_') {
                        cout << "Erro! Rótulo com caracteres não permitidos!" << endl;
                        return -1;
                    }
                }

                if(labels.find("str") != labels.end()) {
                    cout << "Erro! Redefinição de rótulo!" << endl;
                    return -1;
                }

                encontrouLabel = true;
                ultimaLabel = str;
                labels[str.erase(str.find(':'))] = "lixo";
            } else if(str.compare("EQU") == 0) {
                if(!encontrouLabel) {
                    cout << "Erro! EQU sem rótulo!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                if(encontrouEQU) {
                    cout << "Erro! Duas diretivas EQU seguidas!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                encontrouEQU = true;
                equs[ultimaLabel] = "lixo";
            } else if(!eDiretiva(str) && !eInstrucao(str) && encontrouEQU && encontrouLabel) {
                equs[ultimaLabel] = str;
                labels[ultimaLabel] = "EQU";
                encontrouLabel = false;
                encontrouEQU = false;
            }
        }
        pos = arquivoEntrada.tellg();
        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
        entradaSubstrings = substrings(linhaEntrada);
    }

    if(arquivoEntrada.eof() || arquivoEntrada.bad()) {
        cout << "Erro! Diretiva SECTION não encontrada!" << endl;
        return -1;
    }
    
    arquivoEntrada.seekg(pos);

    return 0;
}

int geraPre(fstream& arquivoEntrada, fstream& arquivoSaida) {

    int pos = arquivoEntrada.tellg();
    string linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
    vector<string> entradaSubstrings = substrings(linhaEntrada);
    int contadorLinhas = 0;

    if(entradaSubstrings.size() == 2 && entradaSubstrings[0].compare("SECTION") == 0 && entradaSubstrings[1].compare("TEXT") == 0 ) {
        arquivoSaida << "SECTION TEXT" << endl;
    }else {
        cout << "Erro! SECTION TEXT deve existir em linha sozinha e estar antes de SECTION DATA." << endl;
        return -1;
    }

    pos = arquivoEntrada.tellg();
    linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
    entradaSubstrings = substrings(linhaEntrada);
    string ultimaLabel;
    bool encontrouLabel = false;
    bool encontrouDiretiva = false;
    bool encontrouInstrucao = false;
    bool encontrouMacro = false;

    while(linhaEntrada.find("STOP") == string::npos && !arquivoEntrada.eof() && !arquivoEntrada.bad()) {
        for(string str : entradaSubstrings) {
            if(str.find(':') != string::npos) {

                if(encontrouLabel) {
                    cout << "Erro! Dois rótulos seguidos encontrados!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                if(str[0] >= '0' && str[0] <= '9') {
                    cout << "Erro! Rótulo iniciado por dígito numérico." << endl;
                    return -1;
                }

                for(char c : str) {
                    if(!(c >= 'A' && c <= 'Z') && !( c>= '0' && c <= '9') && c != '_') {
                        cout << "Erro! Rótulo com caracteres não permitidos!" << endl;
                        return -1;
                    }
                }

                if(labels.find("str") != labels.end()) {
                    cout << "Erro! Redefinição de rótulo!" << endl;
                    return -1;
                }

                ultimaLabel = str;
                labels[str] = to_string(contadorLinhas);
                labels[str.erase(str.find(':'))] = "lixo";
            } else if(eInstrucao(str)) {
                if(encontrouLabel) {
                    labels[ultimaLabel] = to_string(contadorLinhas);
                    encontrouLabel = false;
                }
                encontrouInstrucao = true;
                contadorLinhas += instrucoes[str].getTam();
            } else if(str.compare("MACRO") == 0) {
                if(!encontrouLabel) {
                    cout << "Erro! Macro sem rótulo!" << endl;
                    return -1;
                }
                encontrouLabel = false;
                labels[ultimaLabel] = "MACRO";
                
            }
        }
    }

    if(arquivoEntrada.eof() || arquivoEntrada.bad()) {
        cout << "Erro! Instrução STOP não encontrada!" << endl;
        return -1;
    }

    return 0;
}

int salvaMacro() {

}
