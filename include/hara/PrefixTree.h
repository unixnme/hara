#ifndef HARA_PREFIX_TREE_H
#define HARA_PREFIX_TREE_H

#include <vector>
#include "Macros.h"

namespace hara {

template<typename Key, typename Object>
class PrefixTree;

template<typename Key, typename Object>
class PrefixLeaf;

template<typename Key, typename Object>
class PrefixNode {
private:
    explicit PrefixNode(PrefixNode *parent, const Key &key)
            : parent_{parent}, key_{key}, data_{nullptr}, num_leafs_{0} {
    }

    ~PrefixNode() {
        delete data_;
        for (auto child : children_)
            delete child;
    }

    size_t Size() const { return num_leafs_; }

    bool Empty() const { return Size() == 0; }

    PrefixNode *End() const {
        return nullptr;
    }

    PrefixNode *Insert(const std::vector<Key> &prefix, const Object &obj) {
        auto node = this;
        for (auto key : prefix) {
            auto child = node->Find(key);
            if (child == nullptr) {
                child = new PrefixNode{node, key};
                node->children_.emplace_back(child);
            }
            node = child;
        }
        if (node->data_ == nullptr) {
            // insert a new element
            node->data_ = new Object{obj};
            node->Increment();
        } else {
            // replace the existing element
            *node->data_ = obj;
        }
        return node;
    }

    void Remove() {
        if (data_) Decrement();
        delete data_;
        data_ = nullptr;
    }

    void Increment() {
        auto ptr = this;
        while (ptr != nullptr) {
            ++ptr->num_leafs_;
            ptr = ptr->parent_;
        }
    }

    void Decrement() {
        auto ptr = this;
        while (ptr != nullptr) {
            --ptr->num_leafs_;
            ptr = ptr->parent_;
        }
    }

    PrefixNode *Find(const std::vector<Key> &prefix) {
        PrefixNode *child = this;
        for (auto key: prefix) {
            child = child->Find(key);
            if (child == nullptr)
                break;
        }
        return child;
    }

    PrefixNode *Find(const Key &key) const {
        for (auto child : children_) {
            if (child->key_ == key) return child;
        }
        return nullptr;
    }

    std::vector<Key> Prefix() const {
        std::vector<Key> result;
        const PrefixNode *node = this;
        while (node->parent_ != nullptr) {
            result.emplace_back(node->key_);
            node = node->parent_;
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    void AppendLeafs(std::vector<PrefixNode *> &leafs) {
        auto count = this->num_leafs_;
        if (data_ != nullptr) {
            leafs.emplace_back(this);
            --count;
        }
        if (count == 0) return;
        for (auto child : children_) {
            child->appendLeafs(leafs);
        }
    }

    Key key_;
    Object *data_;
    size_t num_leafs_;
    PrefixNode *parent_;
    std::vector<PrefixNode *> children_;

    friend class PrefixLeaf<Key, Object>;

    friend class PrefixTree<Key, Object>;
};

template<typename Key, typename Object>
class PrefixLeaf {
public:
    PrefixLeaf Insert(const std::vector<Key> &prefix, const Object &obj) {
        Check();
        return PrefixLeaf{addr_->Insert(prefix, obj)};
    }

    PrefixLeaf Insert(const Key &key, const Object &obj) {
        return Insert(std::vector<Key>{key}, obj);
    }

    PrefixLeaf Find(const std::vector<Key> &prefix) {
        Check();
        auto child = addr_->Find(prefix);
        if (child && child->data_) {
            return PrefixLeaf{child};
        }
        return PrefixLeaf{addr_->End()};
    }

    PrefixLeaf Find(const Key &key) {
        return Find(std::vector<Key>{key});
    }

    std::vector<PrefixLeaf> FindAll() {
        Check();
        std::vector<PrefixNode<Key, Object> *> nodes;
        nodes.reserve(Size());
        addr_->AppendLeafs(nodes);
        assert(nodes.size() == Size());

        std::vector<PrefixLeaf> leafs;
        leafs.reserve(Size());
        for (size_t i = 0; i < nodes.size(); ++i)
            leafs.emplace_back(PrefixLeaf{nodes[i]});
        return leafs;
    }

    PrefixLeaf Parent() const {
        Check();
        PrefixNode<Key, Object> *node = addr_->parent_;
        while (node != nullptr) {
            if (node->data_ != nullptr)
                return PrefixLeaf{node};
            node = node->parent_;
        }
        return PrefixLeaf{addr_->End()};
    }

    /**
     * this call will invalidate this leaf
     * any subsequent methods to the object will have undefined behavior
     */
    void Remove() {
        Check();
        addr_->Remove();
    }

    std::vector<Key> Prefix() const {
        Check();
        return addr_->Prefix();
    }

    Object &Data() const {
        Check();
        return *addr_->data_;
    }

    size_t Size() const {
        return Empty() ? 0 : addr_->Size();
    }

    bool Empty() const {
        return addr_ == nullptr || addr_->Empty();
    }

    bool operator==(const PrefixLeaf &other) const {
        Check();
        return addr_ == other.addr_;
    }

    bool operator!=(const PrefixLeaf &other) const {
        Check();
        return !this->operator==(other);
    }

private:
    explicit PrefixLeaf(PrefixNode<Key, Object> *addr)
            : addr_{addr && !addr->Empty() ? addr : nullptr} {
    }

    inline void Check() const {
        ASSERT(addr_ != nullptr, "Error: action cannot be performed on an empty leaf");
    }

    PrefixNode<Key, Object> *addr_;

    friend class PrefixTree<Key, Object>;

    friend class PrefixNode<Key, Object>;
};

template<typename Key, typename Object>
class PrefixTree {
public:
    using leaf = PrefixLeaf<Key, Object>;

    explicit PrefixTree() : root_{nullptr, Key()} {
    }

    leaf Insert(const std::vector<Key> &prefix, const Object &obj) {
        return leaf{root_.Insert(prefix, obj)};
    }

    leaf Find(const std::vector<Key> &prefix) {
        return leaf{root_.Find(prefix)};
    }

    std::vector<leaf> FindAll() {
        std::vector<PrefixNode<Key, Object> *> nodes;
        nodes.reserve(Size());
        root_.AppendLeafs(nodes);
        ASSERT(nodes.size() == Size(), "Node sizes equal");

        std::vector<leaf> leafs;
        leafs.reserve(nodes.size());
        for (size_t i = 0; i < nodes.size(); ++i)
            leafs.emplace_back(leaf{nodes[i]});
        return leafs;
    }

    leaf End() const {
        return leaf{root_.End()};
    }

    size_t Size() const { return root_.Size(); }

    bool Empty() const { return root_.Empty(); }

private:
    PrefixNode<Key, Object> root_;
};

}

#endif //HARA_PREFIX_TREE_H