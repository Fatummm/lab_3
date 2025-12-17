#pragma once
#include <vector>
#include <list>

template <typename Key_t, typename Value_t>
class Block {
public:
    Block(size_t sz): arr(sz), sz(sz) {}

    void add(const std::pair<Key_t, Value_t>& pr) {
        size_t pos = hs(pr.first) % sz;
        bool inserted = false;
        for (auto& elem: arr[pos]) {
            if (elem.first == pr.first) {
                elem.second.first += pr.second;
                ++elem.second.second;
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            arr[pos].push_front({pr.first, {pr.second, 1}});
        }
    }
    
    void merge(std::vector<std::pair<Key_t, Value_t>>& v) {
        for (size_t i = 0; i != arr.size(); ++i) {
            for (auto& p: arr[i]) {
                v.push_back({p.first, p.second.first / p.second.second});
            }
        }
    }

private:

    std::hash<Key_t> hs;
    // Each list in block array contains a pair of <key, <summ, number_of_elements>>
    std::vector<std::list<std::pair<Key_t, std::pair<Value_t, size_t>>>> arr; 
    size_t sz;
};

template <typename Key_t, typename Value_t>
void merge(std::vector<std::pair<Key_t, Value_t>>& v, Block<Key_t, Value_t>& block) {
    for (size_t i = 0; i != block.arr.size(); ++i) {
        for (auto& p: block.arr[i]) {
            v.push_back({p.first, p.second.first / p.second.second});
        }
    }
}

// Calculate average value for each key.
template<typename Key_t, typename Value_t>
std::vector<std::pair<Key_t, Value_t>> AvgWithPartitioning(const std::vector<std::pair<Key_t, Value_t>>& kv_array
    , std::size_t block_size = 8)
{
    std::vector<std::pair<Key_t, Value_t>> result;
    if (kv_array.size() == 0) return result;
    std::hash<Key_t> partition_hash;

    size_t block_count = (kv_array.size() / block_size) + (kv_array.size() % block_size != 0);

    std::vector<Block<Key_t, Value_t>> blocks(block_count, block_size);

    for (size_t i = 0; i != kv_array.size(); ++i) {
        size_t block_index = partition_hash(kv_array[i].first) % block_count;
        blocks[block_index].add(kv_array[i]);
    }
    for (size_t i = 0; i != block_count; ++i) {
        blocks[i].merge(result);
    }
    return result;
}
