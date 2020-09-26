#pragma once

#include <string>
#include <iostream>
#include <iomanip>

using std::string;

using std::ostream;
using std::cout;

using std::setw;

class Diretiva {

    public:
        Diretiva(string, int, int);
        string getMnemonico();
        int getNumOperandos();
        int getTam();

        bool operator<(const Diretiva&);
        friend ostream& operator<<(ostream& os, const Diretiva& dir) {

            int w = 10;

            os << "Mnemônico: " << setw(w)  << dir.mnemonico
            <<  " Operandos: " << setw(w) << dir.numOperandos
            <<  " Tamanho: " << setw(w) << dir.tam;

            return os; 
        }

    private:
        string  mnemonico;
        int     numOperandos;
        int     tam;
};