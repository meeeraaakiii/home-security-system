#include <string>

std::string read_file(const std::string& path);

void serve_css_file(const crow::request& req, crow::response& res, const std::string& file_path);
