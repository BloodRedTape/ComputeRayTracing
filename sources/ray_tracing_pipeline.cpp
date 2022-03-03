#include "ray_tracing_pipeline.hpp"
#include "utils.hpp"
#include <graphics/api/gpu.hpp>

const String RayTracingPipeline::s_TraceComputeSources = ReadEntireFile("shaders/trace.glsl.comp");

void RayTracingPipeline::BindSpheresBuffer(const StorageBufferList<Sphere> &spheres_buffer) {
    m_Set->UpdateStorageBufferBinding(0, 0, spheres_buffer);
}
void RayTracingPipeline::BindCameraBuffer(const Buffer* camera_buffer) {
    m_Set->UpdateUniformBinding(1, 0, camera_buffer);
}

void RayTracingPipeline::Dispatch(const Texture2D &output, const Semaphore &wait, const Semaphore &signal){
    m_Set->UpdateStorageTextureBinding(2, 0, &output);
    m_CmdBuffer->Begin();
    m_CmdBuffer->Bind(m_ComputePipeline.Get());
    m_CmdBuffer->Bind(m_Set.Get());
    m_CmdBuffer->Dispatch(output.Size().x, output.Size().y);
    m_CmdBuffer->End();

    GPU::Execute(m_CmdBuffer.Get(), wait, signal);
}
