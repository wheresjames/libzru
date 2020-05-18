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

namespace zru::parsers
{
    //---------------------------------------------------------------
    /** Parses command line arguments

        @param [in] x_sStr      - String containing command line
        @param [in] pos         - Position in the string to start
        @param [in] max         - Position in the string to stop

        @example

            "myapp.exe ok -v -xyz 1234 -i \"C:\\\\Program Files\" --out 'C:/temp' --p1:hi --p2:\"hi 1\" --p3:hi\\ 2 --p4 hi\\ 3 --p5:##1";

            Will parse to...

            #: {
              0: myapp.exe
              1: ok
              2: 1234
              3: C:\Program Files
              4: C:/temp
              5: hi 3
            }
            i: C:\Program Files
            out: C:/temp
            p1: hi
            p2: hi 1
            p3: hi 2
            p4: hi 3
            p5: ok
            v: 1234
            x: 1234
            y: 1234
            z: 1234

    */
    template<typename t_str = zru::string, typename t_pb = zru::property_bag>
        static t_pb parse_command_line( const t_str &x_sStr,
                                        typename t_str::size_type &pos,
                                        typename t_str::size_type max = t_str::npos)
    {
        typedef typename t_str::value_type t_char;

        zruCHECK_MAX(x_sStr, max);

        // White space
        const t_str sWhiteSpace = tcTT(t_char, " \t\r\n");
        const t_str sQuotes = tcTT(t_char, "\"'");
        const t_str sEscape = tcTT(t_char, "\\");

        t_pb pb;
        while (t_str::npos != pos && pos < max)
        {
            // Skip white space
            pos = str::find_first_not_of(x_sStr, sWhiteSpace, pos, max);
            if (t_str::npos == pos)
                break;

            t_char ch = x_sStr[pos];

            // Switch
            if (tcTC(t_char, '-') == ch)
            {
                pos++;
                if (pos >= max)
                    break;

                // Double switch?
                if (tcTC(t_char, '-') == x_sStr[pos])
                {
                    pos++;
                    if (pos >= max)
                        break;

                    auto r = str::parse_quoted_assignment(x_sStr, pos, max,
                                                            t_str(tcTT(t_char, "\"'")),
                                                            t_str(tcTT(t_char, ":=")),
                                                            sWhiteSpace);
                    if (0 < r.second.length())
                        pb[r.first] = r.second;
                    else
                        pb[r.first] = any("##") += pb["#"].size();
                }

                // Read in switches
                else
                    while (pos < max && t_str::npos == sWhiteSpace.find(x_sStr[pos]))
                        pb[x_sStr[pos++]] = any("##") += pb["#"].size();

                continue;
            }

            // Not a switch (naked parameter)
            pb["#"].push(str::unquote(x_sStr, pos, max, sQuotes, sQuotes, sEscape, sWhiteSpace));
        }

        // Merge switch values
        for (auto it = pb.begin(); pb.end() != it; it++)
            if (it->second.val().isString() && it->second.val().toString().substr(0, 2) == "##")
            {   t_str s = it->second.val().toString().substr(2);
                if (pb["#"].isset(s))
                    it->second = pb["#"][s];
            }

        return pb;
    }
    template<typename t_str = zru::string, typename t_pb = zru::property_bag>
        static t_pb parse_command_line(const t_str &x_sStr)
        {
            typename t_str::size_type pos = 0;
            return parse_command_line<t_str, t_pb>(x_sStr, pos);
        }

    // Set value into property bag
#   define zruSETVAL(at, pb, key, val) \
        if (1 == at && key.length()) \
            pb[key] = val; \
        else if (2 == at && val.isSet()) \
            pb[idx++] = val;

