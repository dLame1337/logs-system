#pragma once
namespace nstd {
    template<class K = int, class V = int>
    struct pair {
        K key_;
        V value_;

        pair(const K& key, const V& value)
        {
            key_ = key;
            value_ = value;
        }
        pair()
        {
            key_ = K();
            value_ = V();
        }
    };
}