#pragma once

#include <unordered_map>

// https://gpfault.net/posts/mapping-types-to-values.txt.html
template <class ValueType>
class TypeMap {
    // Internally, we'll use a hash table to store mapping from type
    // IDs to the values.
    typedef std::unordered_map<int, ValueType> InternalMap;
public:
    typedef typename InternalMap::iterator iterator;
    typedef typename InternalMap::const_iterator const_iterator;
    typedef typename InternalMap::value_type value_type;

    const_iterator begin() const { return map.begin(); }
    const_iterator end() const { return map.end();  }
    iterator begin() { return map.begin();  }
    iterator end() { return map.end(); }
    void clear() { map.clear(); }

    // Finds the value associated with the type "Key" in the type map.
    template <class Key>
    iterator find() { return map.find(getTypeId<Key>());  }

    // Same as above, const version
    template <class Key>
    const_iterator find() const { return map.find(getTypeId<Key>()); }

    // Associates a value with the type "Key"
    template <class Key>
    void put(ValueType &&value) {
        map[getTypeId<Key>()] = std::forward<ValueType>(value);
    }

private:
    template <class Key>
    inline static int getTypeId() {
        static const int id = LastTypeId++;
        return id;
    }

    static int LastTypeId;
    InternalMap map;
};

template <class ValueType>
int TypeMap<ValueType>::LastTypeId(0);
