#ifndef CSV_OPERATIONS_H
#define CSV_OPERATIONS_H

#include <stdbool.h>


void pegar_data_atual(char *data);

int pegar_indice_horario(const char *horario);

bool verificar_registro(const char *filename, const char *sala, const char *data, const char *horario_inicio, const char *horario_fim);

int agendar_horario_sala(const char *filename, const char *sala, const char *data, const char *horario_inicio, const char *horario_fim);

void apagar_reserva(const char *filename, const char *sala, const char *data, const char *horario_inicio, const char *horario_fim);

void pegar_nome_arquivo(char *nome_arquivo, const char *bloco);

void trim(char *str);

void carregar_reservas(const char *filename, char *reservas[19], char *sala, char *data);

void liberar_reservas(char *reservas[19]); 

int cadastrar_usuario(const char *filename, const char login[20], const char senha[40]);

int autenticar_usuario(const char *filename, const char *login, const char *senha);

#endif // CSV_OPERATIONS_H
