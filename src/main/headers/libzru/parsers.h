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

        CHECK_MAX(x_sStr, max);

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

            typename t_str::value_type ch = x_sStr[pos];

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


    //---------------------------------------------------------------
    /** Dump property bag
        @param [in] pb      - Property bag to dump
        @param [in] depth   - Current depth

        Dumps the property bag contents in a human readable form.
    */
    template<typename t_pb>
        static void dump(const t_pb &pb, int depth = 0)
        {
            for (auto it = pb.begin(); it != pb.end(); it++)
            {
                std::cout << std::setw(2 * depth) << it->first << ": ";

                if (it->second.size())
                {
                    std::cout << "{" << std::endl;
                    dump(it->second, depth + 1);
                    std::cout << std::setw(2 * depth) << "}" << std::endl;
                }
                else
                    std::cout << it->second.val().toString() << std::endl;
            }

            if (!depth)
                std::cout << std::endl;
        }


}

