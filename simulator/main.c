#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "expression_notation.h"
#include "process_scheduler.h"
#include "processor.h"
#include "virtual_process.h"
#include "process_data_dependency.h"

/*___________________________________________________________________*/
// Gtk UI
static GtkWidget* window;
static GtkBuilder* builder;
static GtkCssProvider* cssProvider;

struct SimulatorUIController{
    GtkScrolledWindow* processListWindow;
    GtkListBox* processList;

    GtkBox* timeframeList;
    GtkBox* simulatorProcessorList;
    GtkBox* simulatorGanttChart;

    GtkLabel* numberOfProcessorLabel;
    GtkLabel* currentTimeframeLabel;

    GtkLabel* averageResponseTimeLabel;
    GtkLabel* averageWaitingTimeLabel;
    GtkLabel* averageTurnaroundTimeLabel;

    gboolean autoSimulation;
}uiController;
/*___________________________________________________________________*/


static ProcessScheduler* simulatorProcessScheduler;
static int processorAmount = 0;

char* intToString(int num);
char* doubleToString(double num);

void startSimulator();
void resetSimulator();
void initializeSimulator();
void simulatorAddProcess(ProcessScheduler* processScheduler, expression infixExpression);

gboolean updateProcessList();
gboolean updateProcessorList();
gboolean updateProcessorList();
gboolean updateTimeframeList();
gboolean updateGanttChart();
gboolean updateLabelDisplay();
gboolean updateSimulatorProcessScheduler();


/*___________________________________________________________________*/
// signal

void focusMainWindow(){
    gtk_widget_set_sensitive(window, TRUE);
}

// addProcessButton
static GtkWidget* addProcessWindow;

void addProcessWindowAddProcess(GtkWidget* widget, gpointer entry){
    const char* collectedEntryValue = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
    int collectedEntryValueLength = gtk_entry_get_text_length(entry);

    expression infixExpression = malloc(sizeof(char) * (collectedEntryValueLength + 1));
    for (int i = 0; i < collectedEntryValueLength; i++){
        infixExpression[i] = collectedEntryValue[i];
    }
    infixExpression[collectedEntryValueLength] = '\0';

    simulatorAddProcess(simulatorProcessScheduler, infixExpression);

    updateLabelDisplay();
    gtk_window_close(GTK_WINDOW(addProcessWindow));
}

void closeAddProcessWindow(){
    gtk_window_close(GTK_WINDOW(addProcessWindow));
}

G_MODULE_EXPORT void addProcessButton__clicked(GtkButton* button, gpointer data){
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

// resetButton
static GtkWidget* resetSimulatorWindow;

void resetSimulatorWindowReset(GtkWidget* button, gpointer entry){
    if (strtol(gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry)), NULL, 10) > 0){
        resetSimulator(strtol(gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry)), NULL, 10));
    }

    gtk_window_close(GTK_WINDOW(resetSimulatorWindow));
}

