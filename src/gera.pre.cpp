#include "gera.pre.h"

#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#include "funcoes.h"
#include "instrucao.h"
#include "diretiva.h"

using std::unordered_map;

using std::vector;

using std::cout;
using std::cerr;
using std::endl;
using std::fstream;

using std::find;
using std::count;

using std::to_string;

using std::string;
using std::stoi;

unordered_map<string, string> equs;
vector<string> labels;
unordered_map<string, vector<string>> macros;
unordered_map<string, string> macroArgs;
unordered_map<string, vector<string>> macroLabels;

int mainPre(string nomeArquivoEntrada, fstream& arquivoEntrada) {

    unordered_map<string, Diretiva*> diretivas      = inicializaDiretivas();
    unordered_map<string, Instrucao*> instrucoes    = inicializaInstrucoes();

    string nomeArquivoSaida = alteraExtensaoNomeArquivo(nomeArquivoEntrada, "pre");

    fstream arquivoSaida;
    arquivoSaida.open(nomeArquivoSaida, fstream::in | fstream::out | fstream::trunc);

    if(!arquivoSaida.is_open() || arquivoSaida.bad())  {
        cerr << "Erro ao criar arquivo " << nomeArquivoSaida << endl;
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
        return -1;
    }

    cout << "Gerando arquivo " << nomeArquivoSaida << endl;

    if( mapeiaEqus(arquivoEntrada) != 0 ) {
        arquivoSaida.close();
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
        return -1;
    }

    #if DEBUG
    cout << "Diretivas EQU encontradas:" << endl;

    for(auto it = equs.begin(); it != equs.end(); it++) {
        cout << it->first << " = " << it->second << endl;
    }
    #endif

    if(geraPre(arquivoEntrada, arquivoSaida, instrucoes, diretivas) != 0) {
        arquivoSaida.close();
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
        return -1;
    }

    #if DEBUG
    cout << "MACROS encontradas:" << endl;

    for(auto it = macros.begin(); it != macros.end(); it++) {
        cout << it->first << ": ";
        vector<string> vetor = it->second;
        for(string s : vetor) {
            cout << s << endl;
        }
    }
    #endif

    arquivoSaida.close();
    
    deletaDiretivas(diretivas);
    deletaInstrucoes(instrucoes);

    return 0;
}

