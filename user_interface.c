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
        {"-delcol", cmdDeleteCol},
        {"-save", cmdSaveDbToFile},
        {"-load", cmdLoadDbFromFile},
        {"-colname", cmdChangeColName}
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

        // Get the user input 
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
            inputBuffer[strcspn(inputBuffer, "\n ")] = '\0';
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

    printf("\nSupported commands: \n");
    printf("------------------------\n");
    printf("1) -help\tShows list of commands\n");
    printf("2) -new\t\tCreate a new database\n");
    printf("3) -newcol\tCreate a new column (Columns MUST be created before rows)\n");
    printf("4) -newrow\tCreate a new row\n");
    printf("5) -writecell\tWrite a cell\n");
    printf("6) -colname\tChange a column name\n");
    printf("7) -delete\tDelete the current database\n");
    printf("8) -delrow\tDelete a row specified by it's index\n");
    printf("9) -delcol\tDelete a column specified by it's index\n");
    printf("10) -print\tPrint a table\n");
    printf("11) -switch\tSwitch to a different database\n");
    printf("12) -list\tList the available databases\n");
    printf("13) -quit\tExit the program\n");
    printf("14) -save\tSave the database to a .csv file\n");
    printf("15) -load\tLoad a database from a .csv file\n");
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

    // Verify that a database with that name does not exist already
    for (size_t index = 0; index < dbl->dbCount; index++) {
        if (strcmp(dbName, dbl->dbList[index]->dbName) == 0) {
            printf("Database with name %s already exists. Choose a new name.\n");
            return;
        }
    }

    // If there's space in the database, add the new database
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

// Delete a database from the UI
void cmdDeleteDB(DatabaseList* dbl, Database** currentDB, char* name) {

    printf("Enter the name of the Database to delete:\ndbName > ");

    char dbName[STRING_LEN];

    if (fgets(dbName, sizeof(dbName), stdin) != NULL) {
        dbName[strcspn(dbName, "\n ")] = '\0';
    }

    if (*currentDB && strcmp(dbName, (*currentDB)->dbName) == 0)
        *currentDB = NULL;

    deleteDatabaseFromList(dbl, dbName);
}

// Switch to a different database
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

    if ((rowValue >= (*currentDB)->numRows) || (colValue >= (*currentDB)->numCols)) {
        fprintf(stderr, "Invalid table indices entered.\n");
        return;
    }

    switch ((*currentDB)->cols[colValue].type) {

        case INT_TYPE :
            printf("Index (%zu, %zu) has type INT. Enter an integer: ", rowValue, colValue);

            if (safeReadInt(*currentDB, rowValue, colValue) < 0) {
                printf("Error adding value to cell.\n");
                return;
            }
            break;

        case FLOAT_TYPE :
            printf("Index (%zu, %zu) has type FLOAT. Enter a float: ", rowValue, colValue);

            if (safeReadFloat(*currentDB, rowValue, colValue) < 0) {
                printf("Error adding value to cell.\n");
                return;
            }
            break;

        case DOUBLE_TYPE :
            printf("Index (%zu, %zu) has type DOUBLE. Enter a double: ", rowValue, colValue);

            if (safeReadDouble(*currentDB, rowValue, colValue) < 0) {
                printf("Error adding value to cell.\n");
                return;
            }
            break;
        
        default :
            fprintf(stderr, "Index (%zu, %zu) has unrecognized type.\n", rowValue, colValue);
            
    }

    printDatabase(*currentDB);
}

// Delete a specified row (user specifies by index)
void cmdDeleteRow(DatabaseList* dbl, Database** currentDB, char* name) {

    if (!(*currentDB)->rows) {
        printf("Database '%s' has no rows to delete.\n", (*currentDB)->dbName);
        return;
    }

    printf("Database '%s' has row indices (0-%zu) available.\n", (*currentDB)->dbName, (*currentDB)->numRows - 1);
    printf("Enter the index of the row to delete > ");

    size_t index = safeReadSize();
    deleteRow(*currentDB, index);
}

