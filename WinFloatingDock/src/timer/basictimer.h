#pragma once

#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace WinFloatingDock {
    namespace timer {

        class BasicTimer {
        private:
            std::thread thisThread;
            std::function<void()> callbackFunc;
            uint32_t sleepDuration;
            std::atomic<bool> running;
            bool loop;

            std::condition_variable cv;
            std::mutex mtx;

            void ThreadTask();

        public:
            BasicTimer(std::function<void()> callback, uint32_t sleep_ms, bool shouldLoop);
            void Start();
            void Cancel();
            ~BasicTimer();
        };
    }
}