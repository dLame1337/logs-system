#pragma once
#include "list.h"
#include "array.h"

#define DEFAULT_BUCKETS_COUNT 16

namespace nstd {


    template<class K = int, class V = int>
    class unordered_map {
    private:
        float load_factor_ = 0.0f;
        float load_factor_trigger_ = 1.0f;
        size_t buckets_count_;
        size_t elements_count_ = 0;

        array<list<K, V>*>* buckets_;

    private:
        array<list<K, V>*>* alloc_buckets(size_t count);
        bool check_buckets_present();
        void init_buckets(size_t size = DEFAULT_BUCKETS_COUNT);

        size_t get_index(const K& key) const;
        void rehash(size_t new_bucket_count);

        void rehash_if_need();

    public:
        class iterator {
        private:
            unordered_map* map_;
            size_t bucket_idx_;
            typename list<K, V>::iterator list_it_;

            void advance_to_valid() {
                while (bucket_idx_ < map_->buckets_count_ && (!(*map_->buckets_)[bucket_idx_] || list_it_ == (*map_->buckets_)[bucket_idx_]->end())) {
                    ++bucket_idx_;
                    if (bucket_idx_ < map_->buckets_count_ && (*map_->buckets_)[bucket_idx_]) {
                        list_it_ = (*map_->buckets_)[bucket_idx_]->begin();
                    }
                }
            }
        public:
            iterator(unordered_map* map, size_t bucket_idx, typename list<K, V>::iterator list_it)
                : map_(map), bucket_idx_(bucket_idx), list_it_(list_it) {
                advance_to_valid();
            }

            iterator& operator++() {
                ++list_it_;
                advance_to_valid();
                return *this;
            }

            pair<K, V>& operator*() { return *list_it_; }
            pair<K, V>* operator->() { return &(*list_it_); }

            bool operator==(const iterator& other) const {
                return bucket_idx_ == other.bucket_idx_ && list_it_ == other.list_it_;
            }
            bool operator!=(const iterator& other) const { return !(*this == other); }
        };


    public:
        unordered_map();

        unordered_map(const std::initializer_list<pair<K, V>>& args);

        unordered_map(const unordered_map& other);
        unordered_map(unordered_map&& other) noexcept;
        ~unordered_map();

        unordered_map& operator=(const unordered_map& other);
        unordered_map& operator=(unordered_map&& other) noexcept;

        V& at(const K& key);
        V& operator[](const K& key);

        void insert(const pair<K, V>& p);
        void emplace(K&& key, V&& value);
        void erase(const K& key);
        void clear();

        iterator begin() {
            for (size_t i = 0; i < buckets_count_; ++i) {
                if ((*buckets_)[i]) {
                    return iterator(this, i, (*buckets_)[i]->begin());
                }
            }
            return end();
        }
        iterator end() { return iterator(this, buckets_count_, typename list<K, V>::iterator(nullptr)); }

        bool contains(const K& key);

        bool empty() const;
        size_t size() const;
        size_t max_size() const;

        size_t bucket_count() const;
        float load_factor() const;
        void set_load_factor_trigger(float load_factor_trigger);
        void reserve(size_t size);
        void resize(size_t size);

    };

}

namespace nstd {
    template<class K, class V>
    inline array<list<K, V>*>* unordered_map<K, V>::alloc_buckets(size_t count)
    {
        array<list<K, V>*>* new_buckets = new array<list<K, V>*>(count);
        memset(new_buckets->data(), 0, count * sizeof(list<K, V>*));
        return new_buckets;
    }

    template<class K, class V>
    inline bool unordered_map<K, V>::check_buckets_present()
    {
        if (!buckets_) {
            init_buckets();
            return false;
        }

        return true;
    }

    template<class K, class V>
    inline void unordered_map<K, V>::init_buckets(size_t size)
    {
        elements_count_ = 0;
        buckets_count_ = size;
        buckets_ = alloc_buckets(size);
    }

    template<class K, class V>
    inline size_t unordered_map<K, V>::get_index(const K& key) const
    {
        return std::hash<K>{}(key) % buckets_count_;
    }

