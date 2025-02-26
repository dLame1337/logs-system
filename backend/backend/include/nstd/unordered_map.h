#pragma once
#include "list.h"
#include "array.h"

#define DEFAULT_BUCKETS_COUNT 16

namespace nstd {
    

    template<class K = int, class V = int>
    class unordered_map {
    private:
        float load_factor_ = 0.0f; // степінь загруженості таблиці
        float load_factor_trigger_ = 1.0f; // по дефолту 1
        size_t buckets_count_;
        size_t elements_count_ = 0;

        array<list<K, V>*>* buckets_; // вказівник на массив бакетів

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
        // конструктори та деструктори
        unordered_map();

        unordered_map(const std::initializer_list<pair<K, V>>& args);

        unordered_map(const unordered_map& other);
        unordered_map(unordered_map&& other) noexcept;
        ~unordered_map();

        // оператор присвоювання
        unordered_map& operator=(const unordered_map& other);
        unordered_map& operator=(unordered_map&& other) noexcept;

        // доступ до елементів
        V& at(const K& key);
        V& operator[](const K& key);

        // модифікатори
        void insert(const pair<K, V>& p);
        void emplace(K&& key, V&& value);
        void erase(const K& key);
        void clear();

        // ітератори
        iterator begin() {
            for (size_t i = 0; i < buckets_count_; ++i) {
                if ((*buckets_)[i]) {
                    return iterator(this, i, (*buckets_)[i]->begin());
                }
            }
            return end();
        }
        iterator end() { return iterator(this, buckets_count_, typename list<K, V>::iterator(nullptr)); }

        // перевірка елементів
        bool contains(const K& key);

        // розмір та ємкість
        bool empty() const;
        size_t size() const;
        size_t max_size() const;

        // функції хеш таблиці
        size_t bucket_count() const;
        float load_factor() const;
        void set_load_factor_trigger(float load_factor_trigger);
        void reserve(size_t size);
        void resize(size_t size);

    };

}
#pragma once
#include "nstd/unordered_map.h"
namespace nstd {
    template<class K, class V>
    inline array<list<K, V>*>* unordered_map<K, V>::alloc_buckets(size_t count)
    {
        array<list<K, V>*>* new_buckets = new array<list<K, V>*>(count); // виділяємо бакети
        memset(new_buckets->data(), 0, count * sizeof(list<K, V>*)); // зануляємо массив щоб розуміти в якому бакеті ще немає листа.
        return new_buckets;
    }

    template<class K, class V>
    inline bool unordered_map<K, V>::check_buckets_present()
    {
        if (!buckets_) { // якщо немає бакетів ->
            init_buckets(); // ініціалізуємо
            return false; // повертаємо false, тому що бакетів не було
        }

        return true; // true тому що бакети вже були
    }

    template<class K, class V>
    inline void unordered_map<K, V>::init_buckets(size_t size)
    {
        elements_count_ = 0;
        buckets_count_ = size; // по дефолту 16
        buckets_ = alloc_buckets(size); // виділяємо бакети
    }

    template<class K, class V>
    inline size_t unordered_map<K, V>::get_index(const K& key) const
    {
        return std::hash<K>{}(key) % buckets_count_; // ключ->хеш->остача від ділення на кількість бакетів
    }

    template<class K, class V>
    inline void unordered_map<K, V>::rehash_if_need()
    {
        load_factor_ = static_cast<double>(elements_count_) / static_cast<double>(buckets_count_); // якщо елементів стає більше ніж бакетів лоад фактор стає більшим ніж 1
        if (load_factor_ >= load_factor_trigger_) { // якщо досягли трігерної степені завантаження - рехешуєм
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
        buckets_count_ = DEFAULT_BUCKETS_COUNT; // по дефолту 16
        buckets_ = alloc_buckets(DEFAULT_BUCKETS_COUNT); // виділяємо бакети
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
            unordered_map temp(other); // створюєму повну копію за допромогую конструктора копіювання
            std::swap(load_factor_, temp.load_factor_); // міняємо містями всі поля
            std::swap(load_factor_trigger_, temp.load_factor_trigger_);
            std::swap(buckets_count_, temp.buckets_count_);
            std::swap(elements_count_, temp.elements_count_);
            std::swap(buckets_, temp.buckets_);
            // деструктор у temp викликається автоматично (нашу таблицю очищати не потрібно тому що вона очиститься в temp)
        }
        return *this;
    }

