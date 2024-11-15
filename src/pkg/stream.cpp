#include "config.h"
#include "login.h"

#include "crow.h"

void handle_main_page(const crow::request& req, crow::response& res, SessionMiddleware::context& ctx) {
    // Retrieve the middleware context
    if (!ctx.is_logged_in) {
        res.redirect("/login");
        res.end();
        return;
    }

    // Load and render the template
    crow::mustache::context context;
    auto rendered_template = crow::mustache::load("stream.html").render(context);
    res.write(rendered_template.body_);
    res.end();
}

void serve_stream_files(const crow::request& req, crow::response& res, SessionMiddleware::context& ctx, const std::string& path) {
    // Retrieve the middleware context
    if (!ctx.is_logged_in) {
        res.redirect("/login");
        res.end();
        return;
    }

    std::string file_path = static_cast<std::string>(config.at("recordings_dir")) + "/" + path;
    std::ifstream in(file_path, std::ios::binary);

    if (!in) {
        res.code = 404;
        res.write("File not found");
        res.end();
        return;
    }

    if (path.find(".m3u8") != std::string::npos) {
        res.set_header("Content-Type", "application/vnd.apple.mpegurl");
    } else if (path.find(".ts") != std::string::npos) {
        res.set_header("Content-Type", "video/MP2T");
    } else if (path.find(".mp4") != std::string::npos) {
        res.set_header("Content-Type", "video/mp4");
    }

    // Read the file contents into the response directly
    res.write(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
    in.close();
    res.end();
}
