#include "funcoes.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

using std::sort;
using std::find;

using std::toupper;

using std::endl;

using std::make_pair;

using std::vector;

using std::string;

using std::map;

using std::fstream;

static const vector<string> INST_VEC = {"ADD", "SUB", "MULT", "DIV", "JMP", "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};
static const vector<string> DIR_VEC = {"SECTION", "SPACE", "CONST", "EQU", "IF", "MACRO", "ENDMACRO"};

bool acabaCom(string const &stringOrigem, string const &final) {

    if(stringOrigem.length() >= final.length()) {
        return (stringOrigem.compare(stringOrigem.length() - final.length(), final.length(), final) == 0);
    } else {
        return false;
    }
}

map<string, Instrucao*>  inicializaInstrucoes() {

    map<string, Instrucao*> mapInst;

    mapInst["ADD"]      = new Instrucao("ADD",    1, 1,  2);
    mapInst["SUB"]      = new Instrucao("SUB",    1, 2,  2);
    mapInst["MULT"]     = new Instrucao("MULT",   1, 3,  2);
    mapInst["DIV"]      = new Instrucao("DIV",    1, 4,  2);
    mapInst["JMP"]      = new Instrucao("JMP",    1, 5,  2);
    mapInst["JMPN"]     = new Instrucao("JMPN",   1, 6,  2);
    mapInst["JMPP"]     = new Instrucao("JMPP",   1, 7,  2);
    mapInst["JMPZ"]     = new Instrucao("JMPZ",   1, 8,  2);
    mapInst["COPY"]     = new Instrucao("COPY",   2, 9,  3);
    mapInst["LOAD"]     = new Instrucao("LOAD",   1, 10, 2);
    mapInst["STORE"]    = new Instrucao("STORE",  1, 11, 2);
    mapInst["INPUT"]    = new Instrucao("INPUT",  1, 12, 2);
    mapInst["OUTPUT"]   = new Instrucao("OUTPUT", 1, 13, 2);
    mapInst["STOP"]     = new Instrucao("STOP",   0, 14, 1);

    return mapInst;
}

map< string, Diretiva*> inicializaDiretivas() {

    map<string, Diretiva*> mapDir;

    mapDir["SECTION"]   = new Diretiva("SECTION",  1, 0);
    mapDir["SPACE"]     = new Diretiva("SPACE",    0, 1);
    mapDir["CONST"]     = new Diretiva("CONST",    1, 1);
    mapDir["EQU"]       = new Diretiva("EQU",      1, 0);
    mapDir["IF"]        = new Diretiva("IF",       1, 0);
    mapDir["MACRO"]     = new Diretiva("MACRO",    0, 0);
    mapDir["ENDMACRO"]  = new Diretiva("ENDMACRO", 0, 0);

    return mapDir;
}

string getLineModificado(fstream& arquivo) {

    char c = arquivo.get();
    string linha;

    while(!arquivo.eof() && !arquivo.bad()) {
        if(c == CR) {
            if(arquivo.peek() == LF)
                c = arquivo.get();
            break;
        } else if (c == LF) {
            break;
        }

        linha += c;

        c = arquivo.get();
    }
    
    return linha;
}

void putLine(fstream& arquivo, vector<string> &toPut) {

    for(vector<string>::iterator it = toPut.begin(); it != toPut.end() - 1; it++) {
        arquivo << (*it).c_str() << ' ';
    }

    arquivo << toPut.back() << endl;
}

void ignoraChars(fstream& arquivo, vector<char> chars) {

    char c;

    if(arquivo.eof() || arquivo.bad())
        return;

    c = arquivo.get();

    vector<char>::iterator it = find(chars.begin(), chars.end(), c);

    while( it != chars.end() && !arquivo.eof() && !arquivo.bad()) {
        c = arquivo.get();
        it = find(chars.begin(), chars.end(), c);
    }

    if(c == ';') //se encontrou comentário, ignora
        procuraFimLinha(arquivo);

    ignoraChars(arquivo, chars); //continua a ignorar depois do comentário

    if(!arquivo.eof())
        arquivo.putback(c);
}

void ignoraEspacoTab(fstream& arquivo) {

    ignoraChars(arquivo, vector<char>{SPACE, TAB});
}

void ignoraQuebraLinha(fstream& arquivo) {

    ignoraChars(arquivo, vector<char>{CR, LF});
}

void ignoraVazios(fstream& arquivo) {

    ignoraChars(arquivo, vector<char>{SPACE, TAB, CR, LF});
}

string alteraExtensaoNomeArquivo(string nomeArquivo, string novaExtensao) {

    string nomeArquivoNovo = nomeArquivo.substr(0, nomeArquivo.size() - 3); //retira "asm" ou "pre" da extensao
    nomeArquivoNovo += novaExtensao; //adiciona nova extensao

    return nomeArquivoNovo;
}

bool encontrarNaLinha(fstream& arquivo, string procurado) {

    int posOriginal = arquivo.tellg();

    string linhaLida = getLineModificado(arquivo);

    for(string::iterator it = linhaLida.begin(); it != linhaLida.end(); it++) {
        *it = toupper(*it);
    }

    for(string::iterator it = procurado.begin(); it != procurado.end(); it++) {
        *it = toupper(*it);
    }

    arquivo.seekg(posOriginal);

    return linhaLida.find(procurado);
}

string getLabel(fstream& arquivo) {

    string label;
    char c;
    bool encontrouEspaco = false; //flag para caso encontre espaço/tab na linha

    c = arquivo.get();

    while ( !arquivo.bad() && !arquivo.eof() && c != ':' ) {
        c = toupper(c);

        if( !(c >= 'A' && c <= 'Z') && c != '_' && !(c >= '0' && c <= '9') && c != SPACE && c != TAB ) {
            return "ERRO!";
        }

        //se primeira vez encontrando espaço/tab, flag vira verdade
        if(!encontrouEspaco && (c == SPACE || c == TAB)) {
            encontrouEspaco = true;
        }
        
        //label só tem caracteres válidos
        if(!encontrouEspaco) {
            label += c;
        } else if(c != SPACE && c != TAB) {
            //encontrou caracteres válidos separados por espaço/tab
            return "ERRO!";
        }

        c = arquivo.get();
    }
    
    if(!arquivo.bad() && !arquivo.eof()) {
        arquivo.putback(c);
    }

    return label;
}

string getWord(fstream& arquivo) {

    string word;
    char c;

    c = arquivo.get();

    while(!arquivo.bad() && !arquivo.eof() && c != CR && c != LF && c != SPACE && c != TAB && c != ';') {
        word += c;
        c = arquivo.get();
    }

    return word;
}

void procuraFimLinha(fstream& arquivo) {

    char c;
    
    c = arquivo.get();

    while(!arquivo.bad() && !arquivo.eof() && c != CR && c != LF) {
        c = arquivo.get();
    }

    if(!arquivo.eof())
        arquivo.putback(c);
}

vector<string> substrings(string linha) {

    vector<string> substrings;
    string umaString;

    string::iterator it;

    for(it = linha.begin(); it != linha.end(); it++) {
        char c = *it;

        if(c == ';' || c == CR || c == LF) {
            if(!umaString.empty()) {
                substrings.push_back(umaString);
            }
            return substrings;
        } else if(c == SPACE || c == TAB) {
            if(!umaString.empty()) {
                substrings.push_back(umaString);
            }
            umaString.clear();
        } else if(c == ':') {
            if(umaString.empty()) {
                //adiciona : junto à última substring
                substrings.back() += c;
            } else {
                umaString += c;
            }
            substrings.push_back(umaString);
            umaString.clear();
        } else {
            umaString += toupper(c);
        }
    }

    if(!umaString.empty()) {
        substrings.push_back(umaString);
    }

    return substrings;
}

string toupperStr(string str) {

    string maiusculas;

    for(auto it = str.begin(); it != str.end(); it++) {
        maiusculas += toupper(*it);
    }

    return maiusculas;
}

bool eDiretiva(string str) {

    for(auto it = DIR_VEC.begin(); it != DIR_VEC.end(); it++) {
        if(str.compare(*it) == 0)
            return true;
    }

    return 0;
}

bool eInstrucao(string str) {

    for(auto it = INST_VEC.begin(); it != INST_VEC.end(); it++) {
        if(str.compare(*it) == 0)
            return true;
    }

    return 0;
}
