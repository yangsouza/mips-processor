#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_REGISTROS 32

int registradores[NUM_REGISTROS];

typedef union {
    struct {
        unsigned int opcode : 6;
        unsigned int rs : 5;
        unsigned int rt : 5;
        unsigned int rd : 5;
        unsigned int shamt : 5;
        unsigned int funct : 6;
    } formato_R;
    
    struct {
        unsigned int opcode : 6;
        unsigned int rs : 5;
        unsigned int rt : 5;
        unsigned int imediato : 16;
    } formato_I;
    
    struct {
        unsigned int opcode : 6;
        unsigned int endereco : 26;
    } formato_J;

    unsigned int instrucao_binaria;
} Instrucao;

void inicializa_registradores() {
    for (int i = 0; i < NUM_REGISTROS; i++) {
        registradores[i] = 0;
    }
}

void executa_add(Instrucao instrucao) {
    registradores[instrucao.formato_R.rd] = registradores[instrucao.formato_R.rs] + registradores[instrucao.formato_R.rt];
}

void executa_addi(Instrucao instrucao) {
    registradores[instrucao.formato_I.rt] = registradores[instrucao.formato_I.rs] + instrucao.formato_I.imediato;
}

void executa_beq(Instrucao instrucao) {
    if (registradores[instrucao.formato_I.rs] == registradores[instrucao.formato_I.rt]) {
        printf("Desvio para %d\n", instrucao.formato_I.imediato);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <instrução> [argumentos]\n", argv[0]);
        return 1;
    }

    inicializa_registradores();

    Instrucao instrucao;

    if (strcmp(argv[1], "add") == 0) {
        instrucao.formato_R.opcode = 0x00; 
        instrucao.formato_R.funct = 0x20; 
        instrucao.formato_R.rs = atoi(argv[2]);
        instrucao.formato_R.rt = atoi(argv[3]);
        instrucao.formato_R.rd = atoi(argv[4]);
        executa_add(instrucao);
    } 
    else if (strcmp(argv[1], "addi") == 0) {
        instrucao.formato_I.opcode = 0x08;
        instrucao.formato_I.rs = atoi(argv[2]);
        instrucao.formato_I.rt = atoi(argv[3]);
        instrucao.formato_I.imediato = atoi(argv[4]);
        executa_addi(instrucao);
    } 
    else if (strcmp(argv[1], "beq") == 0) {
        instrucao.formato_I.opcode = 0x04;
        instrucao.formato_I.rs = atoi(argv[2]);
        instrucao.formato_I.rt = atoi(argv[3]);
        instrucao.formato_I.imediato = atoi(argv[4]);
        executa_beq(instrucao);
    } 
    else {
        printf("Instrução desconhecida\n");
        return 1;
    }

    printf("Estado dos registradores:\n");
    for (int i = 0; i < NUM_REGISTROS; i++) {
        printf("R%d: %d\n", i, registradores[i]);
    }

    return 0;
}
