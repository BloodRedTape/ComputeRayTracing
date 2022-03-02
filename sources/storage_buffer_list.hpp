#ifndef STORAGE_BUFFER_LIST_HPP
#define STORAGE_BUFFER_LIST_HPP

#include <core/unique_ptr.hpp>
#include <core/list.hpp>
#include <graphics/api/buffer.hpp>

template<typename Type>
class StorageBufferList: public List<Type>{
private:
    UniquePtr<Buffer> m_StorageBuffer{
        Buffer::Create(
            sizeof(int),
            BufferMemoryType::DynamicVRAM,
            BufferUsageBits::StorageBuffer | BufferUsageBits::TransferDestination
        )
    };
public:
    void UploadToGPU() {
        int count = Size();
        const int count_size = sizeof(count);
        const int positions_size = count * sizeof(Type);

        const int required_size = count_size + positions_size;
        
        if (m_StorageBuffer->Size() < required_size)
            m_StorageBuffer->Realloc(required_size);

        m_StorageBuffer->Copy(&count,           count_size,     0);
        m_StorageBuffer->Copy(&(*this)[0],  positions_size, 2*count_size);
    }

    operator const Buffer*()const {
        return m_StorageBuffer.Get();
    }
};

#endif//STORAGE_BUFFER_LIST_HPP