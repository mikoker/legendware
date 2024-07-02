#include "includes.h"
#include "data.h"
#include "globals.h"

#include <deque>
#include <mutex>
#include <thread>
#include <future>
#include <semaphore>
#include <algorithm>
#include <memory>
#include <functional>
#include <map>
#include <queue>
#include <any>

// credits
// https://habr.com/ru/post/656515/
// https://github.com/bshoshany/thread-pool

enum class task_status_t
{
    in_q,
    completed
};

// C++ 17
class c_task
{
public:
    template <typename func_ret_type, typename ...fargs, typename ...func_types>
    c_task(func_ret_type(*func)(func_types...), fargs&&... args) :
        is_void{ std::is_void_v<func_ret_type> }
    {
        if constexpr (std::is_void_v<func_ret_type>)
        {
            void_func = std::bind(func, args...);
            any_func = []()->int { return 0; };
        }
        else {
            void_func = []()->void {};
            any_func = std::bind(func, args...);
        }
    }

    void operator() ()
    {
        void_func();
        any_func_result = any_func();
    }

    bool has_result()
    {
        return !is_void;
    }

    std::any get_result() const
    {
        //assert(!is_void);
       // assert(any_func_result.has_value());
        return any_func_result;
    }

private:
    std::function<void()> void_func;
    std::function<std::any()> any_func;
    std::any any_func_result;
    bool is_void;
};

struct task_info_t
{
    task_status_t status = task_status_t::in_q;
    std::any result;
};

class ThreadPool
{
private:
    std::vector<std::thread> threads;

    std::queue<std::pair<c_task, uint64_t>> q;
    std::mutex q_mtx;
    std::condition_variable q_cv;

    std::unordered_map<uint64_t, task_info_t> tasks_info;
    std::condition_variable tasks_info_cv;
    std::mutex tasks_info_mtx;

    std::condition_variable wait_all_cv;

    std::atomic<bool> quite{ false };
    std::atomic<std::uint64_t> last_idx{ 0 };
    std::atomic<std::uint64_t> cnt_completed_tasks{ 0 };

    using allocate_thread_id_fn = std::int32_t(*)();
    using free_thread_id_fn = void(*)();

    allocate_thread_id_fn allocate_thread_id;
    free_thread_id_fn free_thread_id;

    void run()
    {
        allocate_thread_id();

        while (!quite)
        {
            std::unique_lock<std::mutex> lock(q_mtx);
            q_cv.wait(lock, [this]()->bool { return !q.empty() || quite; });

            if (!q.empty() && !quite)
            {
                std::pair<c_task, uint64_t> task = std::move(q.front());
                q.pop();
                lock.unlock();

                task.first();

                std::lock_guard<std::mutex> lock(tasks_info_mtx);
                if (task.first.has_result()) {
                    tasks_info[task.second].result = task.first.get_result();
                }
                tasks_info[task.second].status = task_status_t::completed;
                ++cnt_completed_tasks;
            }
            wait_all_cv.notify_all();
            tasks_info_cv.notify_all(); // notify for wait function
        }

        free_thread_id();
    }

public:
    int get_threads_count(HMODULE tier0)
    {
        if (tier0)
        {
            auto tier0_threads = (bool*)((uintptr_t)tier0 + 0x54E30);
            auto count = 1;

            for (auto i = 1; i < 128; ++i)
            {
                if (!tier0_threads[i])
                    break;

                ++count;
            }

            return clamp((int)std::thread::hardware_concurrency(), 1, 32 - count - 1);
        }

        return 3;
    }

    void init()
    {
        auto tier0 = GetModuleHandleA(crypt_str("tier0.dll"));

        allocate_thread_id = (allocate_thread_id_fn)GetProcAddress(tier0, crypt_str("AllocateThreadID"));
        free_thread_id = (free_thread_id_fn)GetProcAddress(tier0, crypt_str("FreeThreadID"));

        const auto& num_threads = get_threads_count(tier0);

        threads.reserve(num_threads);
        for (int i = 0; i < num_threads; ++i)
            threads.emplace_back(&ThreadPool::run, this);
    }

    void remove()
    {
        quite = true;
        q_cv.notify_all();
        for (int i = 0; i < threads.size(); ++i)
            threads[i].join();
    }

    ~ThreadPool()
    {
        remove();
    }

    template <typename Func, typename ...fargs, typename ...func_types>
    std::uint64_t add_task(Func(*func)(func_types...), fargs&&... args)
    {
        const std::uint64_t task_id = last_idx++;

        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info[task_id] = task_info_t();
        lock.unlock();

        std::lock_guard<std::mutex> q_lock(q_mtx);
        q.emplace(c_task(func, std::forward<fargs>(args)...), task_id);
        q_cv.notify_one();
        return task_id;
    }

    void wait(const std::uint64_t task_id)
    {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool
            {
                return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
            });
    }

    std::any wait_result(const std::uint64_t task_id)
    {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool
            {
                return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
            });
        return tasks_info[task_id].result;
    }

    template<class T>
    void wait_result(const std::uint64_t task_id, T& value)
    {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool
            {
                return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
            });
        value = std::any_cast<T>(tasks_info[task_id].result);
    }

    void wait_all()
    {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        wait_all_cv.wait(lock, [this]()->bool { return cnt_completed_tasks == last_idx; });
    }

    bool calculated(const std::uint64_t task_id)
    {
        std::lock_guard<std::mutex> lock(tasks_info_mtx);
        return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
    }
};

extern crypt_ptr<ThreadPool> thread_pool;