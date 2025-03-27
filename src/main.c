#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "db_operations.h"

#define NUM_HORARIOS 19

GtkBuilder *builder;
GtkWidget *window;

int main(int argc, char *argv[]) {
    char *reservas[NUM_HORARIOS];

    gtk_init(&argc, &argv);

    // Initialize database
    if (init_database() != 0) {
        g_critical("Failed to initialize database");
        return 1;
    }

    builder = gtk_builder_new_from_file("interface.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    if (!window) {
        g_critical("Falha em obter main_window");
        return 1;
    }

    // Carregar o arquivo CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    if (!gtk_css_provider_load_from_path(css_provider, "style.css", NULL)) {
        g_warning("Não foi possível carregar o arquivo style.css");
    }

    // Aplicar o CSS globalmente
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    printf("Lembrar login: %d\n", lembrar_login_cond());
    if (lembrar_login_cond() == 1) {
        char login[MAX_TAM_INFO], senha[MAX_TAM_INFO];
        ler_ultimo_login(login, senha);

        GtkEntry *entry_login = GTK_ENTRY(gtk_builder_get_object(builder, "entry_login"));
        GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "entry_senha"));

        if (strlen(login) > 0 && strlen(senha) > 0) {
            gtk_entry_set_text(entry_login, login);
            gtk_entry_set_text(entry_senha, senha);
        }
    }

    gtk_builder_connect_signals(builder, builder);
    gtk_widget_show_all(window);
    gtk_main();

    // Close database connection before exiting
    close_database();

    return 0;
}