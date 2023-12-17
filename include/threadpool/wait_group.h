#ifndef THREADPOOL_WAIT_GROUP_H
#define THREADPOOL_WAIT_GROUP_H

#include <mutex>
#include <cstdlib>
#include <condition_variable>

namespace shagit {

class WaitGroup {
public:
    void Add(size_t count)
    {
        std::lock_guard lock(mutex_);
        if (count_ += count; count_ == 0) {
            is_zero_.notify_all();
        }
    }

    void Done()
    {
        std::lock_guard lock(mutex_);
        if (--count_; count_ == 0) {
            is_zero_.notify_all();
        }
    }

    void Wait()
    {
        std::unique_lock lock(mutex_);
        is_zero_.wait(lock, [&count = count_] { return count == 0; });
    }

private:
    size_t count_ {0};
    std::mutex mutex_;
    std::condition_variable is_zero_;
};

}  // namespace shagit

#endif  // THREADPOOL_WAIT_GROUP_H
