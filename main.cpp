#include <iostream>
#include <vector>
#include <string>

template<typename T>
class MyUniquePtr
{
private:
    T* ptr;
public:
    MyUniquePtr() : ptr(nullptr) {}
    explicit MyUniquePtr(T* p) : ptr(p) {}

    ~MyUniquePtr()
    {
        delete ptr;
    }

    T& operator*() const
    {
        return *ptr;
    }

    operator bool() const
    {
        return ptr != nullptr;
    }


    MyUniquePtr(MyUniquePtr&& other) noexcept
    {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    MyUniquePtr& operator =(MyUniquePtr&& other) noexcept
    {
        if(this != &other)
        {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr operator =(const MyUniquePtr&) = delete;
};

template<typename T>
class MySharedPtr
{
private:
    T* ptr;
    size_t* refCount;
private:
    void release()
    {
        (*refCount)--;
        if(*refCount == 0)
        {
            delete ptr;
            delete refCount;
        }
    }
public:
    MySharedPtr() : ptr(nullptr), refCount(new size_t(0)) {}
    explicit MySharedPtr(T* p) : ptr(p), refCount(new size_t(1)) {}

    ~MySharedPtr()
    {
        release();
    }

    MySharedPtr(const MySharedPtr& other) : ptr(other.ptr), refCount(other.refCount)
    {
        (*refCount)++;
    }

    MySharedPtr& operator =(const MySharedPtr& other)
    {
        if(this != &other)
        {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            (*refCount)++;
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

    operator bool() const
    {
        return ptr != nullptr;
    }
};

template<typename T>
class MyWeakPtr
{
private:
    T* ptr;
    size_t* refCount;
public:
    MyWeakPtr() : ptr(nullptr), refCount(nullptr) {}
    explicit MyWeakPtr(const MySharedPtr<T>& sharedPtr) : ptr(sharedPtr.ptr) {}

    ~MyWeakPtr() = default;

    MyWeakPtr(const MyWeakPtr& other) : ptr(other.ptr), refCount(other.refCount) {}

    MyWeakPtr& operator=(const MyWeakPtr& other)
    {
        if (this != &other)
        {
            ptr = other.ptr;
            refCount = other.refCount;
        }
        return *this;
    }

    MySharedPtr<T> lock() const
    {
        if (expired())
        {
            return MySharedPtr<T>();
        }
        return MySharedPtr<T>(*this);
    }

    bool expired() const
    {
        return (refCount == nullptr || *refCount == 0);
    }

private:
    friend class MySharedPtr<T>;
};


class Widget {
private:
    Widget* parent;
    std::vector<Widget*> children;

public:
    Widget() : parent(nullptr), children() {
        std::cout << "Widget created" << std::endl;
    }

    virtual ~Widget() {
        std::cout << "Widget destroyed" << std::endl;
    }

    void setParent(Widget* newParent) {
        if (parent) {
            parent->removeChild(this);
        }
        parent = newParent;
        if (parent) {
            parent->addChild(this);
        }
    }

    void addChild(Widget* child) {
        children.push_back(child);
    }

    void removeChild(Widget* child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
        }
    }

    Widget* getParent() const {
        return parent;
    }

    virtual std::string getType() const {
        return "Widget";
    }
};

class TabWidget : public Widget {
public:
    TabWidget() : Widget() {
        std::cout << "TabWidget created" << std::endl;
    }

    ~TabWidget() {
        std::cout << "TabWidget destroyed" << std::endl;
    }

    std::string getType() const override {
        return "TabWidget";
    }
};

class CalendarWidget : public Widget {
public:
    CalendarWidget() : Widget() {
        std::cout << "CalendarWidget created" << std::endl;
    }

    ~CalendarWidget() {
        std::cout << "CalendarWidget destroyed" << std::endl;
    }

    std::string getType() const override {
        return "CalendarWidget";
    }
};

int main() {
    TabWidget* tabWidget = new TabWidget();
    CalendarWidget* calendarWidget = new CalendarWidget();

    tabWidget->setParent(calendarWidget);

    std::cout << "Parent pointer in tabWidget: " << tabWidget->getParent() << std::endl;
    std::cout << "Poiter of calendarWidget " << calendarWidget << std::endl;
    std::cout << "Parent pointer in calendarWidget: " << calendarWidget->getParent() << std::endl;

    std::cout << "Type of tabWidget: " << tabWidget->getType() << std::endl;
    std::cout << "Type of calendarWidget: " << calendarWidget->getType() << std::endl;

    delete tabWidget;
    delete calendarWidget;

    return 0;
}






