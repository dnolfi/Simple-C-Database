#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "database.h"

const char* data_types[] = {"INT", "FLOAT", "DOUBLE", "STRING"};

// Should initialize with zeros 
Database* createDatabase(const char* name) {

    Database* db = malloc(sizeof(Database));

    // Check if the pointer returned NULL
    if (db == NULL) {
        fprintf(stderr, "malloc returned NULL pointer for Database object\n");
        exit(1);
    }

    // Initialize Database struct members to 0 and NULL
    db->numRows = 0;
    db->numCols = 0;
    db->rows = NULL;
    db->cols = NULL;

    // Copy the db name and null terminate
    strncpy(db->dbName, name, STRING_LEN);
    db->dbName[STRING_LEN - 1] = '\0';

    return db;
}

// Create a column and add it to our Database object
void createColumn(Database* db, const char* name, DataTypes type) {

    // Need to reallocate memory for the new column object
    Column* newCols = realloc(db->cols, (db->numCols + 1)*sizeof(Column));

    if (!newCols) {
        fprintf(stderr, "realloc returned NULL pointer while reallocating memory for col array\n");
        exit(1);
    }

    db->cols = newCols;

    // Initialize our Column objects member variables
    db->cols[db->numCols].type = type;
    strncpy(db->cols[db->numCols].colName, name, STRING_LEN);
    db->cols[db->numCols].colName[STRING_LEN - 1] = '\0';

    db->numCols++;

    // If there are rows, we need to allocate memory for the new column cells by adding a cell to each row
    if (db->rows) {
        for (size_t i = 0; i < db->numRows; i++) {

            db->rows[i].cells = realloc(db->rows[i].cells, db->numCols * sizeof(Cell));

            if (!db->rows[i].cells) {
                fprintf(stderr, "realloc returned NULL pointer for Row object\n");
                exit(1);
            }

            if (type == INT_TYPE)
                db->rows[i].cells[db->numCols - 1].value.i = 0;
            else if (type == FLOAT_TYPE) 
                db->rows[i].cells[db->numCols - 1].value.f = 0.0f;
            else if (type == DOUBLE_TYPE) 
                db->rows[i].cells[db->numCols - 1].value.d = 0.00;
        }
    }
}

// Create a row of Cells for our Database
void createRow(Database* db) {

    // Allocate memory for a row
    Row* newRows = realloc(db->rows, (db->numRows + 1) * sizeof(Row));

    if (!newRows) {
        fprintf(stderr, "realloc returned NULL pointer for Row object\n");
        exit(1);
    }

    db->rows = newRows;

    // Initalize the memory in each cell to '0'
    db->rows[db->numRows].cells = calloc(db->numCols, sizeof(Cell));

    if ( db->rows[db->numRows].cells == NULL) {
        fprintf(stderr, "calloc returned NULL pointer for Cell object in Row");
        exit(1);
    }

    db->numRows++;
}

// Deletes a row and all its allocated Cells, resizes and reindxes the rows of the Database
void deleteRow(Database* db, size_t rowIndex) {

    if (rowIndex >= db->numRows) {
        fprintf(stderr, "Invalid row index.\n");
        return;
    }
    // Free the allocated memory for the cells in this row
    free(db->rows[rowIndex].cells);

    // Shift the row elements down 
    for (size_t index = rowIndex; index < db->numRows-1; index++) {
        db->rows[index] = db->rows[index + 1];
    }

    // Decrementing the number of rows in our Database
    db->numRows--;

    // Resize the array of rows
    if (db->numRows > 0) {

        Row* newRows = realloc(db->rows, db->numRows * sizeof(Row));

        if (!newRows) {
            fprintf(stderr, "realloc returned NULL pointer for Row object\n");
            exit(1);
        }

        db->rows = newRows;

    } else {
        free(db->rows);
        db->rows = NULL;
    }
}

