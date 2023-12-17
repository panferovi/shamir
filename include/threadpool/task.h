#ifndef THREADPOOL_TASK_H
#define THREADPOOL_TASK_H

#include <functional>

namespace shagit {

using Task = std::function<void()>;

}  // namespace shagit

#endif  // THREADPOOL_TASK_H
