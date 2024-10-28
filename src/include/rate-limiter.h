#ifndef RATELIMITER_H
#define RATELIMITER_H

#include "crow.h"
#include <unordered_map>
#include <chrono>
#include <mutex>

class RateLimitMiddleware {
public:
    struct context {};

    // Default constructor
    RateLimitMiddleware() = default;

    // Initialization function to set maxRequests and timeWindow
    void init(int maxRequests, int timeWindowSeconds);

    void before_handle(crow::request& req, crow::response& res, context& ctx);
    void after_handle(crow::request& req, crow::response& res, context& ctx);

private:
    int maxRequests_ = 0;
    int timeWindow_ = 0;
    std::unordered_map<std::string, std::pair<std::chrono::steady_clock::time_point, int>> requestTimes_;
    std::mutex mutex_;
};

#endif // RATELIMITER_H
