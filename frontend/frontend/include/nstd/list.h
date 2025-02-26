#pragma once
#include "pair.h"
namespace nstd {

    template<class K = int, class V = int>
    struct Node {
        pair<K, V> pair_;
        Node* next_;

        inline Node(const K& key, const V& value, Node* next = nullptr)
            : pair_{ key, value }, next_(next) {}
    };

    template<class K = int, class V = int>
    class list {
    private:
        Node<K, V>* root_ = nullptr;
        size_t size_ = 0;

        Node<K, V>* find(const K& key);

        void add_to_list(const K& key, const V& value);

    public:
        list(const K& key, const V& value);
        list();
        ~list();

        list(const list& other); 
        list& operator=(const list& other); 

        list(list&& other) noexcept;
        list& operator=(list&& other) noexcept;

        bool insert(const K& key, const V& value);
        bool upsert(const K& key, const V& value);
        bool earse(const K& key);

        const size_t size() const;

        V* search(const K& key);

        void clear();

        Node<K, V>* get_root();

        class iterator {
        private:
            Node<K, V>* current_;
        public:
            iterator() : current_(nullptr) {}
            explicit iterator(Node<K, V>* node) : current_(node) {}

            iterator& operator++() {
                if (current_) current_ = current_->next_;
                return *this;
            }

            pair<K, V>& operator*() { return current_->pair_; }
            pair<K, V>* operator->() { return &(current_->pair_); }

            bool operator==(const iterator& other) const { return current_ == other.current_; }
            bool operator!=(const iterator& other) const { return current_ != other.current_; }
        };


        iterator begin() { return iterator(root_); }
        iterator end() { return iterator(nullptr); }

    };
}

namespace nstd {
    template<class K, class V>
    inline list<K, V>::list(const K& key, const V& value)
    {
        size_++;
        this->root_ = new Node<K, V>(key, value);
    }

    template<class K, class V>
    inline list<K, V>::list()
    {
        this->root_ = nullptr;
    }

    template<class K, class V>
    inline list<K, V>::~list()
    {
        this->clear();
    }

    template<class K, class V>
    inline list<K, V>::list(const list<K, V>& other) : root_(nullptr) {
        if (other.root_) {
            root_ = new Node<K, V>(other.root_->pair_.key_, other.root_->pair_.value_);
            Node<K, V>* current = root_;
            Node<K, V>* otherCurrent = other.root_->next_;
            while (otherCurrent) {
                current->next_ = new Node<K, V>(otherCurrent->pair_.key_, otherCurrent->pair_.value_);
                current = current->next_;
                otherCurrent = otherCurrent->next_;
            }

            size_ = other.size_;
        }
    }

    template<class K, class V>
    inline list<K, V>& list<K, V>::operator=(const list<K, V>& other) {
        if (this != &other) {
            clear();
            if (other.root_) {
                root_ = new Node<K, V>(other.root_->pair_.key_, other.root_->pair_.value_);
                Node<K, V>* current = root_;
                Node<K, V>* otherCurrent = other.root_->next_;
                while (otherCurrent) {
                    current->next_ = new Node<K, V>(otherCurrent->pair_.key_, otherCurrent->pair_.value_);
                    current = current->next_;
                    otherCurrent = otherCurrent->next_;
                }
                size_ = other.size_;
            }
        }
        return *this;
    }

    template<class K, class V>
    inline list<K, V>::list(list<K, V>&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
    }

    template<class K, class V>
    inline list<K, V>& list<K, V>::operator=(list<K, V>&& other) noexcept {
        if (this != &other) {
            clear();
            root_ = other.root_;
            size_ = other.size_;
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    template<class K, class V>
    inline const size_t list<K, V>::size() const {
        return size_;
    }

    template<class K, class V>
    inline bool list<K, V>::insert(const K& key, const V& value)
    {
        Node<K, V>* tmp = find(key);
        if (tmp) { 
            return false; 
        }
        else {
            add_to_list(key, value); 
            return true; 
        }
    }

    template<class K, class V>
    bool list<K, V>::upsert(const K& key, const V& value)
    {
        Node<K, V>* tmp = find(key);
        if (tmp) { 
            tmp->pair_.value_ = value;
            return false;
        }
        else {
            add_to_list(key, value); 
            return true;
        }
    }

    template<class K, class V>
    inline bool list<K, V>::earse(const K& key)
    {
        if (!this->root_) return false;

        if (this->root_->pair_.key_ == key) {
            Node<K, V>* old_root = this->root_;
            this->root_ = old_root->next_;
            size_--;
            delete old_root;
            return true;
        }

        Node<K, V>* prev_node = this->root_;
        Node<K, V>* target_node = this->root_->next_;

        while (target_node) {
            if (target_node->pair_.key_ == key) {
                prev_node->next_ = target_node->next_;
                size_--;
                delete target_node;
                return true;
            }
            prev_node = target_node;
            target_node = target_node->next_;
        };

        return false;
    }

    template<class K, class V>
    inline V* list<K, V>::search(const K& key)
    {
        Node<K, V>* target_node = this->find(key);
        if (!target_node) return nullptr;

        return &target_node->pair_.value_;
    }

    template<class K, class V>
    inline Node<K, V>* list<K, V>::get_root()
    {
        return root_;
    }

    template<class K, class V>
    inline void list<K, V>::clear()
    {
        Node<K, V>* current = root_;
        while (current != nullptr)
        {
            Node<K, V>* temp = current;
            current = current->next_;
            delete temp;
        }
        root_ = nullptr;
        size_ = 0;
    }

    template<class K, class V>
    inline Node<K, V>* nstd::list<K, V>::find(const K& key)
    {
        Node<K, V>* current = root_;
        while (current) {
            if (current->pair_.key_ == key) {
                return current;
            }
            current = current->next_;
        }
        return nullptr;
    }

    template<class K, class V>
    void list<K, V>::add_to_list(const K& key, const V& value)
    {
        size_++;
        this->root_ = new Node<K, V>(key, value, this->root_);
    }
}
