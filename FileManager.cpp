#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

class FileManager {
private:
    fs::path currentPath;

    void displayHelp() {
        cout << "\nAvailable Commands:\n";
        cout << "ls              - List files and directories\n";
        cout << "cd <path>       - Change directory\n";
        cout << "mkdir <name>    - Create new directory\n";
        cout << "cp <src> <dest> - Copy file\n";
        cout << "mv <src> <dest> - Move file\n";
        cout << "rm <name>       - Remove file\n";
        cout << "pwd             - Print working directory\n";
        cout << "help            - Show this help message\n";
        cout << "exit            - Exit program\n";
    }

    void listDirectory() {
        try {
            cout << "\nContents of " << currentPath << ":\n";
            vector<fs::directory_entry> items;
            
            for (const auto& entry : fs::directory_iterator(currentPath)) {
                items.push_back(entry);
            }
            
            // Sort items (directories first, then files)
            sort(items.begin(), items.end(), 
                [](const fs::directory_entry& a, const fs::directory_entry& b) {
                    if (fs::is_directory(a) && !fs::is_directory(b)) return true;
                    if (!fs::is_directory(a) && fs::is_directory(b)) return false;
                    return a.path().filename() < b.path().filename();
                });
            
            // Display items
            for (const auto& entry : items) {
                string prefix = fs::is_directory(entry) ? "[DIR] " : "[FILE] ";
                cout << prefix << entry.path().filename().string() << "\n";
            }
        } catch (const fs::filesystem_error& e) {
            cout << "Error listing directory: " << e.what() << endl;
        }
    }

    void changeDirectory(const string& path) {
        try {
            fs::path newPath;
            if (path == "..") {
                newPath = currentPath.parent_path();
            } else if (path[0] == '/') {
                newPath = path;
            } else {
                newPath = currentPath / path;
            }

            if (fs::exists(newPath) && fs::is_directory(newPath)) {
                currentPath = fs::canonical(newPath);
                cout << "Changed directory to: " << currentPath << endl;
            } else {
                cout << "Directory does not exist: " << newPath << endl;
            }
        } catch (const fs::filesystem_error& e) {
            cout << "Error changing directory: " << e.what() << endl;
        }
    }

    void createDirectory(const string& name) {
        try {
            fs::path newDir = currentPath / name;
            if (fs::create_directory(newDir)) {
                cout << "Created directory: " << newDir << endl;
            } else {
                cout << "Failed to create directory" << endl;
            }
        } catch (const fs::filesystem_error& e) {
            cout << "Error creating directory: " << e.what() << endl;
        }
    }

    void copyFile(const string& source, const string& dest) {
        try {
            fs::path sourcePath = currentPath / source;
            fs::path destPath = currentPath / dest;

            if (!fs::exists(sourcePath)) {
                cout << "Source file does not exist" << endl;
                return;
            }

            fs::copy(sourcePath, destPath, fs::copy_options::overwrite_existing);
            cout << "Copied " << source << " to " << dest << endl;
        } catch (const fs::filesystem_error& e) {
            cout << "Error copying file: " << e.what() << endl;
        }
    }

    void moveFile(const string& source, const string& dest) {
        try {
            fs::path sourcePath = currentPath / source;
            fs::path destPath = currentPath / dest;

            if (!fs::exists(sourcePath)) {
                cout << "Source file does not exist" << endl;
                return;
            }

            fs::rename(sourcePath, destPath);
            cout << "Moved " << source << " to " << dest << endl;
        } catch (const fs::filesystem_error& e) {
            cout << "Error moving file: " << e.what() << endl;
        }
    }

    void removeFile(const string& name) {
        try {
            fs::path path = currentPath / name;
            if (fs::exists(path)) {
                fs::remove_all(path);
                cout << "Removed: " << name << endl;
            } else {
                cout << "File or directory does not exist" << endl;
            }
        } catch (const fs::filesystem_error& e) {
            cout << "Error removing file: " << e.what() << endl;
        }
    }

public:
    FileManager() {
        currentPath = fs::current_path();
    }

    void start() {
        string command;
        cout << "\nUmer File Manager\n";
        cout << "Type 'help' for available commands\n";

        while (true) {
            cout << "\n" << currentPath << "> ";
            getline(cin, command);

            if (command.empty()) continue;

            // Parse command and arguments
            vector<string> args;
            size_t pos = 0;
            string token;
            while ((pos = command.find(" ")) != string::npos) {
                token = command.substr(0, pos);
                args.push_back(token);
                command.erase(0, pos + 1);
            }
            args.push_back(command);

            string cmd = args[0];

            if (cmd == "exit") {
                break;
            } else if (cmd == "help") {
                displayHelp();
            } else if (cmd == "ls") {
                listDirectory();
            } else if (cmd == "pwd") {
                cout << currentPath << endl;
            } else if (cmd == "cd" && args.size() > 1) {
                changeDirectory(args[1]);
            } else if (cmd == "mkdir" && args.size() > 1) {
                createDirectory(args[1]);
            } else if (cmd == "cp" && args.size() > 2) {
                copyFile(args[1], args[2]);
            } else if (cmd == "mv" && args.size() > 2) {
                moveFile(args[1], args[2]);
            } else if (cmd == "rm" && args.size() > 1) {
                removeFile(args[1]);
            } else {
                cout << "Invalid command. Type 'help' for available commands." << endl;
            }
        }
    }
};

int main() {
    FileManager fm;
    fm.start();
    return 0;
}