    template<class K, class V>
    inline void unordered_map<K, V>::rehash_if_need()
    {
        load_factor_ = static_cast<double>(elements_count_) / static_cast<double>(buckets_count_);
        if (load_factor_ >= load_factor_trigger_) {
            //std::cout << "REHASHING!" << std::endl
            //    << "elements_count_" << elements_count_ << std::endl
            //    << "buckets_count_" << buckets_count_ << std::endl
            //    << "load_factor_ :" << load_factor_ << std::endl;
            rehash(buckets_count_ * 2);
        }
    }

    template<class K, class V>
    inline unordered_map<K, V>::unordered_map()
    {
        buckets_count_ = DEFAULT_BUCKETS_COUNT;
        buckets_ = alloc_buckets(DEFAULT_BUCKETS_COUNT);
    }

    template<class K, class V>
    inline unordered_map<K, V>::unordered_map(const std::initializer_list<pair<K, V>>& args)
    {
        for (const auto arg : args) {
            insert(arg);
        }
    }

    template<class K, class V>
    inline unordered_map<K, V>::unordered_map(const unordered_map& other)
        : load_factor_(other.load_factor_),
        load_factor_trigger_(other.load_factor_trigger_),
        buckets_count_(other.buckets_count_),
        elements_count_(0)
    {
        buckets_ = alloc_buckets(buckets_count_);
        for (size_t i = 0; i < other.buckets_->size(); ++i) {
            list<K, V>* other_list = (*other.buckets_)[i];
            if (other_list) {
                (*buckets_)[i] = new list<K, V>(*other_list);
                elements_count_ += other_list->size();
            }
        }
    }

    template<class K, class V>
    inline unordered_map<K, V>& unordered_map<K, V>::operator=(const unordered_map& other) {
        if (this != &other) {
            unordered_map temp(other);
            std::swap(load_factor_, temp.load_factor_);
            std::swap(load_factor_trigger_, temp.load_factor_trigger_);
            std::swap(buckets_count_, temp.buckets_count_);
            std::swap(elements_count_, temp.elements_count_);
            std::swap(buckets_, temp.buckets_);
        }
        return *this;
    }

    template<class K, class V>
    inline unordered_map<K, V>::unordered_map(unordered_map&& other) noexcept
        : load_factor_(other.load_factor_),
        load_factor_trigger_(other.load_factor_trigger_),
        buckets_count_(other.buckets_count_),
        elements_count_(other.elements_count_),
        buckets_(other.buckets_)
    {
        other.buckets_ = nullptr;
        other.buckets_count_ = 0;
        other.elements_count_ = 0;
        other.load_factor_ = 0.0f;
    }

    template<class K, class V>
    inline unordered_map<K, V>& unordered_map<K, V>::operator=(unordered_map&& other) noexcept {
        if (this != &other) {
            clear();
            load_factor_ = other.load_factor_;
            load_factor_trigger_ = other.load_factor_trigger_;
            buckets_count_ = other.buckets_count_;
            elements_count_ = other.elements_count_;
            buckets_ = other.buckets_;
            other.buckets_ = nullptr;
            other.buckets_count_ = 0;
            other.elements_count_ = 0;
            other.load_factor_ = 0.0f;
        }
        return *this;
    }

    template<class K, class V>
    inline unordered_map<K, V>::~unordered_map()
    {
        clear();
    }

    template<class K, class V>
    inline V& unordered_map<K, V>::at(const K& key)
    {
        check_buckets_present();
        rehash_if_need();

        size_t index = get_index(key);
        list<K, V>** curr_list = &((*buckets_)[index]);

        if (!(*curr_list)) {
            *curr_list = new list<K, V>(key, V());
            elements_count_++;
            return (*curr_list)->get_root()->pair_.value_;
        }

        V* result = (*curr_list)->search(key);
        if (!result) {
            (*curr_list)->insert(key, V());
            elements_count_++;
            return (*curr_list)->get_root()->pair_.value_;
        }

        return *result;
    }

    template<class K, class V>
    inline V& unordered_map<K, V>::operator[](const K& key)
    {
        return at(key);
    }

