#ifndef CSV_OPERATIONS_H
#define CSV_OPERATIONS_H

#include <stdbool.h>

#define MAX_TAM_SALA 20
#define TAM_DATA 20
#define TAM_HORARIO 10
#define TAM_LINHA 256
#define MAX_TAM_INFO 50
#define NUM_SALAS 10
#define NUM_HORARIOS 19

extern char *horarios[];

// Database functions
int init_database();
void close_database();

int pegar_indice_horario(const char *horario);
void pegar_data_atual(char *data);

// Room booking functions
bool verificar_registro(const char *bloco, const char *sala, const char *data,
                       const char *horario_inicio, const char *horario_fim);
int agendar_horario_sala(const char *bloco, const char *sala, const char *data,
                        const char *horario_inicio, const char *horario_fim,
                        const char *professor, const char *disciplina, const char *turma);
void apagar_reserva(const char *bloco, const char *sala, const char *data,
                   const char *horario_inicio, const char *horario_fim);
void carregar_reservas(const char *bloco, char *reservas[NUM_HORARIOS], char *sala, char *data);
void liberar_reservas(char *reservas[NUM_HORARIOS]);
void trim(char *str);

// User authentication functions
int cadastrar_usuario(const char *login, const char senha[MAX_TAM_INFO]);
int autenticar_usuario(const char *login, const char *senha);
void escrever_ultimo_login(const char *login, const char *senha);
void ler_ultimo_login(char *login, char *senha);
char lembrar_login_cond();
void set_lembrar_login(int value);

#endif /* CSV_OPERATIONS_H */