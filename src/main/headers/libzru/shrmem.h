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

namespace zru
{

class shrmem
{

public:

    /// Default constructor
    shrmem() : m_fd((void*)-1), m_pMem((void*)-1), m_sz(0), m_bExisting(false) {}

    /// Default destructor
    ~shrmem() { close(); }

    /// Close shared memory
    void close();

    /// Create a new shared memory region
    bool open(const t_str &sFile, int64_t sz, bool bCreate = true);

    /// Returns non-zero if the share already exists
    bool isExisting() { return m_bExisting; }

    /// Returns a pointer to the share
    void* ptr() { return ((void*)-1 != m_pMem) ? m_pMem : 0; }

    /// Returns a char* to the share
    char* str() { return (char*)(((void*)-1 != m_pMem) ? m_pMem : 0); }

    /// Returns the shares memory size
    int64_t size() { return m_sz;}

    /// Returns the shares file name
    t_str getFileName() { return m_sFile; }

private:

    /// Share name
    t_str               m_sFile;

    /// File handle
    void*               m_fd;

    /// Pointer to the memory
    void*               m_pMem;

    /// Size of the memory buffer
    int64_t             m_sz;

    /// Sets to non zero if the share already exists
    bool                m_bExisting;

};

} // end namespace
