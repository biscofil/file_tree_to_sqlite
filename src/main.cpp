#include <string>
#include <filesystem>
#include "sqliteManager.h"

namespace fs = std::filesystem;

/**
 *
 * @param path
 * @param sql
 * @param parentID
 */
void analyzeFolder(std::string path, SqliteManager &sql, int parentID) {
    for (const auto &entry : fs::directory_iterator(path)) {
        auto fn = entry.path().filename();
        if (entry.is_directory()) {
            sql.log(fn, -1, parentID);
            auto ID = sql.getLastInsertedID();
            analyzeFolder(entry.path(), sql, ID);
        } else {
            sql.log(fn, entry.file_size(), parentID);
        }
    }
}

int main() {

    char filename[1024];
    FILE *f = popen("zenity --file-selection --directory", "r");
    fgets(filename, 1024, f);
    std::string path = filename;

    path.erase(path.find_last_not_of(" \n\r\t")+1);
    std::cout << "Reading '" << path << "'" << std::endl;

    SqliteManager sqliteManager("test.db");

    sqliteManager.log(path, -1, -1);
    auto ID = sqliteManager.getLastInsertedID();

    analyzeFolder(path, sqliteManager, ID);
}