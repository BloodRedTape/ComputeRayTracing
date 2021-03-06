#ifndef STORAGE_BUFFER_LIST_HPP
#define STORAGE_BUFFER_LIST_HPP

#include <core/unique_ptr.hpp>
#include <core/list.hpp>
#include <core/math/functions.hpp>
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
        int count_size = sizeof(count);
        int positions_offset = Math::AlignUp<size_t>(count_size, alignof(Type));
        int positions_size = count * sizeof(Type);

        int required_size = positions_offset + positions_size;
        
        if (m_StorageBuffer->Size() < required_size)
            m_StorageBuffer->Realloc(required_size);

        m_StorageBuffer->Copy(&count,           count_size, 0);
        m_StorageBuffer->Copy(&(*this)[0],  positions_size, positions_offset);
    }

    operator const Buffer*()const {
        return m_StorageBuffer.Get();
    }
};

#endif//STORAGE_BUFFER_LIST_HPP