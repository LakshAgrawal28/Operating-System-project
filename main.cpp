#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cctype>
#include <utility>

namespace fs = std::filesystem;

using namespace std;

struct DirItem {
    string name;
    fs::path path;
    bool isDir = false;
    uintmax_t size = 0; // size for files only
};

static void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c); });
    return s;
}

static vector<DirItem> listDirectory(const fs::path& p) {
    vector<DirItem> out;
    try {
        for (auto& entry : fs::directory_iterator(p)) {
            DirItem it;
            it.path = entry.path();
            it.name = entry.path().filename().string();
            it.isDir = entry.is_directory();
            if (!it.isDir) {
                error_code ec;
                it.size = entry.file_size(ec);
            }
            out.push_back(move(it));
        }
        // Sort: directories first, then files, by name (case-insensitive)
        sort(out.begin(), out.end(), [](const DirItem& a, const DirItem& b){
            if (a.isDir != b.isDir) return a.isDir > b.isDir;
            return toLower(a.name) < toLower(b.name);
        });
    } catch (const exception& ex) {
        cout << "Error listing directory: " << ex.what() << "\n";
    }
    return out;
}

static void printDirectory(const fs::path& p, const vector<DirItem>& items) {
    cout << "\nCurrent directory:\n  " << p.string() << "\n\n";
    if (items.empty()) {
        cout << "(empty)\n";
        return;
    }
    cout << left << setw(5) << "#" << setw(6) << "TYPE" << setw(12) << "SIZE" << "NAME" << "\n";
    cout << string(60, '-') << "\n";
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& it = items[i];
        cout << left << setw(5) << (i+1)
             << setw(6) << (it.isDir ? "DIR" : "FILE")
             << setw(12);
        if (it.isDir) {
            cout << "-";
        } else {
            cout << it.size;
        }
        cout << it.name << "\n";
    }
}

static bool showFilePreview(const fs::path& filePath, size_t maxLines = 200) {
    ifstream in(filePath, ios::binary);
    if (!in) {
        cout << "Could not open file: " << filePath.string() << "\n";
        return false;
    }
    // Light heuristic: if first 4KB contains a null byte, warn it's likely binary
    {
        char buf[4096];
        in.read(buf, sizeof(buf));
        streamsize n = in.gcount();
        bool hasNull = any_of(buf, buf + n, [](char c){ return c == '\0'; });
        in.clear();
        in.seekg(0, ios::beg);
        if (hasNull) {
            cout << "(Binary file preview suppressed)\n";
            return true;
        }
    }

    cout << "\n----- File: " << filePath.filename().string() << " -----\n";
    string line;
    size_t count = 0;
    while (count < maxLines && getline(in, line)) {
        cout << line << "\n";
        ++count;
    }
    if (in && !in.eof()) {
        cout << "... (truncated after " << maxLines << " lines)\n";
    }
    cout << "----- End of preview -----\n";
    return true;
}

static void fileExplorer() {
    fs::path current = fs::current_path();
    while (true) {
        auto items = listDirectory(current);
        printDirectory(current, items);

        cout << "\nFile Explorer Menu:\n";
        cout << "  1) Open directory by #\n";
        cout << "  2) View file by #\n";
        cout << "  3) Go up (..)\n";
        cout << "  4) Change directory by path\n";
        cout << "  5) Refresh\n";
        cout << "  0) Back to Main Menu\n";
        cout << "> ";
        int choice = -1;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) return;
        else if (choice == 1) {
            cout << "Enter item # to open (directory): ";
            size_t idx; if (!(cin >> idx)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (idx >= 1 && idx <= items.size() && items[idx-1].isDir) {
                current = items[idx-1].path;
            } else {
                cout << "Invalid directory selection.\n";
                pressEnterToContinue();
                cin.get();
            }
        } else if (choice == 2) {
            cout << "Enter item # to view (file): ";
            size_t idx; if (!(cin >> idx)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (idx >= 1 && idx <= items.size() && !items[idx-1].isDir) {
                showFilePreview(items[idx-1].path);
                pressEnterToContinue();
                cin.get();
            } else {
                cout << "Invalid file selection.\n";
                pressEnterToContinue();
                cin.get();
            }
        } else if (choice == 3) {
            if (current.has_parent_path()) current = current.parent_path();
        } else if (choice == 4) {
            cout << "Enter path: ";
            string pathStr; getline(cin, pathStr);
            fs::path np = fs::path(pathStr);
            error_code ec;
            if (fs::exists(np, ec) && fs::is_directory(np, ec)) {
                current = fs::canonical(np, ec);
                if (ec) current = np; // fallback if canonical fails
            } else {
                cout << "Not a directory: " << np.string() << "\n";
                pressEnterToContinue();
                cin.get();
            }
        } else if (choice == 5) {
            // refresh implicitly by continuing
        } else {
            cout << "Unknown choice.\n";
        }
    }
}

int main() {
    // Directly launch File Explorer only
    cout << "\n==== File Explorer (C++) ====\n";
    fileExplorer();
    cout << "\nExiting File Explorer. Goodbye!\n";
    return 0;
}