G_MODULE_EXPORT void resetSimulator__clicked(GtkButton* button, gpointer data){
    gtk_widget_set_sensitive(window, FALSE);
    uiController.autoSimulation = FALSE;

    resetSimulatorWindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(resetSimulatorWindow), "Reset simulator");
    gtk_window_set_default_size(GTK_WINDOW(resetSimulatorWindow), 600, 400);
    g_signal_connect(resetSimulatorWindow, "close-request", G_CALLBACK(focusMainWindow), NULL);

    GtkLabel* averageResponseTimeLabel = GTK_LABEL(gtk_label_new("Average response time:"));
    GtkLabel* averageWaitingTimeLabel = GTK_LABEL(gtk_label_new("Average waiting time:"));
    GtkLabel* averageTurnaroundTimeLabel = GTK_LABEL(gtk_label_new("Average turnaround time:"));
    GtkLabel* averageResponseTimeDataLabel = GTK_LABEL(gtk_label_new(doubleToString(simulatorProcessScheduler -> averageResponseTime)));
    GtkLabel* averageWaitingTimeDataLabel = GTK_LABEL(gtk_label_new(doubleToString(simulatorProcessScheduler -> averageWaitingTime)));
    GtkLabel* averageTurnaroundTimeDataLabel = GTK_LABEL(gtk_label_new(doubleToString(simulatorProcessScheduler -> averageTurnaroundTime)));

    GtkLabel* statisticsLabel = GTK_LABEL(gtk_label_new("Previous simulation statistics:"));

    GtkBox* resetSimulatorWindowContent = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    GtkBox* averageResponseTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    GtkBox* averageWaitingTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    GtkBox* averageTurnaroundTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));

    gtk_widget_set_valign(GTK_WIDGET(resetSimulatorWindowContent), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(resetSimulatorWindowContent), GTK_ALIGN_CENTER);
    gtk_box_set_homogeneous(resetSimulatorWindowContent, TRUE);

    gtk_box_set_homogeneous(averageResponseTimeSection, TRUE);
    gtk_box_set_homogeneous(averageWaitingTimeSection, TRUE);
    gtk_box_set_homogeneous(averageTurnaroundTimeSection, TRUE);

    gtk_box_append(averageResponseTimeSection, GTK_WIDGET(averageResponseTimeLabel));
    gtk_box_append(averageResponseTimeSection, GTK_WIDGET(averageResponseTimeDataLabel));
    gtk_box_append(averageWaitingTimeSection, GTK_WIDGET(averageWaitingTimeLabel));
    gtk_box_append(averageWaitingTimeSection, GTK_WIDGET(averageWaitingTimeDataLabel));
    gtk_box_append(averageTurnaroundTimeSection, GTK_WIDGET(averageTurnaroundTimeLabel));
    gtk_box_append(averageTurnaroundTimeSection, GTK_WIDGET(averageTurnaroundTimeDataLabel));

    GtkBox* statisticsSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4));
    gtk_box_append(statisticsSection, GTK_WIDGET(statisticsLabel));
    gtk_box_append(statisticsSection, GTK_WIDGET(averageResponseTimeSection));
    gtk_box_append(statisticsSection, GTK_WIDGET(averageWaitingTimeSection));
    gtk_box_append(statisticsSection, GTK_WIDGET(averageTurnaroundTimeSection));

    gtk_box_append(resetSimulatorWindowContent, GTK_WIDGET(statisticsSection));

    GtkBox* resetSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkBox* resetLabelSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16));
    
    gtk_box_set_homogeneous(resetSection, TRUE);
    gtk_box_set_homogeneous(resetLabelSection, TRUE);
    gtk_widget_set_halign(GTK_WIDGET(resetSection), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(resetSection), GTK_ALIGN_CENTER);

    GtkLabel* entryLabel = GTK_LABEL(gtk_label_new("Enter new simulator processor amount:"));
    GtkEntry* resetEntry = GTK_ENTRY(gtk_entry_new());
    gtk_editable_set_max_width_chars(GTK_EDITABLE(resetEntry), 20);

    GtkButton* resetButton = GTK_BUTTON(gtk_button_new_with_label("Reset"));
    g_signal_connect(resetButton, "clicked", G_CALLBACK(resetSimulatorWindowReset), resetEntry);
    gtk_button_set_can_shrink(resetButton, TRUE);

    gtk_box_append(resetLabelSection, GTK_WIDGET(entryLabel));
    gtk_box_append(resetLabelSection, GTK_WIDGET(resetEntry));
    
    gtk_box_append(resetSection, GTK_WIDGET(resetLabelSection));
    gtk_box_append(resetSection, GTK_WIDGET(resetButton));

    gtk_box_append(resetSimulatorWindowContent, GTK_WIDGET(resetSection));

    gtk_window_set_child(GTK_WINDOW(resetSimulatorWindow), GTK_WIDGET(resetSimulatorWindowContent));
    gtk_window_present(GTK_WINDOW(resetSimulatorWindow));
}

