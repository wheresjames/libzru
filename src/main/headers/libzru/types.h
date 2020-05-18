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

#pragma once

namespace zru
{
    typedef char        t_char;
    typedef wchar_t     t_wchar;

#   define tcTT(c, s)      ( (sizeof(t_char)==sizeof(c)) \
                                ? ((c*)(s)) \
                                : (sizeof(t_wchar)==sizeof(c)) \
                                  ? ((c*)(L##s)) \
                                  : s \
                            )
#   define tcTC(c, s)      ( (sizeof(t_char)==sizeof(c)) \
                                ? ((c)(s)) \
                                : (sizeof(t_wchar)==sizeof(c)) \
                                  ? ((c)(L##s)) \
                                  : s \
                        )
#   define tcTTEXT(c, s)   tcTT(c, s)

    // Byte array
    typedef std::vector<char> vector;

    // Strings
    typedef std::string string;
    typedef std::wstring wstring;
    typedef std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strcnv;

    typedef std::string t_str;
    typedef std::wstring t_wstr;
    typedef std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> t_wstrcnv;

    template<typename T>
        static T& r2l(T &&r)
        {   return r;
        }

    inline t_str::size_type& strpos(t_str::size_type &&r) { return r; }

#   define CONSOLE_BLACK        "\033[90m"
#   define CONSOLE_RED          "\033[91m"
#   define CONSOLE_GREEN        "\033[92m"
#   define CONSOLE_YELLOW       "\033[93m"
#   define CONSOLE_BLUE         "\033[94m"
#   define CONSOLE_MAGENTA      "\033[95m"
#   define CONSOLE_CYAN         "\033[96m"
#   define CONSOLE_WHITE        "\033[97m"
#   define CONSOLE_BOLD         "\033[1m"
#   define CONSOLE_FAINT        "\033[2m"
#   define CONSOLE_ITALIC       "\033[3m"
#   define CONSOLE_UNDERLINE    "\033[4m"
#   define CONSOLE_BLINK        "\033[5m"
#   define CONSOLE_NAGATIVE     "\033[7m"
#   define CONSOLE_STRIKEOUT    "\033[9m"
#   define CONSOLE_DEFAULT      "\033[0m"

#   define ZruLog(...) std::cout << zru::str::filename<zru::string>(__FILE__) \
                                 << ":" << __FUNCTION__ \
                                 << "(" << __LINE__ << "): " \
                                 << zru::str::join<zru::string>("", __VA_ARGS__) \
                                 << std::endl;

#   define ZruLevel(level, ...) std::cout << zru::str::filename<zru::string>(__FILE__) \
                                          << ":" << __FUNCTION__ \
                                          << "(" << __LINE__ << "): " \
                                          << zru::str::console_log_level_str<zru::string>(level) << " " \
                                          << zru::str::join<zru::string>("", __VA_ARGS__) \
                                          << std::endl;

#   define ZruError(...)    ZruLevel(1, __VA_ARGS__)
#   define ZruWarning(...)  ZruLevel(2, __VA_ARGS__)
#   define ZruInfo(...)     ZruLevel(3, __VA_ARGS__)
}

