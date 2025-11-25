#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "database.h"

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

// Loads a database from a file
void loadDatabase(FILE* file) {

}

// Saves the current database to a file
void saveDatabase(Database* db) {

}

// Calculate the average value of a row
double calculateRowAverage(Database* db) {

}

// Calculate the average value of a column
double calculateColAverage(Database* db) {

}