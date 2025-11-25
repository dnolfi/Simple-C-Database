#ifndef DATABASE_LIST_H
#define DATABASE_LIST_H
#include "database.h"

typedef struct {

    Database** dbList;
    size_t dbCount;
    size_t dbLimit;

}DatabaseList;

DatabaseList* createDatabaseList(size_t limit);

void deleteDatabaseFromList(DatabaseList* dbl, const char* name);
void addDatabaseToList(Database* db, DatabaseList* dbl);
void removeDatabaseAtIndex(DatabaseList* dbl, size_t index);
void printDatabaseList(DatabaseList* dbl);
void deleteDatabaseList(DatabaseList* dbl);

int findDatabaseInList(DatabaseList* dbl, const char* name);

#endif