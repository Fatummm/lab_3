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
        for (auto& elem: buffer) {
            if (elem == key) {
                elem.GetValue() = value;
                ++elem;
                return;
            }
        }

        // if the buffer is not full yet
        if (sz_ < cap_) {
            buffer.push_back(CacheItem{{key, value}});
            ++sz_;
            return;
        }

        // the buffer is full, the key is not in the buffer -> Clock-sweeping
        int i = 0;
        while (true) {
            if (buffer[i].IsZero()) {
                buffer[i] = CacheItem{{key, value}};
                return;
            }
            --buffer[i];
            i = (i + 1) % sz_;
        }
    }
    
    Value get(const Key& key) override {
        for (auto& elem: buffer) {
            if (elem == key) {
                ++elem;
                return elem.GetValue();
            }
        }
        // if there is no key in the buffer
        throw std::out_of_range("There is no such key!");
    }
    
    bool contains(const Key& key) const override {
        for (auto& elem: buffer) {
            if (elem == key) {
                return true;
            }
        }
        // There is no such key
        return false;
    }
    
    void clear() override {
        buffer.clear();
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
        CacheItem(const std::pair<Key, Value>& p): key(p.first), value(p.second), usage_count(1) {}

        bool operator == (const Key& key) const  {
            return this->key == key;
        }

        CacheItem& operator --() {
            --usage_count;
            return *this;
        }

        CacheItem& operator ++() {
            if (usage_count < 5) ++usage_count;
            return *this;
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
};
