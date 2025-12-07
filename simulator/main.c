#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "expression_notation.h"
#include "process_scheduler.h"
#include "processor.h"
#include "process.h"
#include "process_data_dependency.h"

/*___________________________________________________________________*/
// Gtk UI
static GtkWidget* window;
static GtkBuilder* builder;
static GtkCssProvider* cssProvider;

struct SimulatorUIController{
    GtkScrolledWindow* processListWindow;
    GtkListBox* processList;
}uiController;
/*___________________________________________________________________*/

static ProcessScheduler* simulatorProcessScheduler;
static int processorAmount = 0;

void resetSimulator();
void initializeSimulator();
void startSimulator();
void simulatorAddProcess(ProcessScheduler* processScheduler, expression infixExpression);

void updateProcessList();

/*___________________________________________________________________*/
// signal
// addProcessButton
static GtkWidget* addProcessWindow;

void focusMainWindow(){
    gtk_widget_set_sensitive(window, TRUE);
}

void addProcessWindowAddProcess(GtkWidget* widget, gpointer entry){
    printf("addprocesswindowaddprocess: %s\n", gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry)));

    const char* collectedEntryValue = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
    int collectedEntryValueLength = gtk_entry_get_text_length(entry);

    expression infixExpression = malloc(sizeof(char) * (collectedEntryValueLength + 1));
    for (int i = 0; i < collectedEntryValueLength; i++){
        infixExpression[i] = collectedEntryValue[i];
    }
    infixExpression[collectedEntryValueLength] = '\0';

    simulatorAddProcess(simulatorProcessScheduler, infixExpression);

    gtk_window_close(GTK_WINDOW(addProcessWindow));
}

void closeAddProcessWindow(){
    gtk_window_close(GTK_WINDOW(addProcessWindow));
}

G_MODULE_EXPORT void addProcessButton__clicked(GtkButton* button, gpointer data){
    gtk_widget_set_sensitive(window, FALSE);

    addProcessWindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(addProcessWindow), "Add process");
    gtk_window_set_default_size(GTK_WINDOW(addProcessWindow), 500, 300);
    gtk_window_set_resizable(GTK_WINDOW(addProcessWindow), FALSE);
    g_signal_connect(addProcessWindow, "close-request", G_CALLBACK(focusMainWindow), NULL);

    GtkLabel* label = GTK_LABEL(gtk_label_new("Enter an expression:"));
    GtkEntry* entry = GTK_ENTRY(gtk_entry_new());
    GtkButton* addButton = GTK_BUTTON(gtk_button_new_with_label("Add process"));
    GtkButton* cancelButton = GTK_BUTTON(gtk_button_new_with_label("Cancel"));

    g_signal_connect(GTK_WIDGET(addButton), "clicked", G_CALLBACK(addProcessWindowAddProcess), entry);
    g_signal_connect(GTK_WIDGET(cancelButton), "clicked", G_CALLBACK(closeAddProcessWindow), NULL);

    GtkBox* addProcessWindowContent = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkBox* entrySection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    GtkBox* buttonSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24));

    gtk_widget_set_halign(GTK_WIDGET(entrySection), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(entrySection), GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(GTK_WIDGET(entrySection), TRUE);
    gtk_widget_set_halign(GTK_WIDGET(buttonSection), GTK_ALIGN_END);

    gtk_editable_set_max_width_chars(GTK_EDITABLE(entry), 40);

    gtk_box_append(addProcessWindowContent, GTK_WIDGET(entrySection));
    gtk_box_append(addProcessWindowContent, GTK_WIDGET(buttonSection));

    gtk_box_append(entrySection, GTK_WIDGET(label));
    gtk_box_append(entrySection, GTK_WIDGET(entry));

    gtk_box_append(buttonSection, GTK_WIDGET(addButton));
    gtk_box_append(buttonSection, GTK_WIDGET(cancelButton));

    gtk_widget_add_css_class(GTK_WIDGET(buttonSection), "processListWindow");
    gtk_widget_add_css_class(GTK_WIDGET(entrySection), "processSection");
    gtk_widget_add_css_class(GTK_WIDGET(addProcessWindowContent), "windowContent");

    gtk_window_set_child(GTK_WINDOW(addProcessWindow), GTK_WIDGET(addProcessWindowContent));
    gtk_window_present(GTK_WINDOW(addProcessWindow));
}
/*___________________________________________________________________*/

void resetSimulator(){
    free(simulatorProcessScheduler);
    processorAmount = 0;

    initializeSimulator();
}

void initializeSimulator(){
    uiController.processListWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "processListWindow"));
    uiController.processList = GTK_LIST_BOX(gtk_builder_get_object(builder, "processList"));

    simulatorProcessScheduler = createProcessScheduler();
}

void simulatorAddProcess(ProcessScheduler* processScheduler, expression infixExpression){
    addProcess(processScheduler, infixExpression);

    updateProcessList();
}

void updateProcessList(){
    gtk_list_box_remove_all(uiController.processList);
    
    void* dummyCollector = simulatorProcessScheduler -> processList;
    while(((Node*)dummyCollector) -> next != NULL){
        dummyCollector = ((Node*)dummyCollector) -> next;
        gtk_list_box_append(uiController.processList, gtk_label_new((*((Process**)(((Node*)dummyCollector) -> data))) -> dependencyInformation -> infixExpression));
        printf("%s\n", (*((Process**)(((Node*)dummyCollector) -> data))) -> dependencyInformation -> infixExpression);
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