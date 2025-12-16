#pragma once
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <functional>




// Caclculate average value for each key.
template<typename Key_t, typename Value_t>
std::vector<std::pair<Key_t, Value_t>> AvgWithPartitioning(const std::vector<std::pair<Key_t, Value_t>>& kv_array
    , std::size_t block_size = 8)
{
    
    if (kv_array.empty())return {};

    size_t parts_num = (kv_array.size() + block_size - 1) / block_size;
    if (parts_num == 0)parts_num = 1;

    std::vector<std::vector<std::pair<Key_t, Value_t>>> partitions(parts_num);
    std::hash<Key_t> hasher;
    std::hash<size_t> size_hasher;
    std::vector<std::pair<Key_t, Value_t>> result;

    for (int i = 0;i < kv_array.size();i++){
        size_t hash1 = hasher(kv_array[i].first);
        size_t hash2 = size_hasher(hash1);
        
        size_t idx = hash2 % parts_num;
        partitions[idx].push_back(kv_array[i]);
    }

    for (const auto& part : partitions){
        std::unordered_map<Key_t, std::pair<Value_t, std::size_t>> mp;
        
        for (const auto& kv : part){
            mp[kv.first].first += kv.second;
            mp[kv.first].second++;
        }

        for (const auto& [key, val_cnt] : mp){
            result.push_back({key, val_cnt.first / static_cast<Value_t>(val_cnt.second)});
        }
    }


    return result;
}