int mapeiaEqus(fstream& arquivoEntrada) {

    int pos;
    string linhaEntrada;
    vector<string> entradaSubstrings;
    bool encontrouLabel = false;
    string ultimaLabel, str;

    do {
        pos = arquivoEntrada.tellg();
        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
        entradaSubstrings = substrings(linhaEntrada);
        int tam = entradaSubstrings.size();

        if(tam > 0) {
            str = entradaSubstrings[0];

            if(equs.find(str) != equs.end()) {
                entradaSubstrings[0] = equs[str];
                str = entradaSubstrings[0];
            }

            if(str.back() == ':') {
                str.pop_back();

                encontrouLabel = true;
                ultimaLabel = str;
                labels.push_back(str);
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            
            if(tam > 0) {
                str = entradaSubstrings[0];
                
                if(str.compare("SECTION") == 0) {
                    break;
                }

                if(str.compare("EQU") == 0) {

                    if(tam != 2) {
                        cout << "Erro! EQU com número de operandos incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if(!encontrouLabel) {
                        cout << "Erro! EQU sem rótulo!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    equs[ultimaLabel] = entradaSubstrings[1];
                    encontrouLabel = false;
                } else if((eDiretiva(str) || eInstrucao(str))) {
                    cout << "Erro! Diretiva/Instrução " << str << " fora da seção de instruções!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                    
                }
            }
        }
    } while(!arquivoEntrada.eof() && !arquivoEntrada.bad());

    if(arquivoEntrada.eof() || arquivoEntrada.bad()) {
        cout << "Erro! Diretiva SECTION não encontrada!" << endl;
        return -1;
    }
    
    arquivoEntrada.seekg(pos);

    return 0;
}

int geraPre(fstream& arquivoEntrada, fstream& arquivoSaida, std::unordered_map<std::string, Instrucao*>& instrucoes, std::unordered_map<std::string, Diretiva*>& diretivas) {

    int pos, tam, contadorMacros = 0;
    string linhaEntrada;
    vector<string> entradaSubstrings;

    string ultimaLabel;
    bool encontrouLabel = false, encontrouIf = false, ifVerdadeiro = false, contemData = false;
    vector<string> toWrite;

    do {
        
        pos = arquivoEntrada.tellg();
        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
        entradaSubstrings = substrings(linhaEntrada);
        tam = entradaSubstrings.size();

        if(tam > 0) {
            for(int i = 0; i < tam; i++) {
                string s = entradaSubstrings[i];
                if(equs.find(s) != equs.end())
                    entradaSubstrings[i] = equs[s];
            }
        }

        if(tam > 0) {
            string str = entradaSubstrings[0];
            //linha com label
            if(str.back() == ':') {

                str.pop_back();

                encontrouLabel = true;
                labels.push_back(str);
                ultimaLabel = str;
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            if(tam > 0) {
                string str;

                if(encontrouIf && !ifVerdadeiro) {
                    encontrouIf = false;
                    encontrouLabel = false;
                    continue;
                }

                str = entradaSubstrings[0];

                if(eInstrucao(str)) {
                    //tratamento de instruções
                    if(encontrouLabel) {
                        encontrouLabel = false;
                        ultimaLabel.push_back(':');
                        toWrite.push_back(ultimaLabel);
                    }

                    for(string s : entradaSubstrings) {

                        toWrite.push_back(s);
                    }

                } else if(eDiretiva(str)) {
                    //tratamento de diretivas

                    if(str.compare("IF") == 0) {
                        encontrouIf = true;
                        
                        if(encontrouLabel) {
                            toWrite.push_back(ultimaLabel);
                            encontrouLabel = false;
                        }

                        string toCheck = entradaSubstrings[1];

                        ifVerdadeiro = (stoi(toCheck) != 0);
                    } else if(str.compare("MACRO") == 0) {
                        if(!encontrouLabel) {
                            cout << "Erro! Macro sem rótulo!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }
                        if(tam > 1) {
                            string args;
                            for(int i = 1; i < tam; i++) {
                                string arg = entradaSubstrings[i];

                                if(arg.front() != '&') {
                                    cout << "Erro! Argumento de macro não iniciado por \'&\'!" << endl;
                                    arquivoEntrada.seekg(pos);
                                    return -1;
                                }
                                string varNum = "#";
                                varNum.append(to_string(i));

                                if(arg.back() == ',') {
                                    if(i != tam - 1)
                                        arg.pop_back();
                                    else {
                                        cout << "Erro! Argumento final acabado em \',\'!" << endl;
                                    }
                                }
                                
                                if(arg.compare("&") == 0) {
                                    cout << "Erro! Argumento de macro vazio!" << endl;
                                    arquivoEntrada.seekg(pos);
                                    return -1;
                                }
                                macroArgs[arg] = varNum;
                                args.append(varNum).push_back(SPACE);
                            }

                            if(!args.empty()) {
                                args.pop_back();
                            }
                            macros[ultimaLabel].push_back(args);
                        } else {
                            macros[ultimaLabel].push_back("");
                        }
                        encontrouLabel = false;
                        pos = arquivoEntrada.tellg();
                        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
                        entradaSubstrings = substrings(linhaEntrada);
                        
                        vector<string>& macroCorpo = macros[ultimaLabel];
                        vector<string>& macroLbls = macroLabels[ultimaLabel];
                        arquivoEntrada.seekg(pos);

                        if(mapeiaMacro(arquivoEntrada, macroCorpo, macroLbls, instrucoes, diretivas) != 0)
                            return -1;

                        macroArgs.clear();
                    } else if(str.compare("SECTION") == 0){
                        
                        for(string s : entradaSubstrings) {
                            toWrite.push_back(s);
                        }

                        if(find(entradaSubstrings.begin(), entradaSubstrings.end(), "DATA") != entradaSubstrings.end()) {
                            contemData = true;

                            if(!toWrite.empty()) {
                                putLine(arquivoSaida, toWrite);
                                toWrite.clear();
                            }
                            break;
                        }
                    }
                } else if(macros.find(str) != macros.end()) {
                    //expansão de macros

                    vector<string> args = substrings(macros[str][0]);
                    int numArgs = args.size();
                    ++contadorMacros;

                    if(tam != numArgs + 1) {
                        cout << "Tam = " << tam << " numArgs + 1 = " << numArgs + 1 << endl;
                        cout << "Erro! Chamada da macro " << str << " com número incorreto de argumentos!";
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }
                    
                    for(int i = 0; i < numArgs; i++) {
                        string variavel = entradaSubstrings[i + 1];
                        if(variavel.back() == ',')
                            variavel.pop_back();

                        macroArgs[args[i]] = variavel;
                    }

                    vector<string> macroAtual = macros[str];
                    int tamMacro = macroAtual.size();
                    vector<string> macroLbls = macroLabels[str];

                    if(encontrouLabel) {
                        ultimaLabel.push_back(':');
                        toWrite.push_back(ultimaLabel);
                        encontrouLabel = false;
                    }

                    for(int i = 1; i < tamMacro; i++ ){
                        vector<string> subst = substrings(macroAtual[i]);
                        bool temVirgula = false;

                        for(string umaString : subst) {
                            if (umaString.front() == '#') {
                                string aux;

                                if(umaString.back() == ',') {
                                    umaString.pop_back();
                                    temVirgula = true;
                                }

                                aux = macroArgs[umaString];

                                if(temVirgula) {
                                    aux.push_back(',');
                                    temVirgula = false;
                                }

                                if(aux.front() == '&') {
                                    aux.erase(aux.begin());
                                }

                                toWrite.push_back(aux);

                            } else if(umaString.back() == ':') {
                                umaString.pop_back();
                                umaString.push_back('_');
                                umaString.append(str);
                                umaString.append(to_string(contadorMacros));
                                umaString.push_back(':');

                                toWrite.push_back(umaString);
                            } else if(find(macroLbls.begin(), macroLbls.end(), umaString) != macroLbls.end()) {
                                umaString.push_back('_');
                                umaString.append(str);
                                umaString.append(to_string(contadorMacros));

                                toWrite.push_back(umaString);

                            } else {
                                toWrite.push_back(umaString);
                            }
                        }

                        if(!toWrite.empty()) {
                            putLine(arquivoSaida, toWrite);
                            toWrite.clear();
                        }
                    }

                    macroArgs.clear();
                } else {
                    for(string s : entradaSubstrings) {
                        toWrite.push_back(s);
                    }
                }

                if(!toWrite.empty()) {
                    putLine(arquivoSaida, toWrite);
                    toWrite.clear();
                }
            }
        }
    } while(!(arquivoEntrada.eof() || arquivoEntrada.bad()));

    if(contemData) {
        encontrouLabel = false;
        encontrouIf = false;
        ifVerdadeiro = false;
        contemData = false;
        do {

            pos = arquivoEntrada.tellg();
            linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
            entradaSubstrings = substrings(linhaEntrada);

            tam = entradaSubstrings.size();

            if(tam > 0) {
                for(int i = 0; i < tam; i++) {
                    string s = entradaSubstrings[i];
                    if(equs.find(s) != equs.end())
                        entradaSubstrings[i] = equs[s];
                }
            }

            if(tam > 0) {
                string str = entradaSubstrings[0];
                //linha com label
                if(str.back() == ':') {

                    str.pop_back();

                    encontrouLabel = true;
                    labels.push_back(str);
                    ultimaLabel = str;
                    tam--;
                    entradaSubstrings.erase(entradaSubstrings.begin());
                }

                if(tam > 0) {
                    string str;

                    if(!encontrouIf || ifVerdadeiro) {
                        str = entradaSubstrings[0];
                    } else {
                        str = "";
                    }
                    encontrouIf = false;

                    if(eDiretiva(str)) {

                        if(str.compare("SECTION") == 0) {
                            cout << "Erro! Diretiva SECTION dentro da seção de dados!" << endl;
                            return -1;
                        }
                        if(str.compare("IF") == 0) {
                            encontrouIf = true;
                            
                            if(encontrouLabel) {
                                toWrite.push_back(ultimaLabel);
                                encontrouLabel = false;
                            }

                            string toCheck = entradaSubstrings[1];

                            ifVerdadeiro = (stoi(toCheck) != 0);
                        } else {

                            if(encontrouLabel) {
                                ultimaLabel.push_back(':');
                                toWrite.push_back(ultimaLabel);
                                encontrouLabel = false;
                            }

                            for(string s: entradaSubstrings) {
                                toWrite.push_back(s);
                            }
                        }
                    }

                }

                if(!toWrite.empty()) {
                    putLine(arquivoSaida, toWrite);
                    toWrite.clear();
                }
            }
        } while(!(arquivoEntrada.eof() || arquivoEntrada.bad()));
    }

    return 0;
}

int mapeiaMacro(fstream& arquivoEntrada, vector<string>& macroCorpo, vector<string>& macroLbls, unordered_map<string, Instrucao*>& instrucoes, unordered_map<string, Diretiva*>& diretivas) {

    int pos;
    string linhaEntrada;
    vector<string> entradaSubstrings;
    bool encontrouLabel = false, encontrouIf = false, ifVerdadeiro = false;
    string ultimaLabel;

    do {
        pos = arquivoEntrada.tellg();
        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
        entradaSubstrings = substrings(linhaEntrada);

        if(arquivoEntrada.eof() || arquivoEntrada.bad()) {
            cout << "Erro! Diretiva ENDMACRO não encontrada!" << endl;
            return -1;
        }
        int tam = entradaSubstrings.size();

        if(tam > 0) {
            string str = entradaSubstrings[0];
            string linha = "";

            //linha com label
            if(str.back() == ':') {
                encontrouLabel = true;
                ultimaLabel = str;
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
                str.pop_back();
                macroLbls.push_back(str);
            }

            if(tam > 0) {
                string str = entradaSubstrings[0];

                if(encontrouIf && !ifVerdadeiro) {
                    encontrouIf = false;
                    encontrouLabel = false;
                    continue;
                }

                if(eInstrucao(str)) {
                    //tratamento de instruções

                    if(encontrouLabel) {
                        linha.append(ultimaLabel + SPACE);
                        encontrouLabel = false;
                    }

                    for(auto it = entradaSubstrings.begin(); it != entradaSubstrings.end(); it++) {
                        string s = *it;
                        string aux = s;
                        bool comVirgula = false;

                        if(s.back() == ',') {
                            comVirgula = true;
                            s.pop_back();
                        }

                        if(macroArgs.find(s) != macroArgs.end()) {
                            aux = macroArgs[s];
                            if(comVirgula)
                                aux.push_back(',');
                        }

                        linha.append(aux);
                        if(it != entradaSubstrings.end() - 1)
                            linha.push_back(SPACE);
                    }

                    if(!linha.empty())
                        macroCorpo.push_back(linha);

                } else if(eDiretiva(str)) {
                    //tratamento de diretivas

                    if(str.compare("ENDMACRO") == 0) {
                        break;
                    }

                    if(str.compare("MACRO") == 0) {
                        cout << "Erro! Definição de macro dentro de outra definição de macro!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if(str.compare("IF") == 0) {
                        encontrouIf = true;
                        
                        if(encontrouLabel) {
                            macroLbls.push_back(ultimaLabel);
                            encontrouLabel = false;
                        }

                        string toCheck = entradaSubstrings[1];
                        if(equs.find(toCheck) == equs.end()) {
                            cout << "Erro! Diretiva IF com rótulo inválido!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }

                        int val = stoi(equs[toCheck]);
                        ifVerdadeiro = val != 0;
                    }
                } else {
                    //Supondo que instrução não reconhecida é válida


                    for(auto it = entradaSubstrings.begin(); it != entradaSubstrings.end(); it++) {
                        string s = *it;
                        string aux = s;
                        bool comVirgula = false;

                        if(s.back() == ',') {
                            comVirgula = true;
                            s.pop_back();
                        }

                        if(macroArgs.find(s) != macroArgs.end()) {
                            aux = macroArgs[s];
                            if(comVirgula)
                                aux.push_back(',');
                        }

                        linha.append(aux);
                        if(it != entradaSubstrings.end() - 1)
                            linha.push_back(SPACE);
                    }

                    if(!linha.empty())
                        macroCorpo.push_back(linha);
                }
            }
        }
    } while(linhaEntrada.find("ENDMACRO") == string::npos);

    if(entradaSubstrings.size() != 1) {
        cout << "Erro! Diretiva ENDMACRO deve vir sozinha na linha!" << endl;
        return -1;
    }

    return 0;
}