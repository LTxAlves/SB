#include "funcoes.h"

#include <algorithm>
#include <cctype>

using std::sort;
using std::find;

using std::toupper;

/**
 * Checa se uma string acaba com uma substring
 * @param stingOrigem string onde buscar
 * @param final substring a ser buscada no final
 * @returns true se stringOrigem acaba com final, falso caso contrário
 * */
bool acabaCom(string const &stringOrigem, string const &final) {

    if(stringOrigem.length() >= final.length()) {
        return (stringOrigem.compare(stringOrigem.length() - final.length(), final.length(), final) == 0);
    } else {
        return false;
    }
}

vector<Instrucao>  inicializaInstrucoes() {

    vector<Instrucao> instrucoes;

    instrucoes.push_back(Instrucao("ADD",    1, 1,  2));
    instrucoes.push_back(Instrucao("SUB",    1, 2,  2));
    instrucoes.push_back(Instrucao("MULT",   1, 3,  2));
    instrucoes.push_back(Instrucao("DIV",    1, 4,  2));
    instrucoes.push_back(Instrucao("JMP",    1, 5,  2));
    instrucoes.push_back(Instrucao("JMPN",   1, 6,  2));
    instrucoes.push_back(Instrucao("JMPP",   1, 7,  2));
    instrucoes.push_back(Instrucao("JMPZ",   1, 8,  2));
    instrucoes.push_back(Instrucao("COPY",   2, 9,  3));
    instrucoes.push_back(Instrucao("LOAD",   1, 10, 2));
    instrucoes.push_back(Instrucao("STORE",  1, 11, 2));
    instrucoes.push_back(Instrucao("INPUT",  1, 12, 2));
    instrucoes.push_back(Instrucao("OUTPUT", 1, 13, 2));
    instrucoes.push_back(Instrucao("STOP",   0, 14, 1));

    sort(instrucoes.begin(), instrucoes.end());

    return instrucoes;
}

vector<Diretiva> inicializaDiretivas() {

    vector<Diretiva> diretivas;

    diretivas.push_back(Diretiva("SECTION",  1, 0));
    diretivas.push_back(Diretiva("SPACE",    0, 1));
    diretivas.push_back(Diretiva("CONST",    1, 0));
    diretivas.push_back(Diretiva("EQU",      1, 0));
    diretivas.push_back(Diretiva("IF",       1, 0));
    diretivas.push_back(Diretiva("MACRO",    0, 0));
    diretivas.push_back(Diretiva("ENDMACRO", 0, 0));

    sort(diretivas.begin(), diretivas.end());

    return diretivas;
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

    string nomeArquivoNovo = nomeArquivo.substr(0, nomeArquivo.size() - 3); //retira "asm" da extensao
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

    while ( !arquivo.bad() && !arquivo.eof() && c != ':' ){
        c = toupper(c);

        if( !(c >= 'A' && c <= 'Z') && c != '_' && !(c >= '0' && c <= '9') && c != SPACE && c != TAB ) {
            return "ERRO!";
        }

        //se primeira vez encontrando espaço/tab, flag vira verdade
        if(!encontrouEspaco && (c == SPACE || c == TAB)){
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
    
    if(!arquivo.bad() && !arquivo.eof()){
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
            if(umaString.compare("") != 0) {
                substrings.push_back(umaString);
            }
            return substrings;
        }

        if(c == SPACE || c == TAB){
            if(umaString.compare("") != 0) {
                substrings.push_back(umaString);
            }
            umaString.clear();
        }
        else {
            umaString += toupper(c);
        }
    }

    return substrings;
}