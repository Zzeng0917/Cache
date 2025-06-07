#include <iostream>
#include <unordered_map>
#include <deque>
#include <chrono>

using namespace std;

class Lrucache {
private:
    struct Node {
        int key;
        int value;
        Node *pre, *next;
        Node(int k, int v): key(k), value(v), pre(nullptr), next(nullptr) {}
    };

    int capacity;
    int k_threshold;
    unordered_map<int, Node *> cache;
    Node *head;
    Node *tail;
    unordered_map<int, deque<long long>> history;

    //获取当前时间戳(ms)
    long long getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return chrono::duration_cast<chrono::milliseconds>(duration).count();
    }

    //从双向链表中移除一个节点，用于删除最久未使用的缓存节点
    void removeNode(Node *node) {
        Node *pre = node->pre;
        Node *next = node->next;
        pre->next = next;
        next->pre = pre;
        cache.erase(node->key);
        delete node;
    }

    //将节点移动到双向链表的头部（表示最近使用）
    void moveToHead(Node *node) {
        node->pre->next = node->next;
        node->next->pre = node->pre;
        node->next = head->next;
        node->pre = head;
        head->next->pre = node;
        head->next = node;
    }

    //将一个新节点添加到双向链表头部
    void addToHead(Node *node) {
        node->next = head->next;
        node->pre = head;
        head->next->pre = node;
        head->next = node;
        cache[node->key] = node;
    }

    //获取value 
    int getValueFromDataSource(int key) {
        return key;
    }

public:
    //初始化函数
    Lrucache(int capacity, int k) : capacity(capacity), k_threshold(k) {
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->pre = head;
    }

    //对数据块的访问操作
    int get(int key) {
        if (cache.count(key)) {
            Node *node = cache[key];
            moveToHead(node);
            history[key].push_back(getCurrentTimestamp());//记录访问
            return node->value;
        } else {
            history[key].push_back(getCurrentTimestamp());//记录未命中访问

            if (history[key].size() >= k_threshold) {
                int value = getValueFromDataSource(key);
                put(key, value);

                return value;
            }
            return -1;//缓存未命中未达到k次访问 
        }
    }

    //将缓冲块放置到缓冲区
    void put(int key, int value) {
        if (cache.count(key)) {
            Node *node = cache[key];
            node->value = value;
            moveToHead(node);
        }else {
            if (cache.size() >= capacity) {
                Node *evictNode = tail->pre;
                removeNode(evictNode);
            }
            Node *newNode = new Node(key, value);
            addToHead(newNode);
            history.erase(key);
        }
    }

    unordered_map<int, deque<long long>>& getHistory() {
        return history;
    }
};

int main() {
    Lrucache cache(2, 2); // 容量为 2，K 值为 2
 
    cout << cache.get(1) << endl; // 历史记录：{1: [ts1]}，返回 -1
    cout << cache.get(1) << endl; // 历史记录：{1: [ts1, ts2]}，满足 K=2，从底层获取 value (假设为 1)，调用 put(1, 1)，返回 1
    cout << cache.get(2) << endl; // 历史记录：{2: [ts3]}，返回 -1
    cout << cache.get(2) << endl; // 历史记录：{2: [ts3, ts4]}，满足 K=2，从底层获取 value (假设为 2)，调用 put(2, 2)，返回 2
    cout << cache.get(1) << endl; // 缓存命中，返回 1
    cache.put(3, 3); // 手动调用 put，会直接加入缓存（按照当前逻辑）
    cout << cache.get(3) << endl; // 缓存命中，返回 3
    cout << cache.get(2) << endl; // 缓存命中，返回 2
    cout << cache.get(1) << endl; // 缓存命中，返回 1
 
    std::unordered_map<int, std::deque<long long>>& historyMap = cache.getHistory();
    for (const auto& pair : historyMap) {
        std::cout << "Key " << pair.first << " history: ";
        for (long long timestamp : pair.second) {
            std::cout << timestamp << " ";
        }
        std::cout << std::endl;
    }
 
    return 0;
}
