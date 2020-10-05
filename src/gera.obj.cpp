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

int mainObj(string nomeArquivoEntrada, fstream& arquivoEntrada) {
    
    map<string, Diretiva*> diretivas      = inicializaDiretivas();
    map<string, Instrucao*> instrucoes    = inicializaInstrucoes();

    string nomeArquivoSaida = alteraExtensaoNomeArquivo(nomeArquivoEntrada, "obj");

    fstream arquivoSaida;
    arquivoSaida.open(nomeArquivoSaida, fstream::in | fstream::out | fstream::trunc);

    if(!arquivoSaida.is_open() || arquivoSaida.bad()) {
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
        cerr << "Erro ao criar arquivo " << nomeArquivoSaida << endl;
        return -1;
    }

    cout << "Gerando arquivo " << nomeArquivoSaida << endl;

    cout << "Primeira passagem..." << endl;

    if(primeiraPassagem(arquivoEntrada, instrucoes, diretivas) != 0) {
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
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
    
    if(segundaPassagem(arquivoEntrada, arquivoSaida, instrucoes, diretivas) != 0) {
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
        cout << "Erro ao executar segunda passagem!" << endl;
        return -1;
    }

    cout << "Segunda passagem finalizada!" << endl;

    deletaDiretivas(diretivas);
    deletaInstrucoes(instrucoes);

    arquivoSaida.close();

    return 0;
}

int primeiraPassagem(fstream& arquivoEntrada, map<string, Instrucao*>& instrucoes, map<string, Diretiva*>& diretivas) {

    string linhaEntrada;
    vector<string> entradaSubstrings;
    string str;
    int pos, PC = 0;
    bool encontrouLabel = false, sectionText = false, sectionData = false;

    do {
        pos = arquivoEntrada.tellg();
        linhaEntrada = getLineModificado(arquivoEntrada);
        entradaSubstrings = substrings(linhaEntrada);
        int tam = entradaSubstrings.size();
        
        if(tam > 0) {
            str = entradaSubstrings[0];

            if(str.back() == ':') {
                str.pop_back();

                if(str.empty()) {
                    cout << "Erro! Rótulo vazio!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                if(eDiretiva(str) || eInstrucao(str)) {
                    cout << "Erro! Rótulo proibido: palavra reservada!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                if(encontrouLabel) {
                    cout << "Erro! Dois rótulos seguidos encontrados!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                if(str[0] >= '0' && str[0] <= '9') {
                    cout << "Erro! Rótulo iniciado por dígito numérico." << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

                for(char c : str) {
                    if(!(c >= 'A' && c <= 'Z') && !( c >= '0' && c <= '9') && c != '_') {
                        cout << "Erro! Rótulo com caracteres não permitidos!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }
                }

                if(tabelaLabels.find(str) != tabelaLabels.end()) {
                    cout << "Erro! Redefinição de rótulo!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }


                tabelaLabels[str] = PC;
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            
            if(tam > 0) {
                str = entradaSubstrings[0];
                if (eInstrucao(str)) {

                    if(sectionData) {
                        cout << "Erro! Instrução na seção de dados!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }
                    

                    int tamInst = instrucoes[str]->getTam();

                    if(tam != tamInst) {
                        cout << "Erro! Número de operandos da instrução " << str << " incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    PC += instrucoes[str]->getTam();
                    encontrouLabel = false;
                } else if(eDiretiva(str)) {

                    int tamDir = diretivas[str]->getNumOperandos() + 1;

                    if(tam != tamDir) {
                        cout << "Erro! Número de operandos da diretiva " << str << " incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }


                    if(str.compare("SECTION") == 0) {
                        if(entradaSubstrings[1].compare("DATA") == 0) {
                            if(sectionData) {
                                cout << "Erro! Redefinição da seção de dados!" << endl;
                                arquivoEntrada.seekg(pos);
                                return -1;
                            }
                            sectionText = false;
                            sectionData = true;
                        } else if (entradaSubstrings[1].compare("TEXT") == 0) {
                            if(sectionText) {
                                cout << "Erro! Redefinição da seção de texto!" << endl;
                                arquivoEntrada.seekg(pos);
                                return -1;
                            }
                            sectionText = true;
                            sectionData = false;
                        } else {
                            cout << "Erro! Diretiva SECTION deve vir acompanhada de TEXT ou DATA!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }
                        continue;
                    }

                    PC += diretivas[str]->getTam();
                    encontrouLabel = false;
                } else {
                    cout << "Erro! Instrução " << str << " não identificada!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }
            }
        }

    } while (!(arquivoEntrada.eof() || arquivoEntrada.bad()));

    return 0;
}

int segundaPassagem(fstream& arquivoEntrada, fstream& arquivoSaida, map<string, Instrucao*>& instrucoes, map<string, Diretiva*>& diretivas) {

    string linhaEntrada;// = toupperStr(getLineModificado(arquivoEntrada));
    vector<string> entradaSubstrings;// = substrings(linhaEntrada);
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
                    int tamInst = instrucoes[str]->getTam();
                    toWrite.push_back(instrucoes[str]->getCod());
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
