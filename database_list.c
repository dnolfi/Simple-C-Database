#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "database_list.h"
#include "database.h"

// Instantiate a database list
DatabaseList* createDatabaseList(size_t limit) {

    DatabaseList* dbl = malloc(sizeof(DatabaseList));

    if (!dbl) {
        fprintf(stderr, "malloc returned NULL ptr for DatabaseList object\n");
        exit(1);
    }

    dbl->dbList = NULL;
    dbl->dbCount = 0;
    dbl->dbLimit = limit;

    return dbl;
}

// Adds a database to our list of active DBs
void addDatabaseToList(Database* db, DatabaseList* dbl) {

    if (dbl->dbCount >= dbl->dbLimit) {
        printf("Database List has reached its maximum size. Delete a Database to continue. \n");
        return;
    }

    Database** newDbList = realloc(dbl->dbList, (dbl->dbCount + 1) * sizeof(Database*));

    if (!newDbList) {
        fprintf(stderr, "Failed to realloc memory for db list\n");
        exit(1);
    }

    dbl->dbList = newDbList;
    dbl->dbList[dbl->dbCount] = db;
    dbl->dbCount++;

    printf("Successfully added Database: %s\n", db->dbName);
}

// Searches for a Database object by name in a DatabaseList
int findDatabaseInList(DatabaseList* dbl, const char* name) {

    for (size_t i = 0; i < dbl->dbCount; i++) {
        // Found the right db
        if (strncmp(name, dbl->dbList[i]->dbName, STRING_LEN) == 0) {   
            return i;
        }
    }   
    return -1;
}

void removeDatabaseAtIndex(DatabaseList* dbl, size_t index) {

     deleteDatabase(dbl->dbList[index]);

    // Shift the list to get rid of our garbage values
    for (size_t i = index; i < dbl->dbCount - 1; i++) {
        dbl->dbList[i] = dbl->dbList[i+1];
    }

    dbl->dbCount--;

    if (dbl->dbCount > 0) {
        Database** newDbList = realloc(dbl->dbList, (dbl->dbCount) * sizeof(Database*));

        if (!newDbList) {
            fprintf(stderr, "Failed to realloc memory for db list\n");
            exit(1);
        }
        dbl->dbList = newDbList;

    } else {
        free(dbl->dbList);
        dbl->dbList = NULL;
        dbl->dbCount = 0;
    }
}

// Delete a DB from our list of active DBs
void deleteDatabaseFromList(DatabaseList* dbl, const char* name) {

    // Search for our DB in the list
    int index = findDatabaseInList(dbl, name);

    // If found, remove it
    if (index >= 0) {
        removeDatabaseAtIndex(dbl, index);
    } else {
        printf("Database %s not found. Enter a valid name.\n", name);
        return;
    }

    printf("Successfully deleted Database: %s\n", name);
}

// Print the list of available DBs
void printDatabaseList(DatabaseList* dbl) {

    for (size_t i = 0; i < dbl->dbCount; i++)
        printf("%d) %s\n", i + 1, dbl->dbList[i]->dbName);
}

// Deletes a DatabaseList object
void deleteDatabaseList(DatabaseList* dbl) {

    if (!dbl)
        return;

    for (size_t i = 0; i < dbl->dbCount; i++) {
        deleteDatabase(dbl->dbList[i]);
    }

    free(dbl->dbList);

    free(dbl);
}