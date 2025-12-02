#ifndef DATABASE_H
#define DATABASE_H

#define STRING_LEN 30
#define DB_LIMIT 16

#include <stddef.h>

extern const char* data_types[];

typedef enum {

    INT_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE

} DataTypes;

typedef union {

    int i;
    float f;
    double d;
    char* s;

} DataValues;

typedef struct {

    DataValues value;

} Cell;

typedef struct {

    DataTypes type;
    char colName[STRING_LEN];

} Column;

typedef struct {

    Cell* cells;

} Row;

typedef struct {

    Column* cols;
    Row* rows;
    size_t numRows;
    size_t numCols;
    char dbName[STRING_LEN];

} Database;

Database* createDatabase(const char* name);
Database* loadDatabaseFromCSV(const char* fileName);

void createColumn(Database* db, const char* name, DataTypes type);
void createRow(Database* db);
void deleteDatabase(Database* db);
void deleteRow(Database* db, size_t rowIndex);
void deleteColumn(Database* db, size_t columnIndex);
void printDatabase(Database* db);
void saveDatabaseToCSV(Database* db, const char* fileName);
void changeColumnName(Database* db, char* newName, char* column);

int addInt(Database* db, size_t rowIndex, size_t colIndex, int value);
int addFloat(Database* db, size_t rowIndex, size_t colIndex, float value);
int addDouble(Database* db, size_t rowIndex, size_t colIndex, double value);

size_t loadColumnsFromCSV(Database* db, FILE* csvPtr);
size_t loadRowFromCSV(Database* db, FILE* csvPtr, size_t numCols);

#endif