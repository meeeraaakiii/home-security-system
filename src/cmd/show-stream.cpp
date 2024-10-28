#include "crow.h"
// #include "crow/mustache.h"

#include "files.h"
#include "login.h"
#include "stream.h"

int main() {
    crow::App<SessionMiddleware> app;
    // app.loglevel(crow::LogLevel::Debug);
    crow::mustache::set_global_base("src/assets/html");

    // Serve CSS files from the "src/assets/static/css" directory
    CROW_ROUTE(app, "/static/css/<string>")([](const crow::request& req, crow::response& res, std::string file_path) {
        serve_css_file(req, res, file_path);
    });
    // Login routes
    CROW_ROUTE(app, "/login").methods("GET"_method)([](const crow::request& req, crow::response& res) {
        crow::mustache::context ctx;
        serve_login_page(req, res, ctx);
    });
    CROW_ROUTE(app, "/login").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        login_page_handler(req, res);
    });
    // Assuming `app` is available as a shared_ptr or you can capture a reference_wrapper
    // CROW_ROUTE(app, "/").methods("GET"_method)([app_ref = std::ref(app)](const crow::request& req, crow::response& res) {
    CROW_ROUTE(app, "/").methods("GET"_method)([&app](const crow::request& req, crow::response& res) {
        // SessionMiddleware::context& ctx = app_ref.get().get_context<SessionMiddleware>(req);
        SessionMiddleware::context& ctx = app.get_context<SessionMiddleware>(req);
        handle_main_page(req, res, ctx);
    });
    // HLS file serving route
    CROW_ROUTE(app, "/<string>")([&app](const crow::request& req, crow::response& res, const std::string& path) {
        // Retrieve the middleware context
        SessionMiddleware::context& ctx = app.get_context<SessionMiddleware>(req);
        serve_stream_files(req, res, ctx, path);
    });

    app.port(8080).multithreaded().run();
}