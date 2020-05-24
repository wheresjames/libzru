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

namespace zru::str
{
#   define zruCHECK_MAX(s, m) m = (0 > m || t_str::npos == m || m > s.length()) ? s.length() : m
#   define zruCHR(ch) tcTC(t_char, ch)
#   define zruTXT(ch) tcTT(t_char, ch)

    /** Converts string positions to a length
        @param[in]      start       - Start position
        @param[in]      end         - End position

        @returns Length or t_str::npos
    */
    template<typename T>
        T str2len(T start, T end)
    {
        if (start >= end || 0 > start || 0 > end
            || string::npos == start || string::npos == end)
            return string::npos;
        return end - start;
    }

    // Trim strings
    template<typename t_str>
        static t_str& ltrim(t_str &s, const t_str &t)
        {   s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&t](int ch)
            {   return t_str::npos == t.find(ch);
            }));
            return s;
        }
    template<typename t_str>
        static t_str& rtrim(t_str &s, const t_str &t)
        {   s.erase(std::find_if(s.rbegin(), s.rend(), [&t](int ch)
            {   return t_str::npos == t.find(ch);
            }).base(), s.end());
            return s;
        }
    template<typename t_str>
        static t_str& trim(t_str &s, const t_str &t)
        {   ltrim(s, t);
            rtrim(s, t);
            return s;
        }
    template<typename t_str>
        static t_str ltrim_copy(t_str s, const t_str &t)
        {   ltrim(s, t);
            return s;
        }
    template<typename t_str>
        static t_str rtrim_copy(t_str s, const t_str &t)
        {   rtrim(s, t);
            return s;
        }
    template<typename t_str>
        static t_str trim_copy(t_str s, const t_str &t)
        {   trim(s, t);
            return s;
        }

    /** Interprets a string token
        @param[in]      x_sStr          - String to search
        @param[in]      x_m             - String to value map
        @param[in]      x_def           - Default value to return if not found
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
        @param[in]      bCaseSensitive  - true if compare should be case sensitive

        @warning To do a case insensitive compare, put lower case values in the map
    */
    template<typename t_anymap>
        typename t_anymap::mapped_type
            map_values(const typename t_anymap::key_type &x_sStr, const t_anymap &x_m,
                       typename t_str::size_type &pos,
                       typename t_str::size_type max = t_str::npos,
                       const typename t_anymap::mapped_type &x_def = zru::t_any(),
                       bool bCaseSensitive = false)
    {
        zruCHECK_MAX(x_sStr, max);
        typename t_anymap::key_type::size_type sz = max - pos;

        // Get max string size
        typename t_anymap::key_type::size_type longest = 0;
        for (auto it = x_m.begin(); x_m.end() != it; it++)
            if (longest < it->first.length())
                longest = it->first.length();

        // Anything to do?
        if (0 >= longest)
            return x_def;

        // Can't be longer than what we have
        if (longest > sz)
            longest = sz;

        // String to compare
        typename t_anymap::key_type cmp = x_sStr.substr(pos, longest);
        if (!bCaseSensitive)
            std::transform(cmp.begin(), cmp.end(), cmp.begin(),
                           [](unsigned char c){ return std::tolower(c); });

        for (auto it = x_m.begin(); x_m.end() != it; it++)
        {   typename t_anymap::key_type::size_type len = it->first.length();
            if (sz >= len && cmp.substr(0, len) == it->first)
            {   pos += len;
                return it->second;
            }
        }

        return x_def;
    }

    /// Finds the first character in the string that is in the list
    /**
        @param[in]      x_sStr          - String to search
        @param[in]      x_sFind         - String to find
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
    */
    template<typename t_str>
        typename t_str::size_type find_first_of(const t_str &x_sStr, const t_str &x_sFind,
                                                    typename t_str::size_type &pos,
                                                    typename t_str::size_type max = -1)
    {
        zruCHECK_MAX(x_sStr, max);

        while (pos < max)
        {
            if (t_str::npos != x_sFind.find(x_sStr[pos]))
                return pos;
            pos++;
        }
        return t_str::npos;
    }

    /// Finds the first character in the string that is *not* in the list
    /**
        @param[in]      x_sStr          - String to search
        @param[in]      x_sFind         - String to find
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
    */
    template<typename t_str>
        typename t_str::size_type find_first_not_of(const t_str &x_sStr, const t_str &x_sFind,
                                                    typename t_str::size_type &pos,
                                                    typename t_str::size_type max = -1)
    {
        zruCHECK_MAX(x_sStr, max);

        while (pos < max)
        {
            if (t_str::npos == x_sFind.find(x_sStr[pos]))
                return pos;
            pos++;
        }
        return t_str::npos;
    }

    /** Unescapes a string
        @param[in]      x_sStr          - String to unescape
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process

        @returns Unescaped string
    */
    template<typename t_str>
        t_str UnescapeStr(const t_str &x_sStr,
                          typename t_str::size_type &pos,
                          typename t_str::size_type max = -1)
    {
        typedef typename t_str::value_type t_char;
        zruCHECK_MAX(x_sStr, max);

        t_str r;
        while (pos < max)
        {
            typename t_str::value_type ch = x_sStr[pos];

            if (zruCHR('\\') != ch)
            {   pos++;
                r += ch;
            }
            else
            {
                pos++;
                if (pos < max)
                {
                    ch = x_sStr[pos++];
                    switch(ch)
                    {
                        default: r += ch; break;
                        case zruCHR('b') : r += '\b'; break;
                        case zruCHR('r') : r += '\r'; break;
                        case zruCHR('n') : r += '\n'; break;
                        case zruCHR('t') : r += '\t'; break;
                        case zruCHR('u') :
                        {
                            typename t_str::size_type n = max - pos;
                            if (0 < n)
                            {
                                if (4 < n)
                                    n = 4;
                                r += (t_char)any(x_sStr.substr(pos, n)).toUInt(16);
                                pos += n;
                            }
                        }
                    }
                }
            }
        }
        return r;
    }

    /** Escapes a string
        @param[in]      x_sStr          - String to escape
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process

        @returns Unescaped string
    */
    template<typename t_str>
        t_str EscapeStr(const t_str &x_sStr,
                          typename t_str::size_type &pos,
                          typename t_str::size_type max = -1)
    {
        typedef typename t_str::value_type t_char;
        zruCHECK_MAX(x_sStr, max);

        t_str r;
        while (pos < max)
        {
            typename t_str::value_type ch = x_sStr[pos++];

            switch(ch)
            {
                default: r += ch; break;
                case zruCHR('\b') : r += "\\\b"; break;
                case zruCHR('\r') : r += "\\\r"; break;
                case zruCHR('\n') : r += "\\\n"; break;
                case zruCHR('\t') : r += "\\\t"; break;
                case zruCHR('\"') : r += "\\\""; break;
                case zruCHR('\'') : r += "\\\'"; break;
                case zruCHR('\\') : r += "\\\\"; break;
            }
        }
        return r;
    }

    /// Unquotes the given string
    /**
        @param[in] 	    x_sStr		    - String to be split
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
        @param[in]		x_sOpen		    - List of open quotes
        @param[in]		x_sClose 	    - List of close quotes
        @param[in]		x_sEsc		    - List of escape characters
        @param[in]      bPlusUnquoted   - Also collects characters outside of quotes
        @param[in]      bBreakAfter     - If true, breaks after the first quoted string
    */
    template<typename t_str>
        t_str unquote(const t_str &x_sStr,
                      typename t_str::size_type &pos, typename t_str::size_type max,
                      const t_str &x_sOpen, const t_str &x_sClose, const t_str &x_sEsc,
                      const t_str &x_sBreak, bool bPlusUnquoted = true, bool bBreakAfter = false)
    {
        zruCHECK_MAX(x_sStr, max);

        t_str r;
        bool inQuote = false;
        while (pos < max)
        {
            typename t_str::value_type ch = x_sStr[pos];

            // Escape character?
            if (t_str::npos != x_sEsc.find(ch))
            {
                pos++;
                if (pos < max)
                {
                    if (inQuote || bPlusUnquoted)
                    {
                        r += ch;
                        r += x_sStr[pos];
                    }
                    pos++;
                }
            }

            // We're not in quotes
            else if (!inQuote)
            {
                // Check for break character
                if (0 < x_sBreak.length() && t_str::npos != x_sBreak.find(ch))
                    break;

                // Start quotes?
                if (t_str::npos != x_sOpen.find(ch))
                    inQuote = true;

                else if (bPlusUnquoted)
                    r += ch;

                pos++;
            }

            // In quotes
            else
            {
                pos++;

                // End quote?
                if (t_str::npos != x_sClose.find(ch))
                {
                    if (bBreakAfter)
                        break;
                    inQuote = false;
                }
                else
                    r += ch;
            }
        }

        return UnescapeStr(r, strpos(0));
    }

    /// Parses a string in to a quoted key/value pair
    /**
        @param[in] 	    x_sStr		    - String to be split
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
        @param[in]      sQuotes         - Quote characters
        @param[in]      sSep            - Characters that separate key/value
        @param[in]      sBreak          - Characters to break processing on
        @param[in]      bPlusUnquoted   - Also collects characters outside of quotes
        @param[in]      bBreakAfter     - If true, breaks after the first quoted string

        @returns key/value pair
    */
    template<typename t_str = zru::string>
        static std::pair<t_str, t_str> parse_quoted_assignment(
                const t_str &x_sStr,
                typename t_str::size_type &pos,
                typename t_str::size_type max,
                const t_str &sQuotes, const t_str &sSep,
                const t_str &sBreak,
                bool bPlusUnquoted = true,
                bool bBreakAfter = false
                )
    {
        // Char
        t_str key = unquote(x_sStr, pos, max, sQuotes, sQuotes,
                            t_str(tcTT(t_char, "\\")), sSep + sBreak,
                            bPlusUnquoted, bBreakAfter);

        // Key only?
        if (t_str::npos == pos || pos >= max || t_str::npos == sSep.find(x_sStr[pos]))
            return std::pair<t_str, t_str>(key, t_str());

        // Get the value
        pos++;
        t_str val = unquote(x_sStr, pos, max, sQuotes, sQuotes,
                            t_str(tcTT(t_char, "\\")), sBreak,
                            bPlusUnquoted, bBreakAfter);

        return std::pair<t_str, t_str>(key, val);
    }


    /// Splits a string into an array by cutting on any character in m_pSep, while respecting quotes
    /**
        @param[in] 	    x_sStr		    - String to be split
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
        @param[in]		x_sSep		    - List of separators
        @param[in]		x_sOpen		    - List of open quotes
        @param[in]		x_sClose 	    - List of close quotes
        @param[in]		x_sEsc		    - List of escape characters
        @param[in]      x_bPlusUnquoted - Also collects characters outside of quotes

        The opening and closing quote characters must correspond within the string.

        open    = "<{[(";
        close   = ">}])";
    */
    template<typename t_str, typename t_lst>
        static t_lst split_quoted(const t_str &x_sStr,
                                  typename t_str::size_type &pos, typename t_str::size_type max,
                                  const t_str &x_sSep, const t_str &x_sOpen, const t_str &x_sClose,
                                  const t_str &x_sEsc, bool x_bPlusUnquoted = true)
    {
        zruCHECK_MAX(x_sStr, max);

        t_lst lst;
        while (t_str::npos != pos && pos < max)
        {
            // Skip leading separators
            pos = find_first_not_of(x_sStr, x_sSep, pos, max);
            if (t_str::npos == pos)
                break;

            // Add a quoted string to the list
            t_str s = unquote(x_sStr, pos, max, x_sOpen, x_sClose, x_sEsc, t_str(), x_bPlusUnquoted);
            if (0 < s.length())
                lst.push_back(s);
        }

        return lst;
    }

    template<typename t_str>
        t_str filename(const t_str &path, const t_str &seps = tcTT(typename t_str::value_type, "/\\"))
    {
        typename t_str::size_type pos = path.find_last_of(seps);
        if (t_str::npos == pos)
            return path;
        return t_str(path, pos + 1);
    }

    template<typename t_str>
        t_str dirname(const t_str &path, const t_str &seps = tcTT(typename t_str::value_type, "/\\"))
    {
        typename t_str::size_type pos = path.find_last_of(seps);
        if (t_str::npos == pos)
            return path;
        return t_str(path, pos);
    }

    template <typename t_str, typename... Args>
        t_str join(const t_str &sSep, Args... args)
        {
            t_str r;
            std::stringstream ss;
            std::vector<zru::any> vec = {args...};
            for (size_t i = 0; i < vec.size(); i++)
            {
                if (0 < i)
                    r += sSep;
                r += vec[i].toString();
            }
            return r;
        }

    template <typename t_str>
        t_str console_cmd(int nCmd)
        {
            switch(nCmd)
            {
                default: return CONSOLE_DEFAULT;
                case 1:  return CONSOLE_RED;
                case 2:  return CONSOLE_YELLOW;
                case 3:  return CONSOLE_CYAN;
                case 4:  return CONSOLE_GREEN;
                case 5:  return CONSOLE_BLUE;
                case 6:  return CONSOLE_MAGENTA;
                case 7:  return CONSOLE_WHITE;
                case 8:  return CONSOLE_BLACK;
                case 9:  return CONSOLE_BOLD;
                case 10:  return CONSOLE_FAINT;
                case 11: return CONSOLE_ITALIC;
                case 12: return CONSOLE_UNDERLINE;
                case 13: return CONSOLE_BLINK;
                case 14: return CONSOLE_NAGATIVE;
                case 15: return CONSOLE_STRIKEOUT;
            }
        }

    template <typename t_str>
        t_str console_cmd(const t_str &sStr, int nStart, int nEnd = 0)
        {
            if (!nStart && !nEnd)
                return sStr;
            return console_cmd<t_str>(nStart) + sStr + console_cmd<t_str>(nEnd);
        }

    template <typename t_str>
        t_str console_log_level_str(int nLevel)
        {
            if (!nLevel)
                return t_str();
            t_str s;
            switch(nLevel)
            {
                default : s = "L:"; s += any(nLevel).toString(); break;
                case 1 : s = "ERROR"; break;
                case 2 : s = "WARNING"; break;
                case 3 : s = "INFO"; break;
            }
            return t_str() + "[" + console_cmd<t_str>(nLevel) + s + console_cmd<t_str>(0) + "]";
        }
}

