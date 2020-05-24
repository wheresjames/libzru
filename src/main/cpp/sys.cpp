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
#   include <unistd.h>
#   include <signal.h>
#elif defined(ZRU_WINDOWS)
#   include <windows.h>
#endif


namespace zru
{

static volatile int *g_fCount = 0;
static void ctrl_c_handler(int s)
{
    if (!g_fCount || 8 < (*g_fCount))
    {   ZruError("~ ctrl-c ~");
        exit(1);
    }

    (*g_fCount)++;
    ZruWarning("~ ctrl-c ~");
}


//-------------------------------------------------------------------
#if defined(ZRU_POSIX)

void install_ctrl_c_handler(volatile int *fCount)
{
    // Save the flag location
    g_fCount = fCount;

    // Install the signal handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, 0);
}


//-------------------------------------------------------------------
#elif defined(ZRU_WINDOWS)

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    if (CTRL_C_EVENT != fdwCtrlType)
        return FALSE;

    ctrl_c_handler(0);

    return TRUE;
}

void install_ctrl_c_handler(volatile int *fCount)
{
    // Save the flag location
    g_fCount = fCount;

    // Set ctrl handler
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
}

#endif


} // end namespace