    template<class K, class V>
    inline unordered_map<K, V>::unordered_map(unordered_map&& other) noexcept // просто забираємо поля чужого класу
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
            clear(); // очищаємо нашу таблицю від минулих данних
            load_factor_ = other.load_factor_; // забираємо всі поля
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
        rehash_if_need(); // рехешуємо якщо потрібно

        size_t index = get_index(key); // беремо індекс бакета
        list<K, V>** curr_list = &((*buckets_)[index]); // получаємо вказівник на елемент массиву по індексу

        if (!(*curr_list)) { // якщо листа не існувало ->
            *curr_list = new list<K, V>(key, V()); // створюємо новий лист та зразу в нього добавляємо елемент
            elements_count_++;
            return (*curr_list)->get_root()->pair_.value_; // получаємо перший та єдиний елемент
        }

        V* result = (*curr_list)->search(key); // шукаємо в листу елемент
        if (!result) { // якщо елемент не знайдений ->
            (*curr_list)->insert(key, V()); // добавляємо новий елемент
            elements_count_++;
            return (*curr_list)->get_root()->pair_.value_; // получаємо перший та єдиний елемент
        }

        return *result; // повертаємо значення
    }

    template<class K, class V>
    inline V& unordered_map<K, V>::operator[](const K& key)
    {
        return at(key); // повертаємо значення
    }

    template<class K, class V>
    inline void unordered_map<K, V>::insert(const pair<K, V>& p)
    {
        check_buckets_present();
        rehash_if_need(); // рехешуємо якщо потрібно

        const K key = p.key_;
        const V value = p.value_;

        size_t index = get_index(key); // получаємо індекс бакета
        list<K, V>** curr_list = &((*buckets_)[index]); // получаємо вказівник на елемент массиву по індексу

        if (!(*curr_list)) { // якщо листа не існувало ->
            //std::cout << "(insert)index: " << index << std::endl << "Creating new list for: " << key << ":" << value << std::endl;
            *curr_list = new list<K, V>(key, value); // створюємо новий лист та зразу в нього добавляємо елемент
            elements_count_++;
            return; // получаємо перший та єдиний елемент
        }
        // *лист існує*

        if ((*curr_list)->insert(key, value)) { // просто добавляємо елемент в список(якщо вже такий існує ставка проігнорується)
            //std::cout << "(insert)index: " << index << std::endl << "Just adding new value: " << key << ":" << value << std::endl;
            elements_count_++; // якщо добавився новий елемент->збільшуємо кількість елементів в мапі
        }
    }

    template<class K, class V>
    inline void unordered_map<K, V>::emplace(K&& key, V&& value)
    {
        rehash_if_need(); // рехешуємо якщо потрібно

        size_t index = get_index(key); // получаємо індекс бакета
        list<K, V>** curr_list = &buckets_[index]; // получаємо вказівник на елемент массиву по індексу

        if (!(*curr_list)) { // якщо листа не існувало ->
            //std::cout << "(emplace)index: " << index << std::endl << "Creating new list for: " << key << ":" << value << std::endl;
            *curr_list = new list<K, V>(key, value); // створюємо новий лист та зразу в нього добавляємо елемент
            elements_count_++;
            return (*curr_list)->search(key); // робимо пошук в листу щоб повернути елемент(він буде моментальний тому що елемент лише 1)
        }
        // *лист існує*

        if ((*curr_list)->insert(key, value)) { // просто добавляємо елемент в список(якщо вже такий існує ставка проігнорується)
            elements_count_++; // якщо добавився новий елемент->збільшуємо кількість елементів в мапі
            //std::cout << "(emplace)index: " << index << std::endl << "Just adding new value: " << key << ":" << value << std::endl;
        }
    }

    template<class K, class V>
    inline void unordered_map<K, V>::erase(const K& key)
    {
        if (!check_buckets_present()) return; // можна не продовжувати пошук тому що бакетів і так не було
        // не перевіряю рехеш тому що в меншу сторорону всерівно не буду міняти бакети

        size_t index = get_index(key); // получаємо індекс бакета
        list<K, V>* curr_list = (*buckets_)[index]; // получаємо вказівник на елемент массиву по індексу
        if (curr_list) {
            if (curr_list->earse(key)) { // видаляємо з листа елемент по ключу
                //std::cout << "(earse)index: " << index << std::endl << "deleting: " << key << std::endl;
                elements_count_--; // зменшуємо кількість елементів якщо був видалений якись елемент
            }
        }
    }

    template<class K, class V>
    inline void unordered_map<K, V>::clear()
    {
        if (!buckets_) return;

        for (int i = 0; i < buckets_->size(); ++i) {
            list<K, V>* curr = (*buckets_)[i];
            if (curr) // очищаємо всі листи(кожен лист сам очистить свої елементи)
                delete curr;
        }
        delete buckets_; // очищаємо бакети
        load_factor_ = 0;
        elements_count_ = 0;
        buckets_count_ = 0;
        buckets_ = nullptr;
    }

    template<class K, class V>
    inline bool unordered_map<K, V>::contains(const K& key)
    {
        if (!check_buckets_present()) return false; // повертаємо зразу false тому що бакетів і так не було а значіть і ключа такого не було

        size_t index = get_index(key); // получаємо індекс бакета
        //std::cout << std::endl << "index: " << index << " key: " << key << std::endl;
        list<K, V>* curr_list = (*buckets_)[index]; // получаємо вказівник на елемент массиву по індексу
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
        load_factor_trigger_ = load_factor_trigger; // можна настроїти на якій степені загрузки таблиці буде ставатися рехешування
    }

    template<class K, class V>
    inline void unordered_map<K, V>::rehash(size_t new_bucket_count)
    {
        array<list<K, V>*>* new_buckets = alloc_buckets(new_bucket_count); // створюємо новий массив бакетів
        for (int i = 0; i < buckets_->size(); ++i) { // проходимося по всім минулим бакетам
            list<K, V>* curr_list = (*buckets_)[i];
            if (!curr_list) continue; // якщо бакет пустий - скіпаємо

            for (const auto& element : *curr_list) { // ітеруємося по всім елементам листа
                size_t hash = std::hash<K>{}(element.key_); // вичісляємо хеш ключа(можна оптимізувати тим що разом з ключем зберігати вже готовий хеш)
                size_t new_index = hash % new_bucket_count;

                if (!(*new_buckets)[new_index]) { // якщо немає листу на цьому бакеті ->
                    (*new_buckets)[new_index] = new list<K, V>(element.key_, element.value_); // створюємо новий лист та добавляємо в нього елемент
                    continue;
                }
                // інакше ->
                (*new_buckets)[new_index]->insert(element.key_, element.value_); // добавляємо в існуючий лист елемент
            }
        }

        delete buckets_; // очищаємо старі бакети
        buckets_count_ = new_bucket_count; // встановлюємо нову кількість бакетів
        buckets_ = new_buckets; // тепер бакети вказують на новий массив
    }

    template<class K, class V>
    inline void unordered_map<K, V>::reserve(size_t size)
    {
        clear(); // очищаємо старі бакети
        init_buckets(size); // виділяємо нові бакети
    }

    template<class K, class V>
    inline void unordered_map<K, V>::resize(size_t size)
    {
        rehash(size); // просто рехешуєм таблицю зберігаючи старі елементи
    }
}
