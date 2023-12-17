#include "threadpool/thread_pool.h"

#include "utils/macros.h"

namespace shagit {

thread_local ThreadPool *current_pool;

ThreadPool::ThreadPool(size_t threads) : threads_cnt_(threads) {}

void ThreadPool::Start()
{
    workers_done_.store(false);
    workers_.reserve(threads_cnt_);
    for (size_t thread = 0; thread < threads_cnt_; ++thread) {
        workers_.emplace_back([this] { Work(); });
    }
}

ThreadPool::~ThreadPool()
{
    ASSERT(workers_done_);
}

void ThreadPool::Submit(Task task)
{
    if (task_queue_.Put(std::move(task))) {
        uncompleted_tasks_.Add(1);
    }
}

ThreadPool *ThreadPool::Current()
{
    return current_pool;
}

void ThreadPool::WaitIdle()
{
    uncompleted_tasks_.Wait();
}

void ThreadPool::Stop()
{
    workers_done_.store(true);
    task_queue_.Close();
    for (auto &&worker : workers_) {
        worker.join();
    }
}

void ThreadPool::Work()
{
    while (!workers_done_.load()) {
        auto task = task_queue_.Take();
        if (task != std::nullopt) {
            current_pool = this;
            try {
                (*task)();
                uncompleted_tasks_.Done();
            } catch (...) {
                std::abort();
            }
            current_pool = nullptr;
        }
    }
}

}  // namespace shagit