// autoButton
G_MODULE_EXPORT void autoButton__clicked(GtkButton* button, gpointer data){
    uiController.autoSimulation = TRUE;
}

// stopAutoButton
G_MODULE_EXPORT void stopAutoButton__clicked(GtkButton* button, gpointer data){
    uiController.autoSimulation = FALSE;
}

// nextTimeFrameButton
G_MODULE_EXPORT void nextTimeFrameButton__clicked(GtkButton* button, gpointer data){
    if (uiController.autoSimulation != TRUE){
        processSchedulerNextTimeframe(simulatorProcessScheduler);

        updateProcessList();
        updateProcessorList();
        updateGanttChart();
        updateTimeframeList();
        updateLabelDisplay();
    }
}

// struct for controlling the scrolled window
struct ScrolledWindowController{
    GtkScrolledWindow* processorListScrolledWindow;
    GtkScrolledWindow* timeframeScrolledWindow;
    GtkScrolledWindow* ganttChartScrolledWindow;
}scrolledWindowController;

// scrolled window
// figure this out, why they are binding together when there is only one listener?
// void scrolledProcessorWindow(GtkAdjustment* adjustment, gpointer userData){
//     if (simulatorProcessScheduler -> currentTimeFrame > 0)
//         gtk_scrolled_window_set_vadjustment(scrolledWindowController.ganttChartScrolledWindow, adjustment);
// }

// link the scroll window of gantt chart and timeframe & processor list
void scrolledGanttChartWindowH(GtkAdjustment* adjustment, gpointer userData){
    gtk_scrolled_window_set_hadjustment(scrolledWindowController.timeframeScrolledWindow, adjustment);
}

void scrolledGanttChartWindowV(GtkAdjustment* adjustment, gpointer userData){
    gtk_scrolled_window_set_vadjustment(scrolledWindowController.processorListScrolledWindow, adjustment);
}
/*___________________________________________________________________*/


/*___________________________________________________________________*/
// utility
char* intToString(int num){
    if (num == 0) return "0";
    if (num < 0) return "N/A";

    int len = 0, duplicateNum = num;
    char* string;

    for (duplicateNum, len; duplicateNum > 0; duplicateNum /= 10, len++);

    string = malloc(sizeof(char) * (len + 1));
    string[len] = '\0';
    for (len, num; len > 0; len--, num /= 10){
        string[len-1] = (char)((num % 10) + 48);
    }
    
    return string;
}

char* doubleToString(double num){
    if (num == 0.0) return "0.0";
    if (num < 0.0) return "N/A";

    // get the front part (in front decimal) of the double
    char* intString = intToString((int)(num));

    int remaining = (int)((num - (int)(num)) * 100000);
    char* remainingString = intToString(remaining);
    
    int totalLen = strlen(intString) + strlen(remainingString) + 2;
    
    char* string = malloc(sizeof(char) * totalLen);

    // join the first part
    for (int i = 0; i < strlen(intString); i++)
        string[i] = intString[i];

    string[strlen(intString)] = '.';

    // join the decimal part
    for (int i = strlen(intString) + 1; i < totalLen; i++)
        string[i] = remainingString[i-strlen(intString)-1];

    return string;
}

/*___________________________________________________________________*/


/*___________________________________________________________________*/
// widget constructor

