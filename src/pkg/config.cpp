#include "json.h"

#include "config.h"

#include <string>
#include <fstream>
#include <iostream>

nlohmann::json config;  // Definition of config

void load_config_file(const std::string& path) {
    // Open the JSON config file
    std::ifstream config_file(path);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open config file at " << path << std::endl;
        exit(1);
    }

    try {
        config_file >> config;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        exit(1);
    }
}
