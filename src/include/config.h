#ifndef CONFIG_H
#define CONFIG_H

#include "json.h"

extern nlohmann::json config;

void load_config_file(const std::string& path);

#endif // CONFIG_H
