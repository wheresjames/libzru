/*------------------------------------------------------------------
// Copyright (c) 2020
// Robert Umbehant
// winglib@wheresjames.com
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

#include "libzru.h"

#if defined(ZRU_POSIX)
#   include <signal.h>
#endif

namespace zru
{

worker_thread::~worker_thread()
{
    join();
}

worker_thread::worker_thread()
{
    m_pThread = 0;
    m_fInit = 0;
    m_fRun = 0;
    m_fEnd = 0;
    m_bRun = false;
}

worker_thread::worker_thread(pfn_Worker fRun, bool bStart)
    : worker_thread()
{
    m_fRun = fRun;

    if (bStart)
        start();
}

worker_thread::worker_thread(pfn_Worker fInit, pfn_Worker fRun, pfn_Worker fEnd, bool bStart)
    : worker_thread()
{
    m_fInit = fInit;
    m_fRun = fRun;
    m_fEnd = fEnd;

    if (bStart)
        start();
}

bool worker_thread::wait(int nMs)
{
    if (0 >= nMs)
        return false;

    std::unique_lock<std::mutex> lk(m_mLock);

    if (!m_bRun)
        return false;

    return std::cv_status::timeout == m_cvLock.wait_for(lk, std::chrono::milliseconds(nMs));
}

void worker_thread::Run()
{
    init();

    try
    {
        // Convince compiler that an exception might happen
        worker_thread::init_exceptions();

        if (m_fInit)
            if (0 > m_fInit())
                return;

        if (m_fRun)
            do
            {
                // Call the run function
                int nDelay = m_fRun();

                // Does the callee want to quit?
                if (0 > nDelay)
                    m_bRun = false;

                // Do they want a delay?
                else if (0 < nDelay)
                    wait(nDelay);

            } while (m_bRun);

        if (m_fEnd)
            m_fEnd();

    }

    catch(throw_abort)
    {
    }

    // Notify that we're leaving
    {
        std::unique_lock<std::mutex> lk(m_mLock);
        m_bQuitting = true;
    }

    // Wake main thread if they are waiting for us to quit
    m_cvLock.notify_all();
}

bool worker_thread::start()
{
    if (m_pThread)
        return true;

    m_bRun = true;
    m_bQuitting = false;

    // Create thread
    m_pThread = new (m_vThread) std::thread([this](){ Run(); });

    return true;
}

bool worker_thread::stop()
{
    if (!m_pThread)
        return true;

    // Signal the thread to stop, and wake it up if needed
    {
        std::unique_lock<std::mutex> lk(m_mLock);
        m_bRun = false;
    }

    m_cvLock.notify_all();

    return true;
}

bool worker_thread::join(int nMsTimeout)
{
    if (!m_pThread)
        return true;

    stop();

    // Are we waiting for the thread to signal that it's quitting
    if (0 < nMsTimeout)
    {
        std::unique_lock<std::mutex> lk(m_mLock);
        if (!m_bQuitting)
            return std::cv_status::timeout == m_cvLock.wait_for(lk, std::chrono::milliseconds(nMsTimeout));
    }

    m_pThread->join();

    m_pThread->~thread();
    m_pThread = 0;

    m_fInit = 0;
    m_fRun = 0;
    m_fEnd = 0;

    return true;
}

bool worker_thread::abort()
{
    if (!m_pThread)
        return false;

#if defined(ZRU_WINDOWS)

    bool bSuccess = false;
    HANDLE h = m_pThread->native_handle();
    if (INVALID_HANDLE_VALUE == h)
        return false;

    if (INFINITE == SuspendThread(h))
        return false;

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    if (GetThreadContext(h, &ctx))
    {
#   if defined( _WIN64 )
        ctx.Rip = (DWORD)(DWORD_PTR)throw_exception;
#   else
        ctx.Eip = (DWORD)(DWORD_PTR)throw_exception;
#   endif

        bSuccess = SetThreadContext(h, &ctx) ? true : false;
    }

    ResumeThread(h);

    return bSuccess;

#elif defined(ZRU_POSIX)

    pthread_kill(m_pThread->native_handle(), SIGUSR2);

#endif

    return false;
}

void worker_thread::throw_exception() noexcept(false)
{
    throw throw_abort();
}

void worker_thread::init_exceptions()
{
    volatile int i = 0;
    if (i)
        throw_exception();
}

#if defined(ZRU_POSIX)

void worker_thread::worker_thread_sig(int sig)
{
    if(SIGUSR2 == sig)
        throw throw_abort();
}

#endif

void worker_thread::init()
{
#if defined(ZRU_POSIX)

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = worker_thread_sig;
    sigaction(SIGUSR2, &sa, 0);

#endif
}

} // end namespace
