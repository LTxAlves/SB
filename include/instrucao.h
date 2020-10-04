#pragma once

#include <string>
#include <iostream>
#include <iomanip>

class Instrucao {

    public:
        Instrucao(std::string, int, int, int);
        std::string getMnemonico();
        int getNumOperandos();
        int getCod();
        int getTam();

        bool operator<(const Instrucao &);
        friend std::ostream& operator<<(std::ostream& os, const Instrucao &inst) {

            int w = 10;

            os << "Mnemônico: " << std::setw(w) << inst.mnemonico
            <<  " Operandos: " << std::setw(w) << inst.numOperandos
            << " Código: " << std::setw(w) << inst.cod
            <<  " Tamanho: " << std::setw(w) << inst.tam;

            return os; 
        }

    private:
        std::string  mnemonico;
        int     numOperandos;
        int     cod;
        int     tam;
};