#include "crow.h"

void serve_login_page(const crow::request& req, crow::response& res, crow::mustache::context ctx);

void login_page_handler(const crow::request& req, crow::response& res);

struct SessionMiddleware {
    struct context {
        bool is_logged_in = false;
    };

    void before_handle(crow::request& req, crow::response&, context& ctx) {
        auto cookie_header = req.get_header_value("Cookie");
        if (cookie_header.find("session_id=valid_session") != std::string::npos) {
            ctx.is_logged_in = true;
        }
    }

    void after_handle(crow::request&, crow::response&, context&) {}
};
