#include <gtk/gtk.h>
#include <stdlib.h>
#include "expression_notation.h"
#include "process_scheduler.h"
#include "processor.h"
#include "process.h"
#include "process_data_dependency.h"

static GtkWidget* window;
static GtkBuilder* builder;
static GtkCssProvider* cssProvider;

struct SimulatorUIController{
    GtkScrolledWindow* processListWindow;
    GtkListBox* processList;
}uiController;

static ProcessScheduler* simulatorProcessScheduler;
static int processorAmount = 0;

void resetSimulator();
void initializeSimulator();
void startSimulator();

void resetSimulator(){
    free(simulatorProcessScheduler);
    processorAmount = 0;

    initializeSimulator();
}

void initializeSimulator(){
    uiController.processListWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "processListWindow"));
    uiController.processList = GTK_LIST_BOX(gtk_builder_get_object(builder, "processList"));

    simulatorProcessScheduler = createProcessScheduler();
    for (int i = 0; i < 20; i++){
        gtk_list_box_append(uiController.processList, gtk_button_new_with_label("hewwo"));
    }
}

static void activate(GtkApplication* app, gpointer user_data){
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Shortest Remaining Time First simulator");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "simulator_ui.ui", NULL);
    GObject* windowContent = gtk_builder_get_object(builder, "windowContent");
    gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(windowContent));
    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "simulator_ui.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    initializeSimulator();

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv){
    GtkApplication* app;
    int status;

    app = gtk_application_new("com.uwu-programming.uwu_SRTF_simulation", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}