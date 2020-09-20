#pragma once

#include <string>
#include <iostream>
#include <iomanip>

using std::string;

using std::ostream;
using std::cout;

using std::setw;

class Instrucao {

    public:
        Instrucao(string, int, int, int);
        string getMnemonico();
        int getNumOperandos();
        int getCod();
        int getTam();

        bool operator<(const Instrucao &);
        friend ostream& operator<<(ostream& os, const Instrucao &inst) {

            int w = 10;

            os << "Mnemônico: " << setw(w) << inst.mnemonico
            <<  " Operandos: " << setw(w) << inst.numOperandos
            << " Código: " << setw(w) << inst.cod
            <<  " Tamanho: " << setw(w) << inst.tam;

            return os; 
        }

    private:
        string  mnemonico;
        int     numOperandos;
        int     cod;
        int     tam;
};