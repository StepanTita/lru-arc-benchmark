#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>

typedef long long ll;

ll ipToKey(std::string &ip) {
    std::vector<int> octets;
    std::istringstream iss(ip);
    std::string octet;

    while (std::getline(iss, octet, '.')) {
        octets.push_back(std::stoi(octet));
    }

    ll key = 0;
    for (int i = 0; i < 4; ++i) {
        key = (key << 8) + octets[i];
    }

    return key;
}

struct DoublyListNode {
    ll key;
    DoublyListNode *next;
    DoublyListNode *prev;

    DoublyListNode() : key(-1), next(NULL), prev(NULL) {}

    DoublyListNode(ll x) : key(x), next(NULL), prev(NULL) {}
};

template<typename V>
class LRUCache {
    std::unordered_map<ll, V> container;

    std::unordered_map<ll, DoublyListNode *> pkey;

    DoublyListNode *head = NULL;
    DoublyListNode *tail = NULL;

    int cap = 0;

    void promote(ll key) {
        auto node = pkey[key];

        // this covers the key when node is head,
        // and node is both head and tail
        if (node == head) return;

        auto prev = node->prev;
        auto next = node->next;

        node->prev = NULL;

        node->next = head;
        if (head != NULL) {
            head->prev = node;
        }

        if (next == NULL) {
            tail = prev;
        }

        if (prev != NULL) {
            prev->next = next;
        }
        if (next != NULL) {
            next->prev = prev;
        }
        head = node;
    }

public:
    LRUCache(int capacity = 0) {
        cap = capacity;
    }

    std::pair<ll, V> evict() {
        ll key = tail->key;
        auto removed = container[tail->key];

        pkey.erase(tail->key);
        container.erase(tail->key);

        // only possible when size is 1
        if (tail == head) {
            auto tmp = tail;
            head = NULL;
            tail = NULL;
            delete tmp;
            return {key, removed};
        }

        auto prev = tail->prev;
        if (prev != NULL) {
            prev->next = NULL;
        }

        // free current tail before erasing its last pointer
        delete tail;
        tail = prev;

        return {key, removed};
    }

    V get(ll key) {
        if (this->container.contains(key)) {
            promote(key);
            return this->container[key];
        };
        return V();
    }

    // returns evicted value and true if was evicted, otherwise empty and false
    std::pair<std::pair<ll, V>, bool> put(ll key, V value) {
        if (head == NULL) {
            head = new DoublyListNode(key);
            tail = head;
            pkey[key] = head;
            container[key] = value;
        } else if (this->container.contains(key)) {
            promote(key);
            container[key] = value;
        } else if (container.size() < cap) {
            auto node = new DoublyListNode(key);
            node->next = head;
            head->prev = node;
            head = node;
            pkey[key] = node;
            container[key] = value;
        } else {
            auto res = evict();

            put(key, value);

            return {res, true};
        }
        return {std::pair<ll, V>(), false};
    }

    void remove(ll key) {
        if (container.find(key) != container.end()) {
            auto node = pkey[key];
            if (node == head) {
                head = node->next;
            }

            if (node == tail) {
                tail = node->prev;
            }

            if (node->prev != NULL) {
                node->prev->next = node->next;
            }

            if (node->next != NULL) {
                node->next->prev = node->prev;
            }

            delete node;
            container.erase(key);
            pkey.erase(key);
        }
    }

    int size() {
        return container.size();
    }

    void resize(int capacity) {
        cap = capacity;
    }
};

template <typename V>
class PartitionedLRU {
public:
    LRUCache<V> T;
    LRUCache<V> B;

    PartitionedLRU(int capacity = 0) {
        T = LRUCache<V>(capacity);
        B = LRUCache<V>(capacity);
    }

    // return true if returned from Top part and false otherwise
    std::pair<V, bool> get(ll key) {
        V empty = V();
        V val = T.get(key);

        if (val != empty) {
            return {val, true};
        }

        return {B.get(key), false};
    }

