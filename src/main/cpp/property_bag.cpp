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

namespace zru
{

// Global property bag
property_bag_ts     pb;


property_bag::property_bag() : m_i(0), m_bArray(false)
{
}

property_bag::property_bag(const t_any &r) : m_i(0), m_bArray(false)
{
    m_v = r;
}

property_bag::property_bag(const property_bag &r) : m_i(0), m_bArray(false)
{
    *this = r;
}

property_bag::property_bag(std::initializer_list<std::pair<t_str, t_any> > a)
     : m_i(0), m_bArray(false)
{
    for(auto it = std::begin(a); std::end(a) != it; it++)
        m_m[it->first] = it->second;
}

// property_bag::property_bag(std::initializer_list<std::pair<t_str, property_bag> > a)
//       : m_i(0), m_bArray(false)
// {
//     for(auto it = std::begin(a); std::end(a) != it; it++)
//         m_m[it->first] = it->second;
// }

property_bag& property_bag::operator = (const property_bag &r)
{
    m_i = r.m_i;
    m_v = r.m_v;
    m_m = r.m_m;
    m_bArray = r.m_bArray;
    return *this;
}

property_bag& property_bag::operator = (const t_any &r)
{
    m_i = 0;
    m_v = r;
    m_m.clear();

    return *this;
}


property_bag& property_bag::operator += (const t_any &r)
{
    m_v += r;

    return *this;
}

property_bag& property_bag::operator -= (const t_any &r)
{
    m_v -= r;

    return *this;
}

property_bag::t_any& property_bag::val()
{
    return m_v;
}

const property_bag::t_any& property_bag::val() const
{
    return m_v;
}

int property_bag::size() const
{
    return m_m.size();
}

int property_bag::length() const
{
    return m_v.toString().length();
}

int property_bag::length(const t_str &k) const
{
    if (!k.length())
        return 0;

    property_bag::const_iterator it = m_m.find(k);
    if (m_m.end() == it)
        return 0;

    return it->second.length();
}

int property_bag::length(const t_str &sep, const t_str &k)
{
    if (!k.length())
        return 0;

    if (!sep.length())
        return length(k);


    t_str::size_type p = 0;
    t_str key = k;
    do
    {
        p = key.find(sep);
        if (t_str::npos == p)
            return length(key);

        if (!p)
            key = key.substr(1);

    } while(!p);

    return (*this)[key.substr(0, p)].length(sep, key.substr(p + sep.length()));
}

bool property_bag::isset(const any &k) const
{
    t_str s = k.toString();
    if (!s.length())
        return false;

    t_map::const_iterator it = m_m.find(s);
    if (m_m.end() == it)
        return false;

    return it->second.isset();
}

bool property_bag::isset(const t_str &k) const
{
    if (!k.length())
        return false;

    t_map::const_iterator it = m_m.find(k);
    if (m_m.end() == it)
        return false;

    return it->second.isset();
}

bool property_bag::isset(const t_str &sep, const t_str &k)
{
    if (!k.length())
        return false;

    if (!sep.length())
        return isset(k);

    t_str::size_type p = 0;
    t_str key = k;
    do
    {
        p = key.find(sep);
        if (t_str::npos == p)
            return isset(key);

        if (!p)
            key = key.substr(1);

    } while(!p);

    return (*this)[key.substr(0, p)].isset(sep, key.substr(p + sep.length()));
}

bool property_bag::isset(std::initializer_list< t_str > a)
{
    for(auto it = std::begin(a); std::end(a) != it; it++)
        if (!isset(*it))
            return false;
    return true;
}

bool property_bag::isset(const t_str &sSep, std::initializer_list< t_str > a)
{
    for(auto it = std::begin(a); std::end(a) != it; it++)
        if (!isset(sSep, *it))
            return false;
    return true;
}

property_bag& property_bag::operator[](const t_any &k)
{
    return m_m[k.toString()];
}

property_bag::const_iterator property_bag::find(const t_any &k) const
{
    return m_m.find(k.toString());
}

property_bag::iterator property_bag::find(const t_any &k)
{
    return m_m.find(k.toString());
}

property_bag& property_bag::at(const t_str &sep, const t_str &k)
{
    // Null length or no sep?
    if (!k.length())
        return *this;

    if (!sep.length())
        return (*this)[k];

    t_str::size_type p = 0;
    t_str key = k;
    do
    {
        p = key.find(sep);
        if (t_str::npos == p)
            return (*this)[key];

        if (!p)
            key = key.substr(1);

    } while(!p);

    return (*this)[key.substr(0, p)].at(sep, key.substr(p + sep.length()));
}

bool property_bag::erase(const t_str &k)
{
    if (!k.length())
        return false;

    t_map::iterator it = m_m.find(k);
    if (m_m.end() == it)
        return false;

    erase(it);

    return true;
}

bool property_bag::erase(const t_str &sep, const t_str &k)
{
    if (!k.length())
        return false;

    if (!sep.length())
        return isset(k);

    t_str::size_type p = 0;
    t_str key = k;
    do
    {
        p = key.find(sep);
        if (t_str::npos == p)
            return erase(key);

        if (!p)
            key = key.substr(1);

    } while(!p);

    return (*this)[key.substr(0, p)].erase(sep, key.substr(p + sep.length()));
}

bool property_bag::apply(const t_str &k, tf_apply f)
{
    if (!k.length())
        return false;

    t_map::iterator it = m_m.find(k);
    if (m_m.end() == it)
        return false;

    return it->second.apply(f);
}

bool property_bag::apply(const t_str &sep, const t_str &k, tf_apply f)
{
    if (!k.length())
        return false;

    if (!sep.length())
        return apply(k, f);

    t_str::size_type p = 0;
    t_str key = k;
    do
    {
        p = key.find(sep);
        if (t_str::npos == p)
            return apply(key, f);

        if (!p)
            key = key.substr(1);

    } while(!p);

    return (*this)[key.substr(0, p)].apply(sep, key.substr(p + sep.length()), f);
}

bool property_bag::apply(std::initializer_list< t_str > a, tf_apply f)
{
    for(auto it = std::begin(a); std::end(a) != it; it++)
        if (!apply(*it, f))
            return false;
    return true;
}

bool property_bag::apply(const t_str &sSep, std::initializer_list< t_str > a, tf_apply f)
{
    for(auto it = std::begin(a); std::end(a) != it; it++)
        if (!apply(sSep, *it, f))
            return false;
    return true;
}

bool property_bag::each(pfn_EachKV f)
{
    for(auto it = begin(); end() != it; it++)
        if (!f(it->first, it->second))
            return false;

    return true;
}

bool property_bag::each(pfn_EachV f)
{
    for(auto it = begin(); end() != it; it++)
        if (!f(it->second))
            return false;

    return true;
}

property_bag::t_size property_bag::push(const property_bag &pb)
{
    (*this)[t_any(m_i).toString()] = pb;
    return m_i++;
}

property_bag::t_size property_bag::push(const t_any &v)
{
    (*this)[t_any(m_i).toString()] = v;
    return m_i++;
}

property_bag property_bag::pop()
{
    iterator it = m_m.begin();
    if (m_m.end() == it)
        return property_bag();

    property_bag ret = it->second;

    m_m.erase(it);

    return ret;
}

property_bag property_bag::pop(long n)
{
    property_bag ret;
    while (0 <= --n)
    {
        iterator it = m_m.begin();
        if (m_m.end() == it)
            break;

        ret[it->first] = it->second;

        m_m.erase(it);
    }

    return ret;
}

property_bag::t_size property_bag::merge(const property_bag &pb, bool bOverwrite)
{
    t_size n = 1;
    m_v = pb.m_v;
    for (t_map::const_iterator it = pb.m_m.begin(); pb.m_m.end() != it; it++)
        if (bOverwrite || !isset(it->first))
            (*this)[it->first].merge(it->second, bOverwrite);

    return n;
}

property_bag::t_size property_bag::map_keys(const property_bag &keys, property_bag &pb, bool bOverwrite, bool bBidirectional)
{
    t_size n = 1;
    m_v = pb.m_v;
    for (auto it = keys.m_m.begin(); keys.m_m.end() != it; it++)
    {
        bool bFirst = isset(it->first);
        bool bSecond = pb.isset(it->second.val().toString());

        if (!bFirst && bSecond)
            (*this)[it->first] = pb[it->second.val()];

        else if (bFirst && bSecond && bOverwrite)
            (*this)[it->first] = pb[it->second.val()];

        else if (bFirst && !bSecond && bBidirectional)
            pb[it->second.val()] = (*this)[it->first];
    }

    return n;
}

int property_bag_ts::size(const string &sSep, const string &sKey)
{
    t_scopelock lk(m_mLock);

    return m_pb.at(sSep, sKey).size();
}

int property_bag_ts::length(const string &sSep, const string &sKey)
{
    t_scopelock lk(m_mLock);

    return m_pb.at(sSep, sKey).val().toString().length();
}

bool property_bag_ts::set(const string &sSep, const string &sKey, const property_bag &pbVal)
{
    t_scopelock lk(m_mLock);

    m_pb.at(sSep, sKey) = pbVal;

    _sig(sSep, sKey);

    return true;
}

bool property_bag_ts::set(const string &sSep, const string &sKey, const property_bag::t_any &vVal)
{
    t_scopelock lk(m_mLock);

    if (vVal.isVoid())
        m_pb.erase(sSep, sKey);
    else
        m_pb.at(sSep, sKey) = vVal;

    _sig(sSep, sKey);

    return true;
}

property_bag property_bag_ts::get(const string &sSep, const string &sKey)
{
    std::unique_lock<std::mutex> lk(m_mLock);

    return m_pb.at(sSep, sKey);
}

bool property_bag_ts::inc(const string &sSep, const string &sKey, const property_bag::t_any &v)
{
    t_scopelock lk(m_mLock);

    m_pb.at(sSep, sKey) += v;

    _sig(sSep, sKey);

    return true;
}

bool property_bag_ts::dec(const string &sSep, const string &sKey, const property_bag::t_any &v)
{
    t_scopelock lk(m_mLock);

    m_pb.at(sSep, sKey) += v;

    _sig(sSep, sKey);

    return true;
}

bool property_bag_ts::merge(const string &sSep, const string &sKey, const property_bag &pbValue, bool bOverwrite)
{
    t_scopelock lk(m_mLock);

    m_pb.at(sSep, sKey).merge(pbValue, bOverwrite);

    _sig(sSep, sKey);

    return true;
}

bool property_bag_ts::map_keys(const string &sSep, const string &sKey, const property_bag &keys, property_bag &pb, bool bOverwrite, bool bBidirectional)
{
    t_scopelock lk(m_mLock);

    m_pb.at(sSep, sKey).map_keys(keys, pb, bOverwrite, bBidirectional);

    _sig(sSep, sKey);

    return true;
}


bool property_bag_ts::isset(const string &sSep, const string &sKey)
{
    t_scopelock lk(m_mLock);

    return m_pb.isset(sSep, sKey);
}

bool property_bag_ts::erase(const string &sSep, const string &sKey)
{
    t_scopelock lk(m_mLock);

    if (!m_pb.erase(sSep, sKey))
        return false;

    _sig(sSep, sKey);

    return true;
}

property_bag::t_size property_bag_ts::push(const string &sSep, const string &sKey, const property_bag &pbValue, bool bSignalAll)
{
    t_scopelock lk(m_mLock);

    property_bag::t_size ret = m_pb.at(sSep, sKey).push(pbValue);

    _sig(sSep, sKey, bSignalAll);

    return ret;
}

property_bag::t_size property_bag_ts::push(const string &sSep, const string &sKey, const property_bag::t_any &vValue, bool bSignalAll)
{
    t_scopelock lk(m_mLock);

    property_bag::t_size ret = m_pb.at(sSep, sKey).push(vValue);

    _sig(sSep, sKey, bSignalAll);

    return ret;
}

property_bag property_bag_ts::popn(const string &sSep, const string &sKey, property_bag::t_size n)
{
    t_scopelock lk(m_mLock);

    property_bag &r = m_pb.at(sSep, sKey);

    if (!r.size())
        return property_bag();

    property_bag ret = r.pop(n);

    return ret;
}


property_bag property_bag_ts::popn(const string &sSep, const string &sKey, property_bag::t_size n, property_bag::t_size uTimeout)
{
    t_scopelock lk(m_mLock);

    property_bag ret;
    property_bag &r = m_pb.at(sSep, sKey);

    if (r.size())
        ret = r.pop(n);

    else if (!uTimeout)
        return ret;

    else
    {
        string key(sSep + sKey);

        t_pbwptr pbw(new pb_waitable());

        _wait(pbw, lk, key, uTimeout);

        property_bag &r = m_pb.at(sSep, sKey);

        if (!r.size())
            return ret;

        ret = r.pop(n);
    }

    return ret;
}

property_bag property_bag_ts::pop(const string &sSep, const string &sKey)
{
    t_scopelock lk(m_mLock);

    property_bag &r = m_pb.at(sSep, sKey);

    if (!r.size())
        return property_bag();

    property_bag ret = r.pop();

    return ret;
}

property_bag property_bag_ts::pop(const string &sSep, const string &sKey, property_bag::t_size uTimeout)
{
    t_scopelock lk(m_mLock);

    property_bag ret;
    property_bag &r = m_pb.at(sSep, sKey);

    if (r.size())
        ret = r.pop();

    else if (!uTimeout)
        return ret;

    else
    {
        string key(sSep + sKey);

        t_pbwptr pbw(new pb_waitable());

        _wait(pbw, lk, key, uTimeout);

        property_bag &r2 = m_pb.at(sSep, sKey);

        if (!r2.size())
            return ret;

        ret = r2.pop();
    }

    return ret;
}

bool property_bag_ts::pop(property_bag &p, const string &sSep, const string &sKey, property_bag::t_size uTimeout)
{
    t_scopelock lk(m_mLock);

    property_bag &r = m_pb.at(sSep, sKey);

    if (r.size())
        p = r.pop();

    else if (!uTimeout)
        return false;

    else
    {
        string key(sSep + sKey);

        t_pbwptr pbw(new pb_waitable());

        _wait(pbw, lk, key, uTimeout);

        property_bag &r2 = m_pb.at(sSep, sKey);

        if (!r2.size())
            return false;

        p = r2.pop();
    }

    return true;
}


property_bag property_bag_ts::swp(const string &sSep, const string &sKey, const property_bag &pbValue)
{
    property_bag pb;
    t_scopelock lk(m_mLock);

    pb = m_pb.at(sSep, sKey);

    m_pb.at(sSep, sKey) = pbValue;

    _sig(sSep, sKey);

    return pb;

}

property_bag property_bag_ts::swp(const string &sSep, const string &sKey, const property_bag::t_any &vValue)
{
    property_bag pb;
    t_scopelock lk(m_mLock);

    pb = m_pb.at(sSep, sKey);

    m_pb.at(sSep, sKey) = vValue;

    _sig(sSep, sKey);

    return pb;

}

property_bag property_bag_ts::swp(const string &sSep, const string &sKey, const property_bag &pbValue, property_bag::t_size uTimeout)
{
    property_bag pb;
    t_scopelock lk(m_mLock);

    property_bag *p = &m_pb.at(sSep, sKey);
    if (uTimeout && !p->size())
    {
        string key(sSep + sKey);

        t_pbwptr pbw(new pb_waitable());

        _wait(pbw, lk, key, uTimeout);

        p = &m_pb.at(sSep, sKey);

        if (!p->size())
            return property_bag();
    }

    pb = *p;

    *p = pbValue;

    _sig(sSep, sKey);

    return pb;
}

property_bag property_bag_ts::swp(const string &sSep, const string &sKey, const property_bag::t_any &vValue, property_bag::t_size uTimeout)
{
    property_bag pb;
    t_scopelock lk(m_mLock);

    property_bag *p = &m_pb.at(sSep, sKey);
    if (uTimeout && !p->length())
    {
        string key(sSep + sKey);

        t_pbwptr pbw(new pb_waitable());

        _wait(pbw, lk, key, uTimeout);

        p = &m_pb.at(sSep, sKey);

        if (!p->size())
            return property_bag();
    }

    pb = *p;

    *p = vValue;

    _sig(sSep, sKey);

    return pb;
}

void property_bag_ts::sig(const t_str &sSep, const t_str &sKey, bool bSignalAll)
{
    t_scopelock lk(m_mLock);

    _sig(sSep, sKey, bSignalAll);
}

bool property_bag_ts::wait(const t_str &sSep, const t_str &sKey, property_bag::t_size uTimeout)
{
    t_str key(sSep + sKey);

    t_pbwptr pbw(new pb_waitable());

    t_scopelock lk(m_mLock);

    return _wait(pbw, lk, key, uTimeout);
}

bool property_bag_ts::wait_multiple(const t_str &sSep, const t_strlist &keys, property_bag::t_size uTimeout)
{
    if (!keys.size())
        return false;

    t_strlist sl;
    if (!sSep.length())
        sl = keys;
    else
        for (auto it = keys.begin(); keys.end() != it; it++)
            sl.push_back(sSep + *it);

    // Create our wait object
    t_pbwptr pbw(new pb_waitable());

    t_scopelock lk(m_mLock);

    return _wait_multiple(pbw, lk, sl, uTimeout);
}

void property_bag_ts::add_named_wait(const t_str &sName, const t_str &sSep, const t_str &sKey)
{
    t_str key(sSep + sKey);

    // Create our wait object
    t_pbwptr pbw(new pb_waitable(key));

    t_scopelock lk(m_mLock);

    // +++ Warn if there are a lot of these
    if (100 < m_waitable.size())
        ZruWarning("There are ", m_waitable.size(), " named waits on property bag ", &sName);

    // Find existing wait
    auto it = m_waitable.find(sName);

    // Remove if it exists, +++ not sure if we shouldn't just fail?
    if(m_waitable.end() != it)
    {   t_strlist &keys = it->second->get_keys();
        for (auto kit = keys.begin(); keys.end() != kit; kit++)
            _remove_wait(*kit, it->second);
        m_waitable.erase(it);
    }

    m_waitable[sName] = pbw;

    _add_wait(key, pbw);
}

void property_bag_ts::add_named_wait_multiple(const t_str &sName, const t_str &sSep, const t_strlist &keys)
{
    if (!keys.size())
        return;

    t_strlist sl;
    if (!sSep.length())
        sl = keys;
    else
        for (auto it = keys.begin(); keys.end() != it; it++)
            sl.push_back(sSep + *it);

    t_pbwptr pbw(new pb_waitable(sl));

    t_scopelock lk(m_mLock);

    // +++ Warn if there are a lot of these
    if (100 < m_waitable.size())
        ZruWarning("There are ", m_waitable.size(), " named waits on property bag ", &sName);

    // Find existing wait
    auto it = m_waitable.find(sName);

    // Remove if it exists, +++ not sure if we shouldn't just fail?
    if(m_waitable.end() != it)
    {   t_strlist &keys = it->second->get_keys();
        for (auto kit = keys.begin(); keys.end() != kit; kit++)
            _remove_wait(*kit, it->second);
        m_waitable.erase(it);
    }

    m_waitable[sName] = pbw;

    for (auto kit = sl.begin(); sl.end() != kit; kit++)
        _add_wait(*kit, pbw);
}

int64_t property_bag_ts::get_named_wait_count(const t_str &sName)
{
    t_scopelock lk(m_mLock);

    auto it = m_waitable.find(sName);
    if(m_waitable.end() == it)
        return false;

    return it->second->get_wait_count();
}

int64_t property_bag_ts::get_named_update_count(const t_str &sName)
{
    t_scopelock lk(m_mLock);

    auto it = m_waitable.find(sName);
    if(m_waitable.end() == it)
        return false;

    return it->second->get_update_count();
}

bool property_bag_ts::named_wait(const t_str &sName, property_bag::t_size uTimeout)
{
    t_scopelock lk(m_mLock);

    auto it = m_waitable.find(sName);
    if(m_waitable.end() == it)
        return false;

    return (std::cv_status::no_timeout == it->second->wait_for(lk, std::chrono::milliseconds(uTimeout)));
}

void property_bag_ts::remove_named_wait(const t_str &sName)
{
    t_scopelock lk(m_mLock);

    auto it = m_waitable.find(sName);
    if(m_waitable.end() == it)
        return;

    t_strlist &keys = it->second->get_keys();
    for (auto kit = keys.begin(); keys.end() != kit; kit++)
        _remove_wait(*kit, it->second);

    m_waitable.erase(it);
}

void property_bag_ts::remove_all_named_waits()
{
    for (auto it = m_waitable.begin(); m_waitable.end() != it;)
    {
        t_strlist &keys = it->second->get_keys();
        for (auto kit = keys.begin(); keys.end() != kit; kit++)
            _remove_wait(*kit, it->second);
        m_waitable.erase(it++);
    }
}

void property_bag_ts::_add_wait(const t_str &sKey, t_pbwptr pbw)
{
    m_wait[sKey].push_back(pbw);
}

void property_bag_ts::_remove_wait(const t_str &sKey, t_pbwptr pbw)
{
    // Erase the wait object
    auto it = m_wait.find(sKey);
    if(m_wait.end() == it)
        return;

    // If size is one, it's just us
    if (it->second.size() <= 1)
        m_wait.erase(it);

    // Find our object in the list (I'm guessing it's not worth a map here)
    else
        for (auto cit = it->second.begin(); it->second.end() != cit; cit++)
            if (cit->get() == pbw.get())
            {
                it->second.erase(cit);
                break;
            }
}

void property_bag_ts::_sig(const t_str &sKey, bool bSignalAll)
{
    if (!sKey.length())
        return;

    auto it = m_wait.find(sKey);
    if (m_wait.end() == it)
        return;

    for (auto cit = it->second.begin(); it->second.end() != cit; cit++)
    {
        if (bSignalAll)
            cit->get()->notify_all();
        else
        {
            cit->get()->notify_one();
            break;
        }
    }
}

void property_bag_ts::_sig(const t_str &sSep, const t_str &sKey, bool bSignalAll)
{
    if (!sKey.length())
        return;

    if (!sSep.length())
        return _sig(sKey, bSignalAll);

    t_str::size_type p = 0;
    t_str k, key = sKey;
    while(key.length())
    {
        p = key.find_first_of(sSep);
        if (t_str::npos == p)
        {	_sig(k.append(sSep).append(key), bSignalAll);
            return;
        }
        else
            _sig(k.append(sSep).append(key.substr(0,p)), bSignalAll),
            key = key.substr(p + sSep.length());
    };
}

bool property_bag_ts::_wait(t_pbwptr pbw, t_scopelock &lk, const t_str &key, property_bag::t_size uTimeout)
{
    _add_wait(key, pbw);

    bool bRet = (std::cv_status::no_timeout == pbw->wait_for(lk, std::chrono::milliseconds(uTimeout)));

    _remove_wait(key, pbw);

    return bRet;
}

bool property_bag_ts::_wait_multiple(t_pbwptr pbw, t_scopelock &lk, const t_strlist &keys, property_bag::t_size uTimeout)
{
    if (!keys.size())
        return false;

    // Add the hooks
    for (auto it = keys.begin(); keys.end() != it; it++)
        _add_wait(*it, pbw);

    // Wait for something
    bool bRet = (std::cv_status::no_timeout == pbw->wait_for(lk, std::chrono::milliseconds(uTimeout)));

    // Remove the hooks
    for (auto it = keys.begin(); keys.end() != it; it++)
        _remove_wait(*it, pbw);

    return bRet;
}

}