void deleteColumn(Database* db, size_t columnIndex) {

    if (columnIndex >= db->numCols) {
        fprintf(stderr, "Invalid column to delete.\n");
        return;
    }

    // Need to remove the column cells from each row and shift
    for (size_t row = 0; row < db->numRows; row++) {

        for (size_t cell = columnIndex; cell < db->numCols -1; cell++) {
            db->rows[row].cells[cell] = db->rows[row].cells[cell+1];
        }

        if (db->numCols > 1) {
            // Reallocate memory for the number of cells in our row, since we just deleted a cell from each row
            Cell* newCells = realloc(db->rows[row].cells, (db->numCols - 1) * sizeof(Cell));

            if (!newCells) {
                fprintf(stderr, "Realloc failed when removing cell\n");
                exit(1);
            }

            db->rows[row].cells = newCells;

        } else {
            free(db->rows[row].cells);
            db->rows[row].cells = NULL;
        }
    }

    // Shift down the other columns
    for (size_t index = columnIndex; index < db->numCols- 1; index++) {
        db->cols[index] = db->cols[index + 1];
    }

    db->numCols--;

    // Reallocate the memory for our columns
    if (db->numCols > 0) {
        Column* newCols = realloc(db->cols, (db->numCols) * sizeof(Column));

        if (!newCols) {
            fprintf(stderr, "realloc returned NULL pointer for Column object\n");
            exit(1);
        }

        db->cols = newCols;
    }
    else {
        free(db->cols);
        db->cols = NULL;
    }

}

void deleteDatabase(Database* db) {

    if (!db) return;

    // Free the memory for our Rows and Cells
    if (db->rows) {
        for (size_t i = 0; i < db->numRows; i++) {
            free(db->rows[i].cells);
        }
        free(db->rows);
        db->rows = NULL;
        db->numRows = 0;
    }
    
    // Free the memory for our Column array
    if (db->cols) {
        free(db->cols);
        db->cols = NULL;
        db->numCols = 0;
    }

    // Free the memory for our database
    free(db);
}

int addInt(Database* db, size_t rowIndex, size_t colIndex, int value) {
    // Checking if the table index passed in is valid
    if (rowIndex >= db->numRows || colIndex >= db->numCols) {
        fprintf(stderr, "Index (%zu, %zu) is out of bounds. Valid range: rows 0-%zu, cols 0-%zu\n",
            rowIndex, colIndex, db->numRows - 1, db->numCols - 1);
        return -1;
    }

    if (db->cols[colIndex].type != INT_TYPE) {
        fprintf(stderr, "Type mismatch. Expected 'INT_TYPE'.\n");
        return -2;
    }

    db->rows[rowIndex].cells[colIndex].value.i = value;

    return 0;
}

int addFloat(Database* db, size_t rowIndex, size_t colIndex, float value) {
    // Checking if the table index passed in is valid
     if (rowIndex >= db->numRows || colIndex >= db->numCols) {
        fprintf(stderr, "Index (%zu, %zu) is out of bounds. Valid range: rows 0-%zu, cols 0-%zu\n",
            rowIndex, colIndex, db->numRows - 1, db->numCols - 1);
        return -1;
    }

    // First check if our passed value is compatible with the type at the index
    if (db->cols[colIndex].type != FLOAT_TYPE) {
        fprintf(stderr, "Type mismatch. Expected 'FLOAT_TYPE'.\n");
        return -1;
    }

    db->rows[rowIndex].cells[colIndex].value.f = value;
    return 0;
}

int addDouble(Database* db, size_t rowIndex, size_t colIndex, double value) {
    // Checking if the table index passed in is valid
     if (rowIndex >= db->numRows || colIndex >= db->numCols) {
        fprintf(stderr, "Index (%zu, %zu) is out of bounds. Valid range: rows 0-%zu, cols 0-%zu\n",
            rowIndex, colIndex, db->numRows - 1, db->numCols - 1);
        return -1;
    }

    // First check if our passed value is compatible with the type at the index
    if (db->cols[colIndex].type != DOUBLE_TYPE) {
        fprintf(stderr, "Type mismatch. Expected 'DOUBLE_TYPE'.\n");
        return -1;
    }

    db->rows[rowIndex].cells[colIndex].value.d = value;
    return 0;
}