    // true if T overflowed
    bool put(ll key, V value) {
        auto [elem, evicted] = T.put(key, value);
        if (evicted) {
            auto [oldKey, oldVal] = elem;
            B.put(oldKey, oldVal);
        }
        return evicted;
    }

    void resize(int capacity) {
        T.resize(capacity);
        while (T.size() > capacity) {
            auto elem = T.evict();
            B.put(elem.first, elem.second);
        }
    }
};

template<typename V>
class ARCache {
private:
    PartitionedLRU<V> L1;
    PartitionedLRU<V> L2;

    int p = 0;
    int cap = 0;

public:
    ARCache(int capacity) : p(capacity / 2), cap(capacity / 2), L1(capacity / 2), L2(capacity / 2) {}

    V get(ll key) {
        V empty = V();

        auto [val, inTop] = L1.get(key);
        if (inTop) {
            // case I - part 1 - T1 cache hit
            L1.T.remove(key);
            L2.put(key, val);
            return val;
        } else if (val != empty) {
            // case II - B1 cache miss
            double B1 = L1.B.size();
            double B2 = L2.B.size();

            int delta = std::max(1, int(B2 / B1));

            // resize p
            p = std::min(p + delta, cap);
            L1.T.resize(p); // this increases
            L2.T.resize(cap - p); // this decreases

            // remove value from B1 cache
            L1.B.remove(key);
            // move value to L2 cache
            L2.put(key, val);

            return val;
        }

        // we didn't see the miss in L1, maybe it is in L2
        auto l2 = L2.get(key);
        val = l2.first, inTop = l2.second;

        if (inTop) {
            // case I - part 2 - T2 cache hit
            return val;
        } else if (val != empty) {
            // case III - B2 cache miss

            double B1 = L1.B.size();
            double B2 = L2.B.size();

            int delta = std::max(1, int(B1 / B2));

            // resize p
            p = std::min(p - delta, cap);
            L1.T.resize(p); // this decreases
            L2.T.resize(cap - p); // this increases

            // remove value from B2 cache
            L2.B.remove(key);
            // move value to L2 cache
            L2.put(key, val);

            return val;
        }

        // complete cache miss
        return empty;
    }

    void put(ll key, V value) {
        L1.put(key, value);
    }
};

void testLRU(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }

    auto lru = LRUCache<std::string>(100);

    std::string line;

    int hit = 0;
    int miss = 0;
    while (std::getline(file, line)) {
        long long key = ipToKey(line);
        auto val = lru.get(key);
        lru.put(key, line);

        if (val != line) {
            ++miss;
        } else {
            ++hit;
        }
    }

    file.close();

    std::cout << "Total examples: " << hit + miss << std::endl;
    std::cout << "Hit: " << hit << std::endl;
    std::cout << "Miss: " << miss << std::endl;

    std::cout << "Hit rate: " << double(hit) / (double(hit) + double(miss)) << std::endl;
}

void testARC(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }

    auto arc = ARCache<std::string>(100);

    std::string line;

    int hit = 0;
    int miss = 0;
    while (std::getline(file, line)) {
        long long key = ipToKey(line);
        auto val = arc.get(key);
        arc.put(key, line);

        if (val != line) {
            ++miss;
        } else {
            ++hit;
        }
    }

    file.close();

    std::cout << "Total examples: " << hit + miss << std::endl;
    std::cout << "Hit: " << hit << std::endl;
    std::cout << "Miss: " << miss << std::endl;

    std::cout << "Hit rate: " << double(hit) / (double(hit) + double(miss)) << std::endl;
}

int main() {
    std::string testFile = "data/ips_uniform.log";

    std::cout << "Running LRU test..." << std::endl;
    testLRU(testFile);

    std::cout << "================" << std::endl;

    std::cout << "Running ARC test..." << std::endl;
    testARC(testFile);

    return 0;
}
