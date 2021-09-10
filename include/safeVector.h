#ifndef SAFEVECTOR_H
#define SAVEVECTOR_H
#include <vector>
#include <mutex>
template <typename T>
class safeVector:public std::vector<T>
{
private:
    std::mutex lock;
public:
    safeVector(/* args */):std::vector<T>()
    {
        
    };
    void push_back(T &value)
    {
        std::lock_guard<std::mutex> lck(lock);
        std::vector<T>::push_back(value);   
    }
    void push_back(T &&value)
    {
        std::lock_guard<std::mutex> lck(lock);
        std::vector<T>::push_back(value);
    }
    size_t size()
    {
        std::lock_guard<std::mutex> lck(lock);
        return std::vector<T>::size();
    }
    typename std::vector<T>::iterator erase(typename std::vector<T>::const_iterator position)
    {
        std::lock_guard<std::mutex> lck(lock);
        return std::vector<T>::erase(position);
    }
    ~safeVector()
    {
        std::vector<T>::~vector();
    };
};
#endif