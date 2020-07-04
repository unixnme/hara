#ifndef HARA_PREFIXTREE_H
#define HARA_PREFIXTREE_H

#include <vector>
#include <map>
#include <queue>
#include "Macros.h"

namespace hara {

template<typename Key, typename Value>
class PrefixNode;

template<typename Key, typename Value>
class PrefixTree;

/**
 * wrapper around node pointer
 */
template<typename Key, typename Value>
class PrefixLeaf {
public:
    // no copies allowed
    PrefixLeaf(const PrefixLeaf &) = delete;

    PrefixLeaf &operator=(const PrefixLeaf &) = delete;

    // move constructor allowed
    // any subsequent method call will result in segfault
    PrefixLeaf(PrefixLeaf &&that) noexcept: node{that.node} {
        that.node = nullptr;
    }

    // move operator not allowed
    PrefixLeaf &operator=(PrefixLeaf &&) noexcept = default;

    std::vector<Key> Prefix() const { return node->Prefix(); }

    const Value &Data() const { return node->Data(); }

    Value &Data() { return node->Data(); }

    size_t Size() const { return node->Size(); }

private:
    explicit PrefixLeaf(PrefixNode<Key, Value> &node) : node{&node} {}

    PrefixNode<Key, Value> *node;

    friend class PrefixTree<Key, Value>;

    friend class PrefixNode<Key, Value>;
};

/**
 * Actual implementation of PrefixTree
 * @tparam Key
 * @tparam Value
 */
template<typename Key, typename Value>
class PrefixNode {
public:
    ~PrefixNode() {
        delete data;
    }

    std::vector<Key> Prefix() const {
        std::vector<Key> prefix;
        auto node = this;
        while (!node->IsRoot()) {
            prefix.push_back(node->key);
            node = node->parent;
        }
        std::reverse(prefix.begin(), prefix.end());
        return prefix;
    }

    /**
     * Return all leafs
     */
    std::vector<PrefixLeaf<Key, Value>> FindAll(const std::vector<Key> &keys) {
        if (Empty()) return {};
        auto node = Find(keys);
        if (!node) return {};

        std::vector<PrefixLeaf<Key, Value>> leafs;
        std::queue<PrefixNode *> queue;
        queue.push(node);
        while (!queue.empty()) {
            node = queue.front();
            queue.pop();

            if (node->data) leafs.push_back(std::move(PrefixLeaf<Key, Value>{*node}));
            for (auto &pair : node->children)
                if (!pair.second.Empty()) queue.push(&pair.second);
        }

        return leafs;
    }

    bool Insert(const std::vector<Key> &keys, Value value) {
        auto node = Find(keys, true);
        if (node->data) return false;
        node->data = new Value{std::move(value)};
        while (node) {
            ++node->num_leafs;
            node = node->parent;
        }
        return true;
    }

    /**
     * Remove the current leaf
     */
    void Erase() {
        ASSERT(data != nullptr, "Not a leaf");
        delete data;
        data = nullptr;
        auto node = this;
        while (node) {
            --node->num_leafs;
            node = node->parent;
        }
    }

    const Value &Data() const { return *data; }

    Value &Data() { return *data; }

    size_t Size() const { return num_leafs; }

    bool Empty() const { return Size() == 0; }

private:
    // all constructors not allowed by client
    PrefixNode() : parent{nullptr}, key{Key{}}, data{nullptr}, num_leafs{0} {}

    explicit PrefixNode(PrefixNode *const parent, Key key)
            : parent{parent}, key{std::move(key)}, data{nullptr}, num_leafs{0} {}

    explicit PrefixNode(PrefixNode *const parent, Key key, Value value)
            : parent{parent}, key{std::move(key)}, data{new Value{std::move(value)}}, num_leafs{1} {}

    bool IsRoot() const { return parent == nullptr; }

    /**
     * Find the node relative to this by keys
     * If create, construct node as you go
     */
    PrefixNode *Find(const std::vector<Key> &keys, bool create = false) {
        auto node = this;
        for (auto &k : keys) {
            auto it = node->children.find(k);
            if (it == node->children.end()) {
                if (create) it = node->children.emplace(k, PrefixNode{node, k}).first;
                else return nullptr;
            }
            node = &it->second;
        }
        return node;
    }

    PrefixNode *const parent;
    const Key key;
    std::map<Key, PrefixNode> children;
    Value *data;
    size_t num_leafs;

    friend class PrefixTree<Key, Value>;
};


/**
 * Wrapper around a PrefixNode as a root
 * @tparam Key
 * @tparam Value
 */
template<typename Key, typename Value>
class PrefixTree {
public:
    /**
     * Return all leafs
     */
    std::vector<PrefixLeaf<Key, Value>> FindAll(const std::vector<Key> &keys = {}) {
        return root.FindAll(keys);
    }

    /**
     * Insert given value at the given keys
     */
    bool Insert(const std::vector<Key> &keys, Value value) {
        return root.Insert(keys, std::move(value));
    }

    /**
     * Insert given value at the given keys relative to the leaf
     * @return
     */
    bool Insert(PrefixLeaf<Key, Value> &leaf, const std::vector<Key> &keys, Value value) {
        return leaf.node->Insert(keys, std::move(value));
    }

    /**
     * Clear all leafs
     */
    void Clear() {
        root.children.clear();
        root.num_leafs = 0;
    }

    /**
     * Erase leaf & invalidate it
     * any further usage of leaf will result in seg fault
     */
    void Erase(PrefixLeaf<Key, Value> &leaf) {
        leaf.node->Erase();
        leaf.node = nullptr;
    }

    /**
     * number of leafs in the tree
     */
    size_t Size() const { return root.Size(); }

    bool Empty() const { return root.Empty(); }

private:
    PrefixNode<Key, Value> root;
};

}

#endif //HARA_PREFIXTREE_H