    template<class K, class V>
    inline void unordered_map<K, V>::insert(const pair<K, V>& p)
    {
        check_buckets_present();
        rehash_if_need();

        const K key = p.key_;
        const V value = p.value_;

        size_t index = get_index(key);
        list<K, V>** curr_list = &((*buckets_)[index]);

        if (!(*curr_list)) { // якщо листа не існувало ->
            //std::cout << "(insert)index: " << index << std::endl << "Creating new list for: " << key << ":" << value << std::endl;
            *curr_list = new list<K, V>(key, value);
            elements_count_++;
            return;
        }
        // *лист існує*

        if ((*curr_list)->insert(key, value)) {
            //std::cout << "(insert)index: " << index << std::endl << "Just adding new value: " << key << ":" << value << std::endl;
            elements_count_++;
        }
    }

    template<class K, class V>
    inline void unordered_map<K, V>::emplace(K&& key, V&& value)
    {
        rehash_if_need();

        size_t index = get_index(key);
        list<K, V>** curr_list = &buckets_[index];

        if (!(*curr_list)) {
            //std::cout << "(emplace)index: " << index << std::endl << "Creating new list for: " << key << ":" << value << std::endl;
            *curr_list = new list<K, V>(key, value);
            elements_count_++;
            return (*curr_list)->search(key);
        }

        if ((*curr_list)->insert(key, value)) {
            elements_count_++;
            //std::cout << "(emplace)index: " << index << std::endl << "Just adding new value: " << key << ":" << value << std::endl;
        }
    }

    template<class K, class V>
    inline void unordered_map<K, V>::erase(const K& key)
    {
        if (!check_buckets_present()) return;

        size_t index = get_index(key);
        list<K, V>* curr_list = (*buckets_)[index];
        if (curr_list) {
            if (curr_list->earse(key)) {
                //std::cout << "(earse)index: " << index << std::endl << "deleting: " << key << std::endl;
                elements_count_--;
            }
        }
    }

    template<class K, class V>
    inline void unordered_map<K, V>::clear()
    {
        if (!buckets_) return;

        for (int i = 0; i < buckets_->size(); ++i) {
            list<K, V>* curr = (*buckets_)[i];
            if (curr)
                delete curr;
        }
        delete buckets_;
        load_factor_ = 0;
        elements_count_ = 0;
        buckets_count_ = 0;
        buckets_ = nullptr;
    }

    template<class K, class V>
    inline bool unordered_map<K, V>::contains(const K& key)
    {
        if (!check_buckets_present()) return false;

        size_t index = get_index(key);
        //std::cout << std::endl << "index: " << index << " key: " << key << std::endl;
        list<K, V>* curr_list = (*buckets_)[index];
        if (curr_list)
            if (curr_list->search(key)) return true;

        return false;
    }

    template<class K, class V>
    inline bool unordered_map<K, V>::empty() const
    {
        return elements_count_ == 0;
    }

    template<class K, class V>
    inline size_t unordered_map<K, V>::size() const
    {
        return elements_count_;
    }

    template<class K, class V>
    inline size_t unordered_map<K, V>::max_size() const
    {
        return size_t();
    }

    template<class K, class V>
    inline size_t unordered_map<K, V>::bucket_count() const
    {
        return buckets_count_;
    }

    template<class K, class V>
    inline float unordered_map<K, V>::load_factor() const
    {
        return load_factor_;
    }

    template<class K, class V>
    void unordered_map<K, V>::set_load_factor_trigger(float load_factor_trigger)
    {
        load_factor_trigger_ = load_factor_trigger;
    }

    template<class K, class V>
    inline void unordered_map<K, V>::rehash(size_t new_bucket_count)
    {
        array<list<K, V>*>* new_buckets = alloc_buckets(new_bucket_count);
        for (int i = 0; i < buckets_->size(); ++i) {
            list<K, V>* curr_list = (*buckets_)[i];
            if (!curr_list) continue;

            for (const auto& element : *curr_list) {
                size_t hash = std::hash<K>{}(element.key_);
                size_t new_index = hash % new_bucket_count;

                if (!(*new_buckets)[new_index]) {
                    (*new_buckets)[new_index] = new list<K, V>(element.key_, element.value_);
                    continue;
                }
                (*new_buckets)[new_index]->insert(element.key_, element.value_);
            }
        }

        delete buckets_;
        buckets_count_ = new_bucket_count;
        buckets_ = new_buckets;
    }

    template<class K, class V>
    inline void unordered_map<K, V>::reserve(size_t size)
    {
        clear();
        init_buckets(size);
    }

    template<class K, class V>
    inline void unordered_map<K, V>::resize(size_t size)
    {
        rehash(size); 
    }
}
