#include "login.h"

#include "crow.h"

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
    if (!submitted_username.empty() && !submitted_password.empty()) {
        std::cout << "Username: " << submitted_username << std::endl;
        std::cout << "Password: " << submitted_password << std::endl;
    } else {
        std::cout << "Username or password is missing." << std::endl;
    }
    std::string session_id = generate_pseudo_uuid();
    std::cout << "Generated Pseudo-UUID: " << session_id << std::endl;

    if (submitted_username == username && submitted_password == password) {
        std::cout << "Username and password correct!" << std::endl;
        // res.add_header("Set-Cookie", "session_id="+session_id+"; HttpOnly; Path=/");
        res.add_header("Set-Cookie", "session_id=valid_session; HttpOnly; Path=/");
        // set status code to 303 to tells the client to redirect with a GET request.
        res.code = 303;
        // manually set location header
        res.set_header("Location", "/");
        res.end();
    } else {
        crow::mustache::context ctx;
        ctx["Error"] = "Invalid username or password";
        ctx["Username"] = submitted_username;
        serve_login_page(req, res, ctx);
    }
}
