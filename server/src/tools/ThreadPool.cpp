#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
: num_threads_(num_threads), stop_all(false) {
    worker_threads_.reserve(num_threads_);
    threadState = new int[num_threads_];
    for(size_t i = 0; i < num_threads_; ++i) {

        worker_threads_.emplace_back([this, i]() { this->WorkerThread(i); });
    }
}

void ThreadPool::WorkerThread(size_t threadNum) {
    while (true) {
        std::unique_lock<std::mutex> lock(m_job_q_);
        cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });
        if (stop_all && this->jobs_.empty()) {
            return;
        }

        // 맨 앞의 job 을 뺀다.
        std::function<void()> job = std::move(jobs_.front());
        jobs_.pop();
        lock.unlock();
        this->threadState[threadNum] = 1;
        job();
        this->threadState[threadNum] = 0;
    }
}

ThreadPool::~ThreadPool() {
    stop_all = true;
    cv_job_q_.notify_all();

    for (auto& t : worker_threads_) {
        t.join();
    }
    delete this->threadState;
}

void ThreadPool::EnqueueJob(std::function<void()> job) {
    if (stop_all) {
        throw std::runtime_error("ThreadPool 사용 중지됨");
    }
    {
        std::lock_guard<std::mutex> lock(m_job_q_);
        jobs_.push(std::move(job));
    }
    cv_job_q_.notify_one();
}

bool ThreadPool::checkAllThread(){
    for(int i =0; i < int(this->num_threads_);++i){
        if(this->threadState[i])return false;
    }

    return true;
}