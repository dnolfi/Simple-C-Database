#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "user_interface.h"
#include "database.h"
#include "database_list.h"

 uiCmd uiCommands[] = { 
        {"-quit", cmdQuit},
        {"-print", cmdPrintDB},
        {"-list", cmdPrintDBList},
        {"-help", cmdHelp},
        {"-new", cmdCreateDB},
        {"-delete", cmdDeleteDB},
        {"-switch", cmdSwitchDB},
        {"-newcol", cmdNewCol},
        {"-newrow", cmdNewRow},
        {"-writecell", cmdWriteCell},
        {"-delrow", cmdDeleteRow},
        {"-delcol", cmdDeleteCol}
    };

/* Refactored this to use handler design pattern */

void userMenu() {

    printf("------ Welcome to SCDB! ------\n");
    printf("Type -help to see the list of available commands and features.\n");

    size_t commandCount = sizeof(uiCommands)/sizeof(uiCommands[0]);

    // Current DB we are working with, need to avoid dangling pointers!
    // If the DB this is pointing to is deleted, we need to set this back to NULL
    Database* currentDB = NULL;

    DatabaseList* dbl = createDatabaseList(DB_LIMIT);

    char inputBuffer[STRING_LEN];

    // Get input from the user so they can do shit
    while(1) {

        // Prompt for the user
        if (!currentDB)  printf("(scdb) >> ");
        else printf("(%s) >> ", currentDB->dbName);

        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
            inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
        }

        int found = 0;

        // Search for the correct command
        for (size_t i = 0; i < commandCount; i++) {
            if (strcmp(inputBuffer, uiCommands[i].command) == 0) {
                uiCommands[i].cmdFunction(dbl, &currentDB, inputBuffer);
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Invalid command. Type -help to see the list of available commands.\n");
        }
    }
}

void cmdQuit(DatabaseList* dbl, Database** db, char* name) {

    // Free the memory for the database list
    deleteDatabaseList(dbl);

    // Exit
    printf("Exiting scdb...\n");
    exit(0);
}

void cmdHelp(DatabaseList* dbl, Database** db, char* name) {

    printf("Supported commands: \n");
    printf("------------------------\n");
    printf("1) -help\tShows list of commands\n");
    printf("2) -new\t\tCreate a new database\n");
    printf("3) -newcol\tCreate a new column (Columns MUST be created before rows)\n");
    printf("4) -newrow\tCreate a new row\n");
    printf("5) -writecell\tWrite a cell\n");
    printf("x) -delete\tDelete the current database\n");
    printf("x) -delrow\tDelete a row specified by it's index\n");
    printf("x) -delcol\tDelete a column specified by it's index\n");
    printf("x) -print\tPrint a table\n");
    printf("x) -switch\tSwitch to a different database\n");
    printf("x) -list\tList the available databases\n");
    printf("x) -quit\tExit the program\n");
    printf("\n");
}

void cmdPrintDB(DatabaseList* dbl, Database** currentDB, char* name) {
    if (*currentDB)
        printDatabase(*currentDB);
    else 
        printf("No database selected, use -switch to switch to a Database or -new to create a new one.\n");
}

void cmdPrintDBList(DatabaseList* dbl, Database** currentDB, char* name) {
    printDatabaseList(dbl);
}

void cmdCreateDB(DatabaseList* dbl, Database** currentDB, char* name) {

    printf("Enter the name of the Database:\ndbName > ");
    char dbName[STRING_LEN];

    if (fgets(dbName, sizeof(dbName), stdin) != NULL) {
        dbName[strcspn(dbName, "\n")] = '\0';
    }

    if (dbl->dbCount < dbl->dbLimit) {

        Database* db = createDatabase(dbName);
        addDatabaseToList(db, dbl);

        if (db) {
            printf("Succesfully created Database: %s\n", dbName);
            *currentDB = db;
        }

    } else {
        printf("Unable to create DB\n");
    }
}

void cmdDeleteDB(DatabaseList* dbl, Database** currentDB, char* name) {

    printf("Enter the name of the Database to delete:\ndbName > ");

    char dbName[STRING_LEN];

    if (fgets(dbName, sizeof(dbName), stdin) != NULL) {
        dbName[strcspn(dbName, "\n")] = '\0';
    }

    if (*currentDB && strcmp(dbName, (*currentDB)->dbName) == 0)
        *currentDB = NULL;

    deleteDatabaseFromList(dbl, dbName);
}

void cmdSwitchDB(DatabaseList* dbl, Database** currentDB, char* name) {

    printf("Enter the name of the Database to switch to:\ndbName > ");
    char dbName[STRING_LEN];

    if (fgets(dbName, sizeof(dbName), stdin) != NULL) {
        dbName[strcspn(dbName, "\n")] = '\0';
    }

    int index = findDatabaseInList(dbl, dbName);

    if (index >= 0) {
        *currentDB = dbl->dbList[index];
        printf("Successfully switched to: %s\n", dbName);
    } else {
        printf("Could not find Database %s\n", dbName);
    }
}

