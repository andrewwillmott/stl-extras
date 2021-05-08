//
//  File:       vector_map.h
//
//  Function:   Defines vector_map -- a map based on std::vector. Avoids alloc
//              per-entry and keeps all key/values in linear storage for coherent
//              iteration.
//
//  Copyright:  Andrew Willmott
//


#include <algorithm>
#include <memory>
#include <vector>

namespace std
{
    template<class Compare, class Pair> struct less_first;  // Helper class for key comparisons

    template
    <
      class Key, class Value,
      class KeyCompare = std::less<Key>,
      class Alloc      = std::allocator<std::pair<Key, Value>>
    >
    class vector_map : public std::vector<std::pair<Key, Value>, Alloc>
    {
    public:
        // Types
        typedef std::pair <Key, Value>           KeyValue;
        typedef less_first<KeyCompare, KeyValue> KeyLess;
        typedef std::vector<KeyValue, Alloc>     super;

        // mimic map types
        typedef Key                              key_type;
        typedef KeyValue                         value_type;
        typedef KeyCompare                       key_compare;
        typedef typename super::iterator         iterator;
        typedef typename super::const_iterator   const_iterator;
        typedef typename super::size_type        size_type;

        typedef std::pair<iterator, bool>        iterator_and_added;

        // Methods
        iterator            find(const Key& key);
        const_iterator      find(const Key& key) const;

        Value&              operator[](const Key& key);

        iterator_and_added  insert(const KeyValue& key_and_value);  // STL semantics: if the key was already present, its value is *not* updated & added=false
        iterator            insert(const_iterator position, const KeyValue& key_and_value);  // Useful if a previous find() has already returned the appropriate insertion point
        size_type           erase (const Key& key);  // erase element with the given key. Returns count of erased items, which is always 0 or 1 in this case.
        using super::erase;  // Avoid method shadowing of vector::erase due to above

        // Custom methods
        Key&                key  (int i);        // <- call commit if keys are updated.
        const Key&          key  (int i) const;
        Value&              value(int i);
        const Value&        value(int i) const;

    #ifdef INHERITED_METHODS
        // Selected methods from vector:
        iterator            begin();
        const_iterator      begin() const;
        iterator            end();
        const_iterator      end() const;

        bool                empty() const;
        size_type           size() const;
        void                reserve(size_type n);

        reference           at(size_type n);                    // <- call commit if keys are updated.
        const_reference     at(size_type n) const;

        void                push_back(const value_type& value); // <- call commit() if you don't add keys in pre-sorted order
        reference           push_back();
    #endif

        void commit();   // Call this after modifying values using the array API. It will re-sort the values according to key.
    };

    template <class Compare, class Pair>
    struct less_first
    {
        bool operator()(const Pair& a, const Pair& b)
        {
            return m_compare(a.first, b.first);
        }
        bool operator()(const Pair& a, const typename Pair::first_type& b)
        {
            return m_compare(a.first, b);
        }
        bool operator()(const typename Pair::first_type& a, const typename Pair::first_type& b)
        {
            return m_compare(a, b);
        }

        Compare m_compare;
    };

    // --------------------------------------------------------------------------
    // Inlines
    // --------------------------------------------------------------------------

    #define VECTOR_MAP_TMPL template<class Key, class Value, class KeyCompare, class Alloc>
    #define VECTOR_MAP      vector_map<Key, Value, KeyCompare, Alloc>

    VECTOR_MAP_TMPL inline typename VECTOR_MAP::iterator VECTOR_MAP::find(const Key& key)
    {
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, it->first))    // if it->first >= key and key >= it->first, then it->first == key.
            return it;

        return this->end();
    }

    VECTOR_MAP_TMPL inline typename VECTOR_MAP::const_iterator VECTOR_MAP::find(const Key& key) const
    {
        KeyLess less;
        const_iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, it->first))
            return it;

        return this->end();
    }

    VECTOR_MAP_TMPL inline Value& VECTOR_MAP::operator[](const Key& key)
    {
        return insert(value_type(key, Value())).first->second;
    }

    VECTOR_MAP_TMPL inline typename VECTOR_MAP::iterator_and_added VECTOR_MAP::insert(const KeyValue& key_and_value)
    {
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key_and_value.first, less);

        if (it != this->end() && !less(key_and_value.first, it->first))  // Don't insert if the key already exists
            return make_pair(it, false);

        return make_pair(super::insert(it, key_and_value), true);
    }

    VECTOR_MAP_TMPL inline typename VECTOR_MAP::iterator VECTOR_MAP::insert(typename VECTOR_MAP::const_iterator it, const KeyValue& key_and_value)
    {
        // This is supposed to be a performance-sensitive class, so we don't try to fix up an incorrect iterator
        return super::insert(it, key_and_value);
    }

    VECTOR_MAP_TMPL inline typename VECTOR_MAP::size_type VECTOR_MAP::erase(const Key& key)
    {
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, it->first))
        {
            super::erase(it);
            return 1;
        }

        return 0;
    }

    VECTOR_MAP_TMPL inline Key& VECTOR_MAP::key(int i)
    {
        return this->data()[i].first;
    }

    VECTOR_MAP_TMPL inline const Key& VECTOR_MAP::key(int i) const
    {
        return this->data()[i].first;
    }

    VECTOR_MAP_TMPL inline Value& VECTOR_MAP::value(int i)
    {
        return this->data()[i].second;
    }

    VECTOR_MAP_TMPL inline const Value& VECTOR_MAP::value(int i) const
    {
        return this->data()[i].second;
    }

    VECTOR_MAP_TMPL inline void VECTOR_MAP::commit()    // Call this after modifying values using the array API.
    {
        stable_sort(this->begin(), this->end());
    }

    #undef VECTOR_MAP_TMPL
    #undef VECTOR_MAP
}
