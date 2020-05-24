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
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#endif


namespace zru
{

#if defined(ZRU_POSIX)

void shrmem::close()
{
    // Unmap memory
    if ((void*)-1 != m_pMem)
    {   munmap(m_pMem, m_sz);
        m_pMem = (void*)-1;
    }
    m_sz = 0;

    // Close the share link if we created it
    if (!m_bExisting && 0 < m_sFile.length())
    {   shm_unlink(m_sFile.c_str());
        m_sFile.clear();
    }

    // Close the file handle
    if ((void*)-1 != m_fd)
    {   ::close((int)(std::intptr_t)m_fd);
        m_fd = (void*)-1;
    }

    m_bExisting = false;
}


bool shrmem::open(const t_str &sFile, int64_t sz, bool bCreate)
{
    int permissions = 0777;

    close();

    // Validate file name
    if (0 >= sFile.length() || zruCHR('/') != sFile[0])
        return false;

    // Validate size
    if (0 >= sz)
        return false;

    // Try to open existing
    m_fd = (void*)(std::intptr_t)shm_open(sFile.c_str(), O_RDWR, permissions);
    if ((void*)-1 == m_fd)
    {
        // Do we want to create one?
        if (!bCreate)
            return false;

        // Try to create a new share
        m_fd = (void*)(std::intptr_t)shm_open(sFile.c_str(), O_RDWR | O_CREAT, permissions);
        if ((void*)-1 == m_fd)
            return false;

        // Set the size
        if (0 > ftruncate((int)(std::intptr_t)m_fd, sz))
        {   close();
            return false;
        }
    }

    // Share already exists
    else
        m_bExisting = true;

    // Save file name
    m_sFile = sFile;

    // Map the shared memory
    m_pMem = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_SHARED, (int)(std::intptr_t)m_fd, 0);
    if ((void *)-1 == m_pMem)
    {   close();
        return false;
    }

    // Save share size
    m_sz = sz;

    return true;
}

#endif


} // end namespace