void cmdNewCol(DatabaseList* dbl, Database** currentDB, char* name) {
    char colName[STRING_LEN];
    printf("Enter the name of the column: > ");

    if (fgets(colName, sizeof(colName), stdin) != NULL) {
        colName[strcspn(colName, "\n")] = '\0';
    }

    printf("Enter the type of data: (int, float, double) > ");

    char type[STRING_LEN];
    DataTypes colType;

    if (fgets(type, sizeof(type), stdin) != NULL) {
        type[strcspn(type, "\n")] = '\0';
    }

    if (strcmp(type, "int") == 0)
        colType = INT_TYPE;
    else if (strcmp(type, "float") == 0)
        colType = FLOAT_TYPE;
    else if (strcmp(type, "double") == 0)
        colType = DOUBLE_TYPE;
    else {
        printf("Invalid column type entered.\n");
        return;
    }
    // Add a new column
    createColumn(*currentDB, colName, colType);
    printf("Column %s successfully created\n", colName);

    printDatabase(*currentDB);
}

void cmdNewRow(DatabaseList* dbl, Database** currentDB, char* name) {

    // Ensure the user is operating on an existing database
    if (!currentDB || !*currentDB) {
        printf("No database selected. Use -switch or -new to select/create a database.\n");
        return;
    }

    // Ensure the Database has columns
    if ((*currentDB)->cols && (*currentDB)->numCols > 0) {
        createRow(*currentDB);
        printf("New row for %s successfully created.\n", (*currentDB)->dbName);

        printDatabase(*currentDB);

    } else {
        printf("Rows for database %s cannot be added until it has columns\nUse -newcol to add a column.\n", 
            (*currentDB)->dbName);
        return;

    }
}

// Write data to a cell in a database
void cmdWriteCell(DatabaseList* dbl, Database** currentDB, char* name) {

    // Check if there is a valid Database to write to
    if (!*currentDB || !dbl) {
        printf("No database selected.\n");
        return;
    }

    // Check if the Database has cells to write to
    if (!((*currentDB)->cols) || !((*currentDB)->rows)) {
        printf("Selected table has no available cells to write to.\n");
        printf("Create a column and a row to insert a cell value.\n");
        return;
    }

    printf("Available table indices: (%d, %d)\n", (*currentDB)->numRows-1, (*currentDB)->numCols-1);
    // 1) ask the user which cell they want to write to
    printf("Enter the table index to enter a value to: ");

    char input[100];

    size_t rowValue;
    size_t colValue;

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Input error.\n");
        return;
    }

    if (sscanf(input, "%zu %zu", &rowValue, &colValue) != 2) {
        printf("Invalid input.\n");
        return;
    }

    if ((*currentDB)->cols[colValue].type == INT_TYPE) {
        printf("Index (%zu, %zu) has type INT. Enter an integer: ", rowValue, colValue);

        if (safeReadInt(*currentDB, rowValue, colValue) < 0) {
            printf("Error adding value to cell.\n");
            return;
        }
    } 

    else if ((*currentDB)->cols[colValue].type == FLOAT_TYPE) {
        printf("Index (%zu, %zu) has type FLOAT. Enter a float: ", rowValue, colValue);

        if (safeReadFloat(*currentDB, rowValue, colValue) < 0) {
            printf("Error adding value to cell.\n");
            return;
        }
    }

    else if ((*currentDB)->cols[colValue].type == DOUBLE_TYPE) {
        printf("Index (%zu, %zu) has type DOUBLE. Enter a double: ", rowValue, colValue);

        if (safeReadDouble(*currentDB, rowValue, colValue) < 0) {
            printf("Error adding value to cell.\n");
            return;
        }
    }

    printDatabase(*currentDB);
}

// Delete a specified row (user specifies by index)
void cmdDeleteRow(DatabaseList* dbl, Database** currentDB, char* name) {

}

// Delete a specified column (user specifies by index)
void cmdDeleteCol(DatabaseList* dbl, Database** currentDB, char* name) {

}

// Safely read an integer value from stdin
int safeReadInt(Database* currentDB, size_t rowValue, size_t colValue) {
    int tableValue;
    char valBuf[16];

    if (fgets(valBuf, sizeof(valBuf), stdin) == NULL) {
        printf("Input error.\n");
        return -1;
    }

    if (sscanf(valBuf, "%d", &tableValue) != 1) {
        printf("Invalid input.\n");
        return -1;
    }

    if (addInt(currentDB, rowValue, colValue, tableValue) < 0){
        return -1;
    }

    return 0;
}

// Safely read a float value from stdin
int safeReadFloat(Database* currentDB, size_t rowValue, size_t colValue) {

    float tableValue;
    char valBuf[16];

    if (fgets(valBuf, sizeof(valBuf), stdin) == NULL) {
        printf("Input error.\n");
        return -1;
    }

    if (sscanf(valBuf, "%ff", &tableValue) != 1) {
        printf("Invalid input.\n");
        return -1;
    }

    if (addFloat(currentDB, rowValue, colValue, tableValue) < 0){
        return -1;
    }

    return 0;
}

// Safely read a double value from stdin
int safeReadDouble(Database* currentDB, size_t rowValue, size_t colValue) {

    double tableValue;
    char valBuf[16];

    if (fgets(valBuf, sizeof(valBuf), stdin) == NULL) {
        printf("Input error.\n");
        return -1;
    }

    if (sscanf(valBuf, "%lf", &tableValue) != 1) {
        printf("Invalid input.\n");
        return -1;
    }

    if (addDouble(currentDB, rowValue, colValue, tableValue) < 0){
        return -1;
    }

    return 0;
}