GtkWidget* createTimeframeGanttChart(ProcessHistory* processHistory){
    GtkBox* newBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel* executingProcessLabel = GTK_LABEL(gtk_label_new("Executing process:"));
    GtkLabel* executingProcessIDLabel = GTK_LABEL(gtk_label_new(intToString(processHistory -> executedProcess -> processID)));
    GtkLabel* expressionLabel = GTK_LABEL(gtk_label_new("Expression:"));
    GtkLabel* expressionDisplayLabel = GTK_LABEL(gtk_label_new(processHistory -> executedProcess -> dependencyInformation -> infixExpression));
    GtkLabel* executingThreadLabel = GTK_LABEL(gtk_label_new("Executing thread:"));
    GtkLabel* executingThreadDisplayLabel = GTK_LABEL(gtk_label_new(""));
    GtkLabel* threadResultLabel = GTK_LABEL(gtk_label_new("Result:"));
    GtkLabel* threadResultDisplayLabel = GTK_LABEL(gtk_label_new(processHistory -> executedExpression -> expressionRepresentation));

    gtk_widget_set_size_request(GTK_WIDGET(newBox), 160, 160);

    GtkBox* dummyBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_set_vexpand(GTK_WIDGET(dummyBox), TRUE);
    gtk_widget_set_valign(GTK_WIDGET(dummyBox), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(dummyBox), GTK_ALIGN_CENTER);

    GtkScrolledWindow* scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    gtk_widget_set_vexpand(GTK_WIDGET(scrolledWindow), TRUE);

    operand infixExpression = malloc(sizeof(char) * 4);
    infixExpression[3] = '\0';
    infixExpression[0] = processHistory -> executedExpression -> operandA[0];
    infixExpression[1] = processHistory -> executedExpression -> expressionOperator;
    infixExpression[2] = processHistory -> executedExpression -> operandB[0];
    gtk_label_set_label(executingThreadDisplayLabel, infixExpression);

    GtkBox* executingProcessSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    GtkBox* expressionSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkBox* executingThreadSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkBox* threadResultSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

    gtk_widget_set_valign(GTK_WIDGET(executingProcessSection), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(expressionSection), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(executingThreadSection), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(threadResultSection), GTK_ALIGN_CENTER);

    gtk_box_append(executingProcessSection, GTK_WIDGET(executingProcessLabel));
    gtk_box_append(executingProcessSection, GTK_WIDGET(executingProcessIDLabel));

    gtk_box_append(expressionSection, GTK_WIDGET(expressionLabel));
    gtk_box_append(expressionSection, GTK_WIDGET(expressionDisplayLabel));

    gtk_box_append(executingThreadSection, GTK_WIDGET(executingThreadLabel));
    gtk_box_append(executingThreadSection, GTK_WIDGET(executingThreadDisplayLabel));

    gtk_box_append(threadResultSection, GTK_WIDGET(threadResultLabel));
    gtk_box_append(threadResultSection, GTK_WIDGET(threadResultDisplayLabel));

    gtk_box_append(dummyBox, GTK_WIDGET(executingProcessSection));
    gtk_box_append(dummyBox, GTK_WIDGET(expressionSection));
    gtk_box_append(dummyBox, GTK_WIDGET(executingThreadSection));
    gtk_box_append(dummyBox, GTK_WIDGET(threadResultSection));

    gtk_scrolled_window_set_child(scrolledWindow, GTK_WIDGET(dummyBox));

    gtk_box_append(newBox, GTK_WIDGET(scrolledWindow));

    return GTK_WIDGET(newBox);
}

