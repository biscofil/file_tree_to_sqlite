//
// Created by filippo on 12/10/20.
//

#ifndef FOLDER_LIST_SQLITE_MANAGER_H
#define FOLDER_LIST_SQLITE_MANAGER_H

#include <sqlite3.h>
#include <iostream>

/**
 *
 */
class SqliteManager {
private:
    sqlite3 *db{};
    char *zErrMsg = 0;
    int rc;

    inline std::string formatInt(int i) const {
        return i < 0 ? "NULL" : std::to_string(i);
    }

public:

    /**
     *
     * @param name
     */
    SqliteManager(const std::string &name) {
        open(name);
        create_table();
    }

    /**
     *
     * @param name
     * @return
     */
    int open(const std::string &name) {
        rc = sqlite3_open(name.c_str(), &db);
        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        }
        return rc;
    }

    /**
     *
     * @return
     */
    int create_table() {
        /* Create SQL statement */
        std::string sql = "CREATE TABLE files(id INTEGER PRIMARY KEY AUTOINCREMENT, name text NOT NULL, pid INT, size INT);";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        return rc;
    }

    /**
     *
     * @param name
     * @param size
     * @param paren_int
     * @return
     */
    void log(std::string name, int size, int parent_int) {

        /* Create SQL statement */
        std::string sql = "INSERT INTO files (name, pid, size) VALUES ('" + name + "', "
                          + formatInt(parent_int) + ", "
                          + formatInt(size) + ");";

        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }

    /**
     *
     * @return
     */
    int getLastInsertedID() {
        sqlite3_stmt *stmt = NULL;
        rc = sqlite3_prepare_v2(db, "SELECT last_insert_rowid() as id", -1, &stmt, NULL);
        rc = sqlite3_step(stmt);
        int ID = sqlite3_column_int(stmt, 0);
        rc = sqlite3_finalize(stmt);
        return ID;
    }

    /**
     *
     */
    ~SqliteManager() {
        sqlite3_close(db);
    }

};

#endif //FOLDER_LIST_SQLITE_MANAGER_H
