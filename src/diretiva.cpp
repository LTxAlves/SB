#include "diretiva.h"

Diretiva::Diretiva(string mnemonico, int numOperandos, int tam) {

    this->mnemonico = mnemonico;
    this->numOperandos = numOperandos;
    this->tam = tam;
}

string Diretiva::getMnemonico() {

    return mnemonico;
}

int Diretiva::getNumOperandos() {

    return numOperandos;
}

int Diretiva::getTam() {

    return tam;
}

bool Diretiva::operator<(const Diretiva& dir) {

    return this->mnemonico < dir.mnemonico;
}