    //---------------------------------------------------------------
    /** Parses JSON string

        @param [in] x_sStr      - String containing json string
        @param [in] pos         - Position in the string to start
        @param [in] max         - Position in the string to stop

    */
    template<typename t_str = zru::string, typename t_pb = zru::property_bag>
        static t_pb json_parse( const t_str &x_sStr,
                                typename t_str::size_type &pos,
                                typename t_str::size_type max = t_str::npos
                              )
    {
        typedef typename t_str::value_type t_char;

        zruCHECK_MAX(x_sStr, max);

        // White space
        const t_str sWhiteSpace = tcTT(t_char, " \t\r\n");
        const t_str sQuotes = tcTT(t_char, "\"'");
        const t_str sEscape = tcTT(t_char, "\\");
        const t_str sNumber = tcTT(t_char, "+-.0123456789");

        // The property bag we will return
        t_pb pb;

        // Skip white space
        pos = str::find_first_not_of(x_sStr, sWhiteSpace, pos, max);
        if (t_str::npos == pos || pos >= max)
            return pb;

        int arrayType = -1;
        switch(x_sStr[pos])
        {
            case zruCHR('{') : arrayType = 1; pos++; break;
            case zruCHR('[') : arrayType = 2; pos++; break;
            default:
                ZruError("Invalid array type character : ", x_sStr[pos], " at ", pos);
                return pb;
        }

        // Flag as an array
        if (2 == arrayType)
            pb.setArray(true);

        // Current key / value
        int idx = 0;
        typename t_pb::t_str key;
        typename t_pb::t_any val;

        // Process data
        while (t_str::npos != pos && pos < max)
        {
            // Skip white space
            pos = str::find_first_not_of(x_sStr, sWhiteSpace, pos, max);
            if (t_str::npos == pos)
                break;

            t_char ch = x_sStr[pos];

            // End of array
            if (zruCHR('}') == ch || zruCHR(']') == ch)
            {
                pos++;
                if (pos >= max)
                    break;

                zruSETVAL(arrayType, pb, key, val);

                return pb;
            }

            // Start array
            else if (zruCHR('{') == ch || zruCHR('[') == ch)
            {
                if (1 == arrayType && 0 >= key.length())
                {   ZruError("No key, Invalid character '", ch, "' at ", pos);
                    return pb;
                }
                else if (val.isSet())
                {   ZruError("Already have value: '", ch, "' at ", pos);
                    return pb;
                }

                if (1 == arrayType)
                {   pb[key] = json_parse(x_sStr, pos, max);
                    key = "";
                }
                else if (2 == arrayType)
                    pb[idx++] = json_parse(x_sStr, pos, max);
            }

            // Assignment
            else if (zruCHR(':') == ch)
            {
                if (2 == arrayType || (1 == arrayType && 0 >= key.length()))
                {   ZruError("Invalid character '", ch, "' at ", pos);
                    return pb;
                }
                else if (val.isSet())
                {   ZruError("Already have value: '", ch, "' at ", pos);
                    return pb;
                }
                pos++;
            }

            // Assignment
            else if (zruCHR(',') == ch)
            {
                pos++;

                zruSETVAL(arrayType, pb, key, val);

                key = "";
                val.clear();
            }

            // Quoted string
            else if (zruCHR('\"') == ch)
            {
                t_str s = str::unquote<t_str>(x_sStr, pos, max,
                                              zruTXT("\""), zruTXT("\""),
                                              zruTXT("\\"), t_str(), false, true);

                if (1 == arrayType)
                {
                    if (0 >= key.length())
                    {
                        if (0 >= s.length())
                        {   ZruError("Invalid key at ", pos);
                            return pb;
                        }
                        key = s;
                    }
                    else if (val.isSet())
                    {   ZruError("Invalid key at ", pos);
                        return pb;
                    }
                    else
                        val = s;
                }
                else if (val.isSet())
                {   ZruError("Invalid value at ", pos);
                    return pb;
                }
                else
                    val = s;
            }

            // Keys must be quoted
            else if (1 == arrayType && 0 >= key.length())
            {   ZruError("No Key, Invalid character '", ch, "' at ", pos);
                return pb;
            }

            // Do we already have a value?
            else if (val.isSet())
            {   ZruError("Already have value, Invalid character '", ch, "' at ", pos);
                return pb;
            }

            // It's something else
            else
            {
                // Check for bool value
                t_anymap mVals({{"true", true}, {"false", false}});
                typename t_str::size_type end = pos;
                t_any v = str::map_values(x_sStr, mVals, end, max);

                // If we got a bool
                if (pos != end)
                {   val = v;
                    pos = end;
                }

                // Is it a number
                else if (t_str::npos != sNumber.find(ch))
                {
                    // Find the end of the number
                    typename t_str::size_type end = pos;
                    str::find_first_not_of(x_sStr, sNumber, end, max);
                    if (t_str::npos == end)
                    {   ZruError("Out of data in number at ", pos);
                        return pb;
                    }

                    if (pos >= end)
                    {   ZruError("Invalid number at ", pos);
                        return pb;
                    }

                    // Read in the number
                    t_str num = x_sStr.substr(pos, end);
                    bool isFloat = t_str::npos != num.find(zruCHR('.'));
                    if (isFloat)
                        val = any(num).toDouble();
                    else
                        val = any(num).toLongLong();

                    pos = end;
                }

                else
                {   ZruError("Invalid character '", ch, "' at ", pos);
                    return pb;
                }

            }
        }

        return pb;
    }
    template<typename t_str = zru::string, typename t_pb = zru::property_bag>
        static t_pb json_parse(const t_str &x_sStr)
        {
            typename t_str::size_type pos = 0;
            return json_parse<t_str, t_pb>(x_sStr, pos);
        }

