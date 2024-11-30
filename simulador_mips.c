#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_REGISTROS 32
#define ARQUIVO_REGISTRADORES "registradores.bin"

int registradores[NUM_REGISTROS];

void salvar_registradores() {
    FILE *arquivo = fopen(ARQUIVO_REGISTRADORES, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para salvar os registradores");
        exit(1);
    }
    fwrite(registradores, sizeof(int), NUM_REGISTROS, arquivo);
    fclose(arquivo);
}

void carregar_registradores() {
    FILE *arquivo = fopen(ARQUIVO_REGISTRADORES, "rb");
    if (arquivo != NULL) {
        fread(registradores, sizeof(int), NUM_REGISTROS, arquivo);
        fclose(arquivo);
    } else {
        for (int i = 0; i < NUM_REGISTROS; i++) {
            registradores[i] = 0;
        }
    }
}

void define_valor_registrador(int indice, int valor) {
    if (indice < 0 || indice >= NUM_REGISTROS) {
        printf("Erro: índice de registrador inválido (R%d).\n", indice);
        return;
    }
    registradores[indice] = valor;
    printf("Registrador R%d configurado com o valor %d.\n", indice, valor);
}

void limpar_registradores() {
    for (int i = 0; i < NUM_REGISTROS; i++) {
        registradores[i] = 0;
    }
    printf("Todos os registradores foram limpos.\n");
}


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

void executa_j(Instrucao instrucao) {
    printf("Instrução de salto para o endereço %d\n", instrucao.formato_J.endereco);
}

void exibe_instrucao(Instrucao instrucao, const char* tipo) {
    printf("\nFormato da Instrução (%s):\n", tipo);
    if (strcmp(tipo, "R") == 0) {
        printf("opcode: %u, rs: %u, rt: %u, rd: %u, shamt: %u, funct: %u\n",
            instrucao.formato_R.opcode, instrucao.formato_R.rs, instrucao.formato_R.rt,
            instrucao.formato_R.rd, instrucao.formato_R.shamt, instrucao.formato_R.funct);
    } else if (strcmp(tipo, "I") == 0) {
        printf("opcode: %u, rs: %u, rt: %u, imediato: %u\n",
            instrucao.formato_I.opcode, instrucao.formato_I.rs, instrucao.formato_I.rt,
            instrucao.formato_I.imediato);
    } else if (strcmp(tipo, "J") == 0) {
        printf("opcode: %u, endereco: %u\n",
            instrucao.formato_J.opcode, instrucao.formato_J.endereco);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <instrução> [argumentos]\n", argv[0]);
        return 1;
    }

    carregar_registradores();

    Instrucao instrucao;

    if (strcmp(argv[1], "clear") == 0) {
        limpar_registradores();
    }

    else if (strcmp(argv[1], "set") == 0) {
        if (argc < 4) {
            printf("Uso: %s set <registrador> <valor>\n", argv[0]);
            return 1;
        }
        int reg = atoi(argv[2]); 
        int valor = atoi(argv[3]); 
        define_valor_registrador(reg, valor);
    }

    else if (strcmp(argv[1], "add") == 0) {
        instrucao.formato_R.opcode = 0x00;
        instrucao.formato_R.funct = 0x20;
        instrucao.formato_R.rs = atoi(argv[2]);
        instrucao.formato_R.rt = atoi(argv[3]);
        instrucao.formato_R.rd = atoi(argv[4]);
        executa_add(instrucao);
        exibe_instrucao(instrucao, "R");
    } 

    else if (strcmp(argv[1], "addi") == 0) {
        instrucao.formato_I.opcode = 0x08;
        instrucao.formato_I.rs = atoi(argv[2]);
        instrucao.formato_I.rt = atoi(argv[3]);
        instrucao.formato_I.imediato = atoi(argv[4]);
        executa_addi(instrucao);
        exibe_instrucao(instrucao, "I");
    } 

    else if (strcmp(argv[1], "beq") == 0) {
        instrucao.formato_I.opcode = 0x04;
        instrucao.formato_I.rs = atoi(argv[2]);
        instrucao.formato_I.rt = atoi(argv[3]);
        instrucao.formato_I.imediato = atoi(argv[4]);
        executa_beq(instrucao);
        exibe_instrucao(instrucao, "I");
    }

    else if (strcmp(argv[1], "j") == 0) {
        instrucao.formato_J.opcode = 0x02;
        instrucao.formato_J.endereco = atoi(argv[2]);
        executa_j(instrucao);
        exibe_instrucao(instrucao, "J");
    }
    else {
        printf("Instrução desconhecida\n");
        return 1;
    }

    salvar_registradores();

    printf("\nEstado dos registradores:\n");
    for (int i = 0; i < NUM_REGISTROS; i++) {
        printf("R%d: %d\n", i, registradores[i]);
    }

    return 0;
}
