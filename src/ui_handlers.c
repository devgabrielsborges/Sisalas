#include <gtk/gtk.h>
#include "db_operations.h"

int autenticado = 0;

void on_lembrar_login_toggled(GtkToggleButton *toggle)
{
    // Use the SQLite function instead of file operation
    set_lembrar_login(gtk_toggle_button_get_active(toggle));
}

void exibir_mensagem(GtkBuilder *builder, const char *mensagem) {
    // Obtém o GtkMessageDialog do arquivo Glade
    GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object(builder, "message_padrao"));

    if (!dialog) {
        g_critical("Erro: Não foi possível encontrar o GtkMessageDialog no Glade.");
        return;
    }

    // Define o texto da mensagem
    GtkWidget *label = GTK_WIDGET(gtk_builder_get_object(builder, "label_mensagem"));
    if (label) {
        gtk_label_set_text(GTK_LABEL(label), mensagem);
    } else {
        g_critical("Erro: Não foi possível encontrar o rótulo da mensagem.");
    }

    // Exibe a caixa de diálogo
    gtk_widget_show(dialog);
}

void on_ok_clicked(GtkWidget *button, gpointer user_data)
{
    // Obtém o topo da hierarquia, que é o GtkDialog
    GtkWidget *dialog = gtk_widget_get_toplevel(button);

    if (GTK_IS_DIALOG(dialog)) {
        gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK); // Finaliza o ciclo do diálogo
        gtk_widget_hide(dialog); // Oculta o diálogo
    }
}

void on_botao_voltar_clicked(GtkWidget *button, gpointer user_data) {
    // Obtém o GtkStack do GtkBuilder
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

    gtk_stack_set_visible_child_name(stack, "box_login");
    autenticado = 0;
}

void popular_lista_reservas(GtkBuilder *builder, const char *bloco, char *sala, char *data) {
    char *reservas[NUM_HORARIOS] = {NULL};

    carregar_reservas(bloco, reservas, sala, data);

    GtkListBox *lista = GTK_LIST_BOX(gtk_builder_get_object(builder, "lista_reservas"));
    if (!lista) {
        g_critical("Falha ao obter a lista");
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(lista), (GtkCallback)gtk_widget_destroy, NULL);

    // Populando a lista com verificações
    for (int i = 0; i < NUM_HORARIOS; i++) {
        if (reservas[i] != NULL && strlen(reservas[i]) > 0 && g_strstrip(g_strdup(reservas[i]))[0] != '\0') {
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *label = gtk_label_new(reservas[i]);
            gtk_widget_set_name(label, "label_reserva");
            gtk_style_context_add_class(gtk_widget_get_style_context(label), "label-lista");
            gtk_container_add(GTK_CONTAINER(row), label);
            gtk_list_box_insert(lista, row, -1);
        }
    }

    gtk_widget_show_all(GTK_WIDGET(lista));
    liberar_reservas(reservas);
}

// Tela login
void on_botao_login_clicked(GtkButton *b, gpointer user_data) {
    g_print("Login button clicked\n");

    // Definindo as entradas
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkEntry *entry_login = GTK_ENTRY(gtk_builder_get_object(builder, "entry_login"));
    GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "entry_senha"));

    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(GTK_BUILDER(builder), "entry_data"));
    GtkCalendar *calendar = GTK_CALENDAR(gtk_builder_get_object(GTK_BUILDER(builder), "calendar"));

    if (!entry_login || !entry_senha) {
        g_critical("Falha ao obter as entradas");
        return;
    }

    const char *login = gtk_entry_get_text(entry_login);
    const char *senha = gtk_entry_get_text(entry_senha);

    // se login e senha forem diferentes de "" -> chamar autenticar_usuario
    if (strcmp(login, "") != 0 && strcmp(senha, "") != 0) {
        int resultado = autenticar_usuario(login, senha);
        if (resultado == 0) {
            autenticado = 1;
            exibir_mensagem(builder, "Login realizado com sucesso!\n");
        } else if (resultado == 1) {
            autenticado = 0;
            exibir_mensagem(builder, "Login ou senha incorretos.\nTente novamente");
        } else {
            autenticado = 0;
            exibir_mensagem(builder, "Erro ao autenticar usuário");
        }
    } else {
        exibir_mensagem(builder, "Preencha o login e a senha!");
    }

    if (autenticado == 1) {
        // Exibir a box denominada box_tela_geral na GtkStack
        GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
        if (!stack) {
            g_critical("Falha ao obter a GtkStack");
            return;
        }

        // se o toggle ativo -> salvar login e senha no arquivo .ultimo_login.csv
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "toggle_lembrar_login"))) == TRUE)
            escrever_ultimo_login(login, senha);

        char data_atual[TAM_DATA];
        pegar_data_atual(data_atual);
        g_print("Data atual: %s\n", data_atual);
        gtk_entry_set_text(entry_data, data_atual);

        // Setando a data atual no calendário
        guint dia, mes, ano;
        sscanf(data_atual, "%02d/%02d/%04d", &dia, &mes, &ano);
        gtk_calendar_select_day(calendar, dia);
        gtk_calendar_select_month(calendar, mes - 1, ano);

        gtk_stack_set_visible_child_name(stack, "box_tela_geral");
    }
}

