#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <atomic>
#include <vector>

#include "threadpool/queue.h"
#include "threadpool/task.h"
#include "threadpool/wait_group.h"
#include "utils/macros.h"

namespace shagit {

// Fixed-size pool of worker threads
class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    NO_COPY_SEMANTIC(ThreadPool);
    NO_MOVE_SEMANTIC(ThreadPool);

    // Launches worker threads
    void Start();

    // Schedules task for execution in one of the worker threads
    void Submit(Task);

    // Locates current thread pool from worker thread
    static ThreadPool *Current();

    // Waits until outstanding work count reaches zero
    void WaitIdle();

    // Stops the worker threads as soon as possible
    void Stop();

private:
    void Work();

private:
    const size_t threads_cnt_;
    std::vector<std::thread> workers_;
    UnboundedBlockingQueue<Task> task_queue_;
    WaitGroup uncompleted_tasks_;
    std::atomic<bool> workers_done_ {true};
};

}  // namespace shagit

#endif  // THREADPOOL_H
