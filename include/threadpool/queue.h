#ifndef THREADPOOL_QUEUE_H
#define THREADPOOL_QUEUE_H

#include <mutex>
#include <deque>
#include <optional>
#include <condition_variable>

namespace shagit {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue
template <typename T>
class UnboundedBlockingQueue {
public:
    bool Put(T element)
    {
        std::lock_guard lock(mutex_);
        if (is_closed_) {
            return false;
        }
        queue_.push_back(std::move(element));
        is_empty_or_closed_.notify_one();
        return true;
    }

    std::optional<T> Take()
    {
        std::unique_lock lock(mutex_);
        is_empty_or_closed_.wait(lock, [this] { return !queue_.empty() || is_closed_; });
        if (is_closed_ && queue_.empty()) {
            return std::nullopt;
        }
        auto element = std::optional(std::move(queue_.front()));
        queue_.pop_front();
        return element;
    }

    void Close()
    {
        std::lock_guard lock(mutex_);
        is_closed_ = true;
        is_empty_or_closed_.notify_all();
    }

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable is_empty_or_closed_;
    bool is_closed_ {false};
};

}  // namespace shagit

#endif  // THREADPOOL_QUEUE_H