void printDatabase(Database* db) {

    if (!(db->cols)) {
        printf("Database has no rows or columns.\n");
        return;
    }

    size_t width = db->numCols * 15 + db->numCols + 1;

    for (size_t i = 0; i < width; i++)
        printf("-");
    printf("\n");

    for (size_t i = 0; i < db->numCols; i++) {
        printf("|%-15s", db->cols[i].colName);
    }
    printf("|\n");

    for (size_t i = 0; i < width; i++)
        printf("-");
    printf("\n");

    for (size_t j = 0; j <db->numRows; j++) {
        for (size_t k = 0; k < db->numCols; k++) {
            if (db->cols[k].type == INT_TYPE)
                printf("|%-15d", db->rows[j].cells[k].value.i);
            else if (db->cols[k].type == FLOAT_TYPE) 
                printf("|%-15f", db->rows[j].cells[k].value.f);
            else if (db->cols[k].type == DOUBLE_TYPE) 
                printf("|%-15lf", db->rows[j].cells[k].value.d);
            else {
                printf("| haleem.");
            }
        }
        printf("|\n");
    }
}

// Loads column header and type from a csv
size_t loadColumnsFromCSV(Database* db, FILE* csvPtr) {

    size_t numCols = 0;

    char nameBuffer[1024];
    char typeBuffer[1024];

    // Read the headers (first line), tokenize the strings, then read the 
    fgets(nameBuffer, sizeof(nameBuffer), csvPtr);
    fgets(typeBuffer, sizeof(typeBuffer), csvPtr);

    char* nameSave; 
    char* typeSave;
    char* nameTokens = strtok_r(nameBuffer, ",", &nameSave);
    char* typeTokens = strtok_r(typeBuffer, ",", &typeSave);

    // Creating columns based on the name and type
    while (nameTokens && typeTokens) {

        // Trim new line characters
        nameTokens[strcspn(nameTokens, "\n")] = 0;
        typeTokens[strcspn(typeTokens, "\n")] = 0;

        if (strcmp("INT", typeTokens) == 0) {
            createColumn(db, nameTokens, INT_TYPE);
        }
        else if (strcmp("FLOAT", typeTokens) == 0) {
            createColumn(db, nameTokens, FLOAT_TYPE);
        }
        else if (strcmp("DOUBLE", typeTokens) == 0) {
            createColumn(db, nameTokens, DOUBLE_TYPE);
        }
        else {
            fprintf(stderr, "Error: unknown type '%s' for column %s\n", typeTokens, nameTokens);
        }

        numCols++;

        nameTokens = strtok_r(NULL, ",", &nameSave);
        typeTokens = strtok_r(NULL, ",", &typeSave);
    }

    return numCols;
}

// Loads rows from a csv 
size_t loadRowFromCSV(Database* db, FILE* csvPtr, size_t numCols) {

    size_t numRows = 0;
    size_t numTokens;

    char rowBuffer[1024];  

    char* valueBuffer;
    char* endPtr;

    // Parse the individual data values, convert to the required 
    while ((fgets(rowBuffer, sizeof(rowBuffer), csvPtr))) {

        // Create the row
        createRow(db);
    
        // Tokenize the values, add them to the row
        numTokens = 0; 

        valueBuffer = strtok(rowBuffer, ",");

        while (valueBuffer) {

            // Check that the number of tokens does not exceed the number of columns
            if (numTokens >= numCols) {
                fprintf(stderr, "Error: file has extraneous column values\n");
                return 0;
            }

            // Trim new lines
            valueBuffer[strcspn(valueBuffer, "\n")] = 0;

            // Check the column type and perform the correct conversion
            switch(db->cols[numTokens].type) {

                case INT_TYPE:
                    if (addInt(db, numRows, numTokens, strtol(valueBuffer, &endPtr, 10)) < 0) 
                        return 0;
                    break;
                case FLOAT_TYPE:
                    if (addFloat(db, numRows, numTokens, strtof(valueBuffer, &endPtr)) < 0)
                        return 0;
                    break;
                case DOUBLE_TYPE:
                    if (addDouble(db, numRows, numTokens, strtod(valueBuffer, &endPtr)) < 0)
                        return 0;
                    break;
                default:
                    printf("Unknown type.\n");
            }
            // Increase our number of tokens
            numTokens++;
            valueBuffer = strtok(NULL, ",");
        }
        if (numTokens != numCols) {
            fprintf(stderr, "Error: too few tokens to assign to columns.\n");
            return 0;
        }
        numRows++;
    }

    return numRows;
}

