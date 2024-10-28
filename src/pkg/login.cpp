#include "login.h"
#include "config.h"

#include "crow.h"

// Global variables for session management
std::unordered_map<std::string, Session> sessions;
std::shared_mutex session_mutex;  // for thread-safe access to sessions

// Function to add a session
void add_session(const std::string& session_id, const std::string& username, int cookie_max_age) {
    std::unique_lock lock(session_mutex);  // exclusive lock for writing
    sessions[session_id] = {username, std::chrono::system_clock::now() + std::chrono::seconds(cookie_max_age/2)};
    // sessions[session_id] = {username, std::chrono::system_clock::now() + std::chrono::hours(cookie_max_age)};
}

// Function to check if a session is valid
bool is_session_valid(const std::string& session_id) {
    std::shared_lock lock(session_mutex);  // shared lock for reading
    auto session_iterator = sessions.find(session_id);
    // sessions.end() is a special iterator that represents one past the last element in the map.
    // If session equals sessions.end(), it means the session ID wasn't found.
    if (session_iterator != sessions.end()) {
        Session session = session_iterator->second;
        // Check if session has expired
        bool expired = std::chrono::system_clock::now() >= session.expires;
        if (expired) {
            std::cout << "Session has expired" << std::endl;
        } else {
            std::cout << "Session did not expire yet" << std::endl;
        }
        return !expired;
    }
    std::cout << "Session is not valid" << std::endl;
    return false;
}

std::string get_cookie_value(const std::string& cookies, const std::string& cookie_name) {
    std::unordered_map<std::string, std::string> cookie_map;
    std::istringstream stream(cookies);
    std::string token;
    while (std::getline(stream, token, ';')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string name = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            // Trim leading and trailing whitespace from name and value
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            cookie_map[name] = value;
        }
    }
    // Find and return the specific cookie value
    auto it = cookie_map.find(cookie_name);
    return it != cookie_map.end() ? it->second : "";
}

// cookie max age is in hours
std::string get_expiration_time(int cookie_max_age) {
    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Add 1 hour to the current time
    auto expiration_time = now + std::chrono::seconds(cookie_max_age);
    // auto expiration_time = now + std::chrono::hours(cookie_max_age);

    // Convert to time_t for formatting
    std::time_t expiration_time_t = std::chrono::system_clock::to_time_t(expiration_time);

    // Format as "Wed, 15 Nov 2024 12:00:00 GMT"
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&expiration_time_t), "%a, %d %b %Y %H:%M:%S GMT");

    std::cout << "Setting expiration time for a cookie: '" << ss.str() << "'" << std::endl;

    return ss.str();
}

std::unordered_map<std::string, std::string> parse_urlencoded_body(const std::string& body) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream stream(body);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        auto pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            params[key] = value;
        }
    }
    return params;
}

std::string generate_pseudo_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << dist(gen) << "-";
    ss << std::setw(4) << (dist(gen) & 0xFFFF) << "-";
    ss << std::setw(4) << ((dist(gen) & 0x0FFF) | 0x4000) << "-";
    ss << std::setw(4) << ((dist(gen) & 0x3FFF) | 0x8000) << "-";
    ss << std::setw(12) << dist(gen);

    return ss.str();
}

void serve_login_page(const crow::request& req, crow::response& res, crow::mustache::context ctx) {
    // Load and render the template
    auto rendered_template = crow::mustache::load("login.html").render(ctx);
    res.write(rendered_template.body_);
    res.end();
}

void login_page_handler(const crow::request& req, crow::response& res) {
    std::cout << req.body << std::endl;
    auto params = parse_urlencoded_body(req.body);
    auto submitted_username = params.find("username") != params.end() ? params["username"] : "";
    auto submitted_password = params.find("password") != params.end() ? params["password"] : "";
    std::string session_id = generate_pseudo_uuid();

    for (const auto& user : config.at("users")) {
        std::string username = user.at("username");
        std::string password = user.at("password");
        if (submitted_username == username && submitted_password == password) {
            std::cout << "Username and password correct!" << std::endl;
            std::string expiration_time = get_expiration_time(config.at("cookie_max_age"));
            add_session(session_id, username, config.at("cookie_max_age"));
            res.add_header(
                "Set-Cookie",
                "session_id=" + session_id + "; "+
                "HttpOnly; "+
                "Path=/; "+
                "Expires=" + get_expiration_time(config.at("cookie_max_age"))
            );
            // set status code to 303 to tells the client to redirect with a GET request.
            res.code = 303;
            // manually set location header
            res.set_header("Location", "/");
            res.end();
            return;
        }
    }
    crow::mustache::context ctx;
    ctx["Error"] = "Invalid username or password";
    ctx["Username"] = submitted_username;
    serve_login_page(req, res, ctx);
}
