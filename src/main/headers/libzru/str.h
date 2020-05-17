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

namespace zru::str
{
#   define CHECK_MAX(s, m) m = (0 > m || t_str::npos == m || m > s.length()) ? s.length() : m

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

    /// Finds the first character in the string that is in the list
    /**
        @param[in]      x_sStr          - String to search
        @param[in]      x_sFind         - String to find
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
    */
    template<typename t_str>
        typename t_str::size_type find_first_of(const t_str &x_sStr, const t_str &x_sFind,
                                                    typename t_str::size_type &pos, typename t_str::size_type max)
    {
        CHECK_MAX(x_sStr, max);

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
                                                    typename t_str::size_type &pos, typename t_str::size_type max)
    {
        CHECK_MAX(x_sStr, max);

        while (pos < max)
        {
            if (t_str::npos == x_sFind.find(x_sStr[pos]))
                return pos;
            pos++;
        }
        return t_str::npos;
    }

    /** Converts escape char
        @param[in]      x_ch        - Character to unescape

        @returns Unescaped character
    */
    template<typename t_char>
        t_char UnescapeChar(const t_char x_ch)
    {
        switch(x_ch)
        {
            case tcTC(t_char, 'r') : return '\r';
            case tcTC(t_char, 'n') : return '\n';
            case tcTC(t_char, 't') : return '\t';
        }
        return x_ch;
    }

    /// Unquotes the given string
    /**
        @param[in] 	    x_sStr		    - String to be split
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
        @param[in]		x_sOpen		    - List of open quotes
        @param[in]		x_sClose 	    - List of close quotes
        @param[in]		x_sEsc		    - List of escape characters
        @param[in]      x_bPlusUnquoted - Also collects characters outside of quotes
    */
    template<typename t_str>
        t_str unquote(const t_str &x_sStr,
                      typename t_str::size_type &pos, typename t_str::size_type max,
                      const t_str &x_sOpen, const t_str &x_sClose, const t_str &x_sEsc,
                      const t_str &x_sBreak, bool x_bPlusUnquoted = true)
    {
        CHECK_MAX(x_sStr, max);

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
                    if (inQuote || x_bPlusUnquoted)
                        r += UnescapeChar(x_sStr[pos]);
                    pos++;
                }
            }

            // We're not in quotes
            else if (!inQuote)
            {
                // Check for break character
                if (0 < x_sBreak.length() && t_str::npos != x_sBreak.find(ch))
                    break;

                if (t_str::npos != x_sOpen.find(ch))
                    inQuote = true;
                else if (x_bPlusUnquoted)
                    r += ch;
                pos++;
            }

            // In quotes
            else
            {
                if (t_str::npos != x_sClose.find(ch))
                    inQuote = false;
                else
                    r += ch;
                pos++;
            }
        }

        return r;
    }

    /// Parses a string in to a quoted key/value pair
    /**
        @param[in] 	    x_sStr		    - String to be split
        @param[in,out]  pos             - Starting / Ending position in string
        @param[in]      max             - Maximum position in string to process
        @param[in]      sQuotes         - Quote characters
        @param[in]      sSep            - Characters that separate key/value
        @param[in]      sBreak          - Characters to break processing on

        @returns key/value pair
    */
    template<typename t_str = zru::string>
        static std::pair<t_str, t_str> parse_quoted_assignment(
                const t_str &x_sStr,
                typename t_str::size_type &pos,
                typename t_str::size_type max,
                const t_str &sQuotes, const t_str &sSep,
                const t_str &sBreak
                )
    {
        // Char
        t_str key = unquote(x_sStr, pos, max, sQuotes, sQuotes,
                            t_str(tcTT(t_char, "\\")), sSep + sBreak);

        // Key only?
        if (t_str::npos == pos || pos >= max || t_str::npos == sSep.find(x_sStr[pos]))
            return std::pair<t_str, t_str>(key, t_str());

        // Get the value
        pos++;
        t_str val = unquote(x_sStr, pos, max, sQuotes, sQuotes,
                            t_str(tcTT(t_char, "\\")), sBreak);
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
        CHECK_MAX(x_sStr, max);

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

