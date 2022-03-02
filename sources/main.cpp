#include <core/print.hpp>
#include <core/span.hpp>
#include <core/ranges.hpp>
#include <core/string_view.hpp>
#include <core/string.hpp>
#include <core/unique_ptr.hpp>
#include <graphics/render_window.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/texture.hpp>
#include <graphics/api/compute_pipeline.hpp>
#include <graphics/api/gpu.hpp>
#include <2d/rect_renderer.hpp>
#include <core/print.hpp>
#include "storage_buffer_list.hpp"
#include "utils.hpp"

const String s_TraceShaderSources = ReadEntireFile("../../../shaders/trace.glsl");

class Application {
private:
    RenderWindow m_Window{1280, 720, "Compute !"};
    RectRenderer m_RectRenderer{m_Window.FramebufferPass()};
    
    UniquePtr<CommandPool> m_Pool{
        CommandPool::Create()
    };

    UniquePtr<CommandBuffer, CommandBufferDeleter> m_CmdBuffer{
        m_Pool->Alloc(), {m_Pool.Get()}
    };

    StorageBufferList<Vector2f> m_SpheresStorageBuffer;

    UniquePtr<Texture2D> m_StorageTexture{
        Texture2D::Create(
            m_Window.Size(),
            TextureFormat::RGBA8, 
            TextureUsageBits::Sampled | TextureUsageBits::Storage, 
            TextureLayout::General
        )
    };

    const Array<ShaderBinding, 2> m_Bindings {
        ShaderBinding{
            0,
            1,
            ShaderBindingType::StorageBuffer,
            ShaderStageBits::Compute
        },
        ShaderBinding{
            1,
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
            Shader::Create(ShaderLang::GLSL, ShaderStageBits::Compute, s_TraceShaderSources),
            m_SetLayout.Get()
        })
    };
public:
    Application() {
        m_Window.SetEventsHanlder({this, &Application::OnEvent});

        m_StorageTexture->Copy(Image(m_StorageTexture->Size().x, m_StorageTexture->Size().y, Color::Mint));



        for(int i = 0; i < 20; i++)
            m_SpheresStorageBuffer.Add(Vector2f(rand() % m_StorageTexture->Size().x, rand() % m_StorageTexture->Size().y ));
        m_SpheresStorageBuffer.UploadToGPU();
    }

    void Run() {
        Semaphore acq, comp, prs;

        while(m_Window.IsOpen()) {
            m_Window.AcquireNextFramebuffer(&acq);

            m_Set->UpdateStorageBufferBinding (0, 0, m_SpheresStorageBuffer);
            m_Set->UpdateStorageTextureBinding(1, 0, m_StorageTexture.Get());
            m_CmdBuffer->Begin();
            m_CmdBuffer->Bind(m_ComputePipeline.Get());
            m_CmdBuffer->Bind(m_Set.Get());
            m_CmdBuffer->Dispatch(m_StorageTexture->Size().x, m_StorageTexture->Size().y);
            m_CmdBuffer->End();

            GPU::Execute(m_CmdBuffer.Get(), acq, comp);

            m_RectRenderer.BeginDrawing(&comp, m_Window.CurrentFramebuffer());
            m_RectRenderer.DrawRect({0,0}, m_StorageTexture->Size(), Color::White, m_StorageTexture.Get());
            m_RectRenderer.EndDrawing(&prs);

            m_Window.PresentCurrentFramebuffer(&prs);

            m_Window.DispatchEvents();
        }
    }

    void OnEvent(const Event& e) {
        if(e.Type == EventType::WindowClose)
            m_Window.Close();
    }
};

int StraitXMain(Span<const char*> args){
    Application().Run();
    return 0;
}