void on_botao_novo_usuario_clicked(GtkButton *b, gpointer user_data) {
    g_print("New user button clicked\n");

    // Passando para tela de cadastro de usuário
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
        if (!stack) {
        g_critical("Falha ao obter a GtkStack");
        return;
    }
    gtk_stack_set_visible_child_name(stack, "box_cadastro_usuario");
}

void on_botao_cadastrar_usuario_clicked(GtkButton *b, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

     // Definindo as entradas
    GtkEntry *entry_login = GTK_ENTRY(gtk_builder_get_object(builder, "entry_inserir_nome"));
    GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "entry_inserir_senha"));
    GtkEntry *entry_confirmar_senha = GTK_ENTRY(gtk_builder_get_object(builder, "entry_confirmar_senha"));

    if (!entry_login || !entry_senha || !entry_confirmar_senha) {
        g_critical("Falha ao obter as entradas");
        return;
    }

    const char *login = gtk_entry_get_text(entry_login);
    const char *senha = gtk_entry_get_text(entry_senha);
    const char *confirmar_senha = gtk_entry_get_text(entry_confirmar_senha);

    if (strcmp(login, "") != 0 && strcmp(senha, "") != 0 && strcmp(confirmar_senha, "") != 0) {
        if (strcmp(senha, confirmar_senha) == 0) {
            int resultado = cadastrar_usuario(login, senha);
            if (resultado == 0) {
                exibir_mensagem(builder, "Usuário cadastrado com sucesso");
                gtk_stack_set_visible_child_name(stack, "box_login");
            } else if (resultado == 1) {
                exibir_mensagem(builder, "Usuário já cadastrado");
            } else {
                exibir_mensagem(builder, "Erro ao cadastrar usuário");
            }
        } else {
            exibir_mensagem(builder, "As senhas não conferem");
        }
    }
    else {
        exibir_mensagem(builder, "Preencha todos os campos");
    }

    if (!stack) {
        g_critical("Falha ao obter a GtkStack");
        return;
    }
}

void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(GTK_BUILDER(builder), "entry_data"));

    guint ano, mes, dia;
    gchar texto_data[TAM_DATA];

    // Pegando a data selecionada no calendário
    gtk_calendar_get_date(calendar, &ano, &mes, &dia);

    // Passando data para a entry
    g_snprintf(texto_data, sizeof(texto_data), "%02d/%02d/%04d", dia, mes + 1, ano);

    // Setando a data na entry
    gtk_entry_set_text(entry_data, texto_data);

    // Atualizando a lista de reservas - se houver bloco e sala selecionados
    GtkComboBoxText *bloco_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "bloco_combobox"));
    GtkComboBoxText *sala_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "sala_combobox"));

    char *bloco = gtk_combo_box_text_get_active_text(bloco_combobox);
    char *sala = gtk_combo_box_text_get_active_text(sala_combobox);

    if (bloco && sala) {
        popular_lista_reservas(builder, bloco, sala, texto_data);
    }
}

