#include "gera.pre.h"

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#include "funcoes.h"
#include "instrucao.h"
#include "diretiva.h"

using std::map;

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

map<string, string> equs;
vector<string> labels;
map<string, vector<string>> macros;
map<string, string> macroArgs;

map<string, Diretiva*> diretivas      = inicializaDiretivas();
map<string, Instrucao*> instrucoes    = inicializaInstrucoes();

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
        arquivoSaida.close();
        return -1;
    }

    #if DEBUG
    cout << "Diretivas EQU encontradas:" << endl;

    for(auto it = equs.begin(); it != equs.end(); it++) {
        cout << it->first << " = " << it->second << endl;
    }
    #endif

    if(geraPre(arquivoEntrada, arquivoSaida) != 0) {
        arquivoSaida.close();
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

                if(find(labels.begin(), labels.end(), str) != labels.end()) {
                    cout << "Erro! Redefinição de rótulo!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

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

int geraPre(fstream& arquivoEntrada, fstream& arquivoSaida) {

    int pos = arquivoEntrada.tellg();
    string linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
    vector<string> entradaSubstrings = substrings(linhaEntrada);
    int tam = entradaSubstrings.size();

    if(tam > 0) {
        for(int i = 0; i < tam; i++) {
            string s = entradaSubstrings[i];
            if(equs.find(s) != equs.end())
                entradaSubstrings[i] = equs[s];
        }
    }

    if(entradaSubstrings.size() == 2 && entradaSubstrings[0].compare("SECTION") == 0 && entradaSubstrings[1].compare("TEXT") == 0) {
        arquivoSaida << "SECTION TEXT" << endl;
    }else {
        cout << "Erro! SECTION TEXT deve existir em linha sozinha e estar antes de SECTION DATA." << endl;
        return -1;
    }

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

        if(find(entradaSubstrings.begin(), entradaSubstrings.end(), "SECTION") != entradaSubstrings.end()) {
            if(tam != 2) {
                cout << "Erro! Diretiva SECTION só recebe um argumento!" << endl;
                arquivoEntrada.seekg(pos);
                return -1;
            }

            if(entradaSubstrings[1].compare("DATA") != 0) {
                cout << "Erro! Diretiva SECTION com argumento incorreto!" << endl;
                arquivoEntrada.seekg(pos);
                return -1;
            }

            arquivoSaida << "SECTION DATA" << endl;
            contemData = true;
            break;
        }

        if(tam > 0) {
            string str = entradaSubstrings[0];
            //linha com label
            if(str.back() == ':') {

                if(encontrouIf) {
                    cout << "Erro! Rótulo dentro de diretiva IF!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

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

                if(find(labels.begin(), labels.end(), str) != labels.end()) {
                    cout << "Erro! Redefinição de rótulo!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                }

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

                if(eInstrucao(str)) {
                    //tratamento de instruções
                    int tamInst = instrucoes[str]->getTam();
                    if(tam != tamInst) {
                        cout << "Erro! Número de operandos da instrução " << str << " incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }
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
                    int tamDir = diretivas[str]->getNumOperandos() + 1;

                    if(str.compare("MACRO") != 0 && tam != tamDir) {
                        cout << "Erro! Número de operandos da diretiva " << str << " incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if(str.compare("IF") == 0) {
                        encontrouIf = true;
                        
                        if(encontrouLabel) {
                            cout << "Erro! Rótulo para diretiva IF!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }

                        string toCheck = entradaSubstrings[1];
                        if(equs.find(toCheck) == equs.end()) {
                            cout << "Erro! Diretiva IF com rótulo inválido!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }

                        int val = stoi(equs[toCheck]);
                        ifVerdadeiro = val != 0;
                    } else if(str.compare("MACRO") == 0) {
                        if(!encontrouLabel) {
                            cout << "Erro! Macro sem rótulo!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }
                        if(tam > 1) {
                            for(int i = 1; i < tam; i++) {
                                string arg = entradaSubstrings[i];

                                if(arg.front() != '&') {
                                    cout << "Erro! Argumento de macro não iniciado por \'&\'!" << endl;
                                    arquivoEntrada.seekg(pos);
                                    return -1;
                                }
                                string varNum = "#";
                                varNum.append(to_string(i));

                                if(arg.back() == ',')
                                    arg.pop_back();
                                
                                if(arg.empty()) {
                                    cout << "Erro! Argumento de macro vazio!" << endl;
                                    arquivoEntrada.seekg(pos);
                                    return -1;
                                }
                                macroArgs[arg] = varNum;
                            }
                            string args;

                            for(auto it = macroArgs.begin(); it != macroArgs.end(); it++) {
                                args.append(it->second);
                                args.append(" ");
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
                        arquivoEntrada.seekg(pos);

                        if(mapeiaMacro(arquivoEntrada, macroCorpo) != 0)
                            return -1;

                        macroArgs.clear();
                    }
                } else if(macros.find(str) != macros.end()) {
                    //expansão de macros
                    vector<string> args = substrings(macros[str][0]);
                    int numArgs = args.size();
                    if(tam != numArgs + 1) {
                        cout << "NumArgs = " << numArgs << "\ttam = " << tam << endl;
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
                    for(int i = 1; i < tamMacro; i++ ){
                        vector<string> subst = substrings(macroAtual[i]);
                        bool temVirgula = false;

                        for(string str : subst) {
                            if (str.front() == '#') {
                                string aux;

                                if(str.back() == ',') {
                                    str.pop_back();
                                    temVirgula = true;
                                }

                                aux = macroArgs[str];

                                if(temVirgula) {
                                    aux.push_back(',');
                                    temVirgula = false;
                                }

                                toWrite.push_back(aux);

                            } else {
                                toWrite.push_back(str);
                            }
                        }

                        if(!toWrite.empty()) {
                            putLine(arquivoSaida, toWrite);
                            toWrite.clear();
                        }
                    }

                    macroArgs.clear();
                } else {
                    cout << "Instrução " << str << " não identificada!";
                    arquivoEntrada.seekg(pos);
                    return -1;
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

                    if(encontrouIf) {
                        cout << "Erro! Rótulo dentro de diretiva IF!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    str.erase(str.end() - 1);

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

                    if(find(labels.begin(), labels.end(), str) != labels.end()) {
                        cout << "Erro! Redefinição de rótulo!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

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

                    if(eInstrucao(str)) {
                        cout << "Erro! Instrução dentro da seção de dados!" << endl;
                        return -1;
                    }

                    if(eDiretiva(str)) {

                        if(str.compare("CONST") == 0 && tam == 3) { //checa se diretiva const veio no formato CONST +/- valor em vez de const valor
                            string sinal = entradaSubstrings[1];

                            if(sinal.compare("+") != 0 && sinal.compare("-") != 0) {
                                cout << "Erro nos argumentos da diretiva CONST!" << endl;
                                arquivoEntrada.seekg(pos);
                                return -1;
                            }

                            entradaSubstrings[1] = sinal.append(entradaSubstrings[2]);
                            entradaSubstrings.pop_back();
                            tam--;
                        }

                        if(str.compare("SECTION") == 0) {
                            cout << "Erro! Só devem existir duas diretivas SECTION!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }
                        if(!encontrouLabel) {
                            cout << "Erro! Diretivas na seção de dados devem ter rótulos!" << endl;
                            return -1;
                        }

                        int tamDir = diretivas[str]->getNumOperandos() + 1;

                        if(tam != tamDir) {
                            cout << "Erro! Diretiva " << str << " com número incorreto de operandos!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }

                        ultimaLabel.push_back(':');
                        toWrite.push_back(ultimaLabel);
                        encontrouLabel = false;

                        for(string s: entradaSubstrings) {
                            toWrite.push_back(s);
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

int mapeiaMacro(fstream& arquivoEntrada, vector<string>& macroCorpo) {

    int pos = arquivoEntrada.tellg();
    string linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
    vector<string> entradaSubstrings = substrings(linhaEntrada);

    bool encontrouLabel = false, encontrouIf = false, ifVerdadeiro = false;

    while(linhaEntrada.find("ENDMACRO") == string::npos) {
        if(arquivoEntrada.eof() || arquivoEntrada.bad()) {
            cout << "Erro! Diretiva ENDMACRO não encontrada!" << endl;
            return -1;
        }
        int tam = entradaSubstrings.size();

        if(tam > 0) {
            string str = entradaSubstrings[0];
            //linha com label
            if(str.back() == ':') {
                cout << "Erro! Definição de label dentro de macro!" << endl;
                arquivoEntrada.seekg(pos);
                return -1;
            }
            if(tam > 0) {
                string str;

                if(!encontrouIf || ifVerdadeiro) {
                    str = entradaSubstrings[0];
                } else {
                    str = "";
                }
                encontrouIf = false;

                if(eInstrucao(str)) {
                    //tratamento de instruções
                    int tamInst = instrucoes[str]->getTam();
                    if(tam != tamInst) {
                        cout << "Erro! Número de operandos da instrução " << str << " incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    string linha = "";

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
                    int tamDir = diretivas[str]->getNumOperandos() + 1;

                    if(str.compare("MACRO") == 0) {
                        cout << "Erro! Definição de macro dentro de outra definição de macro!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if(tam != tamDir) {
                        cout << "Erro! Número de operandos da diretiva incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if(str.compare("IF") == 0) {
                        encontrouIf = true;
                        
                        if(encontrouLabel) {
                            cout << "Erro! Rótulo para diretiva IF!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
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
                    cout << "Instrução " << str << " não identificada!";
                    arquivoEntrada.seekg(pos);
                    return -1;
                }
            }
        }

        pos = arquivoEntrada.tellg();
        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
        entradaSubstrings = substrings(linhaEntrada);
    }

    if(entradaSubstrings.size() != 1) {
        cout << "Erro! Diretiva ENDMACRO deve vir sozinha na linha!" << endl;
        return -1;
    }

    return 0;
}