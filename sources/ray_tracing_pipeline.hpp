#ifndef RAY_TRACING_PIPELINE_HPP
#define RAY_TRACING_PIPELINE_HPP

#include <core/unique_ptr.hpp>
#include <core/array.hpp>
#include <core/string.hpp>
#include <core/math/vector3.hpp>
#include <core/math/vector4.hpp>
#include <graphics/api/texture.hpp>
#include <graphics/api/semaphore.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/descriptor_set.hpp>
#include <graphics/api/compute_pipeline.hpp>
#include <graphics/api/shader.hpp>
#include "storage_buffer_list.hpp"

struct alignas(sizeof(Vector4f)) Sphere{
    Vector3f Position;
    float    Radius;
    Vector4f Color;
};

class RayTracingPipeline {
private:
    static const String s_TraceComputeSources;

    UniquePtr<CommandPool> m_Pool{
        CommandPool::Create()
    };

    UniquePtr<CommandBuffer, CommandBufferDeleter> m_CmdBuffer{
        m_Pool->Alloc(), {m_Pool.Get()}
    };

    const Array<ShaderBinding, 3> m_Bindings {
        ShaderBinding{
            0,
            1,
            ShaderBindingType::StorageBuffer,
            ShaderStageBits::Compute
        },
        ShaderBinding{
            1,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Compute
        },
        ShaderBinding{
            2,
            1,
            ShaderBindingType::StorageTexture,
            ShaderStageBits::Compute
        }
    };

    UniquePtr<DescriptorSetLayout> m_SetLayout{
        DescriptorSetLayout::Create(m_Bindings)
    };

    UniquePtr<DescriptorSetPool> m_SetPool{
        DescriptorSetPool::Create({1, m_SetLayout.Get()})
    };

    UniquePtr<DescriptorSet, DescriptorSetDeleter> m_Set{
        m_SetPool->Alloc(), {m_SetPool.Get()}
    };

    UniquePtr<ComputePipeline> m_ComputePipeline{
        ComputePipeline::Create({
            Shader::Create(ShaderLang::GLSL, ShaderStageBits::Compute, s_TraceComputeSources),
            m_SetLayout.Get()
        })
    };

public:
    void BindSpheresBuffer(const StorageBufferList<Sphere> &spheres_buffer);

    void BindCameraBuffer(const Buffer *camera_buffer);

	void Dispatch(const Texture2D &output, const Semaphore &wait, const Semaphore &signal);
};

#endif//RAY_TRACING_PIPELINE_HPP