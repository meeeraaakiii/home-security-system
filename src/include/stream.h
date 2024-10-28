#ifndef STREAM_H
#define STREAM_H

#include "crow.h"

void handle_main_page(const crow::request& req, crow::response& res, SessionMiddleware::context& ctx);

void serve_stream_files(const crow::request& req, crow::response& res, SessionMiddleware::context& ctx, const std::string& path);

#endif // STREAM_H
