#pragma once

#include <stack>
#include <assert.h>
#include <new>
#include <stdio.h>


template<int NBytes, int NObjects>
class CPageAllocator
{
    struct SMemoryPage
    {
        char Data[NBytes];
    };

public:
    CPageAllocator()
    {

        for (int i = 0; i < NObjects; i++)
        {
            m_FreePages.push(&m_Pages[i]);
        }

    }

    template<class T>
    T* NewObject()
    {
        //printf("Verifying object size...");
        static_assert(sizeof(T) <= NBytes, "Object too large for page"); //verify requiered size not bigger than page size
        assert(sizeof(T) <= NBytes);
        if (sizeof(T) > NBytes) return nullptr;
        //printf("Object is not too big. Verifying free pages...");
        // if no more space: cry
        if (m_FreePages.empty()) return nullptr; // verify there are still free pages
        //printf("Free page found. Creating object in page...");
        T* newObj = new(m_FreePages.top()->Data) T();
        m_FreePages.pop();
        //printf("Object has been created within the page");
        return newObj;
    }

    template<class T>
    void FreeObject(T* InObject)
    {
        if (InObject == nullptr) return; //verify object is not null
        InObject->~T(); //call destructor
        m_FreePages.push(reinterpret_cast<SMemoryPage*>(InObject)); //reassign page as free
    }

    // Getters for ImGui displaying of the allocator
    int GetFreeCount() const { return m_FreePages.size(); }
    int GetUsedCount() const { return NObjects - m_FreePages.size(); }
    int GetTotalCount() const { return NObjects; }

private:
    SMemoryPage m_Pages[NObjects];        // the whole chunk of space
    std::stack<SMemoryPage*> m_FreePages; // the free spaces within the chunk
};
