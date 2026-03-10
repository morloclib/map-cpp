#ifndef __MORLOC__MAP_HPP__
#define __MORLOC__MAP_HPP__

#include <vector>
#include <map>
#include <functional>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <optional>

// --- Pack/Unpack ---

template <class A, class B>
std::map<A,B> morloc_packMap(std::tuple<std::vector<A>,std::vector<B>> items){
    std::map<A,B> m;
    std::vector<A> a = std::get<0>(items);
    std::vector<B> b = std::get<1>(items);
    assert(a.size() == b.size());
    for(std::size_t i = 0; i < a.size(); i++){
        m[a[i]] = b[i];
    }
    return m;
}

template <class A, class B>
std::tuple<std::vector<A>,std::vector<B>> morloc_unpackMap(std::map<A,B> m){
    std::vector<A> a;
    std::vector<B> b;
    for (auto tuple : m) {
        a.push_back(std::get<0>(tuple));
        b.push_back(std::get<1>(tuple));
    }
    return std::make_tuple(a, b);
}

// --- Construction ---

// emptyMap :: Map a b
template <class K, class V>
std::map<K, V> morloc_emptyMap() {
    return std::map<K, V>();
}

// singleton :: a -> b -> Map a b
template <class K, class V>
std::map<K, V> morloc_singleton(const K& key, const V& val) {
    std::map<K, V> m;
    m[key] = val;
    return m;
}

// fromList :: [(a, b)] -> Map a b
template <typename Key, typename Value>
std::map<Key, Value> morloc_from_list(const std::vector<std::tuple<Key, Value>>& tuples) {
    std::map<Key, Value> result;
    for (const auto& t : tuples) {
        result[std::get<0>(t)] = std::get<1>(t);
    }
    return result;
}

// toList :: Map a b -> [(a, b)]
template <class K, class V>
std::vector<std::tuple<K, V>> morloc_to_list(const std::map<K, V>& m) {
    std::vector<std::tuple<K, V>> result;
    for (const auto& pair : m) {
        result.push_back(std::make_tuple(pair.first, pair.second));
    }
    return result;
}

// --- Query ---

// lookup :: a -> Map a b -> ?b
template <class K, class V>
V morloc_lookup(const K& key, const std::map<K, V>& m) {
    auto it = m.find(key);
    if (it == m.end()) {
        throw std::runtime_error("Key not found in map");
    }
    return it->second;
}

// member :: a -> Map a b -> Bool
template <class K, class V>
bool morloc_member(const K& key, const std::map<K, V>& m) {
    return m.count(key) > 0;
}

// size :: Map a b -> Int
template <class K, class V>
int morloc_size(const std::map<K, V>& m) {
    return static_cast<int>(m.size());
}

// --- Modify ---

// insert :: a -> b -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_insert(const K& key, const V& val, const std::map<K, V>& m) {
    std::map<K, V> result(m);
    result[key] = val;
    return result;
}

// delete :: a -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_delete(const K& key, const std::map<K, V>& m) {
    std::map<K, V> result(m);
    result.erase(key);
    return result;
}

// --- Bulk access ---

// keys :: Map a b -> [a]
template <typename Key, typename Value>
std::vector<Key> morloc_keys(const std::map<Key, Value>& map) {
    std::vector<Key> result;
    for (const auto& pair : map) {
        result.push_back(pair.first);
    }
    return result;
}

// vals :: Map a b -> [b]
template <typename Key, typename Value>
std::vector<Value> morloc_vals(const std::map<Key, Value>& map) {
    std::vector<Value> result;
    for (const auto& pair : map) {
        result.push_back(pair.second);
    }
    return result;
}

// --- Combine ---

// union :: Map a b -> Map a b -> Map a b (right-biased)
template <class K, class V>
std::map<K, V> morloc_union(const std::map<K, V>& a, const std::map<K, V>& b) {
    std::map<K, V> result(a);
    for (const auto& pair : b) {
        result[pair.first] = pair.second;
    }
    return result;
}

// unionWith :: (b -> b -> b) -> Map a b -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_unionWith(std::function<V(V, V)> f, const std::map<K, V>& a, const std::map<K, V>& b) {
    std::map<K, V> result(a);
    for (const auto& pair : b) {
        auto it = result.find(pair.first);
        if (it != result.end()) {
            it->second = f(it->second, pair.second);
        } else {
            result[pair.first] = pair.second;
        }
    }
    return result;
}

// intersectionWith :: (b -> b -> b) -> Map a b -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_intersectionWith(std::function<V(V, V)> f, const std::map<K, V>& a, const std::map<K, V>& b) {
    std::map<K, V> result;
    for (const auto& pair : a) {
        auto it = b.find(pair.first);
        if (it != b.end()) {
            result[pair.first] = f(pair.second, it->second);
        }
    }
    return result;
}

// difference :: Map a b -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_difference(const std::map<K, V>& a, const std::map<K, V>& b) {
    std::map<K, V> result;
    for (const auto& pair : a) {
        if (b.count(pair.first) == 0) {
            result[pair.first] = pair.second;
        }
    }
    return result;
}

// --- Transform ---

// mapValues :: (b -> c) -> Map a b -> Map a c
template <typename Key, typename Value, typename NewValue>
std::map<Key, NewValue> morloc_map_val(std::function<NewValue(const Value&)> transform, const std::map<Key, Value>& map) {
    std::map<Key, NewValue> result;
    for (const auto& pair : map) {
        result[pair.first] = transform(pair.second);
    }
    return result;
}

// mapKeys :: (a -> c) -> Map a b -> Map c b
template <typename Key, typename Value, typename NewKey>
std::map<NewKey, Value> morloc_map_key(std::function<NewKey(const Key&)> transform, const std::map<Key, Value>& map) {
    std::map<NewKey, Value> result;
    for (const auto& pair : map) {
        result[transform(pair.first)] = pair.second;
    }
    return result;
}

// mapWithKey :: (a -> b -> c) -> Map a b -> Map a c
template <class K, class V, class W>
std::map<K, W> morloc_mapWithKey(std::function<W(K, V)> f, const std::map<K, V>& m) {
    std::map<K, W> result;
    for (const auto& pair : m) {
        result[pair.first] = f(pair.first, pair.second);
    }
    return result;
}

// filterMap :: (a -> b -> Bool) -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_filter_map(std::function<bool(K, V)> f, const std::map<K, V>& m) {
    std::map<K, V> result;
    for (const auto& pair : m) {
        if (f(pair.first, pair.second)) {
            result[pair.first] = pair.second;
        }
    }
    return result;
}

// foldWithKey :: (c -> a -> b -> c) -> c -> Map a b -> c
template <class K, class V, class C>
C morloc_foldWithKey(std::function<C(C, K, V)> f, C init, const std::map<K, V>& m) {
    C acc = init;
    for (const auto& pair : m) {
        acc = f(acc, pair.first, pair.second);
    }
    return acc;
}

#endif
