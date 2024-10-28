#include "crow.h"

#include <chrono>
#include <shared_mutex>
#include <unordered_map>

// Define the Session struct
struct Session {
    std::string username;
    std::chrono::system_clock::time_point expires;
};

// Global variables for session management
extern std::unordered_map<std::string, Session> sessions;
extern std::shared_mutex session_mutex;  // for thread-safe access to sessions

void add_session(const std::string& session_id, const std::string& username, int cookie_max_age);

bool is_session_valid(const std::string& session_id);

std::string get_cookie_value(const std::string& cookies, const std::string& cookie_name);

void serve_login_page(const crow::request& req, crow::response& res, crow::mustache::context ctx);

void login_page_handler(const crow::request& req, crow::response& res);

struct SessionMiddleware {
    struct context {
        std::string session_id;
        bool is_logged_in = false;
    };

    void before_handle(crow::request& req, crow::response&, context& ctx) {
        std::string cookie_header = req.get_header_value("Cookie");
        std::cout << "Cookie header: '" << cookie_header << "'" << std::endl;

        // Retrieve the session_id cookie
        ctx.session_id = get_cookie_value(cookie_header, "session_id");
        std::cout << "Session ID: '" << ctx.session_id << "'" << std::endl;

        // Check if the session ID is valid
        bool this_session_is_valid = is_session_valid(ctx.session_id);
        ctx.is_logged_in = this_session_is_valid;
    }

    void after_handle(crow::request&, crow::response&, context&) {}
};
