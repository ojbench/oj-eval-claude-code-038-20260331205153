#include <iostream>
#include <cstring>
#include <algorithm>
#include <memory>

template<typename T>
class Vector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    // Growth factor for capacity expansion
    static constexpr double GROWTH_FACTOR = 1.5;

public:
    // Constructor
    Vector() : data_(nullptr), size_(0), capacity_(0) {}

    explicit Vector(size_t n) : size_(n), capacity_(n) {
        if (n > 0) {
            data_ = static_cast<T*>(malloc(n * sizeof(T)));
            for (size_t i = 0; i < n; ++i) {
                new (&data_[i]) T();
            }
        } else {
            data_ = nullptr;
        }
    }

    Vector(size_t n, const T& value) : size_(n), capacity_(n) {
        if (n > 0) {
            data_ = static_cast<T*>(malloc(n * sizeof(T)));
            for (size_t i = 0; i < n; ++i) {
                new (&data_[i]) T(value);
            }
        } else {
            data_ = nullptr;
        }
    }

    // Copy constructor
    Vector(const Vector& other) : size_(other.size_), capacity_(other.capacity_) {
        if (capacity_ > 0) {
            data_ = static_cast<T*>(malloc(capacity_ * sizeof(T)));
            for (size_t i = 0; i < size_; ++i) {
                new (&data_[i]) T(other.data_[i]);
            }
        } else {
            data_ = nullptr;
        }
    }

    // Move constructor
    Vector(Vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Copy assignment
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            // Clean up existing data
            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T();
            }
            free(data_);

            // Copy from other
            size_ = other.size_;
            capacity_ = other.capacity_;
            if (capacity_ > 0) {
                data_ = static_cast<T*>(malloc(capacity_ * sizeof(T)));
                for (size_t i = 0; i < size_; ++i) {
                    new (&data_[i]) T(other.data_[i]);
                }
            } else {
                data_ = nullptr;
            }
        }
        return *this;
    }

    // Move assignment
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            // Clean up existing data
            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T();
            }
            free(data_);

            // Take ownership from other
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Destructor
    ~Vector() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        free(data_);
    }

    // Element access
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }

    T& at(size_t index) {
        if (index >= size_) throw std::out_of_range("Vector::at");
        return data_[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) throw std::out_of_range("Vector::at");
        return data_[index];
    }

    T& front() { return data_[0]; }
    const T& front() const { return data_[0]; }

    T& back() { return data_[size_ - 1]; }
    const T& back() const { return data_[size_ - 1]; }

    T* data() { return data_; }
    const T* data() const { return data_; }

    // Capacity
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            T* new_data = static_cast<T*>(malloc(new_capacity * sizeof(T)));
            for (size_t i = 0; i < size_; ++i) {
                new (&new_data[i]) T(std::move(data_[i]));
                data_[i].~T();
            }
            free(data_);
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

    void shrink_to_fit() {
        if (size_ < capacity_) {
            if (size_ == 0) {
                free(data_);
                data_ = nullptr;
                capacity_ = 0;
            } else {
                T* new_data = static_cast<T*>(malloc(size_ * sizeof(T)));
                for (size_t i = 0; i < size_; ++i) {
                    new (&new_data[i]) T(std::move(data_[i]));
                    data_[i].~T();
                }
                free(data_);
                data_ = new_data;
                capacity_ = size_;
            }
        }
    }

    // Modifiers
    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : static_cast<size_t>(capacity_ * GROWTH_FACTOR);
            reserve(new_capacity);
        }
        new (&data_[size_]) T(value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : static_cast<size_t>(capacity_ * GROWTH_FACTOR);
            reserve(new_capacity);
        }
        new (&data_[size_]) T(std::move(value));
        ++size_;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : static_cast<size_t>(capacity_ * GROWTH_FACTOR);
            reserve(new_capacity);
        }
        new (&data_[size_]) T(std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    void resize(size_t new_size) {
        if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                new (&data_[i]) T();
            }
        } else if (new_size < size_) {
            for (size_t i = new_size; i < size_; ++i) {
                data_[i].~T();
            }
        }
        size_ = new_size;
    }

    void resize(size_t new_size, const T& value) {
        if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                new (&data_[i]) T(value);
            }
        } else if (new_size < size_) {
            for (size_t i = new_size; i < size_; ++i) {
                data_[i].~T();
            }
        }
        size_ = new_size;
    }

    // Iterator support
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() { return data_; }
    const_iterator begin() const { return data_; }
    const_iterator cbegin() const { return data_; }

    iterator end() { return data_ + size_; }
    const_iterator end() const { return data_ + size_; }
    const_iterator cend() const { return data_ + size_; }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // Since we don't have the exact problem description,
    // I'll create a basic test that should handle common vector operations
    int n;
    std::cin >> n;

    Vector<int> vec;
    for (int i = 0; i < n; ++i) {
        int val;
        std::cin >> val;
        vec.push_back(val);
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << vec[i];
    }
    std::cout << "\n";

    return 0;
}
