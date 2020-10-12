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
            sql.insertFolderRecord(fn, -1, parentID);
            auto ID = sql.getLastInsertedID();
            analyzeFolder(entry.path(), sql, ID);
        } else {
            sql.insertFileRecord(fn, entry.file_size(), parentID);
        }
    }
}

std::string selectSourceFolder() {
    char filename[1024];
    FILE *f = popen(R"(zenity --file-selection --directory --title="Select a folder to scan")", "r");
    fgets(filename, 1024, f);
    std::string out = filename;
    out.erase(out.find_last_not_of(" \n\r\t") + 1); // remove last char
    return out;
}

std::string selectDestinationFile() {
    char filename[1024];
    FILE *f = popen(R"(zenity --file-selection --filename="out.db" --save --title="Select the output .db file")", "r");
    fgets(filename, 1024, f);
    std::string out = filename;
    out.erase(out.find_last_not_of(" \n\r\t") + 1); // remove last char
    return out;
}

int main() {

    std::string sourceFolderPath = selectSourceFolder();
    std::string destinationFilePath = selectDestinationFile();

    std::cout << "Reading '" << sourceFolderPath <<
              "' and saving into '" << destinationFilePath << "'" << std::endl;

    SqliteManager sqliteManager(destinationFilePath);

    sqliteManager.insertFolderRecord(sourceFolderPath, -1, -1);
    auto ID = sqliteManager.getLastInsertedID();

    analyzeFolder(sourceFolderPath, sqliteManager, ID);
}