#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <unistd.h>
#include "db_operations.h"

char *horarios[] = {
    "07:10", "08:00", "08:50", "09:40", "10:30", "11:20", "12:10",
    "13:00", "13:50", "14:40", "15:30", "16:20", "17:10", "18:00",
    "18:50", "19:40", "20:30", "21:20", "22:10"
};

sqlite3 *db = NULL;

int init_database() {
    char db_path[PATH_MAX];

    // Get executable directory
    char exec_path[PATH_MAX];
    readlink("/proc/self/exe", exec_path, PATH_MAX);
    char* last_slash = strrchr(exec_path, '/');
    if (last_slash) *last_slash = '\0';

    // Create data directory
    sprintf(db_path, "%s/data", exec_path);
#ifdef _WIN32
    _mkdir(db_path);
#else
    mkdir(db_path, 0755);
#endif

    // Full path to database
    sprintf(db_path, "%s/data/booking.db", exec_path);
    printf("Using database at: %s\n", db_path);

    int rc = sqlite3_open(db_path, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Create tables if they don't exist
    char *sql = "CREATE TABLE IF NOT EXISTS room_bookings ("
                "sala TEXT, "
                "data TEXT, "
                "professor TEXT, "
                "disciplina TEXT, "
                "turma TEXT, "
                "horario_inicio TEXT, "
                "horario_fim TEXT"
                ");"

                "CREATE TABLE IF NOT EXISTS users ("
                "login TEXT PRIMARY KEY, "
                "senha TEXT NOT NULL"
                ");"

                "CREATE TABLE IF NOT EXISTS settings ("
                "key TEXT PRIMARY KEY, "
                "value TEXT NOT NULL"
                ");";

    char *err_msg = NULL;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

void close_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int pegar_indice_horario(const char *horario) {
    for (int i = 0; i < NUM_HORARIOS; i++) {
        if (strcmp(horarios[i], horario) == 0) {
            return i;
        }
    }
    return -1;  // Horário não encontrado
}

void pegar_data_atual(char *data) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(data, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

bool verificar_registro(const char *bloco, const char *sala, const char *data,
                       const char *horario_inicio, const char *horario_fim) {
    if (!db && init_database() != 0) {
        return false;
    }

    char *sql = "SELECT COUNT(*) FROM room_bookings WHERE sala = ? AND data = ? AND "
                "horario_inicio = ? AND horario_fim = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, sala, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, data, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, horario_inicio, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, horario_fim, -1, SQLITE_STATIC);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

int agendar_horario_sala(const char *bloco, const char *sala, const char *data,
                        const char *horario_inicio, const char *horario_fim,
                        const char *professor, const char *disciplina, const char *turma) {
    if (!db && init_database() != 0) {
        return 2;
    }

    char *sql = "INSERT INTO room_bookings (sala, data, professor, disciplina, turma, horario_inicio, horario_fim) "
                "VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 2;
    }

    sqlite3_bind_text(stmt, 1, sala, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, data, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, professor, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, disciplina, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, turma, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, horario_inicio, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, horario_fim, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
        return 2;
    }

    return 0;
}

void apagar_reserva(const char *bloco, const char *sala, const char *data,
                   const char *horario_inicio, const char *horario_fim) {
    if (!db && init_database() != 0) {
        return;
    }

    char *sql = "DELETE FROM room_bookings WHERE sala = ? AND data = ? AND "
                "horario_inicio = ? AND horario_fim = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, sala, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, data, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, horario_inicio, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, horario_fim, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Delete failed: %s\n", sqlite3_errmsg(db));
    }
}

void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

void carregar_reservas(const char *bloco, char *reservas[NUM_HORARIOS], char *sala, char *data) {
    // Initialize reservas array to NULL
    for (int i = 0; i < NUM_HORARIOS; i++) {
        reservas[i] = NULL;
    }

    if (!db && init_database() != 0) {
        return;
    }

    char *sql = "SELECT professor, disciplina, turma, horario_inicio, horario_fim "
                "FROM room_bookings WHERE sala = ? AND data = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, sala, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, data, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *professor = (const char*)sqlite3_column_text(stmt, 0);
        const char *disciplina = (const char*)sqlite3_column_text(stmt, 1);
        const char *turma = (const char*)sqlite3_column_text(stmt, 2);
        const char *horario_inicio = (const char*)sqlite3_column_text(stmt, 3);
        const char *horario_fim = (const char*)sqlite3_column_text(stmt, 4);

        int indice_inicio = pegar_indice_horario(horario_inicio);
        int indice_fim = pegar_indice_horario(horario_fim);

        if (indice_inicio != -1 && indice_fim != -1) {
            for (int i = indice_inicio; i < indice_fim; i++) {
                if (reservas[i] != NULL) {
                    free(reservas[i]);
                }
                reservas[i] = malloc(TAM_LINHA * sizeof(char));
                if (reservas[i] != NULL) {
                    snprintf(reservas[i], TAM_LINHA, "%s - %s - %s - %s",
                            horarios[i], professor, disciplina, turma);
                }
            }
        }
    }

    sqlite3_finalize(stmt);

    // Fill empty slots with just the time
    for (int i = 0; i < NUM_HORARIOS; i++) {
        if (reservas[i] == NULL) {
            reservas[i] = strdup(horarios[i]);
        }
    }
}