    //---------------------------------------------------------------
    /** Encode property_bag as a JSON string
        @param [in] pb      - Property bag to dump
        @param [in] bPretty - More human readable format
        @param [in] depth   - Current depth

        Dumps the property bag contents in a human readable form.
    */
    template<typename t_pb>
        static typename t_pb::t_str json_encode(const t_pb &pb, bool bPretty = false, int depth = 0)
        {
            typedef typename t_pb::t_str::value_type t_char;
            const t_char *tab = zruTXT("    ");
            const t_char *endofs = bPretty ? zruTXT(",\r\n") : zruTXT(",");
            const t_char *seps = bPretty ? zruTXT("\": ") : zruTXT("\":");

            int nCount = 0;
            typename t_pb::t_str r;

            // Array
            if (pb.isArray())
            {
                long sz = pb.size();
                r = bPretty ? zruTXT("[\r\n") : zruTXT("[");
                for (long i = 0; i < sz && pb.isset(i); i++)
                {
                    auto it = pb.find(i);
                    if (pb.end() == it)
                        break;

                    if (0 < nCount++)
                        r += endofs;

                    if (bPretty)
                        for (int i = 0; i < depth; i++)
                            r += tab;

                    // Array
                    if (it->second.size())
                    {
                        r += json_encode(it->second, bPretty, depth + 1);

                        if (bPretty)
                            for (int i = 0; i < depth; i++)
                                r += tab;
                    }

                    // Boolean or number
                    else if (it->second.val().isType(
                            {   any::at_size, any::at_bool, any::at_int, any::at_uint,
                                any::at_long, any::at_ulong, any::at_longlong, any::at_ulonglong,
                                any::at_float, any::at_double, any::at_longdouble
                            }))
                        r += it->second.val().toString();

                    // String
                    else
                        r += t_str() + zruTXT("\"") + str::EscapeStr(it->second.val().toString(), strpos(0)) + zruTXT("\"");
                }

                // End of array
                if (bPretty)
                {
                    r += zruTXT("\r\n");
                    for (int i = 0; i < depth; i++)
                        r += tab;
                    r += zruTXT("]\r\n");
                }
                else
                r += zruTXT("]");
            }

            // Map
            else
            {
                r = bPretty ? zruTXT("{\r\n") : zruTXT("{");
                for (auto it = pb.begin(); it != pb.end(); it++)
                {
                    if (0 < nCount++)
                        r += endofs;

                    if (bPretty)
                        for (int i = 0; i < depth; i++)
                            r += tab;

                    r += t_str() + zruTXT("\"") + str::EscapeStr(it->first, strpos(0)) + seps;

                    // Array
                    if (it->second.size())
                    {
                        r += json_encode(it->second, bPretty, depth + 1);

                        if (bPretty)
                            for (int i = 0; i < depth; i++)
                                r += tab;
                    }

                    // Boolean or number
                    else if (it->second.val().isType(
                            {   any::at_size, any::at_bool, any::at_int, any::at_uint,
                                any::at_long, any::at_ulong, any::at_longlong, any::at_ulonglong,
                                any::at_float, any::at_double, any::at_longdouble
                            }))
                        r += it->second.val().toString();

                    // String
                    else
                        r += t_str() + zruTXT("\"") + str::EscapeStr(it->second.val().toString(), strpos(0)) + zruTXT("\"");
                }

                // End of array
                if (bPretty)
                {
                    r += zruTXT("\r\n");
                    for (int i = 0; i < depth; i++)
                        r += tab;
                    r += zruTXT("}\r\n");
                }
                else
                r += zruTXT("}");
            }

            return r;
        }