GtkWidget* createEmptyGanttChart(){
    GtkBox* newBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel* newLabel = GTK_LABEL(gtk_label_new("-- processor free --"));
    
    GtkBox* dummyBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_set_valign(GTK_WIDGET(dummyBox), GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(GTK_WIDGET(dummyBox), TRUE);

    gtk_widget_set_size_request(GTK_WIDGET(newBox), 160, 160);

    gtk_box_append(dummyBox, GTK_WIDGET(newLabel));
    gtk_box_append(newBox, GTK_WIDGET(dummyBox));

    return GTK_WIDGET(newBox);
}

/*___________________________________________________________________*/


/*___________________________________________________________________*/
// simulator
void resetSimulator(int processorCoreAmount){
    free(simulatorProcessScheduler);

    simulatorProcessScheduler = createProcessScheduler(processorCoreAmount, ON);
    updateProcessList();
    updateProcessorList();
    updateTimeframeList();
    updateGanttChart();
    updateLabelDisplay();
}

void initializeSimulator(){
    uiController.autoSimulation = FALSE;

    uiController.processListWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "processListWindow"));
    uiController.processList = GTK_LIST_BOX(gtk_builder_get_object(builder, "processList"));
    uiController.simulatorProcessorList = GTK_BOX(gtk_builder_get_object(builder, "simulatorProcessorList"));
    uiController.timeframeList = GTK_BOX(gtk_builder_get_object(builder, "timeframeListBox"));
    uiController.simulatorGanttChart = GTK_BOX(gtk_builder_get_object(builder, "simulatorGanttChart"));

    uiController.averageResponseTimeLabel = GTK_LABEL(gtk_builder_get_object(builder, "averageResponseTimeLabel"));
    uiController.averageWaitingTimeLabel = GTK_LABEL(gtk_builder_get_object(builder, "averageWaitingTimeLabel"));
    uiController.averageTurnaroundTimeLabel = GTK_LABEL(gtk_builder_get_object(builder, "averageTurnaroundTimeLabel"));

    simulatorProcessScheduler = createProcessScheduler(4, ON);
    simulatorAddProcess(simulatorProcessScheduler, "a+b+c+d");
    simulatorAddProcess(simulatorProcessScheduler, "a+b");
    simulatorAddProcess(simulatorProcessScheduler, "a+b+c");

    uiController.numberOfProcessorLabel = GTK_LABEL(gtk_builder_get_object(builder, "numberOfProcessorLabel"));
    uiController.currentTimeframeLabel = GTK_LABEL(gtk_builder_get_object(builder, "currentTimeframeLabel"));
    gtk_label_set_label(uiController.numberOfProcessorLabel, intToString(simulatorProcessScheduler -> processorCoreAmount));
    gtk_label_set_label(uiController.currentTimeframeLabel, intToString(simulatorProcessScheduler -> currentTimeFrame));

    scrolledWindowController.processorListScrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "processorListScrolledWindow"));
    scrolledWindowController.timeframeScrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "timeframeScrolledWindow"));
    scrolledWindowController.ganttChartScrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "ganttChartScrolledWindow"));

    g_signal_connect(gtk_scrolled_window_get_hadjustment(scrolledWindowController.ganttChartScrolledWindow), "value-changed", G_CALLBACK(scrolledGanttChartWindowH), NULL);
    g_signal_connect(gtk_scrolled_window_get_vadjustment(scrolledWindowController.ganttChartScrolledWindow), "value-changed", G_CALLBACK(scrolledGanttChartWindowV), NULL);
    gtk_scrolled_window_set_vadjustment(scrolledWindowController.processorListScrolledWindow, gtk_scrolled_window_get_vadjustment(scrolledWindowController.ganttChartScrolledWindow));
    gtk_scrolled_window_set_hadjustment(scrolledWindowController.timeframeScrolledWindow, gtk_scrolled_window_get_hadjustment(scrolledWindowController.ganttChartScrolledWindow));
}

void simulatorAddProcess(ProcessScheduler* processScheduler, expression infixExpression){
    addProcess(processScheduler, infixExpression);

    updateProcessList();
}
/*___________________________________________________________________*/


