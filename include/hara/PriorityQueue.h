#ifndef HARA_PRIORITY_QUEUE_H
#define HARA_PRIORITY_QUEUE_H

#include <vector>
#include <queue>
#include <set>
#include <map>
#include "Macros.h"

namespace hara {

template<typename Impl>
class PriorityQueue {
public:
    using K = typename Impl::Key;
    using V = typename Impl::Value;

    PriorityQueue() : impl{new Impl} {};

    template<typename Iterator>
    PriorityQueue(Iterator begin, Iterator end) : impl{new Impl{begin, end}} {}

    ~PriorityQueue() { delete impl; };

    const std::pair<K, V> &Top() const { return impl->Top(); }

    void Pop() { impl->Pop(); }

    bool Empty() const { return impl->Empty(); }

    size_t Size() const { return impl->Size(); }

    void InsertOrUpdate(std::pair<K, V> pair) { impl->InsertOrUpdate(std::move(pair)); }

    void Erase(const K &key) { impl->Erase(key); }

    bool Contain(const K &key) const { return impl->Contain(key); }

    std::vector<K> Keys() const { return impl->Keys(); }

    /**
     * throws exception if key not found
     * @param key
     * @return
     */
    const V &Peek(const K &key) const { return impl->Peek(key); }

private:
    Impl *const impl;
};

template<typename K, typename V, typename Compare = std::less<V>>
class PriorityQueueImpl {
public:
    using Key = K;
    using Value = V;

    virtual ~PriorityQueueImpl() = default;

    virtual const std::pair<K, V> &Top() const = 0;

    virtual void Pop() = 0;

    virtual bool Empty() const = 0;

    virtual size_t Size() const = 0;

    virtual void InsertOrUpdate(std::pair<K, V> pair) = 0;

    virtual void Erase(const K &key) = 0;

    virtual bool Contain(const K &key) const = 0;

    virtual const V &Peek(const K &key) const = 0;

    virtual std::vector<Key> Keys() const = 0;

protected:
    static inline bool less(const V &a, const V &b) { return Compare()(a, b); }

    static inline bool greater(const V &a, const V &b) { return less(b, a); }

    static inline bool equal(const V &a, const V &b) { return !(less(a, b) || greater(a, b)); }

    static inline bool notequal(const V &a, const V &b) { return !equal(a, b); }

    struct Pair {
        explicit Pair(std::pair<K, V> x) : x{std::move(x)} {}

        std::pair<K, V> x;

        bool operator<(const Pair &that) const {
            return less(x.second, that.x.second) ||
                   (equal(x.second, that.x.second) && x.first < that.x.first);
        }

        bool operator==(const Pair &that) const {
            return equal(x.second, that.x.second) && x.first == that.x.first;
        }

        bool operator!=(const Pair &that) const {
            return !(*this == that);
        }

        bool operator>(const Pair &that) const {
            return that < *this;
        }
    };
};

/**
 * The pqueue should always be in a state where the top element is valid
 * @tparam K
 * @tparam V
 */
template<typename K, typename V, typename Compare = std::less<V>>
class PriorityQueueImpl1 : public PriorityQueueImpl<K, V, Compare> {
public:
    PriorityQueueImpl1() = default;

    template<typename Iterator>
    explicit PriorityQueueImpl1(Iterator begin, Iterator end)
            : queue{begin, end} {
        for (auto it = begin; it != end; ++it) {
            valid.emplace(*it);
        }
    }

    ~PriorityQueueImpl1() override = default;

    /**
     * Complexity: O(1)
     */
    const std::pair<K, V> &Top() const override {
        ASSERT (!Empty(), "Queue is empty");
        return queue.top().x;
    }

    /**
     * Complexity: Amortized O(1)
     */
    void Pop() override {
        if (Empty()) return;
        valid.erase(queue.top().x.first);
        queue.pop();
        PopTillValid();
    }

    bool Empty() const override { return valid.empty(); }

    size_t Size() const override { return valid.size(); }

    /**
     * Complexity: O(lg(N))
     */
    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = valid.find(pair.first);
        if (it == valid.end())
            valid.emplace(pair);
        else
            it->second = pair.second;
        queue.emplace(std::move(pair));
        PopTillValid();
    }

    /**
     * Complexity: O(lg(N))
     */
    void Erase(const K &key) override {
        auto it = valid.find(key);
        if (it == valid.end()) return;

        valid.erase(it);
        PopTillValid();
    }

    /**
     * Complexity: O(lg(N))
     */
    bool Contain(const K &key) const override {
        return valid.find(key) != valid.end();
    }

    /**
    * Complexity: O(N)
    */
    std::vector<K> Keys() const override {
        std::vector<K> keys;
        for (const auto &pair : valid) keys.push_back(pair.first);
        return keys;
    }

    /**
     * Complexity: O(lg(N))
     */
    const V &Peek(const K &key) const override { return valid.at(key); }

private:
    /**
     * Complexity: Amortized O(1)
     */
    void PopTillValid() {
        while (!queue.empty()) {
            auto it = valid.find(queue.top().x.first);
            if (it == valid.end()
                || PriorityQueueImpl<K, V, Compare>::notequal(it->second, queue.top().x.second))
                // this is a spurious element
                queue.pop();
            else break;
        }
    }

    using Pair = typename PriorityQueueImpl<K, V, Compare>::Pair;
    std::priority_queue<Pair> queue;
    std::map<K, V> valid;
};

template<typename K, typename V, typename Compare = std::less<V>>
class PriorityQueueImpl2 : public PriorityQueueImpl<K, V, Compare> {
public:
    PriorityQueueImpl2() = default;

    template<typename Iterator>
    explicit PriorityQueueImpl2(Iterator begin, Iterator end)
            : set{begin, end} {
        for (auto it = begin; it != end; ++it)
            valid.insert(*it);
    }

    ~PriorityQueueImpl2() override = default;

    /**
     * Complexity: O(1)
     */
    const std::pair<K, V> &Top() const override {
        ASSERT (!Empty(), "Queue is empty");
        return set.begin()->x;
    }

    /**
     * Complexity: O(lg(N))
     */
    void Pop() override {
        if (Empty()) return;
        valid.erase(set.begin()->x.first);
        set.erase(set.begin());
    }

    bool Empty() const override { return set.empty(); }

    size_t Size() const override { return set.size(); }

    /**
     * Complexity: O(lg(N))
     */
    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = valid.find(pair.first);
        if (it == valid.end()) {
            valid.insert(pair);
            set.emplace(std::move(pair));
        } else {
            set.erase(Pair{*it});
            it->second = pair.second;
            set.emplace(std::move(pair));
        }
    }

    /**
     * Complexity: O(lg(N))
     */
    void Erase(const K &key) override {
        auto it = valid.find(key);
        if (it == valid.end()) return;

        set.erase(Pair{*it});
        valid.erase(it);
    }

    /**
     * Complexity: O(lg(N))
     */
    bool Contain(const K &key) const override {
        return valid.find(key) != valid.end();
    }

    /**
    * Complexity: O(N)
    */
    std::vector<K> Keys() const override {
        std::vector<K> keys;
        for (const auto &pair : valid) keys.push_back(pair.first);
        return keys;
    }

    /**
     * Complexity: O(lg(N))
     */
    const V &Peek(const K &key) const override { return valid.at(key); }

private:
    using Pair = typename PriorityQueueImpl<K, V, Compare>::Pair;
    std::set<Pair, std::greater<Pair>> set;
    std::map<K, V> valid;
};

}

#endif //HARA_PRIORITY_QUEUE_H
