#pragma once
#include "ICache.h"
#include <cstddef>
#include <unordered_map>
#include <list>
#include <stdexcept>

template<typename Key, typename Value>
class LFUCache : public ICache<Key, Value> {
public:
    struct Node{
        Value value;
        size_t freq;
        typename std::list<Key>::iterator iter;
    };

    std::unordered_map<Key, Node> cache;
    std::unordered_map<size_t, std::list<Key> >freqmp;

    size_t capacity;
    size_t minfreq;

    void update_freq(const Key& key){
        auto &node = cache[key];
        size_t freq = node.freq;
        size_t newfreq = freq + 1;

        freqmp[freq].erase(node.iter);

        if (freqmp[freq].empty()){
            freqmp.erase(freq);
            if (minfreq == freq){
                minfreq++;
            }
        }

        node.freq = newfreq;
        freqmp[newfreq].push_front(key);
        node.iter = freqmp[newfreq].begin();
    }

    LFUCache(size_t capacity) : capacity(capacity), minfreq(0) {        
    }
    
    void set(const Key& key, const Value& value) override {
        if (capacity == 0)return;

        if (cache.find(key) != cache.end()){
            cache[key].value = value;
            update_freq(key);
            return;
        }

        if (cache.size() >= capacity){
            auto& mnfreq_list = freqmp[minfreq];
            Key del_key = mnfreq_list.back();
            mnfreq_list.pop_back();
            if (mnfreq_list.empty()){
                freqmp.erase(minfreq);
            }

            cache.erase(del_key);

        }
        minfreq = 1;
        freqmp[1].push_front(key);
        cache[key] = Node{value, 1, freqmp[1].begin()};
    }
    
    Value get(const Key& key) override {
        auto it = cache.find(key);
        if (it == cache.end()){
            throw std::out_of_range("Key not found");
        }
        update_freq(key);
        
        
        return it->second.value;
    }
    
    bool contains(const Key& key) const override {
        return cache.find(key) != cache.end();
    }
    
    void clear() override {
        cache.clear();
        freqmp.clear();
        minfreq = 0;
        return;
    }
    
    size_t size() const override {
        return cache.size();
    }
};
