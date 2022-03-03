#include <core/print.hpp>
#include <core/unique_ptr.hpp>
#include <graphics/render_window.hpp>
#include <2d/rect_renderer.hpp>
#include <core/print.hpp>
#include "storage_buffer_list.hpp"
#include "ray_tracing_pipeline.hpp"
#include "utils.hpp"

class Application {
private:
    RenderWindow m_Window{1280, 720, "Compute !"};
    RectRenderer m_RectRenderer{m_Window.FramebufferPass()};
    
    StorageBufferList<Sphere> m_SpheresStorageBuffer;

    UniquePtr<Texture2D> m_StorageTexture{
        Texture2D::Create(
            m_Window.Size(),
            TextureFormat::RGBA8, 
            TextureUsageBits::Sampled | TextureUsageBits::Storage, 
            TextureLayout::General
        )
    };

    RayTracingPipeline m_Pipeline;
public:
    Application() {
        m_Window.SetEventsHanlder({this, &Application::OnEvent});

        //for(int i = 0; i < 20; i++)
        //    m_SpheresStorageBuffer.Add({Vector2f(rand() % m_StorageTexture->Size().x, rand() % m_StorageTexture->Size().y )});
        m_SpheresStorageBuffer.Add({Vector3f( 0.5f, 0.f, 1.f), 0.5f, Color::Blue});
        m_SpheresStorageBuffer.Add({Vector3f(-0.5f, 0.f, 1.f), 0.5f, Color::Red});
        m_SpheresStorageBuffer.Add({Vector3f( 0.0f, -100.5f, 1.f), 100.f, Color::Yellow});
        m_SpheresStorageBuffer.UploadToGPU();
    }

    void Run() {
        Semaphore acq, comp, prs;

        while(m_Window.IsOpen()) {
            m_Window.AcquireNextFramebuffer(&acq);

            m_Pipeline.BindSpheresBuffer(m_SpheresStorageBuffer);
            m_Pipeline.Dispatch(*m_StorageTexture, acq, comp);

            m_RectRenderer.BeginDrawing(&comp, m_Window.CurrentFramebuffer());
            m_RectRenderer.DrawRect(Vector2s(0, m_StorageTexture->Size().y), Vector2s(m_StorageTexture->Size() * Vector2u(1, -1)), Color::White, m_StorageTexture.Get());
            m_RectRenderer.EndDrawing(&prs);

            m_Window.PresentCurrentFramebuffer(&prs);

            m_Window.DispatchEvents();
        }
    }

    void OnEvent(const Event& e) {
        if(e.Type == EventType::WindowClose)
            m_Window.Close();

        if(e.Type == EventType::KeyPress)
            if(e.KeyPress.KeyCode == Key::Q)
                m_Window.Close();
    }
};

int StraitXMain(Span<const char*> args){
    Application().Run();
    return 0;
}
