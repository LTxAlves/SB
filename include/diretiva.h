#pragma once

#include <string>
#include <iostream>
#include <iomanip>

class Diretiva {

    public:
        Diretiva(std::string, int, int);
        std::string getMnemonico();
        int getNumOperandos();
        int getTam();

        bool operator<(const Diretiva&);
        friend std::ostream& operator<<(std::ostream& os, const Diretiva& dir) {

            int w = 10;

            os << "Mnemônico: " << std::setw(w)  << dir.mnemonico
            <<  " Operandos: " << std::setw(w) << dir.numOperandos
            <<  " Tamanho: " << std::setw(w) << dir.tam;

            return os; 
        }

    private:
        std::string  mnemonico;
        int     numOperandos;
        int     tam;
};