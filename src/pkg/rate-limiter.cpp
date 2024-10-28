#include "rate-limiter.h"

void RateLimitMiddleware::init(int maxRequests, int timeWindowSeconds) {
    maxRequests_ = maxRequests;
    timeWindow_ = timeWindowSeconds;
}

void RateLimitMiddleware::before_handle(crow::request& req, crow::response& res, context& ctx) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto clientIP = req.remote_ip_address;
    auto now = std::chrono::steady_clock::now();

    // Clean up expired entries
    for (auto it = requestTimes_.begin(); it != requestTimes_.end();) {
        if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second.first) >= std::chrono::seconds(timeWindow_)) {
            it = requestTimes_.erase(it);
        } else {
            ++it;
        }
    }

    auto& [firstRequestTime, count] = requestTimes_[clientIP];
    
    // Reset if the time window has passed
    if (std::chrono::duration_cast<std::chrono::seconds>(now - firstRequestTime) >= std::chrono::seconds(timeWindow_)) {
        firstRequestTime = now;
        count = 0;
    }

    // Check if request count exceeds the limit
    if (count >= maxRequests_) {
        res.code = 429;  // Too Many Requests
        res.body = "Rate limit exceeded. Try again later.";
        res.end();
        return;
    }

    // Increment the request count
    count++;
}

void RateLimitMiddleware::after_handle(crow::request& req, crow::response& res, context& ctx) {}