// Delete a specified column (user specifies by index)
void cmdDeleteCol(DatabaseList* dbl, Database** currentDB, char* name) {
    
    printf("Enter the name of the column to delete > ");
    char colName[STRING_LEN];

    if (fgets(colName, sizeof(colName), stdin) != NULL) {
        colName[strcspn(colName, "\n")] = '\0';
    }

    int found = 0;
    size_t index = 0;
    // search for the column
    for (size_t i = 0; i < (*currentDB)->numCols; i++) {
        if (strcmp(colName, (*currentDB)->cols[i].colName) == 0) {
            index = i;
            found = 1;
            break;
        }
    }

    if (found) {
        deleteColumn(*currentDB, index);
        printf("Successfully deleted column: '%s'\n", colName);

        // if there are no columns left, delete the rows
        if (!((*currentDB)->numCols)) {
            for (size_t j = (*currentDB)->numRows; j > 0; j--) {
                deleteRow(*currentDB, j-1);
            }
        }

    } else {
        printf("Column: '%s' not found.\n", colName);
    }
}

size_t safeReadSize() {

    char indexBuf[16];
    size_t index;

    if (fgets(indexBuf, sizeof(indexBuf), stdin) == NULL) {
        fprintf(stderr, "Input error.\n");
        return __SIZE_MAX__;
    }

    if (sscanf(indexBuf, "%zu", &index) != 1) {
        fprintf(stderr, "Invlaid input.\n");
        return __SIZE_MAX__; 
    }

    return index;
}
// Safely read an integer value from stdin
int safeReadInt(Database* currentDB, size_t rowValue, size_t colValue) {
    int tableValue;
    char valBuf[16];

    if (fgets(valBuf, sizeof(valBuf), stdin) == NULL) {
        fprintf(stderr, "Input error.\n");
        return -1;
    }

    if (sscanf(valBuf, "%d", &tableValue) != 1) {
        fprintf(stderr, "Invalid input.\n");
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

    if (sscanf(valBuf, "%f", &tableValue) != 1) {
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

void cmdSaveDbToFile(DatabaseList* dbl, Database** currentDB, char* name) {

    printf("Saving %s to a .csv file...\n", (*currentDB)->dbName);

    // Create a duplicate string to add the extension (if it doesnt have it)
    const char* csv = ".csv";

    size_t newSize = strlen((*currentDB)->dbName) + strlen(csv) + 1;

    char* fileName = malloc(newSize);

    if (!fileName) {
        fprintf(stderr, "Failed to allocate memory for filename\n");
        return;
    }

    strcpy(fileName, (*currentDB)->dbName);

    strcat(fileName, csv);

    saveDatabaseToCSV(*currentDB, fileName);

    free(fileName);
}

void cmdLoadDbFromFile(DatabaseList* dbl, Database** currentDB, char* name) {
    
    char csvName[STRING_LEN];

    printf("Enter the name of the .csv file to load > ");

    if (fgets(csvName, sizeof(csvName), stdin) != NULL) {
        csvName[strcspn(csvName, "\n")] = '\0';
    }

    if (dbl->dbCount < dbl->dbLimit) {

        Database* db = loadDatabaseFromCSV(csvName);

        if (db) {
            printf("Succesfully loaded Database: %s\n", csvName);
            addDatabaseToList(db, dbl);
            *currentDB = db;
        } else {
            printf("Unable to load: %s. File does not exist.\n", csvName);
            return;
        }

    } else {
        printf("Unable to load DB. Delete a database and try again.\n");
    }

    if (!currentDB)
        return;
}

// Allows user to change a column name
void cmdChangeColName(DatabaseList* dbl, Database** currentDB, char* name) {

    // Get the column to 
    printf("Enter the column name to change > ");
    char inputBuffer[STRING_LEN];

    if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
        inputBuffer[strcspn(inputBuffer, "\n ")] = '\0';
    }

    char newName[STRING_LEN];
    printf("Enter the new name > ");

    if (fgets(newName, sizeof(newName), stdin) != NULL) {
        newName[strcspn(newName, "\n")] = '\0';
    }

    changeColumnName(*currentDB, newName, inputBuffer);
}