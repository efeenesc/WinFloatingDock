#include "basictimer.h"

namespace WinFloatingDock {
    namespace timer {

        void BasicTimer::ThreadTask() {
            while (running) {
                std::unique_lock<std::mutex> lock(mtx);
                if (cv.wait_for(lock, std::chrono::milliseconds(sleepDuration), [this] { return !running.load(); })) {
                    return; // Cancelled
                }

                if (running)
                    callbackFunc();

                if (!loop)
                    break;
            }
        }

        BasicTimer::BasicTimer(std::function<void()> callback, uint32_t sleep_ms, bool shouldLoop)
            : callbackFunc(callback),
            sleepDuration(sleep_ms),
            running(false),
            loop(shouldLoop) {
        }

        void BasicTimer::Start() {
            running = true;
            if (thisThread.joinable())
                thisThread.join();

            thisThread = std::thread(&BasicTimer::ThreadTask, this);
        }

        void BasicTimer::Cancel() {
            running = false;
            cv.notify_all();
            if (thisThread.joinable()) {
                thisThread.join();
            }
        }

        BasicTimer::~BasicTimer() {
            Cancel();
        }
    }
}