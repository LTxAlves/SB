#include "converte_nasm.h"

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

const char BSS = 'B';
const char DATA = 'D';

int mainConverte(string nomeArquivoEntrada, fstream& arquivoEntrada) {

    unordered_map<string, Diretiva*> diretivas      = inicializaDiretivas();
    unordered_map<string, Instrucao*> instrucoes    = inicializaInstrucoes();

    string nomeArquivoSaida = alteraExtensaoNomeArquivo(nomeArquivoEntrada, "s");

    fstream arquivoSaida;
    arquivoSaida.open(nomeArquivoSaida, fstream::in | fstream::out | fstream::trunc);

    if (!arquivoSaida.is_open() || arquivoSaida.bad())  {
        cerr << "Erro ao criar arquivo " << nomeArquivoSaida << endl;
        deletaDiretivas(diretivas);
        deletaInstrucoes(instrucoes);
        return -1;
    }

    cout << "Gerando arquivo " << nomeArquivoSaida << endl;

    if ( mapeiaEqus(arquivoEntrada) != 0 ) {
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

    if (geraConvertido(arquivoEntrada, arquivoSaida, instrucoes, diretivas) != 0) {
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

        if (tam > 0) {
            str = entradaSubstrings[0];

            if (equs.find(str) != equs.end()) {
                entradaSubstrings[0] = equs[str];
                str = entradaSubstrings[0];
            }

            if (str.back() == ':') {
                str.pop_back();

                encontrouLabel = true;
                ultimaLabel = str;
                labels.push_back(str);
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            
            if (tam > 0) {
                str = entradaSubstrings[0];
                
                if (str.compare("SECTION") == 0) {
                    break;
                }

                if (str.compare("EQU") == 0) {

                    if (tam != 2) {
                        cout << "Erro! EQU com número de operandos incorreto!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if (!encontrouLabel) {
                        cout << "Erro! EQU sem rótulo!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    equs[ultimaLabel] = entradaSubstrings[1];
                    encontrouLabel = false;
                } else if ((eDiretiva(str) || eInstrucao(str))) {
                    cout << "Erro! Diretiva/Instrução " << str << " fora da seção de instruções!" << endl;
                    arquivoEntrada.seekg(pos);
                    return -1;
                    
                }
            }
        }
    } while(!arquivoEntrada.eof() && !arquivoEntrada.bad());

    if (arquivoEntrada.eof() || arquivoEntrada.bad()) {
        cout << "Erro! Diretiva SECTION não encontrada!" << endl;
        return -1;
    }
    
    arquivoEntrada.seekg(pos);

    return 0;
}

int geraConvertido(fstream& arquivoEntrada, fstream& arquivoSaida, unordered_map<string, Instrucao*>& instrucoes, unordered_map<string, Diretiva*>& diretivas) {

    int pos, tam, contadorMacros = 0;
    string linhaEntrada;
    vector<string> entradaSubstrings;

    string ultimaLabel;
    bool encontrouLabel = false, encontrouIf = false, ifVerdadeiro = false, contemData = false, escreveuSecao = false;
    vector<string> toWrite;
    char secao = BSS;

    toWrite.push_back("global _start\nsection .text\n_start:");
    putLine(arquivoSaida, toWrite);
    toWrite.clear();

    do {
        pos = arquivoEntrada.tellg();
        linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
        entradaSubstrings = substrings(linhaEntrada);
        tam = entradaSubstrings.size();

        if (tam > 0) {
            for(int i = 0; i < tam; i++) {
                string s = entradaSubstrings[i];
                if (equs.find(s) != equs.end())
                    entradaSubstrings[i] = equs[s];
            }
        }

        if (tam > 0) {
            string str = entradaSubstrings[0];
            //linha com label
            if (str.back() == ':') {

                str.pop_back();

                encontrouLabel = true;
                labels.push_back(str);
                ultimaLabel = str;
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
            }
            if (tam > 0) {
                string str;

                if (encontrouIf && !ifVerdadeiro) {
                    encontrouIf = false;
                    encontrouLabel = false;
                    continue;
                }

                str = entradaSubstrings[0];

                if (eInstrucao(str)) {
                    //tratamento de instruções
                    if (encontrouLabel) {
                        encontrouLabel = false;
                        ultimaLabel.push_back(':');
                        toWrite.push_back(ultimaLabel);
                    }

                    string convertido = "";

                    auto it = entradaSubstrings.begin();
                    it++;

                    //convertendo para nasm
                    if (str == "ADD") {
                        convertido += "add EAX, dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                    } else if (str == "SUB") {
                        convertido += "sub EAX, dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                    } else if (str == "COPY") {
                        convertido += "mov EBX, dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            (*it).pop_back(); //remove virgula
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido.pop_back();
                            convertido += "]";
                        }
                        it++;
                        convertido += "\nmov dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                        convertido += ", EBX";
                    } else if (str == "LOAD") {
                        convertido += "mov EAX, dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                    } else if (str == "STORE") {
                        convertido += "mov dword [" + *it ;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                        convertido += ", EAX";
                    } else if (str == "STOP") {
                        convertido += "mov EAX, 1\nmov EBX, 0\nint 80H";
                    } else if (str == "JMP") {
                        convertido += "jmp " + *it;
                    } else if (str == "JMPN") {
                        convertido += "cmp EAX, 0\njl " + *it;
                    } else if (str == "JMPP") {
                        convertido += "cmp EAX, 0\njg " + *it;
                    } else if (str == "JMPZ") {
                        convertido += "cmp EAX, 0\nje " + *it;
                    } else if (str == "DIV") {
                        convertido += "cdq\nidiv dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                    } else if (str == "MULT") {
                        convertido += "imul dword [" + *it;
                        if (it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2; //mover ponteiro para deslocamento
                            convertido += " + " + to_string((atoi(it->c_str()) * 4)) + "]"; //multiplica por 4, endereçamento em 32 bits/4 bytes
                        } else {
                            convertido += "]";
                        }
                        convertido += "\njo _overflow";
                    } else if (str == "INPUT") {
                        convertido += "push EAX\npush dword " + *it;
                        if(it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2;
                            convertido += " + " + to_string((atoi(it->c_str()) * 4));
                        }
                        convertido += "\ncall _LerInteiro\nadd ESP, 4";
                        convertido += "\nmov dword [_charsLidos], EAX\npush dword _charsLidos\ncall _quantChars\nadd ESP, 4\npop EAX";
                    }  else if (str == "OUTPUT") {
                        convertido += "push EAX\npush dword " + *it;
                        if(it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2;
                            convertido += " + " + to_string((atoi(it->c_str()) * 4));
                        }
                        convertido += "\ncall _EscreverInteiro\nadd ESP, 4";
                        convertido += "\nmov EAX, 4\nmov EBX, 1\nmov ECX, _CRLF\nmov EDX, 2\nint 80H\npop EAX";
                    }  else if (str == "C_INPUT") {
                        convertido += "push EAX\npush dword " + *it;
                        if(it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2;
                            convertido += " + " + to_string((atoi(it->c_str()) * 4));
                        }
                        convertido += "\ncall _LerChar\nadd ESP, 4";
                        convertido += "\nmov dword [_charsLidos], EAX\npush dword _charsLidos\ncall _quantChars\nadd ESP, 4\npop EAX";
                    }  else if (str == "C_OUTPUT") {
                        convertido += "push EAX\npush dword " + *it;
                        if(it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2;
                            convertido += " + " + to_string((atoi(it->c_str()) * 4));
                        }
                        convertido += "\ncall _EscreverChar\nadd ESP, 4";
                        convertido += "\nmov EAX, 4\nmov EBX, 1\nmov ECX, _CRLF\nmov EDX, 2\nint 80H\npop EAX";
                    }  else if (str == "S_INPUT") {
                        
                        if(it->back() == ',')
                            it->pop_back();

                        convertido += "push EAX\npush dword " + *it;
                        if(it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2;

                            if(it->back() == ',')
                                it->pop_back();

                            convertido += " + " + to_string((atoi(it->c_str()) * 4));
                        }
                        it++; //proximo argumento: quantidade de caracteres

                        convertido += "\npush dword " + *it + "\ncall _LerString\nadd ESP, 8";
                        convertido += "\nmov dword [_charsLidos], EAX\npush dword _charsLidos\ncall _quantChars\nadd ESP, 4\npop EAX";
                    }  else if (str == "S_OUTPUT") {

                        if(it->back() == ',')
                            it->pop_back();
                        
                        convertido += "push EAX\npush dword " + *it;
                        if(it + 1 != entradaSubstrings.end() && *(it + 1) == "+") {
                            it += 2;

                            if(it->back() == ',')
                                it->pop_back();

                            convertido += " + " + to_string((atoi(it->c_str()) * 4));
                        }
                        it++; //proximo argumento: quantidade de caracteres

                        convertido += "\npush dword " + *it + "\ncall _EscreverString\nadd ESP, 8\npop EAX";

                    } else {
                        convertido += ";Ainda não traduzi isso aqui";
                    }

                    toWrite.push_back(convertido);

                } else if (eDiretiva(str)) {
                    //tratamento de diretivas

                    if (str.compare("IF") == 0) {
                        encontrouIf = true;
                        
                        if (encontrouLabel) {
                            toWrite.push_back(ultimaLabel);
                            encontrouLabel = false;
                        }

                        string toCheck = entradaSubstrings[1];

                        ifVerdadeiro = (stoi(toCheck) != 0);
                    } else if (str.compare("MACRO") == 0) {
                        if (!encontrouLabel) {
                            cout << "Erro! Macro sem rótulo!" << endl;
                            arquivoEntrada.seekg(pos);
                            return -1;
                        }
                        if (tam > 1) {
                            string args;
                            for(int i = 1; i < tam; i++) {
                                string arg = entradaSubstrings[i];

                                if (arg.front() != '&') {
                                    cout << "Erro! Argumento de macro não iniciado por \'&\'!" << endl;
                                    arquivoEntrada.seekg(pos);
                                    return -1;
                                }
                                string varNum = "#";
                                varNum.append(to_string(i));

                                if (arg.back() == ',') {
                                    if (i != tam - 1)
                                        arg.pop_back();
                                    else {
                                        cout << "Erro! Argumento final acabado em \',\'!" << endl;
                                    }
                                }
                                
                                if (arg.compare("&") == 0) {
                                    cout << "Erro! Argumento de macro vazio!" << endl;
                                    arquivoEntrada.seekg(pos);
                                    return -1;
                                }
                                macroArgs[arg] = varNum;
                                args.append(varNum).push_back(SPACE);
                            }

                            if (!args.empty()) {
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

                        if (mapeiaMacro(arquivoEntrada, macroCorpo, macroLbls, instrucoes, diretivas) != 0)
                            return -1;

                        macroArgs.clear();
                    } else if (str.compare("SECTION") == 0){

                        if (find(entradaSubstrings.begin(), entradaSubstrings.end(), "DATA") != entradaSubstrings.end()) {
                            contemData = true;
                            funcoesIO(arquivoSaida);
                            break;
                        }
                    }
                } else if (macros.find(str) != macros.end()) {
                    //expansão de macros

                    vector<string> args = substrings(macros[str][0]);
                    int numArgs = args.size();
                    ++contadorMacros;

                    if (tam != numArgs + 1) {
                        cout << "Tam = " << tam << " numArgs + 1 = " << numArgs + 1 << endl;
                        cout << "Erro! Chamada da macro " << str << " com número incorreto de argumentos!";
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }
                    
                    for(int i = 0; i < numArgs; i++) {
                        string variavel = entradaSubstrings[i + 1];
                        if (variavel.back() == ',')
                            variavel.pop_back();

                        macroArgs[args[i]] = variavel;
                    }

                    vector<string> macroAtual = macros[str];
                    int tamMacro = macroAtual.size();
                    vector<string> macroLbls = macroLabels[str];

                    if (encontrouLabel) {
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

                                if (umaString.back() == ',') {
                                    umaString.pop_back();
                                    temVirgula = true;
                                }

                                aux = macroArgs[umaString];

                                if (temVirgula) {
                                    aux.push_back(',');
                                    temVirgula = false;
                                }

                                if (aux.front() == '&') {
                                    aux.erase(aux.begin());
                                }

                                toWrite.push_back(aux);

                            } else if (umaString.back() == ':') {
                                umaString.pop_back();
                                umaString.push_back('_');
                                umaString.append(str);
                                umaString.append(to_string(contadorMacros));
                                umaString.push_back(':');

                                toWrite.push_back(umaString);
                            } else if (find(macroLbls.begin(), macroLbls.end(), umaString) != macroLbls.end()) {
                                umaString.push_back('_');
                                umaString.append(str);
                                umaString.append(to_string(contadorMacros));

                                toWrite.push_back(umaString);

                            } else {
                                toWrite.push_back(umaString);
                            }
                        }

                        if (!toWrite.empty()) {
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

                if (!toWrite.empty()) {
                    putLine(arquivoSaida, toWrite);
                    toWrite.clear();
                }
            }
        }
    } while(!(arquivoEntrada.eof() || arquivoEntrada.bad()));

    toWrite.push_back("jmp _fim\n_overflow:\nmov EAX, 4\nmov EBX, 1\nmov ECX, _msgOvf\nmov EDX, _tamMsgOvf\nint 80H\nmov EAX, 1\nmov EBX, 0\nint 80H\n_fim:");
    putLine(arquivoSaida, toWrite);
    toWrite.clear();

    toWrite.push_back("section .bss\n_strAux: resb 101\n_charsLidos: resd 1");
    putLine(arquivoSaida, toWrite);
    toWrite.clear();

    toWrite.push_back("section .data\n_msgOvf: db \'Ocorreu overflow!\', 0DH, 0AH\n_tamMsgOvf: equ $-_msgOvf");
    putLine(arquivoSaida, toWrite);
    toWrite.clear();

    toWrite.push_back("_msgLeitura1: db \'Foram lidos \'\n_tam1: equ $-_msgLeitura1\n_msgLeitura2: db \' caracteres\'\n_CRLF: db 0DH, 0AH\n_tam2: equ $-_msgLeitura2\n_dez: dd 10\n_sinalMenos: db \'-\'");
    putLine(arquivoSaida, toWrite);
    toWrite.clear();

    secao = DATA;
    escreveuSecao = true;

    if (contemData) {
        encontrouLabel = false;
        encontrouIf = false;
        ifVerdadeiro = false;
        contemData = false;
        do {

            pos = arquivoEntrada.tellg();
            linhaEntrada = toupperStr(getLineModificado(arquivoEntrada));
            entradaSubstrings = substrings(linhaEntrada);

            tam = entradaSubstrings.size();

            if (tam > 0) {
                for(int i = 0; i < tam; i++) {
                    string s = entradaSubstrings[i];
                    if (equs.find(s) != equs.end())
                        entradaSubstrings[i] = equs[s];
                }
            }

            if (tam > 0) {
                string str = entradaSubstrings[0];
                //linha com label
                if (str.back() == ':') {

                    str.pop_back();

                    encontrouLabel = true;
                    labels.push_back(str);
                    ultimaLabel = str;
                    tam--;
                    entradaSubstrings.erase(entradaSubstrings.begin());
                }

                if (tam > 0) {
                    string str;

                    if (!encontrouIf || ifVerdadeiro) {
                        str = entradaSubstrings[0];
                    } else {
                        str = "";
                    }
                    encontrouIf = false;

                    if (eDiretiva(str)) {

                        if (str.compare("SECTION") == 0) {
                            cout << "Erro! Diretiva SECTION dentro da seção de dados!" << endl;
                            return -1;
                        }
                        if (str.compare("IF") == 0) {
                            encontrouIf = true;
                            
                            if (encontrouLabel) {
                                toWrite.push_back(ultimaLabel);
                                encontrouLabel = false;
                            }

                            string toCheck = entradaSubstrings[1];

                            ifVerdadeiro = (stoi(toCheck) != 0);
                        } else {

                            string s = entradaSubstrings[0];

                            if (s.compare("SPACE") == 0 && secao != BSS) {
                                secao = BSS;
                                escreveuSecao = false;
                            } else if (s.compare("CONST") == 0 && secao != DATA) {
                                secao = DATA;
                                escreveuSecao = false;
                            }

                            if (!escreveuSecao) {
                                escreveuSecao = true;
                                if (secao == BSS)
                                    toWrite.push_back("section .bss");
                                else
                                    toWrite.push_back("section .data");
                            }

                            if (!toWrite.empty()) {
                                putLine(arquivoSaida, toWrite);
                                toWrite.clear();
                            }

                            if (encontrouLabel) {
                                ultimaLabel.push_back(':');
                                toWrite.push_back(ultimaLabel);
                                encontrouLabel = false;
                            }
            
                            if (secao == BSS) {
                                toWrite.push_back("resd");
                                if (entradaSubstrings.size() > 2) {
                                    cout << "Erro! SPACE com mais de um argumento!" << endl;
                                    return -1;
                                }
                                if (entradaSubstrings.size() == 2)
                                    toWrite.push_back(entradaSubstrings[1]);
                                else
                                    toWrite.push_back("1");
                            } else {
                                toWrite.push_back("dd");
                                if (entradaSubstrings.size() > 2) {
                                    cout << "Erro! CONST com mais de um argumento!" << endl;
                                    return -1;
                                }
                                toWrite.push_back(entradaSubstrings[1]);
                            }

                        }
                    }

                }

                if (!toWrite.empty()) {
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

        if (arquivoEntrada.eof() || arquivoEntrada.bad()) {
            cout << "Erro! Diretiva ENDMACRO não encontrada!" << endl;
            return -1;
        }
        int tam = entradaSubstrings.size();

        if (tam > 0) {
            string str = entradaSubstrings[0];
            string linha = "";

            //linha com label
            if (str.back() == ':') {
                encontrouLabel = true;
                ultimaLabel = str;
                tam--;
                entradaSubstrings.erase(entradaSubstrings.begin());
                str.pop_back();
                macroLbls.push_back(str);
            }

            if (tam > 0) {
                string str = entradaSubstrings[0];

                if (encontrouIf && !ifVerdadeiro) {
                    encontrouIf = false;
                    encontrouLabel = false;
                    continue;
                }

                if (eInstrucao(str)) {
                    //tratamento de instruções

                    if (encontrouLabel) {
                        linha.append(ultimaLabel + SPACE);
                        encontrouLabel = false;
                    }

                    for(auto it = entradaSubstrings.begin(); it != entradaSubstrings.end(); it++) {
                        string s = *it;
                        string aux = s;
                        bool comVirgula = false;

                        if (s.back() == ',') {
                            comVirgula = true;
                            s.pop_back();
                        }

                        if (macroArgs.find(s) != macroArgs.end()) {
                            aux = macroArgs[s];
                            if (comVirgula)
                                aux.push_back(',');
                        }

                        linha.append(aux);
                        if (it != entradaSubstrings.end() - 1)
                            linha.push_back(SPACE);
                    }

                    if (!linha.empty())
                        macroCorpo.push_back(linha);

                } else if (eDiretiva(str)) {
                    //tratamento de diretivas

                    if (str.compare("ENDMACRO") == 0) {
                        break;
                    }

                    if (str.compare("MACRO") == 0) {
                        cout << "Erro! Definição de macro dentro de outra definição de macro!" << endl;
                        arquivoEntrada.seekg(pos);
                        return -1;
                    }

                    if (str.compare("IF") == 0) {
                        encontrouIf = true;
                        
                        if (encontrouLabel) {
                            macroLbls.push_back(ultimaLabel);
                            encontrouLabel = false;
                        }

                        string toCheck = entradaSubstrings[1];
                        if (equs.find(toCheck) == equs.end()) {
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

                        if (s.back() == ',') {
                            comVirgula = true;
                            s.pop_back();
                        }

                        if (macroArgs.find(s) != macroArgs.end()) {
                            aux = macroArgs[s];
                            if (comVirgula)
                                aux.push_back(',');
                        }

                        linha.append(aux);
                        if (it != entradaSubstrings.end() - 1)
                            linha.push_back(SPACE);
                    }

                    if (!linha.empty())
                        macroCorpo.push_back(linha);
                }
            }
        }
    } while(linhaEntrada.find("ENDMACRO") == string::npos);

    if (entradaSubstrings.size() != 1) {
        cout << "Erro! Diretiva ENDMACRO deve vir sozinha na linha!" << endl;
        return -1;
    }

    return 0;
}

void funcoesIO(fstream& arquivoSaida) {
    arquivoSaida << "_LerInteiro:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "push ESI" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 3" << endl;
    arquivoSaida << "mov EBX, 0" << endl;
    arquivoSaida << "mov ECX, _strAux" << endl;
    arquivoSaida << "mov EDX, 5" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "push EAX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, dword [EBP + 8]" << endl;
    arquivoSaida << "mov [EAX], dword 0" << endl;
    arquivoSaida << "xor EAX, EAX" << endl;
    arquivoSaida << "xor EBX, EBX" << endl;
    arquivoSaida << "xor ESI, ESI" << endl;
    arquivoSaida << "mov ECX, 4" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "cmp byte [_strAux], 2DH" << endl;
    arquivoSaida << "jne _loopAtoI" << endl;
    arquivoSaida << "dec ECX" << endl;
    arquivoSaida << "inc EBX" << endl;
    arquivoSaida << "add ESI, 1" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_loopAtoI:" << endl;
    arquivoSaida << "movsx EDX, byte [_strAux + EBX]" << endl;
    arquivoSaida << "sub EDX, 30H" << endl;
    arquivoSaida << "cmp EDX, 9" << endl;
    arquivoSaida << "jg _fimLoop" << endl;
    arquivoSaida << "cmp EDX, 0" << endl;
    arquivoSaida << "jl _fimLoop" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "mul dword [_dez]" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "add EAX, EDX" << endl;
    arquivoSaida << "inc EBX" << endl;
    arquivoSaida << "loop _loopAtoI" << endl;
    arquivoSaida << "_fimLoop:" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "cmp EAX, 999" << endl;
    arquivoSaida << "jle _naoMaior" << endl;
    arquivoSaida << "cdq" << endl;
    arquivoSaida << "div dword [_dez]" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "cmp EAX, 999" << endl;
    arquivoSaida << "jle _naoMaior" << endl;
    arquivoSaida << "cdq" << endl;
    arquivoSaida << "div dword [_dez]" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_naoMaior:" << endl;
    arquivoSaida << "cmp ESI, 1" << endl;
    arquivoSaida << "jne _fimLerInt" << endl;
    arquivoSaida << "neg EAX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_fimLerInt:" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EDX, dword [EBP + 8]" << endl;
    arquivoSaida << "add [EDX], EAX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "pop EAX" << endl;
    arquivoSaida << "pop ESI" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_EscreverInteiro:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "push ESI" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EBX, dword [EBP + 8]" << endl;
    arquivoSaida << "mov ECX, dword [EBX]" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "cmp ECX, 0" << endl;
    arquivoSaida << "jge _naoNegativo" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "neg dword [EBX]" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 4" << endl;
    arquivoSaida << "mov EBX, 1" << endl;
    arquivoSaida << "mov ECX, _sinalMenos" << endl;
    arquivoSaida << "mov EDX, 1" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_naoNegativo:" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "mov EAX, dword [EBX]" << endl;
    arquivoSaida << "xor ECX, ECX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_loopItoA:" << endl;
    arquivoSaida << "cdq" << endl;
    arquivoSaida << "div dword [_dez]" << endl;
    arquivoSaida << "add EDX, 30H" << endl;
    arquivoSaida << "push DX" << endl;
    arquivoSaida << "inc ECX" << endl;
    arquivoSaida << "cmp EAX, 0" << endl;
    arquivoSaida << "jne _loopItoA" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_desempilhaChars:" << endl;
    arquivoSaida << "pop DX" << endl;
    arquivoSaida << "mov byte [_strAux + EAX], DL" << endl;
    arquivoSaida << "inc EAX" << endl;
    arquivoSaida << "cmp EAX, ECX" << endl;
    arquivoSaida << "jne _desempilhaChars" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "push dword _strAux" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "call _EscreverString" << endl;
    arquivoSaida << "add ESP, 8" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_fimEscreverInt:" << endl;
    arquivoSaida << "pop EAX" << endl;
    arquivoSaida << "pop ESI" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_LerChar:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 3" << endl;
    arquivoSaida << "mov EBX, 0" << endl;
    arquivoSaida << "mov ECX, _strAux" << endl;
    arquivoSaida << "mov EDX, 2" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "push EAX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "movzx EBX, byte [_strAux]" << endl;
    arquivoSaida << "mov ECX, dword [EBP + 8]" << endl;
    arquivoSaida << "mov dword [ECX], EBX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "pop EAX" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_EscreverChar:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "push ESI" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 4" << endl;
    arquivoSaida << "mov EBX, 1" << endl;
    arquivoSaida << "mov ECX, dword [EBP + 8]" << endl;
    arquivoSaida << "mov EDX, 1" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "pop ESI" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_LerString:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 3" << endl;
    arquivoSaida << "mov EBX, 0" << endl;
    arquivoSaida << "mov ECX, [EBP + 12]" << endl;
    arquivoSaida << "mov EDX, [EBP + 8]" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_EscreverString:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "push ESI" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 4" << endl;
    arquivoSaida << "mov EBX, 1" << endl;
    arquivoSaida << "mov ECX, dword [EBP + 12]" << endl;
    arquivoSaida << "mov EDX, dword [EBP + 8]" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "pop ESI" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "_quantChars:" << endl;
    arquivoSaida << "enter 0, 0" << endl;
    arquivoSaida << "push EAX" << endl;
    arquivoSaida << "push EBX" << endl;
    arquivoSaida << "push ECX" << endl;
    arquivoSaida << "push EDX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 4" << endl;
    arquivoSaida << "mov EBX, 1" << endl;
    arquivoSaida << "mov ECX, _msgLeitura1" << endl;
    arquivoSaida << "mov EDX, _tam1" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "push dword [EBP + 8]" << endl;
    arquivoSaida << "call _EscreverInteiro" << endl;
    arquivoSaida << "add ESP, 4" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "mov EAX, 4" << endl;
    arquivoSaida << "mov EBX, 1" << endl;
    arquivoSaida << "mov ECX, _msgLeitura2" << endl;
    arquivoSaida << "mov EDX, _tam2" << endl;
    arquivoSaida << "int 80H" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "pop EDX" << endl;
    arquivoSaida << "pop ECX" << endl;
    arquivoSaida << "pop EBX" << endl;
    arquivoSaida << "pop EAX" << endl;
    arquivoSaida << "" << endl;
    arquivoSaida << "leave" << endl;
    arquivoSaida << "ret" << endl;
}