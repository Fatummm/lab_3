#pragma once
#include "ICache.h"
#include <vector>
#include <list>
#include <algorithm>

template<typename Key, typename Value>
class SharedCache : public ICache<Key, Value> {
public:

    SharedCache(size_t buffer_size = 5): sz_(0), cap_(buffer_size) {}
    
    void set(const Key& key, const Value& value) override {
        
        // if the key is already in the buffer
        if (hs.find(key) != hs.end()) {
            size_t index = hs[key];
            buffer[index].GetValue() = value;
            ++buffer[index];
            return;
        }

        // if the buffer is not full yet
        if (sz_ < cap_) {
            buffer.push_back(CacheItem{key, value});
            hs[key] = sz_;
            ++sz_;
            return;
        }

        // the buffer is full, the key is not in the buffer -> Clock-sweeping
        int i = 0;
        while (true) {
            if (buffer[i].IsZero()) {
                hs.erase(buffer[i].GetKey());
                buffer[i] = CacheItem{key, value};
                hs[key] = i;
                return;
            }
            --buffer[i];
            i = (i + 1) % sz_;
        }
    }
    
    Value get(const Key& key) override {
        if (hs.find(key) != hs.end()) {
            return buffer[hs[key]].GetValue();
        }
        // if there is no key in the buffer
        throw std::out_of_range("There is no such key!");
    }
    
    bool contains(const Key& key) const override {
        return hs.find(key) != hs.end();
    }
    
    void clear() override {
        buffer.clear();
        hs.clear();
        sz_ = 0;
    }
    
    size_t size() const override {
        return sz_;
    }

private:
    struct CacheItem {
        Key key;
        Value value;
        size_t usage_count = 1;

        CacheItem() = default;
        CacheItem(const Key& key, const Value& val): key(key), value(val), usage_count(1) {}

        CacheItem& operator --() {
            --usage_count;
            return *this;
        }

        CacheItem& operator ++() {
            if (usage_count < 5) ++usage_count;
            return *this;
        }

        Key& GetKey() {
            return key;
        }

        bool IsZero() {
            return usage_count == 0;
        }
        
        Value& GetValue() {
            return value;
        }
    };

    int sz_ = 0;
    int cap_ = 5;
    std::vector<CacheItem> buffer;
    std::unordered_map<Key, size_t> hs;
};
