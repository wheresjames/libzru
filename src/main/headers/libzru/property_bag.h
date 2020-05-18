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

class property_bag
{
public:

    typedef any t_any;

    typedef string t_str;

    typedef t_str::value_type t_char;

    typedef long t_size;

    typedef std::map<t_str, property_bag> t_map;

public:

    //--------------------------------------------------------------
    // std standard functions
    //--------------------------------------------------------------

    typedef typename t_map::iterator iterator;
    typedef typename t_map::const_iterator const_iterator;

    iterator begin() { return m_m.begin(); }
    const_iterator begin() const { return m_m.begin(); }

    iterator end() { return m_m.end(); }
    const_iterator end() const { return m_m.end(); }

    iterator erase( iterator it ) { return m_m.erase( it ); }

    void clear() { m_m.clear(); m_v = t_any(); m_i = 0; m_bArray = false; }

public:

    property_bag();

    property_bag(const t_any &r);

    property_bag(const property_bag &r);

    property_bag(std::initializer_list<std::pair<t_str, t_any> > a);

    // property_bag(std::initializer_list<std::pair<t_str, property_bag> > a);

public:

    property_bag& operator = (const t_any &r);

    property_bag& operator = (const property_bag &r);

    property_bag& operator += (const t_any &r);

    property_bag& operator -= (const t_any &r);

    property_bag& operator[](const t_any &k);

    iterator find(const t_any &k);
    const_iterator find(const t_any &k) const;

public:

    int size() const;

    int length() const;

    int length(const t_str &k) const;

    int length(const t_str &sep, const t_str &k);

    inline bool isset() const { return m_m.size() || !m_v.isVoid(); }

    bool isset(const any &k) const;

    bool isset(const t_str &k) const;

    bool isset(const t_str &sep, const t_str &k);

    bool isset(std::initializer_list< t_str > a);

    bool isset(const t_str &sSep, std::initializer_list< t_str > a);

    bool isArray() const { return m_bArray; }

    void setArray(bool b) { m_bArray = b; }

    void setIdx(t_size i) { m_i = i; }

    t_any& val();

    const t_any& val() const;

    property_bag& at(const t_str &sep, const t_str &k);

public:

    typedef bool (*tf_apply)(property_bag &);

    bool apply(tf_apply f) { return f(*this); }

    bool apply(const t_str &k, tf_apply f);

    bool apply(const t_str &sep, const t_str &k, tf_apply f);

    bool apply(std::initializer_list< t_str > a, tf_apply f);

    bool apply(const t_str &sSep, std::initializer_list< t_str > a, tf_apply f);

    typedef std::function< bool (const t_str &k, property_bag &v) > pfn_EachKV;
    bool each(pfn_EachKV f);

    typedef std::function< bool (property_bag &v) > pfn_EachV;
    bool each(pfn_EachV f);

public:

    bool erase(const t_str &k);

    bool erase(const t_str &sep, const t_str &k);

    t_size merge(const property_bag &pb, bool bOverwrite = true);

    t_size map_keys(const property_bag &keys, property_bag &pb, bool bOverwrite = true, bool bBidirectional = true);

    t_size map_keys(const property_bag &keys, bool bOverwrite = true, bool bBidirectional = true)
    {   return map_keys(keys, *this, bOverwrite, bBidirectional); }

    t_size push(const property_bag &pb);

    t_size push(const t_any &v);

    property_bag pop();

    property_bag pop(long n);


private:

    // The value
    t_any        m_v;

    // Index variable (used for push())
    t_size       m_i;

    // The property map
    t_map        m_m;

    // Non-zero if this is an array
    bool         m_bArray;

};


class property_bag_ts
{
public:

    typedef property_bag::t_str t_str;

    typedef std::mutex t_lock;

    typedef std::condition_variable t_condition;

    typedef std::unique_lock<t_lock> t_scopelock;

    typedef std::list<t_str> t_strlist;

public:

    class pb_waitable
    {
    public:

        pb_waitable()
        {
            wait_count = -1;
            update_count = 0;
        }

        pb_waitable(const t_str &sKey)
        {
            wait_count = 0;
            update_count = 0;
            keys.push_back(sKey);
        }

        pb_waitable(const t_strlist &lKeys)
        {
            wait_count = 0;
            update_count = 0;
            keys = lKeys;
        }

        void notify_one()
        {
            update_count++;
            cond.notify_one();
        }

        void notify_all()
        {
            update_count++;
            cond.notify_all();
        }

        template<typename REP, typename PERIOD>
            std::cv_status wait_for(std::unique_lock<std::mutex>& lock,
                                    const std::chrono::duration<REP, PERIOD>& rel_time
                                )
            {
                std::cv_status r = std::cv_status::no_timeout;
                if (0 > wait_count || wait_count == update_count)
                    r = cond.wait_for(lock, rel_time);
                wait_count = update_count;
                return r;
            }

        int64_t get_wait_count() { return wait_count; }

        int64_t get_update_count() { return update_count; }

        t_strlist& get_keys() { return keys; }

        t_condition& get_condition() { return cond; }

    private:

        // Incremented when this value is updated
        int64_t         update_count;

        // Set equal to update count after waiting
        int64_t         wait_count;

        // Condition to wait for
        t_condition     cond;

        // The key this condition is for
        t_strlist       keys;
    };

public:

    typedef std::shared_ptr<pb_waitable> t_pbwptr;

    typedef std::list< t_pbwptr > t_wait_list;

    typedef std::map<t_str, t_wait_list> t_wait_map;

    typedef std::map<t_str, t_pbwptr> t_waitable_map;

public:

