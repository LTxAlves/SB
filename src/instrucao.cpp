#include "instrucao.h"

Instrucao::Instrucao(string mnemonico, int numOperandos, int cod, int tam) {

    this->mnemonico = mnemonico;
    this->numOperandos = numOperandos;
    this->cod = cod;
    this->tam = tam;
}

string Instrucao::getMnemonico() {

    return mnemonico;
}

int Instrucao::getNumOperandos() {

    return numOperandos;
}

int Instrucao::getCod() {

    return cod;
}

int Instrucao::getTam() {

    return tam;
}

bool Instrucao::operator<(const Instrucao& inst) {

    return this->mnemonico < inst.mnemonico;
}