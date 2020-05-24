/*------------------------------------------------------------------
// Copyright (c) 2020
// Robert Umbehant
// libzru@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace zru
{

class signal
{
public:

    /// Default constructor
    signal() { m_b = false; }

    /// Default destructor
    ~signal() {}

    /// Release one thread
    void signal_one() { { std::unique_lock<std::mutex> lk(m_m); m_b = true; } m_cv.notify_one(); }

    /// Release all threads
    void signal_all() { { std::unique_lock<std::mutex> lk(m_m); m_b = true; } m_cv.notify_all(); }

    /// Resets the signal
    void reset() { std::unique_lock<std::mutex> lk(m_m); m_b = false; }

    /// Wait for specified time
    bool wait_ms(long lMs)
    {
        std::unique_lock<std::mutex> lk(m_m);
        if (m_b)
            return true;
        if (0 < lMs)
            m_cv.wait_for(lk, std::chrono::milliseconds(lMs));
        return m_b;
    }

private:

    /// Mutex
    std::mutex                  m_m;

    /// Condition
    std::condition_variable     m_cv;

    /// Variable
    volatile bool               m_b;
};

class worker_thread
{
public:

    /// Shared pointer
    typedef std::shared_ptr<zru::worker_thread> sptr;

    /// Function type
    typedef std::function< int () > pfn_Worker;

    /// Default constructor
    worker_thread();

    /// Default destructor
    ~worker_thread();

    /// Construct with single run function
    worker_thread(pfn_Worker fRun, bool bStart = true);

    /// Construct with init, run, and end functions
    worker_thread(pfn_Worker fInit, pfn_Worker fRun, pfn_Worker fEnd, bool bStart = true);

public:

    /// Initialize threads
    static void init();

public:

    /// Starts the thread
    bool start();

    /// Signals the thread to stop, does not wait for the thread
    bool stop();

    /// Signals the thread to stop and waits for the thread to exit
    bool join(int nMsTimeout = -1);

    /// Returns non-zero if the thread is running
    bool isRunning() { return m_pThread ? m_pThread->joinable() : false; }

    /// Non-zero if the thread should be running
    bool wantRun() { return m_bRun; }

    /// Waits for the specified interval, but returns early if the threads run status changes
    bool wait(int nMs);

    /// Attempt to abort thread
    bool abort();

private:

    /// Called only by the thread
    void Run();

private:

    class throw_abort {};

#   if __linux__

    static void worker_thread_sig(int sig);

#   endif

    /// Function to inject
    static void throw_exception() noexcept(false);

    /// Prevent compiler from optimizing out the try / catch blocks
    static void init_exceptions();

private:

    /// The thread object
    std::thread                 *m_pThread;

    /// Thread memory
    char                        m_vThread[sizeof(std::thread)];

    /// Init function
    pfn_Worker                  m_fInit;

    /// Run function
    pfn_Worker                  m_fRun;

    /// End function
    pfn_Worker                  m_fEnd;

    /// Non-zero if the thread should run
    bool                        m_bRun;

    /// Set to non-zero when the thread is quitting
    bool                        m_bQuitting;

    /// Thread mutex
    std::mutex                  m_mLock;

    /// Signals the thread
    std::condition_variable     m_cvLock;
};

} // end namespace
