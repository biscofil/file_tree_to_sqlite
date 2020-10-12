//
// Created by filippo on 12/10/20.
//

#ifndef FOLDER_LIST_SQLITE_MANAGER_H
#define FOLDER_LIST_SQLITE_MANAGER_H

#include <sqlite3.h>
#include <iostream>
#include <utility>
#include <vector>

inline std::string formatInt(int i) {
    return i < 0 ? "NULL" : std::to_string(i);
}

class FileRecord {
public:
    std::string name;
    int size;
    int pid;

    FileRecord(std::string n, int s, int p) : name(std::move(n)), size(s), pid(p) {}

    std::string getSqlValuesStr() const {
        return "('" + name + "', " + formatInt(size) + ", " + formatInt(pid) + ")";
    }

};

/**
 *
 */
class SqliteManager {
private:

    int flush_count = 5000;
    std::vector<FileRecord> files;

    sqlite3 *db{};
    char *zErrMsg = 0;
    int rc;

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
        std::string sql = "CREATE TABLE files(id INTEGER PRIMARY KEY AUTOINCREMENT, name text NOT NULL, size INT);";
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            fprintf(stdout, "%s", sql.c_str());
            sqlite3_free(zErrMsg);
            return rc;
        }

        sql = "ALTER TABLE files ADD COLUMN pid INT REFERENCES files(id);";
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            fprintf(stdout, "%s", sql.c_str());
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
    void insertFolderRecord(std::string name, int size, int parent_int) {

        /* Create SQL statement */
        std::string sql = "INSERT INTO files (name, pid, size) VALUES ('" + name + "', "
                          + formatInt(size) + ", "
                          + formatInt(parent_int) + ");";

        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            fprintf(stdout, "%s", sql.c_str());
            sqlite3_free(zErrMsg);
        }
    }

    void insertFileRecord(std::string name, int size, int parent_int) {
        files.emplace_back(name, size, parent_int);
        if (files.size() > flush_count) {
            flushFileRecords();
        }
    }

    void flushFileRecords() {
        if (files.empty()) {
            return;
        }
        std::string sql = "INSERT INTO files (name, pid, size) VALUES ";
        bool first = true;
        for (auto r : files) {
            if (first) {
                first = false;
            } else {
                sql += ",";
            }
            sql += r.getSqlValuesStr();
        }
        sql += ";";
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            fprintf(stdout, "%s", sql.c_str());
            sqlite3_free(zErrMsg);
            return;
        }
        files.clear();

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
        flushFileRecords();
        sqlite3_close(db);
    }

};

#endif //FOLDER_LIST_SQLITE_MANAGER_H
