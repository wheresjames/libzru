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
#   define ZRZ_SIMPLE_TYPE_NO_OV(t, i, m) \
        any& set_##i(const t &v) { clear(); type = typeOf(v); v##m = v; return *this; }

#   define ZRZ_SIMPLE_TYPE(t, i, m) \
        int sizeOf(const t) const { return at_##i & at_size_mask; } \
        Type typeOf(const t) const { return at_##i; } \
        bool is##m() const { return type == at_##i; } \
        any(const t v) { type = typeOf(v); v##m = v; } \
        any& set_##i(const t &v) { clear(); type = typeOf(v); v##m = v; return *this; } \
        any& operator =(const t &v) { return set_##i(v); }

#   define ZRZ_SIMPLE_TYPE_PTR(t, i, m) \
        int sizeOf(const t) const { return at_##i & at_size_mask; } \
        Type typeOf(const t) const { return at_##i; } \
        bool is##m() { return type == at_##i; } \
        any(const t v) { type = typeOf(v); v##m = v; } \
        any& set_##i(const t v) { clear(); type = typeOf(v); v##m = v; return *this; } \
        any& operator =(const t v) { return set_##i(v); }

#   define ZRZ_SIMPLE_CAST(t) \
        case at_bool: return (t)vBool; \
        case at_char: return (t)vChar; \
        case at_uchar: return (t)vUChar; \
        case at_wchar: return (t)vWChar; \
        case at_uwchar: return (t)vUWChar; \
        case at_size: return (t)vSize; \
        case at_int: return (t)vInt; \
        case at_uint: return (t)vUInt; \
        case at_long: return (t)vLong; \
        case at_ulonglong: return (t)vULongLong; \
        case at_longlong: return (t)vLongLong; \
        case at_ulong: return (t)vULong; \
        case at_float: return (t)vFloat; \
        case at_double: return (t)vDouble; \
        case at_longdouble: return (t)vLongDouble; \
        case at_voidptr: return (t)(std::uintptr_t)vVoidPtr;

#   define ZRZ_SIMPLE_CONV(t) \
        case at_bool: return t(vBool); \
        case at_char: return t(vChar); \
        case at_uchar: return t(vUChar); \
        case at_wchar: return t(vWChar); \
        case at_uwchar: return t(vUWChar); \
        case at_size: return t(vSize); \
        case at_int: return t(vInt); \
        case at_uint: return t(vUInt); \
        case at_long: return t(vLong); \
        case at_ulonglong: return t(vULongLong); \
        case at_longlong: return t(vLongLong); \
        case at_ulong: return t(vULong); \
        case at_float: return t(vFloat); \
        case at_double: return t(vDouble); \
        case at_longdouble: return t(vLongDouble); \
        case at_voidptr: return t(vVoidPtr);

    class any
    {
    public:

        typedef string t_str;
        typedef wstring t_wstr;

        enum Type
        {
            // Masks
            at_type_mask    = 0xff00,
            at_size_mask    = 0x00ff,

            // Types
            at_char_t       = 0x0100,
            at_uchar_t      = 0x0200,
            at_size_t       = 0x0300,
            at_int_t        = 0x0400,
            at_uint_t       = 0x0500,
            at_long_t       = 0x0600,
            at_ulong_t      = 0x0700,
            at_longlong_t   = 0x0800,
            at_ulonglong_t  = 0x0900,
            at_float_t      = 0x0A00,
            at_double_t     = 0x0B00,
            at_longdouble_t = 0x0C00,
            at_bool_t       = 0x0D00,
            at_void_t       = 0x0E00,

            // POD
            at_void         = 0,

            // Ptr
            at_voidptr      = at_void_t         | sizeof(void*),

            // Char
            at_char         = at_char_t         | sizeof(char),
            at_wchar        = at_char_t         | sizeof(wchar_t),
            at_uchar        = at_uchar_t        | sizeof(unsigned char),
            at_uwchar       = at_uchar_t        | sizeof(unsigned wchar_t),

            // Size
            at_size         = at_size_t         | sizeof(size_t),

            // Types
            at_bool         = at_bool_t         | sizeof(bool),
            at_int          = at_int_t          | sizeof(int),
            at_uint         = at_uint_t         | sizeof(unsigned int),
            at_long         = at_long_t         | sizeof(long),
            at_ulong        = at_ulong_t        | sizeof(unsigned long),
            at_longlong     = at_longlong_t     | sizeof(unsigned long long),
            at_ulonglong    = at_ulonglong_t    | sizeof(unsigned long long),
            at_float        = at_float_t        | sizeof(float),
            at_double       = at_double_t       | sizeof(double),
            at_longdouble   = at_longdouble_t   | sizeof(long double),

            // Array
            at_array        = 0x10000,
            at_arrstring    = at_array | at_char,
            at_arrwstring   = at_array | at_wchar,

            // Objects
            at_object       = 0x20000,
            at_string       = at_object | 1,
            at_wstring      = at_object | 2,
            at_vector       = at_object | 3,

            at_user         = 0x30000
        };

    public:

        // Default constructor
        any() { type = at_void; pVoid = 0; }

        // Copy constructor
        any(const any& r) { (*this) = r; }

        // Default destructor
        ~any() { clear(); }

        // Returns the type of data in this container
        Type getType() const { return type; }

        /// Returns non-zero if a value is set
        bool isVoid() const { return at_void == type; }

        /// Returns non-zero if a value is set
        bool isSet() const { return at_void != type; }

        bool isType(std::initializer_list<int> types) const
        {
            for(auto it = std::begin(types); std::end(types) != it; it++)
                if (getType() == *it)
                    return true;
            return false;
        }

        /// Creates the specified type
        void make(Type t)
        {
            clear();

            switch(t)
            {
                default : break;
                case at_bool : type = t; vBool = false; break;
                case at_char : type = t; vChar = 0; break;
                case at_uchar : type = t; vUChar = 0; break;
                case at_wchar : type = t; vWChar = 0; break;
                case at_uwchar : type = t; vUWChar = 0; break;
                case at_size : type = t; vSize = 0; break;
                case at_int : type = t; vInt = 0; break;
                case at_uint : type = t; vUInt = 0; break;
                case at_long : type = t; vLong = 0; break;
                case at_ulong : type = t; vULong = 0; break;
                case at_longlong : type = t; vLongLong = 0; break;
                case at_ulonglong : type = t; vULongLong = 0; break;
                case at_float : type = t; vFloat = 0; break;
                case at_double : type = t; vDouble = 0; break;
                case at_longdouble : type = t; vLongDouble = 0; break;
                case at_voidptr : type = t; vVoidPtr = 0; break;
                case at_string : type = t; pString = new(&vString) t_str(); break;
                case at_wstring : type = t; pWString = new(&vWString) t_wstr(); break;
                case at_vector : type = t; pVector = new(&vVector) vector(); break;
            }
        }

        void clear()
        {
            if (pVoid)
            {
                switch(type)
                {
                    default : break;
                    case at_string : pString->~t_str(); break;
                    case at_wstring : pWString->~t_wstr(); break;
                    case at_vector : pVector->~vector(); break;
                }

            }

            pVoid = 0;
            type = at_void;
        }

        any& set(any& v, const any& r)
        {
            switch(r.getType())
            {
                default : break;
                case at_bool : v.set_bool(r.vBool); break;
                case at_char : v.set_char(r.vChar); break;
                case at_uchar : v.set_uchar(r.vUChar); break;
                case at_wchar : v.set_wchar(r.vWChar); break;
                case at_uwchar : v.set_uwchar(r.vUWChar); break;
                case at_size : v.set_size(r.vSize); break;
                case at_int : v.set_int(r.vInt); break;
                case at_uint : v.set_uint(r.vUInt); break;
                case at_long : v.set_long(r.vLong); break;
                case at_ulong : v.set_ulong(r.vULong); break;
                case at_longlong : v.set_longlong(r.vLongLong); break;
                case at_ulonglong : v.set_ulonglong(r.vULongLong); break;
                case at_float : v.set_float(r.vFloat); break;
                case at_double : v.set_double(r.vDouble); break;
                case at_longdouble : v.set_longdouble(r.vLongDouble); break;
                case at_voidptr : v.set_voidptr(r.vVoidPtr); break;
                case at_string : v.set_string(*r.pString); break;
                case at_wstring : v.set_wstring(*r.pWString); break;
                case at_vector : v.set_vector(*r.pVector); break;
            }
            return v;
        }

        any& operator = (const any& r)
        {
            return set(*this, r);
        }

        static any& inc(any& v, const any &r)
        {
            switch(v.getType())
            {
                default : break;
                case at_bool : v = v.toBool() ^ r.toBool(); break;
                case at_char : v = v.toChar() + r.toChar(); break;
                case at_uchar : v = v.toUChar() + r.toUChar(); break;
                case at_wchar : v = v.toWChar() + r.toWChar(); break;
                case at_uwchar : v = v.toUWChar() + r.toUWChar(); break;
                case at_size : v = v.toSize() + r.toSize(); break;
                case at_int : v = v.toInt() + r.toInt(); break;
                case at_uint : v = v.toUInt() + r.toUInt(); break;
                case at_long : v = v.toLong() + r.toLong(); break;
                case at_ulong : v = v.toULong() + r.toULong(); break;
                case at_longlong : v = v.toLongLong() + r.toLongLong(); break;
                case at_ulonglong : v = v.toULongLong() + r.toULongLong(); break;
                case at_float : v = v.toFloat() + r.toFloat(); break;
                case at_double : v = v.toDouble() + r.toDouble(); break;
                case at_longdouble : v = v.toLongDouble() + r.toLongDouble(); break;
                case at_voidptr : v = v.toVoidPtr() + r.toVoidPtr(); break;
                case at_string : v = v.toString() + r.toString(); break;
                case at_wstring : v = v.toWString() + r.toWString(); break;
            }
            return v;
        }

        any& operator += (const any &r)
        {
            return inc(*this, r);
        }

        any& operator + (const any &r)
        {
            return inc(*this, r);
        }

        any& operator ++ ()
        {
            return inc(*this, 1);
        }

        static any& dec(any& v, const any &r)
        {
            switch(v.getType())
            {
                default : break;
                case at_bool : v = v.toBool() ^ r.toBool(); break;
                case at_char : v = v.toChar() - r.toChar(); break;
                case at_uchar : v = v.toUChar() - r.toUChar(); break;
                case at_wchar : v = v.toWChar() - r.toWChar(); break;
                case at_uwchar : v = v.toUWChar() - r.toUWChar(); break;
                case at_size : v = v.toSize() - r.toSize(); break;
                case at_int : v = v.toInt() - r.toInt(); break;
                case at_uint : v = v.toUInt() - r.toUInt(); break;
                case at_long : v = v.toLong() - r.toLong(); break;
                case at_ulong : v = v.toULong() - r.toULong(); break;
                case at_longlong : v = v.toLongLong() - r.toLongLong(); break;
                case at_ulonglong : v = v.toULongLong() - r.toULongLong(); break;
                case at_float : v = v.toFloat() - r.toFloat(); break;
                case at_double : v = v.toDouble() - r.toDouble(); break;
                case at_longdouble : v = v.toLongDouble() - r.toLongDouble(); break;
                case at_voidptr : v = v.toVoidPtr() - r.toVoidPtr(); break;
            }
            return v;
        }

        any& operator -= (const any &r)
        {
            return dec(*this, r);
        }

        any& operator - (const any &r)
        {
            return dec(*this, r);
        }

        any& operator -- ()
        {
            return dec(*this, 1);
        }

        bool eq(any& v, const any& r)
        {
            switch(r.getType())
            {
                default : break;
                case at_bool : return v.toBool() == r.toBool();
                case at_char : return v.toChar() == r.toChar();
                case at_uchar : return v.toUChar() == r.toUChar();
                case at_wchar : return v.toWChar() == r.toWChar();
                case at_uwchar : return v.toUWChar() == r.toUWChar();
                case at_size : return v.toSize() == r.toSize();
                case at_int : return v.toInt() == r.toInt();
                case at_uint : return v.toUInt() == r.toUInt();
                case at_long : return v.toLong() == r.toLong();
                case at_ulong : return v.toULong() == r.toULong();
                case at_longlong : return v.toLongLong() == r.toLongLong();
                case at_ulonglong : return v.toULongLong() == r.toULongLong();
                case at_float : return v.toFloat() == r.toFloat();
                case at_double : return v.toDouble() == r.toDouble();
                case at_longdouble : return v.toLongDouble() == r.toLongDouble();
                case at_voidptr : return v.toVoidPtr() == r.toVoidPtr();
                case at_string : return v.toString() == r.toString();
                case at_wstring : return v.toWString() == r.toWString();
            }
            return false;
        }

        bool operator == (const any &r)
        {
            return eq(*this, r);
        }

        // [ bool ]
        ZRZ_SIMPLE_TYPE(bool, bool, Bool)
        bool toBool() const
        {
            switch(type)
            {
                default : break;
                ZRZ_SIMPLE_CAST(bool);
                case at_string:
                {   const char *pStr = pString->c_str();
                    switch(pString->length())
                    {   case 1: if ('1' == pStr[0]) return true;
                        case 2: if ('o' == std::tolower(pStr[0])
                                    && 'n' == std::tolower(pStr[1])
                                    ) return true;
                        case 3: if ('y' == std::tolower(pStr[0])
                                    && 'e' == std::tolower(pStr[1])
                                    && 's' == std::tolower(pStr[2])
                                    ) return true;
                        case 4: if ('t' == std::tolower(pStr[0])
                                    && 'r' == std::tolower(pStr[1])
                                    && 'u' == std::tolower(pStr[2])
                                    && 'e' == std::tolower(pStr[3])
                                    ) return true;
                    }
                    return false;
                }
                case at_wstring:
                {   const wchar_t *pWStr = pWString->c_str();
                    switch(pString->length())
                    {   case 1: if (L'1' == pWStr[0]) return true;
                        case 2: if (L'o' == std::tolower(pWStr[0])
                                    && L'n' == std::tolower(pWStr[1])
                                    ) return true;
                        case 3: if (L'y' == std::tolower(pWStr[0])
                                    && L'e' == std::tolower(pWStr[1])
                                    && L's' == std::tolower(pWStr[2])
                                    ) return true;
                        case 4: if (L't' == std::tolower(pWStr[0])
                                    && L'r' == std::tolower(pWStr[1])
                                    && L'u' == std::tolower(pWStr[2])
                                    && L'e' == std::tolower(pWStr[3])
                                    ) return true;
                    }
                    return false;
                }
            }
            return 0;
        }

        // [ char ]
        ZRZ_SIMPLE_TYPE(char, char, Char)
        char toChar() const
        {
            switch(type)
            {
                default : break;
                ZRZ_SIMPLE_CAST(char);
                case at_string: return (char)(pString->length() ? (*pString)[0] : 0);
                case at_wstring: return (char)(pString->length() ? (*pWString)[0] : 0);
            }
            return 0;
        }

        // [ unsigned char ]
        ZRZ_SIMPLE_TYPE(unsigned char, uchar, UChar)
        unsigned char toUChar() const
        {
            switch(type)
            {
                default : break;
                ZRZ_SIMPLE_CAST(unsigned char);
                case at_string: return (unsigned char)(pString->length() ? (*pString)[0] : 0);
                case at_wstring: return (unsigned char)(pString->length() ? (*pWString)[0] : 0);
            }
            return 0;
        }

        // [ wchar ]
        ZRZ_SIMPLE_TYPE(wchar_t, wchar, WChar)
        wchar_t toWChar() const
        {
            switch(type)
            {
                default : break;
                ZRZ_SIMPLE_CAST(wchar_t);
                case at_string: return (wchar_t)(pString->length() ? (*pString)[0] : 0);
                case at_wstring: return (wchar_t)(pString->length() ? (*pWString)[0] : 0);
            }
            return 0;
        }

        // [ uwchar ]
        ZRZ_SIMPLE_TYPE_NO_OV(unsigned wchar_t, uwchar, UWChar)
        unsigned wchar_t toUWChar() const
        {
            switch(type)
            {
                default : break;
                ZRZ_SIMPLE_CAST(unsigned wchar_t);
                case at_string: return (unsigned wchar_t)(pString->length() ? (*pString)[0] : 0);
                case at_wstring: return (unsigned wchar_t)(pString->length() ? (*pWString)[0] : 0);
            }
            return 0;
        }

        // [ size ]
        ZRZ_SIMPLE_TYPE_NO_OV(size_t, size, Size)
        size_t toSize() const
        {
            switch(type)
            {
                default : break;
                ZRZ_SIMPLE_CAST(size_t);
                case at_string: return (size_t)std::stoll(*pString);
                case at_wstring: return (size_t)std::stoll(*pWString);
            }
            return 0;
        }

        // [ int ]
        ZRZ_SIMPLE_TYPE(int, int, Int)
        int toInt(int base = 10) const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(int);
                    case at_string: return std::stoi(*pString, 0, base);
                    case at_wstring: return std::stoi(*pWString, 0, base);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ unsigned ]
        ZRZ_SIMPLE_TYPE(unsigned, uint, UInt)
        unsigned toUInt(int base = 10) const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(unsigned);
                    case at_string: return std::stoul(*pString, 0, base);
                    case at_wstring: return std::stoul(*pWString, 0, base);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ long ]
        ZRZ_SIMPLE_TYPE(long, long, Long)
        long toLong(int base = 10) const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(long);
                    case at_string: return std::stol(*pString, 0, base);
                    case at_wstring: return std::stol(*pWString, 0, base);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ unsigned long ]
        ZRZ_SIMPLE_TYPE(unsigned long, ulong, ULong)
        unsigned long toULong(int base = 10) const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(unsigned long);
                    case at_string: return std::stoul(*pString, 0, base);
                    case at_wstring: return std::stoul(*pWString, 0, base);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ long long ]
        ZRZ_SIMPLE_TYPE(long long, longlong, LongLong)
        long long toLongLong(int base = 10) const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(long long);
                    case at_string: return std::stoll(*pString, 0, base);
                    case at_wstring: return std::stoll(*pWString, 0, base);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ unsigned long long ]
        ZRZ_SIMPLE_TYPE(unsigned long long, ulonglong, ULongLong)
        unsigned long long toULongLong(int base = 10) const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(unsigned long long);
                    case at_string: return std::stoull(*pString, 0, base);
                    case at_wstring: return std::stoull(*pWString, 0, base);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ float ]
        ZRZ_SIMPLE_TYPE(float, float, Float)
        float toFloat() const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(float);
                    case at_string: return std::stof(*pString);
                    case at_wstring: return std::stof(*pWString);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ double ]
        ZRZ_SIMPLE_TYPE(double, double, Double)
        double toDouble() const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(double);
                    case at_string: return std::stod(*pString);
                    case at_wstring: return std::stod(*pWString);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ long double ]
        ZRZ_SIMPLE_TYPE(long double, longdouble, LongDouble)
        long double toLongDouble() const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(long double);
                    case at_string: return std::stold(*pString);
                    case at_wstring: return std::stold(*pWString);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ void* ]
        ZRZ_SIMPLE_TYPE_PTR(void*, voidptr, VoidPtr)
        long double toVoidPtr() const
        {
            try
            {
                switch(type)
                {
                    default : break;
                    ZRZ_SIMPLE_CAST(long double);
                    case at_string: return std::stoull(*pString);
                    case at_wstring: return std::stoull(*pWString);
                }
            } catch(std::invalid_argument &e) {}
            return 0;
        }

        // [ string ]
        Type typeOf(const string &) { return at_string; }
        Type typeOf(const char *) { return at_string; }
        bool isString() { return type == at_string; }
        any(const string &v) { make(typeOf(v)); (*pString) = v; }
        any(const char *v) { make(typeOf(v)); (*pString) = v; }
        any& operator =(const string &v) { make(typeOf(v)); (*pString) = v; return *this; }
        any& set_string(const string &v) { make(typeOf(v)); (*pString) = v; return *this; }
        any& operator =(const char *v) { make(typeOf(v)); (*pString) = v; return *this; }
        any& set_char_ptr(const char *v) { make(typeOf(v)); (*pString) = v; return *this; }
        string toString() const
        {
            try
            {
                switch(type)
                {
                    default : break;
    //                ZRZ_SIMPLE_CONV(std::to_string); // <<< Ditching this so I can clip the trailing zeros on floating points
                    case at_bool:           return vBool ? "true" : "false";
                    case at_char:           return t_str() += vChar;
                    case at_uchar:          return t_str() += vUChar;
                    case at_wchar:          return t_str() += (t_str::value_type)vWChar;
                    case at_uwchar:         return t_str() += (t_str::value_type)vUWChar;
                    case at_size:           return std::to_string(vSize);
                    case at_int:            return std::to_string(vInt);
                    case at_uint:           return std::to_string(vUInt);
                    case at_long:           return std::to_string(vLong);
                    case at_ulonglong:      return std::to_string(vULongLong);
                    case at_longlong:       return std::to_string(vLongLong);
                    case at_ulong:          return std::to_string(vULong);
                    case at_float:          { auto s = std::to_string(vFloat); s.erase(s.find_last_not_of('0')+1); return s; }
                    case at_double:         { auto s = std::to_string(vDouble); s.erase(s.find_last_not_of('0')+1); return s; }
                    case at_longdouble:     { auto s = std::to_string(vLongDouble); s.erase(s.find_last_not_of('0')+1); return s; }
                    case at_voidptr:        { std::stringstream ss; ss << std::hex << "[0x" << std::uppercase << (std::uintptr_t)vVoidPtr << "]"; return ss.str(); }
                    case at_string:         return *pString;
                    case at_wstring:        return strcnv().to_bytes(*pWString);
                    case at_vector:         return string(pVector->begin(), pVector->end());
                }
            } catch(...){}
            return string();
        }
        string toNumString(int base = 10, bool bUpperCase = false) const
        {
            try
            {
                unsigned long long ll = 0;
                switch(type)
                {
                    default : break;
    //                ZRZ_SIMPLE_CONV(std::to_string); // <<< Ditching this so I can clip the trailing zeros on floating points
                    case at_bool:           ll = (unsigned long long)vBool ? 1 : 0; break;
                    case at_char:           ll = (unsigned long long)vChar; break;
                    case at_uchar:          ll = (unsigned long long)vUChar; break;
                    case at_wchar:          ll = (unsigned long long)vWChar; break;
                    case at_uwchar:         ll = (unsigned long long)vUWChar; break;
                    case at_size:           ll = (unsigned long long)vSize; break;
                    case at_int:            ll = (unsigned long long)vInt; break;
                    case at_uint:           ll = (unsigned long long)vUInt; break;
                    case at_long:           ll = (unsigned long long)vLong; break;
                    case at_ulong:          ll = (unsigned long long)vULong; break;
                    case at_longlong:       ll = (unsigned long long)vLongLong; break;
                    case at_ulonglong:      ll = (unsigned long long)vULongLong; break;
                    case at_float:          ll = (unsigned long long)vFloat; break;
                    case at_double:         ll = (unsigned long long)vDouble; break;
                    case at_longdouble:     ll = (unsigned long long)vLongDouble; break;
                    case at_voidptr:        ll = (unsigned long long)(std::uintptr_t)vVoidPtr; break;
                }
                std::stringstream ss;
                if (16 == base)
                    ss << std::hex;
                if (bUpperCase)
                    ss << std::uppercase;
                ss << ll;
                return ss.str();
            } catch(...){}
            return string();
        }

        // [ wstring ]
        Type typeOf(const wstring &) { return at_wstring; }
        Type typeOf(const wchar_t *) { return at_wstring; }
        bool isWString() { return type == at_wstring; }
        any(const wstring &v) { make(typeOf(v)); (*pWString) = v; }
        any(const wchar_t *v) { make(typeOf(v)); (*pWString) = v; }
        any& operator =(const wstring &v) { make(typeOf(v)); (*pWString) = v; return *this; }
        any& set_wstring(const wstring &v) { make(typeOf(v)); (*pWString) = v; return *this; }
        any& operator =(const wchar_t *v) { make(typeOf(v)); (*pWString) = v; return *this; }
        any& set_wchar_ptr(const wchar_t *v) { make(typeOf(v)); (*pWString) = v; return *this; }
        wstring toWString(int base = 10) const
        {
            switch(type)
            {
                default : break;
//                ZRZ_SIMPLE_CONV(std::to_wstring); // <<< Ditching this so I can clip the trailing zeros on floating points
                case at_bool:           return vBool ? L"true" : L"false";
                case at_char:           return t_wstr() += (t_wstr::value_type)vChar;
                case at_uchar:          return t_wstr() += (t_wstr::value_type)vUChar;
                case at_wchar:          return t_wstr() += vWChar;
                case at_uwchar:         return t_wstr() += vUWChar;
                case at_int:            return std::to_wstring(vInt);
                case at_size:           return std::to_wstring(vSize);
                case at_uint:           return std::to_wstring(vUInt);
                case at_long:           return std::to_wstring(vLong);
                case at_ulonglong:      return std::to_wstring(vULongLong);
                case at_longlong:       return std::to_wstring(vLongLong);
                case at_ulong:          return std::to_wstring(vULong);
                case at_float:          { auto s = std::to_wstring(vFloat); s.erase(s.find_last_not_of('0')+1); return s; }
                case at_double:         { auto s = std::to_wstring(vDouble); s.erase(s.find_last_not_of('0')+1); return s; }
                case at_longdouble:     { auto s = std::to_wstring(vLongDouble); s.erase(s.find_last_not_of('0')+1); return s; }
                case at_voidptr:        { std::wstringstream ss; ss << std::hex << "[0x" << std::uppercase << (std::uintptr_t)vVoidPtr << "]"; return ss.str(); }
                case at_string:         return strcnv().from_bytes(*pString);
                case at_wstring:        return *pWString;
                case at_vector:         return strcnv().from_bytes(string(pVector->begin(), pVector->end()));
            }
            return wstring();
        }

        // [ vector ]
        Type typeOf(const vector &) { return at_vector; }
        bool isVector() { return type == at_vector; }
        any(const vector &v) { make(typeOf(v)); (*pVector) = v; }
        any& operator =(const vector &v) { make(typeOf(v)); (*pVector) = v; return *this; }
        any& set_vector(const vector &v) { make(typeOf(v)); (*pVector) = v; return *this; }
        vector toVector()
        {
            switch(type)
            {
                default :
                {   string s = toString();
                    return vector(s.begin(), s.end());
                }
                case at_string:         return vector(pString->begin(), pString->end());
//                case at_wstring:        return vector(pWString->begin(), pWString->end());
                case at_vector:         return *pVector;
            }
            return vector();
        }

    private:

        union
        {
            bool                vBool;
            char                vChar;
            unsigned char       vUChar;
            wchar_t             vWChar;
            unsigned wchar_t    vUWChar;
            size_t              vSize;
            int                 vInt;
            unsigned            vUInt;
            long                vLong;
            unsigned long       vULong;
            long long           vLongLong;
            unsigned long long  vULongLong;
            float               vFloat;
            double              vDouble;
            double              vLongDouble;
            const void*         vVoidPtr;
            char                vString[sizeof(t_str)];
            char                vWString[sizeof(t_wstr)];
            char                vVector[sizeof(vector)];
        };

        union
        {
            void                *pVoid;
            t_str               *pString;
            t_wstr              *pWString;
            vector              *pVector;
        };

        Type        type;
    };

    typedef any t_any;
    typedef std::map<zru::string, zru::t_any> t_anymap;
    typedef std::map<zru::wstring, zru::t_any> t_wanymap;

}

