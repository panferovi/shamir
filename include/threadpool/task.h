#ifndef THREADPOOL_TASK_H
#define THREADPOOL_TASK_H

#include <functional>

namespace shamir {

using Task = std::function<void()>;

}  // namespace shamir

#endif  // THREADPOOL_TASK_H