void liberar_reservas(char *reservas[NUM_HORARIOS]) {
    for (int i = 0; i < NUM_HORARIOS; i++) {
        if (reservas[i] != NULL) {
            free(reservas[i]);
        }
    }
}

int cadastrar_usuario(const char *login, const char senha[MAX_TAM_INFO]) {
    if (!db && init_database() != 0) {
        return 2;
    }

    // Check if user already exists
    char *sql_check = "SELECT COUNT(*) FROM users WHERE login = ?";
    sqlite3_stmt *stmt_check;

    int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 2;
    }

    sqlite3_bind_text(stmt_check, 1, login, -1, SQLITE_STATIC);

    int count = 0;
    if (sqlite3_step(stmt_check) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt_check, 0);
    }

    sqlite3_finalize(stmt_check);

    if (count > 0) {
        return 1; // User already exists
    }

    // Insert new user
    char *sql_insert = "INSERT INTO users (login, senha) VALUES (?, ?)";
    sqlite3_stmt *stmt_insert;

    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 2;
    }

    sqlite3_bind_text(stmt_insert, 1, login, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_insert, 2, senha, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt_insert);
    sqlite3_finalize(stmt_insert);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
        return 2;
    }

    return 0; // User registered successfully
}

int autenticar_usuario(const char *login, const char *senha) {
    if (!db && init_database() != 0) {
        return 2;
    }

    char *sql = "SELECT senha FROM users WHERE login = ?";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 2;
    }

    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        const char *db_senha = (const char*)sqlite3_column_text(stmt, 0);
        if (strcmp(senha, db_senha) == 0) {
            sqlite3_finalize(stmt);
            return 0; // Authentication successful
        }
    }

    sqlite3_finalize(stmt);
    return 1; // Authentication failed
}

void escrever_ultimo_login(const char *login, const char *senha) {
    if (!db && init_database() != 0) {
        return;
    }

    // Store the last login
    char *sql = "INSERT OR REPLACE INTO settings (key, value) VALUES ('ultimo_login', ?), "
                "('ultima_senha', ?)";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
    }
}

void ler_ultimo_login(char *login, char *senha) {
    if (!db && init_database() != 0) {
        return;
    }

    // Get the last login
    char *sql_login = "SELECT value FROM settings WHERE key = 'ultimo_login'";
    char *sql_senha = "SELECT value FROM settings WHERE key = 'ultima_senha'";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql_login, -1, &stmt, 0);

    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(login, (const char*)sqlite3_column_text(stmt, 0));
    } else {
        login[0] = '\0';
    }

    sqlite3_finalize(stmt);

    // Get the last password
    rc = sqlite3_prepare_v2(db, sql_senha, -1, &stmt, 0);

    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(senha, (const char*)sqlite3_column_text(stmt, 0));
    } else {
        senha[0] = '\0';
    }

    sqlite3_finalize(stmt);
}

char lembrar_login_cond() {
    if (!db && init_database() != 0) {
        return 0;
    }

    char *sql = "SELECT value FROM settings WHERE key = 'lembrar_login'";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *value = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        return value[0] - '0';
    }

    sqlite3_finalize(stmt);
    return 0;
}

void set_lembrar_login(int value) {
    if (!db && init_database() != 0) {
        return;
    }

    char str_value[2];
    sprintf(str_value, "%d", value ? 1 : 0);

    char *sql = "INSERT OR REPLACE INTO settings (key, value) VALUES ('lembrar_login', ?)";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, str_value, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
    }
}