    /// Default constructor
    property_bag_ts() {}

    /// Default destructor
    ~property_bag_ts() {}

    /// Returns the size of the specified array
    int size(const t_str &sSep, const t_str &sKey);

    /// Returns the length of the specified t_str
    int length(const t_str &sSep, const t_str &sKey);

    /// Sets the specified value in the property bag
    bool set(const t_str &sSep, const t_str &sKey, const property_bag &pbValue);

    /// Sets the specified value in the property bag
    bool set(const t_str &sSep, const t_str &sKey, const property_bag::t_any &vValue);

    /// Gets the specified value from the property bag
    property_bag get(const t_str &sSep, const t_str &sKey);

    /// Increments the specified key by the given amount
    bool inc(const t_str &sSep, const t_str &sKey, const property_bag::t_any &v);

    /// Decrements the specified key by the given amount
    bool dec(const t_str &sSep, const t_str &sKey, const property_bag::t_any &v);

    /// Merge the given property bag into the specified key
    bool merge(const t_str &sSep, const t_str &sKey, const property_bag &pbValue, bool bOverwrite = true);

    /// Map the specified keys
    bool map_keys(const string &sSep, const string &sKey, const property_bag &keys, property_bag &pb, bool bOverwrite = true, bool bBidirectional = true);

    /// Map the specified keys
    bool map_keys(const string &sSep, const string &sKey, const property_bag &keys, bool bOverwrite = true, bool bBidirectional = true)
    {   return map_keys(sSep, sKey, keys, m_pb, bOverwrite, bBidirectional); }

    /// Returns non-zero if the specified key is set to a non-void value
    bool isset(const t_str &sSep, const t_str &sKey);

    /// Erases the value at the specified key
    bool erase(const t_str &sSep, const t_str &sKey);

public:

    /// Pushes the specified value into the property_bag
    property_bag::t_size push(const t_str &sSep, const t_str &sKey, const property_bag &pbValue, bool bSignalAll);

    /// Pushes the specified value into the property_bag
    property_bag::t_size push(const t_str &sSep, const t_str &sKey, const property_bag::t_any &vValue, bool bSignalAll);

    /// Pops the specified number of items fromm the property bag
    property_bag popn(const t_str &sSep, const t_str &sKey, property_bag::t_size n);

    /// Pops the specified number of items fromm the property bag, waits up to uTimeout for something to appear
    property_bag popn(const t_str &sSep, const t_str &sKey, property_bag::t_size n, property_bag::t_size uTimeout);

    /// Pops one item from the property bag
    property_bag pop(const t_str &sSep, const t_str &sKey);

    /// Pops one item from the property bag, waits up to uTimeout for something to appear
    property_bag pop(const t_str &sSep, const t_str &sKey, property_bag::t_size uTimeout);

    /// Pops one item from the property bag, waits up to uTimeout for something to appear
    bool pop(property_bag &p, const t_str &sSep, const t_str &sKey, property_bag::t_size uTimeout);

    /// Swaps the specified values
    property_bag swp(const t_str &sSep, const t_str &sKey, const property_bag &pbValue);

    /// Swaps the specified values
    property_bag swp(const t_str &sSep, const t_str &sKey, const property_bag::t_any &vValue);

    /// Swaps the specified values
    property_bag swp(const t_str &sSep, const t_str &sKey, const property_bag &pbValue, property_bag::t_size uTimeout);

    /// Swaps the specified values
    property_bag swp(const t_str &sSep, const t_str &sKey, const property_bag::t_any &vValue, property_bag::t_size uTimeout);

public:

    /// Signals that the specified key has changed
    void sig(const t_str &sSep, const t_str &sKey, bool bSignalAll = true);

    /// Waits for the specified key to change
    bool wait(const t_str &sSep, const t_str &sKey, property_bag::t_size uTimeout);
    bool wait_multiple(const t_str &sSep, const t_strlist &keys, property_bag::t_size uTimeout);

    /// Waits for the specified key to change
    void add_named_wait(const t_str &sName, const t_str &sSep, const t_str &sKey);
    void add_named_wait_multiple(const t_str &sName, const t_str &sSep, const t_strlist &keys);
    bool named_wait(const t_str &sName, property_bag::t_size uTimeout);
    int64_t get_named_wait_count(const t_str &sName);
    int64_t get_named_update_count(const t_str &sName);
    void remove_named_wait(const t_str &sName);
    void remove_all_named_waits();

protected:

    // --- These functions don't lock, you must lock before calling them --- //

    /// Waits for the specified key to change
    bool _wait(t_pbwptr pbw, t_scopelock &lk, const t_str &key, property_bag::t_size uTimeout);
    bool _wait_multiple(t_pbwptr pbw, t_scopelock &lk, const t_strlist &keys, property_bag::t_size uTimeout);

    /// Signals that the specified key has changed, does not lock
    void _sig(const t_str &sKey, bool bSignalAll = true);
    void _sig(const t_str &sSep, const t_str &sKey, bool bSignalAll = true);

    /// Adds the specified key to the wait list
    void _add_wait(const t_str &sKey, t_pbwptr pbw);

    /// Removes the specified key from the wait list
    void _remove_wait(const t_str &sKey, t_pbwptr pbw);

private:

    /// The protected property bag
    property_bag                m_pb;

    /// Thread mutex
    t_lock                      m_mLock;

    /// Signals the thread
    t_condition                 m_cvLock;

    /// Map of wait objects
    t_wait_map                  m_wait;

    /// Map of waitable objects
    t_waitable_map              m_waitable;
};

    // Global property bag
    extern property_bag_ts     pb;
}
