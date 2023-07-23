#include <iostream>
#include <vector>
#include <string>

template <typename T>
class MyUniquePtr {
public:

    MyUniquePtr() : ptr(nullptr) {}
    explicit MyUniquePtr(T* ptr) : ptr(ptr) {}

    ~MyUniquePtr()
    {
        delete ptr;
    }

    MyUniquePtr(MyUniquePtr&& other)
    {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    MyUniquePtr& operator=(MyUniquePtr&& other)
    {
        if (this != &other)
        {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const
    {
        return *ptr;
    }

    T* operator->() const
    {
        return ptr;
    }

    explicit operator bool() const
    {
        return ptr != nullptr;
    }

    T* get() const
    {
        return ptr;
    }

    void reset(T* newPtr = nullptr)
    {
        delete ptr;
        ptr = newPtr;
    }

    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

private:
    T* ptr;

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(const MyUniquePtr&) = delete;
};

template <typename T>
class MyWeakPtr;
template <typename T>
class MySharedPtr {
public:
    MySharedPtr() : ptr(nullptr), ref_count(new size_t(0)) {}
    explicit MySharedPtr(T* ptr) : ptr(ptr), ref_count(new size_t(1)) {}
    MySharedPtr(const MyWeakPtr<T>& weakPtr) : ptr(weakPtr.ptr), ref_count(weakPtr.ref_count)
    {
        increaseRefCount();
    }

    ~MySharedPtr()
    {
        release();
    }

    MySharedPtr(const MySharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count)
    {
        increaseRefCount();
    }

    MySharedPtr& operator=(const MySharedPtr& other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            increaseRefCount();
        }
        return *this;
    }

    MySharedPtr(MySharedPtr&& other) : ptr(other.ptr), ref_count(other.ref_count)
    {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    MySharedPtr& operator=(MySharedPtr&& other)
    {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    T& operator*() const
    {
        return *ptr;
    }

    T* operator->() const
    {
        return ptr;
    }

    explicit operator bool() const
    {
        return ptr != nullptr;
    }

    T* get() const
    {
        return ptr;
    }

    size_t use_count() const
    {
        if (ref_count) {
            return *ref_count;
        }
        return 0;
    }

private:
    T* ptr;
    size_t* ref_count;
    friend class MyWeakPtr<T>;
    MySharedPtr(T* ptr, size_t* ref_count) : ptr(ptr), ref_count(ref_count)
    {
        increaseRefCount();
    }
    // Методы
    void increaseRefCount()
    {
        if (ref_count) {
            (*ref_count)++;
        }
    }

    void decreaseRefCount()
    {
        if (ref_count)
        {
            (*ref_count)--;
        }
    }

    void release()
    {
        decreaseRefCount();
        if (ref_count && *ref_count == 0)
        {
            delete ptr;
            delete ref_count;
            ptr = nullptr;
            ref_count = nullptr;
        }
    }
};

template <typename T>
class MyWeakPtr
{
public:
    MyWeakPtr() : ptr(nullptr), ref_count(nullptr) {}
    MyWeakPtr(const MySharedPtr<T>& sharedPtr) : ptr(sharedPtr.get()), ref_count(sharedPtr.ref_count) {}

    bool expired() const
    {
        return use_count() == 0;
    }

    MySharedPtr<T> lock() const
    {
        if (!expired())
        {
            return MySharedPtr<T>(*this);
        }
        return MySharedPtr<T>();
    }

    size_t use_count() const
    {
        if (ref_count)
        {
            return *ref_count;
        }
        return 0;
    }

private:
    T* ptr;
    size_t* ref_count;
    friend class MySharedPtr<T>;
};


class Widget
{
public:
    Widget() : parent(MyWeakPtr<Widget>()) {}
    virtual ~Widget() {}

    void setParent(const MySharedPtr<Widget>& newParent)
    {
        parent = newParent;
    }

    MyWeakPtr<Widget> getParent() const
    {
        return parent;
    }

    virtual std::string getType() const
    {
        return "Widget";
    }

private:
    MyWeakPtr<Widget> parent;
};


class TabWidget : public Widget
{
public:
    std::string getType() const override
    {
        return "TabWidget";
    }
};

class CalendarWidget : public Widget
{
public:
    std::string getType() const override
    {
        return "CalendarWidget";
    }
};

template<typename T>
using MyWidget = MySharedPtr<T>;

int main() {
    //Демонстрация MyUniquePtr
    MyUniquePtr<int> ptr1(new int(42));
    if(ptr1)
    {
        std::cout << "Value -> " << *ptr1 << std::endl;
    }
    MyUniquePtr<int> ptr2  = std::move(ptr1);
    if(!ptr1)
    {
        std::cout << "ptr1 is empty." << std::endl;
    }
    if(ptr2)
    {
        std::cout << "Value ptr2 -> " << *ptr2 << std::endl;
    }

    //Демонстрация MySharedPtr
    MySharedPtr<int> ptr3(new int(10));
    std::cout << "ptr3 -> " << *ptr3 << " ptr3 use count: " << ptr3.use_count() << std::endl;
    MySharedPtr<int> ptr4 = ptr3;
    std::cout << "ptr4 -> " << *ptr4 << " ptr4 use count: " << ptr4.use_count() << std::endl;
    *ptr4 = 100;
    std::cout << "After modifying ptr4, ptr3: " << *ptr3 << std::endl;

    //Демонстрация работы WeakPtr
    MySharedPtr<int> sharedPtr(new int(42));
    MyWeakPtr<int> weakPtr(sharedPtr);

    std::cout << "sharedPtr use count: " << sharedPtr.use_count() << std::endl;
    std::cout << "weakPtr use count: " << weakPtr.use_count() << std::endl;

    if(MySharedPtr<int> lockedPtr = weakPtr.lock())
    {
        *lockedPtr = 100;
        std::cout << "Value via lockedPtr: " << *lockedPtr << std::endl;
        std::cout << "sharedPtr use count after modification: " << sharedPtr.use_count() << std::endl;
    }
    else
    {
        std::cout << "The object is no longer valid." << std::endl;
    }
    std::cout << "sharedPtr use count after lock: " << sharedPtr.use_count() << std::endl;

    std::cout << "sharedPtr value: " << *sharedPtr << std::endl;

    std::cout << "sharedPtr use count after block: " << sharedPtr.use_count() << std::endl;

    //Работа с виджетами
    MyWidget<Widget> rootWidget = MyWidget<Widget>(new Widget());

    MyWidget<TabWidget> tabWidget1 = MyWidget<TabWidget>(new TabWidget());
    MyWidget<TabWidget> tabWidget2 = MyWidget<TabWidget>(new TabWidget());
    MyWidget<TabWidget> tabWidget3 = MyWidget<TabWidget>(new TabWidget());

    tabWidget1->setParent(rootWidget);
    tabWidget2->setParent(rootWidget);
    tabWidget3->setParent(rootWidget);

    std::cout << "tabWidget1: " << tabWidget1->getType() << ", parent: " << tabWidget1->getParent().lock()->getType() << std::endl;
    std::cout << "tabWidget2: " << tabWidget2->getType() << ", parent: " << tabWidget2->getParent().lock()->getType() << std::endl;
    std::cout << "tabWidget3: " << tabWidget3->getType() << ", parent: " << tabWidget3->getParent().lock()->getType() << std::endl;


    return 0;
}