void on_entry_data_activate(GtkEntry *entry_data, gpointer user_data) {
    const char *texto_data = gtk_entry_get_text(entry_data);
    guint ano, mes, dia;

    // Tenta analisar a data no formato DD/MM/YYYY
    if (sscanf(texto_data, "%02d/%02d/%04d", &dia, &mes, &ano) == 3) {
        // Obtém o GtkCalendar do GtkBuilder (certifique-se de passar o GtkBuilder correto)
        GtkBuilder *builder = GTK_BUILDER(user_data);
        GtkCalendar *calendar = GTK_CALENDAR(gtk_builder_get_object(builder, "calendar"));

        if (calendar) {
            // Ajusta os valores conforme esperado pelo GTK (mês baseado em 0)
            gtk_calendar_select_month(calendar, mes - 1, ano);
            gtk_calendar_select_day(calendar, dia);
        } else {
            g_warning("Não foi possível obter o calendário");
        }
    }
    else {
        g_warning("Data inválida: %s", texto_data);
    }
}

void atualizar_sala_combobox(GtkComboBoxText *sala, const char *bloco) {
    GtkComboBoxText *sala_combobox = GTK_COMBO_BOX_TEXT(sala);
    char salas[NUM_SALAS][MAX_TAM_SALA];

    // Obtendo as salas do bloco
    for (int i = 0; i < NUM_SALAS; i++) {
        snprintf(salas[i], sizeof(salas[i]), "%s%02d", bloco, i + 1);
    }

    // Limpa o combobox
    gtk_combo_box_text_remove_all(sala_combobox);

    // Adiciona os itens
    for (int i = 0; i < NUM_SALAS; i++) {
        gtk_combo_box_text_append_text(sala_combobox, salas[i]);
    }
}

void on_bloco_combobox_changed(GtkComboBox *bloco_combobox, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkComboBoxText *sala_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "sala_combobox"));

    // Obtendo o bloco selecionado
    const char *bloco = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bloco_combobox));

    // Limpar itens do combobox
    gtk_combo_box_text_remove_all(sala_combobox);

     // Atualiza os itens de combo2 com base no item selecionado em combo1
    atualizar_sala_combobox(sala_combobox, bloco);
}

void on_sala_combobox_changed(GtkComboBoxText *sala_combobox, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkComboBoxText *bloco_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "bloco_combobox"));

    // Texto do bloco
    char *bloco = gtk_combo_box_text_get_active_text(bloco_combobox);

    // Texto do sala_combobox
    char *sala = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(sala_combobox));

    // Texto da entry_data
    const char *data = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entry_data")));

    if (sala && bloco && data) {
        popular_lista_reservas(builder, bloco, sala,(char*) data);
    }
}

// Array para armazenar os índices
int indices_linhas[2];
int indice_inicio, indice_fim;

void on_lista_reservas_row_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data) {
    int index = gtk_list_box_row_get_index(row);
    g_message("Linha selecionada: %d", index);

    // Update indices_linhas based on the selected row
    if (indices_linhas[0] == -1) {
        indices_linhas[0] = index;
    } else if (indices_linhas[1] == -1) {
        indices_linhas[1] = index;
    } else {
        indices_linhas[0] = index;
        indices_linhas[1] = -1;
    }
    indice_inicio = indices_linhas[0];
    indice_fim = indices_linhas[1] + 1;

    g_message("Índices atualizados: %d, %d", indices_linhas[0], indices_linhas[1]);
}

void on_botao_inserir_reserva_clicked(GtkButton *b, int *indices_linhas, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkComboBoxText *bloco_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "bloco_combobox"));
    GtkComboBoxText *sala_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "sala_combobox"));
    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(builder, "entry_data"));

    const char *bloco = gtk_combo_box_text_get_active_text(bloco_combobox);
    const char *sala = gtk_combo_box_text_get_active_text(sala_combobox);
    const char *data = gtk_entry_get_text(entry_data);

    if (!bloco || !sala || !data) {
        exibir_mensagem(builder, "Preencha todos os campos");
        return;
    }

    if (indices_linhas[0] == -1 || indices_linhas[1] == -1) {
        exibir_mensagem(builder, "Selecione dois horários");
        return;
    }

    char horario_inicio[TAM_HORARIO];
    char horario_fim[TAM_HORARIO];

    g_message("Indice inicio: %d\nIndice fim: %d", indice_inicio, indice_fim);
    g_message("Horario inicio: %s\nHorario fim: %s", horarios[indice_inicio], horarios[indice_fim]);

    snprintf(horario_inicio, sizeof(horario_inicio), "%s", horarios[indice_inicio]);
    snprintf(horario_fim, sizeof(horario_fim), "%s", horarios[indice_fim]);

    if (verificar_registro(bloco, sala, data, horario_inicio, horario_fim)) {
        exibir_mensagem(builder, "Você está editando uma reserva existente");
    }

    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    if (!stack) {
        g_critical("Falha ao obter a GtkStack");
        return;
    }

    gtk_stack_set_visible_child_name(stack, "box_form");
}

