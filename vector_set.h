//
//  File:       vector_set.h
//
//  Function:   Defines vector_map -- a set based on std::vector. Avoids alloc
//              per-entry and keeps all key/values in linear storage for coherent
//              iteration.
//
//  Copyright:  Andrew Willmott
//

#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace std
{
    template
    <
        class Key,
        class KeyCompare = std::less<Key>,
        class Alloc = std::allocator<Key>
    >
    class vector_set : public std::vector<Key, Alloc>
    {
    public:
        // Types
        typedef KeyCompare                     KeyLess;
        typedef std::vector<Key>               super;

        // mimic set types
        typedef Key                            key_type;
        typedef Key                            value_type;
        typedef KeyCompare                     key_compare;
        typedef typename super::iterator       iterator;
        typedef typename super::const_iterator const_iterator;
        typedef typename super::size_type      size_type;

        typedef std::pair<iterator, bool>      iterator_and_added;

        // Methods
        iterator            find(const Key& key);
        const_iterator      find(const Key& key) const;

        iterator_and_added  insert(const Key& key);  // STL semantics: if the key was already present, its value is *not* updated & added=false
        size_type           erase (const Key& key);  // erase element with the given key. Returns count of erased items, which is always 0 or 1 in this case.
        using super::erase;  // Avoid method shadowing of vector::erase due to above

        // Custom methods
        Key&                key(int i);                    // <- call commit if keys are updated.
        const Key&          key(int i) const;

        bool                update(const Key& key);  // Update previous key or insert if not present. Returns true if key was added rather than updated.

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

        void commit();   // Call this after modifying values using the array API. It will re-sort the values.
    };


    // --------------------------------------------------------------------------
    // Inlines
    // --------------------------------------------------------------------------

    #define VECTOR_SET_TMPL template<class Key, class KeyCompare, class Alloc>
    #define VECTOR_SET vector_set<Key, KeyCompare, Alloc>

    VECTOR_SET_TMPL inline typename VECTOR_SET::iterator VECTOR_SET::find(const Key& key)
    {
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, *it))  // if it->first >= key and key >= it->first, then it->first == key.
            return it;

        return this->end();
    }

    VECTOR_SET_TMPL inline typename VECTOR_SET::const_iterator VECTOR_SET::find(const Key& key) const
    {
        KeyLess less;
        const_iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, *it))  //
            return it;

        return this->end();
    }

    VECTOR_SET_TMPL inline typename VECTOR_SET::iterator_and_added VECTOR_SET::insert(const Key& key)
    {
        // reproduce bizarre STL semantics
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, *it))  // Don't insert if the key already exists
            return make_pair(it, false);

        return make_pair(super::insert(it, key), true);
    }

    VECTOR_SET_TMPL inline typename VECTOR_SET::size_type VECTOR_SET::erase(const Key& key)
    {
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, *it))
        {
            super::erase(it);
            return 1;
        }

        return 0;
    }

    VECTOR_SET_TMPL inline Key& VECTOR_SET::key(int i)
    {
        return this->data()[i];
    }

    VECTOR_SET_TMPL inline const Key& VECTOR_SET::key(int i) const
    {
        return this->data()[i];
    }

    VECTOR_SET_TMPL inline bool VECTOR_SET::update(const Key& key)
    {
        KeyLess less;
        iterator it = lower_bound(this->begin(), this->end(), key, less);

        if (it != this->end() && !less(key, *it))  // Don't insert if the key already exists
        {
            *it = key;
            return false;
        }

        super::insert(it, key);
        return true;
    }

    VECTOR_SET_TMPL inline void VECTOR_SET::commit()  // Call this after modifying values using the array API.
    {
        stable_sort(this->begin(), this->end());
    }

    #undef VECTOR_SET_TMPL
    #undef VECTOR_SET
}