#ifndef CSV_OPERATIONS_H
#define CSV_OPERATIONS_H

#define NUM_HORARIOS 19
#define NUM_SALAS 20
#define TAM_DATA 20
#define TAM_HORARIO 6
#define TAM_LINHA 256
#define MAX_TAM_SALA 6
#define MAX_TAM_INFO 30

#include <stdbool.h>

extern char *horarios[];

typedef struct Registro_aula{
    char sala[MAX_TAM_SALA];
    char data[TAM_DATA];
    char professor[MAX_TAM_INFO];
    char disciplina[MAX_TAM_INFO];
    char turma[MAX_TAM_INFO];
    char horario_inicio[TAM_HORARIO];
    char horario_fim[TAM_HORARIO];
} registro_aula;

void pegar_data_atual(char *data);

int pegar_indice_horario(const char *horario);

bool verificar_registro(const char *filename, const char *sala, const char *data, const char *horario_inicio, const char *horario_fim);

int agendar_horario_sala(const char *filename, const char *sala, const char *data, const char *horario_inicio, const char *horario_fim, const char *professor, const char *disciplina, const char *turma);

void apagar_reserva(const char *filename, const char *sala, const char *data, const char *horario_inicio, const char *horario_fim);

void pegar_nome_arquivo(char *nome_arquivo, const char *bloco);

void trim(char *str);

void carregar_reservas(const char *filename, char *reservas[19], char *sala, char *data);

void liberar_reservas(char *reservas[19]); 

int cadastrar_usuario(const char *filename, const char login[20], const char senha[40]);

int autenticar_usuario(const char *filename, const char *login, const char *senha);

#endif // CSV_OPERATIONS_H
