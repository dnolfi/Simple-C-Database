#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "database_list.h"

typedef struct {
    char* command;
    void (*cmdFunction)(DatabaseList* dbl, Database** currentDB, char* name);
} uiCmd;

extern uiCmd uiCommands[];
extern size_t commandCount;

void userMenu();

void cmdQuit(DatabaseList* dbl, Database** currentDB, char* name);
void cmdPrintDB(DatabaseList* dbl, Database** currentDB, char* name);
void cmdPrintDBList(DatabaseList* dbl, Database** currentDB, char* name);
void cmdHelp(DatabaseList* dbl, Database** currentDB, char* name);
void cmdCreateDB(DatabaseList* dbl, Database** currentDB, char* name);
void cmdDeleteDB(DatabaseList* dbl, Database** currentDB, char* name);
void cmdSwitchDB(DatabaseList* dbl, Database** currentDB, char* name);
void cmdNewCol(DatabaseList* dbl, Database** currentDB, char* name);
void cmdNewRow(DatabaseList* dbl, Database** currentDB, char* name);
void cmdWriteCell(DatabaseList* dbl, Database** currentDB, char* name);
void cmdDeleteRow(DatabaseList* dbl, Database** currentDB, char* name);
void cmdDeleteCol(DatabaseList* dbl, Database** currentDB, char* name);
void cmdSaveDbToFile(DatabaseList* dbl, Database** currentDB, char* name);
void cmdLoadDbFromFile(DatabaseList* dbl, Database** currentDB, char* name);
void cmdChangeColName(DatabaseList* dbl, Database** currentDB, char* name);

int safeReadInt(Database* currentDB, size_t rowValue, size_t colValue);
int safeReadFloat(Database* currentDB, size_t rowValue, size_t colValue);
int safeReadDouble(Database* currentDB, size_t rowValue, size_t colValue);

size_t safeReadSize();

#endif