#include "crow.h"

#include <string>
#include <fstream>
#include <sstream>

std::unordered_map<std::string, std::string> templates;
std::unordered_map<std::string, std::string> css;

// Helper function to read file content
std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";  // Return empty if the file doesn't exist
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void serve_css_file(const crow::request& req, crow::response& res, const std::string& file_path) {
    std::string full_path = "src/assets/static/css/" + file_path;

    std::string content = read_file(full_path);
    if (content.empty()) {
        res.code = 404;
        res.end("File not found: " + full_path);
    } else {
        res.write(content);
        res.end();
    }
}