    //---------------------------------------------------------------
    /** Dump property bag
        @param [in] pb      - Property bag to dump
        @param [in] depth   - Current depth

        Dumps the property bag contents in a human readable form.
    */
    template<typename t_pb>
        static typename t_pb::t_str dumpPb(const t_pb &pb, int depth = 0)
        {
            typedef typename t_pb::t_str::value_type t_char;
            const t_char *tab = zruTXT("    ");

            typename t_pb::t_str r;
            for (auto it = pb.begin(); it != pb.end(); it++)
            {
                for (int i = 0; i < depth; i++) r += tab;
                r += it->first + zruTXT(": ");

                if (it->second.size())
                {
                    r += zruTXT("{\r\n");
                    r += dumpPb(it->second, depth + 1);
                    for (int i = 0; i < depth; i++) r += tab;
                    r += zruTXT("}\r\n");
                }
                else
                    r += it->second.val().toString() + zruTXT("\r\n");
            }

            if (!depth)
                r += zruTXT("\r\n");

            return r;
        }

    //---------------------------------------------------------------
    /** Dump String
        @param [in] str     - String to dump
        @param [in] maxline - Maximum line length

        Dumps the string contents in a human readable form.
    */
    template<typename t_str>
        static t_str dumpStr(const t_str &str, typename t_str::size_type maxline = 16)
        {
            t_str r;
            typename t_str::size_type pos = 0;
            typename t_str::size_type max = str.length();

            while (pos < max)
            {
                typename t_str::size_type l = 0;

                // First the hex
                while (l < maxline && (l - pos) < maxline)
                {
                    if (l < max)
                    {
                        typename t_str::value_type ch = str[l], n;

                        n = (ch & 0xf0) >> 4;
                        r += (9 >= (int)n) ? ('0' + n) : ('A' + (n - 10));

                        n = ch & 0x0f;
                        r += (9 >= (int)n) ? ('0' + n) : ('A' + (n - 10));

                        r += zruCHR(' ');
                    }
                    else
                        r += zruTXT(".. ");

                    l++;
                }

                r += zruTXT("    ");

                // Now the ascii
                l = pos;
                while (l < max && (l - pos) < maxline)
                {
                    typename t_str::value_type ch = str[l];

                    if (zruCHR(' ') <= ch && zruCHR('~') >= ch)
                        r += ch;
                    else
                        r += zruCHR('.');

                    l++;
                }

                r += zruTXT("\r\n");

                pos += maxline;
            }

            return r;
        }

}

