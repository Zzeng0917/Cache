#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

class LruCache {
private:
    int capacity;//缓存容量
    list<int> keys;//双向链表存储
    unordered_map<int, pair<int, list<int>::iterator>> cache;//存储键值对

public:
    //构造函数，初始化缓存容量
    LruCache(int capacity) : capacity(capacity) {}

    //获取缓存中键对应的值,查询值变为最近常用的搜索，移到头部
    int get(int key) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            return -1; //如果键值不存在，返回-1 
        }
        //更新访问顺序，将该键移动到链表头部
        keys.erase(it->second.second);
        keys.push_front(key);
        it->second.second = keys.begin();
        return it->second.first; // 返回键对应的值 
    }

    //插入或者更新缓存中的键值对
    void put(int key, int value) {
        if (cache.size() >= capacity && cache.find(key) == cache.end()) {
            //如果缓存满并且键不存在，淘汰尾部的键
            auto last = keys.back();
            cache.erase(cache.find(last));
            keys.pop_back();
        }
        cache[key] = {value, keys.insert(keys.begin(), key)};
    }

};

int main() {
    LruCache cache(2);
    //创建一个容量值为2的缓存

    cache.put(1, 1);
    cout << "get(1) = " << cache.get(1) << endl; 

    cache.put(2, 2);
    cout << "get(2) = " << cache.get(2) << endl; 

    cache.put(3, 3);
    cout << "get(1) = " << cache.get(1) << endl; 
    cout << "get(3) = " << cache.get(3) << endl;

    cache.put(4, 4);
    cout << "get(1) = " << cache.get(1) << endl;  
    cout << "get(3) = " << cache.get(3) << endl; 
    cout << "get(2) = " << cache.get(2) << endl;
    cout << "get(4) = " << cache.get(4) << endl;
}