void on_botao_cancelar_reserva_clicked(GtkButton *b, int *indices_linhas, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

    // Pegando bloco, sala, data, horario_inicio, horario_fim
    GtkComboBoxText *bloco_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "bloco_combobox"));
    GtkComboBoxText *sala_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "sala_combobox"));
    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(builder, "entry_data"));

    const char *bloco = gtk_combo_box_text_get_active_text(bloco_combobox);
    const char *sala = gtk_combo_box_text_get_active_text(sala_combobox);
    const char *data = gtk_entry_get_text(entry_data);

    if (!bloco || !sala || !data) {
        exibir_mensagem(builder, "Preencha todos os campos");
        return;
    }

    if (indices_linhas[0] == -1 || indices_linhas[1] == -1) {
        exibir_mensagem(builder, "Selecione dois horários");
        return;
    }

    char horario_inicio[TAM_HORARIO];
    char horario_fim[TAM_HORARIO];

    snprintf(horario_inicio, sizeof(horario_inicio), "%s", horarios[indice_inicio]);
    snprintf(horario_fim, sizeof(horario_fim), "%s", horarios[indice_fim]);

    if (verificar_registro(bloco, sala, data, horario_inicio, horario_fim)) {
        apagar_reserva(bloco, sala, data, horario_inicio, horario_fim);
        exibir_mensagem(builder, "Reserva cancelada com sucesso");
        popular_lista_reservas(builder, bloco, (char*)sala, (char*)data);
    } else {
        exibir_mensagem(builder, "Registro não encontrado");
    }
}

void on_botao_reservar_clicked(GtkButton *b, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    GtkComboBoxText *bloco_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "bloco_combobox"));
    GtkComboBoxText *sala_combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "sala_combobox"));
    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(builder, "entry_data"));
    GtkEntry *entry_professor = GTK_ENTRY(gtk_builder_get_object(builder, "entry_professor"));
    GtkEntry *entry_disciplina = GTK_ENTRY(gtk_builder_get_object(builder, "entry_disciplina"));
    GtkEntry *entry_turma = GTK_ENTRY(gtk_builder_get_object(builder, "entry_turma"));

    const char *bloco = gtk_combo_box_text_get_active_text(bloco_combobox);
    const char *sala = gtk_combo_box_text_get_active_text(sala_combobox);
    const char *data = gtk_entry_get_text(entry_data);
    const char *professor = gtk_entry_get_text(entry_professor);
    const char *disciplina = gtk_entry_get_text(entry_disciplina);
    const char *turma = gtk_entry_get_text(entry_turma);

    if (!bloco || !sala || !data || !professor || !disciplina || !turma) {
        exibir_mensagem(builder, "Preencha todos os campos");
        return;
    }

    char horario_inicio[TAM_HORARIO];
    char horario_fim[TAM_HORARIO];

    snprintf(horario_inicio, sizeof(horario_inicio), "%s", horarios[indice_inicio]);
    snprintf(horario_fim, sizeof(horario_fim), "%s", horarios[indice_fim]);

    // Sala, Data, Professor, Disciplina, Turma, Horario_inicio, Horario_fim
    if (agendar_horario_sala(bloco, sala, data, horario_inicio, horario_fim, professor, disciplina, turma) == 0) {
        exibir_mensagem(builder, "Reserva realizada com sucesso");
        // voltando para tela geral
        gtk_stack_set_visible_child_name(stack, "box_tela_geral");
    } else {
        exibir_mensagem(builder, "Erro ao reservar horário");
    }
}

void on_botao_cancelar_operacao_clicked(GtkButton *b, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    if (!stack) {
        g_critical("Falha ao obter a GtkStack");
        return;
    }

    gtk_stack_set_visible_child_name(stack, "box_tela_geral");
}