// Loads a database from a .csv file
// Should return a Database struct built with the .csv file
Database* loadDatabaseFromCSV(const char* fileName) {

    Database* db = createDatabase(fileName);

    size_t numCols = 0;
    size_t numRows = 0;

    char nameBuffer[1024];
    char typeBuffer[1024];

    // Open the .csv file
    FILE* csvPtr = fopen(fileName, "r");

    if (!csvPtr) {
        fprintf(stderr, "Unable to open file: %s\n", fileName);
        return NULL;
    }

    numCols = loadColumnsFromCSV(db, csvPtr);
    numRows = loadRowFromCSV(db, csvPtr, numCols);

    if (!numRows) {
        fprintf(stderr, "Error: could not read CSV rows.\n");
    }
    
    return db;
}

// Saves the current database to a .csv file
// This needs to be cleaned up
void saveDatabaseToCSV(Database* db, const char* fileName) {

    // Our file to create
    FILE* csvPtr = fopen(fileName, "w");

    // Exit if we can't open the file
    if (!csvPtr) {
        fprintf(stderr, "Unable to create file: %s\n", fileName);
        fclose(csvPtr);
        return;
    }

    // Write the column headers to the file
    for (size_t col = 0; col < db->numCols; col++) {
        // TODO: First special case, check if the string contains a comma, if so, enclose the string in double quotes (add when string support added)

        fprintf(csvPtr, "%s", db->cols[col].colName);
    
        if (col < db->numCols - 1)
            // Before the last column, seperate the names by commas
            fputc(',', csvPtr);
        else 
            // Last column, replace the comma with a newline
            fputc('\n', csvPtr);
    }

    for (size_t col = 0; col < db->numCols; col++) {
        fprintf(csvPtr, "%s", data_types[db->cols[col].type]);
        if (col < db->numCols - 1)
            // Before the last column, seperate the names by commas
            fputc(',', csvPtr);
        else 
            // Last column, replace the comma with a newline
            fputc('\n', csvPtr);
    }

    for (size_t r = 0; r < db->numRows; r++) {
        for (size_t c = 0; c < db->numCols - 1; c++) {
            // Need to check the data type
            switch(db->cols[c].type) {
                case INT_TYPE :     
                    fprintf(csvPtr, "%d,", db->rows[r].cells[c].value.i); 
                    break;
                case FLOAT_TYPE :   
                    fprintf(csvPtr, "%f,", db->rows[r].cells[c].value.f); 
                    break;
                case DOUBLE_TYPE :  
                    fprintf(csvPtr, "%lf,", db->rows[r].cells[c].value.d); 
                    break;
                default :   
                    fprintf(stderr, "Unknown type, cannot write to file.\n");
                    return;
            }
        }
        // Repetitive, i should change this
        switch(db->cols[db->numCols-1].type) {
            case INT_TYPE :     
                fprintf(csvPtr, "%d\n", db->rows[r].cells[db->numCols-1].value.i); 
                break;
            case FLOAT_TYPE :   
                fprintf(csvPtr, "%f\n", db->rows[r].cells[db->numCols-1].value.f); 
                break;
            case DOUBLE_TYPE :  
                fprintf(csvPtr, "%lf\n", db->rows[r].cells[db->numCols-1].value.d); 
                break;
            default :   
                fprintf(stderr, "Unknown type, cannot write to file.\n");
                fclose(csvPtr);
                return;
        }

    }

    fclose(csvPtr);
}

void changeColumnName(Database* db, char* newName, char* column) {

    if (!db) {
        printf("No database selected.\n");
        return;
    }

    int found = 0;
    size_t index = 0;

    // Search for the index
    for (size_t i = 0; i < db->numCols; i++) {
        if (strcmp(db->cols[i].colName, column) == 0) {
            index = i;
            found = 1;
            break;
        }
    }
    // Change the name if the specified column is found
    if (found) {
        printf("Changing column: '%s' to '%s.\n", db->cols[index].colName, newName);
        strncpy(db->cols[index].colName, newName, STRING_LEN);
    } else {
        printf("Column: '%s' not found.\n", column);
    }

}
// Calculate the average value of a row
double calculateRowAverage(Database* db) {

}

// Calculate the average value of a column
double calculateColAverage(Database* db) {

}