/*___________________________________________________________________*/
// thread task
// update the displaying process list
gboolean updateProcessList(){
    gtk_list_box_remove_all(uiController.processList);
    
    void* dummyCollector = simulatorProcessScheduler -> processList;
    while (((Node*)dummyCollector) -> next != NULL){
        dummyCollector = ((Node*)dummyCollector) -> next;
        Process* hoveringProcess = (*((Process**)(((Node*)dummyCollector) -> data)));
    
        GtkBox* processListBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_widget_set_size_request(GTK_WIDGET(processListBox), 90, 130);
        
        GtkBox* dummyBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_widget_set_valign(GTK_WIDGET(dummyBox), GTK_ALIGN_CENTER);
        gtk_widget_set_halign(GTK_WIDGET(dummyBox), GTK_ALIGN_CENTER);
        gtk_widget_set_vexpand(GTK_WIDGET(dummyBox), TRUE);
        gtk_widget_set_hexpand(GTK_WIDGET(dummyBox), TRUE);

        GtkBox* processIDsection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkBox* processExpressionSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        GtkBox* processStatusSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

        GtkBox* arrivalTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkBox* responseTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkBox* completionTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkBox* turnaroundTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkBox* remainingBurstTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkBox* waitingTimeSection = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

        gtk_box_set_homogeneous(processIDsection, TRUE);
        gtk_box_set_homogeneous(processStatusSection, TRUE);
        gtk_box_set_homogeneous(arrivalTimeSection, TRUE);
        gtk_box_set_homogeneous(responseTimeSection, TRUE);
        gtk_box_set_homogeneous(completionTimeSection, TRUE);
        gtk_box_set_homogeneous(turnaroundTimeSection, TRUE);
        gtk_box_set_homogeneous(remainingBurstTimeSection, TRUE);
        gtk_box_set_homogeneous(waitingTimeSection, TRUE);

        gtk_box_append(processIDsection, gtk_label_new("Process ID:"));
        gtk_box_append(processIDsection, gtk_label_new(intToString(hoveringProcess -> processID)));

        gtk_box_append(processExpressionSection, gtk_label_new("Expression:"));
        gtk_box_append(processExpressionSection, gtk_label_new(hoveringProcess -> dependencyInformation -> infixExpression));
    
        gtk_box_append(processStatusSection, gtk_label_new("Process state:"));
        switch (hoveringProcess -> processState){
            case TERMINATED:
                gtk_box_append(processStatusSection, gtk_label_new("TERMINATED"));
                break;
            case READY:
                gtk_box_append(processStatusSection, gtk_label_new("READY"));
                break;
            case RUNNING:
                gtk_box_append(processStatusSection, gtk_label_new("RUNNING"));
                break;
            case NEW:
                gtk_box_append(processStatusSection, gtk_label_new("NEW"));
                break;
            default:
                gtk_box_append(processStatusSection, gtk_label_new("WAITING"));
                break;
        }

        gtk_box_append(arrivalTimeSection, gtk_label_new("Arrival time:"));
        gtk_box_append(arrivalTimeSection, gtk_label_new(intToString(hoveringProcess -> arrivalTime)));

        gtk_box_append(responseTimeSection, gtk_label_new("Response time:"));
        gtk_box_append(responseTimeSection, gtk_label_new(intToString(hoveringProcess -> responseTime)));

        gtk_box_append(completionTimeSection, gtk_label_new("Completion time:"));
        gtk_box_append(completionTimeSection, gtk_label_new(intToString(hoveringProcess -> completionTime)));

        gtk_box_append(turnaroundTimeSection, gtk_label_new("Turnaround time:"));
        gtk_box_append(turnaroundTimeSection, gtk_label_new(intToString(hoveringProcess -> turnaroundTime)));

        gtk_box_append(remainingBurstTimeSection, gtk_label_new("Burst time:"));
        gtk_box_append(remainingBurstTimeSection, gtk_label_new(intToString(hoveringProcess -> remainingBurstTime)));

        gtk_box_append(waitingTimeSection, gtk_label_new("Waiting time:"));
        gtk_box_append(waitingTimeSection, gtk_label_new(intToString(hoveringProcess -> waitingTime)));

        gtk_box_append(dummyBox, GTK_WIDGET(processIDsection));
        gtk_box_append(dummyBox, GTK_WIDGET(processExpressionSection));
        gtk_box_append(dummyBox, GTK_WIDGET(processStatusSection));
        gtk_box_append(dummyBox, GTK_WIDGET(arrivalTimeSection));
        gtk_box_append(dummyBox, GTK_WIDGET(responseTimeSection));
        gtk_box_append(dummyBox, GTK_WIDGET(completionTimeSection));
        gtk_box_append(dummyBox, GTK_WIDGET(turnaroundTimeSection));
        gtk_box_append(dummyBox, GTK_WIDGET(remainingBurstTimeSection));
        gtk_box_append(dummyBox, GTK_WIDGET(waitingTimeSection));

        gtk_box_append(processListBox, GTK_WIDGET(dummyBox));
        gtk_list_box_append(uiController.processList, GTK_WIDGET(processListBox));
    }

    return TRUE;
}

