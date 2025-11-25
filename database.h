#ifndef DATABASE_H
#define DATABASE_H

#define STRING_LEN 30
#define DB_LIMIT 16

#include <stddef.h>

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

void createColumn(Database* db, const char* name, DataTypes type);
void createRow(Database* db);
void deleteDatabase(Database* db);
void deleteRow(Database* db, size_t rowIndex);
void deleteColumn(Database* db, size_t columnIndex);
void printDatabase(Database* db);

int addInt(Database* db, size_t rowIndex, size_t colIndex, int value);
int addFloat(Database* db, size_t rowIndex, size_t colIndex, float value);
int addDouble(Database* db, size_t rowIndex, size_t colIndex, double value);

#endif