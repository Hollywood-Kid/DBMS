#include "../include/database.h"
#include "../include/list.h"

DataBase* create_database(const char *database_name, DataBase *db) {
    db->name = strdup(database_name);
    db->table_count = 0;
    db->tables = NULL;
    db->tuples_limit = 0;

    return db;
}

void add_table_to_database(DataBase *db, const char *table_name) {
    db->table_count++;
    db->tables = realloc(db->tables, db->table_count * sizeof(Table));
    db->tables[db->table_count - 1].table_name = strdup(table_name);
    db->tables[db->table_count - 1].columns = NULL;
    db->tables[db->table_count - 1].column_count = 0;
}

void add_column_to_table(Table *table, const char *column_name) {
    table->column_count++;
    table->columns = realloc(table->columns, table->column_count * sizeof(Column));
    table->columns[table->column_count - 1].name = strdup(column_name);
    table->columns[table->column_count - 1].data = NULL;
}

void add_data_to_table(Table *table, List *list) {
    for (size_t i = 0; i < table->column_count; i++) {
        DataNode *new_node = malloc(sizeof(DataNode));

        new_node->data = strdup(get_element_at(list, i));
        new_node->next = NULL;

        if (table->columns[i].data == NULL) {
            table->columns[i].data = new_node;
        } 
        
        else {
            DataNode *current = table->columns[i].data;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_node;
        }
    }

    table->row_count++;
}

Table* get_table(DataBase *db, const char *table_name) {
    for (size_t i = 0; i < db->table_count; i++) {
        if (strcmp(db->tables[i].table_name, table_name) == 0) {
            return &db->tables[i];
        }
    }

    return NULL;
}

List* get_row_in_table(Table *table, size_t row_index) {
    if (row_index >= table->row_count) {
        return NULL;
    }

    List *row_data = new_list();

    if (row_data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < table->column_count; i++) {
        DataNode *current = table->columns[i].data;
        for (size_t j = 0; j < row_index && current != NULL; j++) {
            current = current->next;
        }

        if (current != NULL) {
            push_back(row_data, strdup(current->data));
        }
    }

    return row_data;
}

int get_column_index(Table *table, const char *column_name) {
    for (size_t i = 0; i < table->column_count; i++) {
        if (strcmp(table->columns[i].name, column_name) == 0) {
            return i;
        }
    }
    return -1;
}

void delete_row(Table *table, size_t row_index) {
    for (size_t j = 0; j < table->column_count; j++) {
        DataNode *prev = NULL;
        DataNode *current = table->columns[j].data;
        
        for (size_t k = 0; k < row_index && current != NULL; k++) {
            prev = current;
            current = current->next;
        }

        if (prev != NULL) {
            prev->next = current->next;
        } else {
            table->columns[j].data = current->next;
        }

        free(current->data);
        free(current);
    }

    table->row_count--;
}

void free_column_data(Column *column) {
    DataNode *current = column->data;
    while (current != NULL) {
        DataNode *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    column->data = NULL;
}

void free_table_data(Table *table) {
    for (size_t i = 0; i < table->column_count; i++) {
        free_column_data(&table->columns[i]);
    }
    table->row_count = 0;
}