gboolean updateProcessorList(){
    GtkWidget* processorListChild = gtk_widget_get_first_child(GTK_WIDGET(uiController.simulatorProcessorList));
    while (processorListChild != NULL){
        gtk_box_remove(uiController.simulatorProcessorList, processorListChild);
        processorListChild = gtk_widget_get_first_child(GTK_WIDGET(uiController.simulatorProcessorList));
    }

    for (int i = 0; i < simulatorProcessScheduler -> processorCoreAmount; i++){
        GtkBox* newProcessorBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        GtkLabel* newProcessorLabel = GTK_LABEL(gtk_label_new(intToString(i+1)));

        GtkBox* dummyBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_widget_set_valign(GTK_WIDGET(dummyBox), GTK_ALIGN_CENTER);
        gtk_widget_set_vexpand(GTK_WIDGET(dummyBox), TRUE);

        gtk_widget_add_css_class(GTK_WIDGET(newProcessorBox), "processorBox");
        gtk_widget_add_css_class(GTK_WIDGET(newProcessorLabel), "processorLabel");

        gtk_widget_set_valign(GTK_WIDGET(newProcessorBox), GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(GTK_WIDGET(newProcessorBox), 160, 160);

        gtk_box_append(dummyBox, GTK_WIDGET(newProcessorLabel));
        gtk_box_append(newProcessorBox, GTK_WIDGET(dummyBox));

        gtk_box_append(uiController.simulatorProcessorList, GTK_WIDGET(newProcessorBox));
    }

    return TRUE;
}

gboolean updateGanttChart(){
    GtkWidget* ganttChartChild = gtk_widget_get_first_child(GTK_WIDGET(uiController.simulatorGanttChart));
    while (ganttChartChild != NULL){
        gtk_box_remove(uiController.simulatorGanttChart, ganttChartChild);
        ganttChartChild = gtk_widget_get_first_child(GTK_WIDGET(uiController.simulatorGanttChart));
    }

    ProcessorList* processorList = simulatorProcessScheduler -> processorList;
    Processor* hoveringProcessor = NULL;

    while (processorList -> next != NULL){
        processorList = processorList -> next;
        hoveringProcessor = *((Processor**)(processorList -> data));

        // start from the presume "maximum" timeframe the processor can be
        int processorCurrentTimeframe = simulatorProcessScheduler -> currentTimeFrame;
        // start pointing at the first process of the processHistoryList instead of the HEAD of list
        ProcessHistoryList* processHistoryList = hoveringProcessor -> processHistoryList;
        if (processHistoryList -> next != NULL)
            processHistoryList = processHistoryList -> next;

        // the GtkBox for storing process history
        GtkBox* processorGanttChartBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16));

        for (processorCurrentTimeframe; processorCurrentTimeframe > 0; processorCurrentTimeframe--){
            ProcessHistory* hoveringHistory = NULL;
            if (processHistoryList -> data != NULL)
                hoveringHistory = *(ProcessHistory**)(processHistoryList -> data);
            
            if (hoveringHistory != NULL && hoveringHistory -> timeEnd == processorCurrentTimeframe){
                gtk_box_prepend(processorGanttChartBox, createTimeframeGanttChart(hoveringHistory));

                if (processHistoryList -> next != NULL){
                    processHistoryList = processHistoryList -> next;
                }
            }else {
                gtk_box_prepend(processorGanttChartBox, createEmptyGanttChart());
            }
        }

        // add independent gantt box into the scrolled window box
        gtk_box_append(uiController.simulatorGanttChart, GTK_WIDGET(processorGanttChartBox));
    }

    return TRUE;
}

