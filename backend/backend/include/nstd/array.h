#pragma once
namespace nstd {
    template<class T>
    class array {
    private:
        T* data_ = nullptr;
        size_t size_ = 0;
        size_t capacity_ = 1;

    public:
        array(size_t size);
        array();
        ~array();

        array(const array& other);
        array& operator=(const array& other);

        array(array&& other) noexcept;
        array& operator=(array&& other) noexcept;

        void push_back(const T& value);

        void resize(size_t size);

        size_t size() const;

        T* data() const;

        class iterator {
        private:
            T* current_;
        public:
            explicit iterator(array* arr, size_t size) : current_(arr->data_ + size) {}
            explicit iterator(array* arr) : current_(arr->data_) {}

            iterator& operator++() {
                current_++;
                return *this;
            }

            T& operator*() { return *current_; }
            T* operator->() { return *current_; }

            bool operator==(const iterator& other) const { return current_ == other.current_; }
            bool operator!=(const iterator& other) const { return current_ != other.current_; }
        };


        iterator begin() { return iterator(this); }
        iterator end() { return iterator(this, size_); }

        T& operator[](size_t index);
        const T& operator[](size_t index) const;

    };
}

namespace nstd {
    template<class T>
    inline array<T>::array()
    {
        data_ = new T[capacity_]; // capacity_ == 1
    }

    template<class T>
    inline array<T>::array(size_t size)
    {
        capacity_ = size * 2;
        size_ = size;
        data_ = new T[capacity_];
    }

    template<class T>
    inline array<T>::~array()
    {
        if (data_)
            delete[] data_;
    }
    template<class T>
    inline array<T>::array(const array<T>& other)
        : size_(other.size_), capacity_(other.capacity_) {
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; i++) {
            data_[i] = other.data_[i];
        }
    }

    template<class T>
    inline array<T>& array<T>::operator=(const array<T>& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = new T[capacity_];
            for (size_t i = 0; i < size_; i++) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    template<class T>
    inline array<T>::array(array<T>&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    template<class T>
    inline array<T>& array<T>::operator=(array<T>&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }


    template<class T>
    inline void array<T>::push_back(const T& value)
    {
        if (size_ >= capacity_)
        {
            capacity_ = capacity_ * 2;
            T* newData = new T[capacity_];

            for (size_t i = 0; i < size_; i++) {
                newData[i] = data_[i];
            }
            delete[] data_;
            data_ = newData;
        }

        data_[size_++] = value;
    }

    template<class T>
    inline void array<T>::resize(size_t size)
    {
        if (size > capacity_) {
            capacity_ = size * 2;
            T* newData = new T[capacity_];
            size_t copy_size;

            if (size > size_) {
                copy_size = size_;
            }
            else {
                copy_size = size;
            }

            for (size_t i = 0; i < copy_size; i++) {
                newData[i] = data_[i];
            }

            delete[] data_;
            data_ = newData;
        }

        size_ = size;
    }

    template<class T>
    inline T& array<T>::operator[](size_t index)
    {
        return data_[index];
    }

    template<class T>
    inline const T& array<T>::operator[](size_t index) const
    {
        return data_[index];
    }

    template<class T>
    inline size_t array<T>::size() const
    {
        return size_;
    }

    template<class T>
    T* array<T>::data() const
    {
        return data_;
    }
}