gboolean updateTimeframeList(){
    GtkWidget* timeFrameListChild = gtk_widget_get_first_child(GTK_WIDGET(uiController.timeframeList));
    while (timeFrameListChild != NULL){
        gtk_box_remove(uiController.timeframeList, timeFrameListChild);
        timeFrameListChild = gtk_widget_get_first_child(GTK_WIDGET(uiController.timeframeList));
    }

    for (int i = 0; i < simulatorProcessScheduler -> currentTimeFrame; i++){
        GtkBox* newTimeframeBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        GtkLabel* newTimeframeStartLabel = GTK_LABEL(gtk_label_new(intToString(i)));
        GtkLabel* newTimeframeEndLabel = GTK_LABEL(gtk_label_new(intToString(i+1)));

        gtk_widget_set_size_request(GTK_WIDGET(newTimeframeBox), 160, 80);
        gtk_box_set_homogeneous(newTimeframeBox, TRUE);
        gtk_label_set_xalign(newTimeframeStartLabel, 0.1);
        gtk_label_set_xalign(newTimeframeEndLabel, 0.9);

        gtk_box_append(newTimeframeBox, GTK_WIDGET(newTimeframeStartLabel));
        gtk_box_append(newTimeframeBox, GTK_WIDGET(newTimeframeEndLabel));

        gtk_box_append(uiController.timeframeList, GTK_WIDGET(newTimeframeBox));
    }

    return TRUE;
}

// update the displaying number of core & timeframe
gboolean updateLabelDisplay(){
    gtk_label_set_label(uiController.numberOfProcessorLabel, intToString(simulatorProcessScheduler -> processorCoreAmount));
    gtk_label_set_label(uiController.currentTimeframeLabel, intToString(simulatorProcessScheduler -> currentTimeFrame));
    
    gtk_label_set_label(uiController.averageResponseTimeLabel, doubleToString(calculateAverageResponseTime(simulatorProcessScheduler)));
    gtk_label_set_label(uiController.averageWaitingTimeLabel, doubleToString(calculateAverageWaitingTime(simulatorProcessScheduler)));
    gtk_label_set_label(uiController.averageTurnaroundTimeLabel, doubleToString(calculateAverageTurnaroundTime(simulatorProcessScheduler)));

    return TRUE;
}

// update process scheduler (update and go to next timeframe)
gboolean updateSimulatorProcessScheduler(){
    if (uiController.autoSimulation)
        processSchedulerNextTimeframe(simulatorProcessScheduler);

    return TRUE;
}
/*___________________________________________________________________*/


static void activate(GtkApplication* app, gpointer user_data){
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Shortest Remaining Time First simulator");
    gtk_window_set_default_size(GTK_WINDOW(window), 1400, 700);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "simulator_ui.ui", NULL);

    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "simulator_ui.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    GObject* windowContent = gtk_builder_get_object(builder, "windowContent");
    gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(windowContent));

    initializeSimulator();

    gtk_window_present(GTK_WINDOW(window));

    g_timeout_add_seconds(1, updateSimulatorProcessScheduler, NULL);
    g_timeout_add_seconds(1, updateProcessList, NULL);
    g_timeout_add_seconds(1, updateProcessorList, NULL);
    g_timeout_add_seconds(1, updateTimeframeList, NULL);
    g_timeout_add_seconds(1, updateGanttChart, NULL);
    g_timeout_add_seconds(1, updateLabelDisplay, NULL);
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