#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <core/math/matrix4.hpp>
#include <core/math/transform.hpp>
#include <core/math/functions.hpp>
#include <core/unique_ptr.hpp>
#include <core/math/vector3.hpp>
#include <graphics/api/buffer.hpp>

class Camera {
private:
	struct UniformLayout{
		Matrix4f CameraDirection;
		Vector3f CameraPosition;
	};

	UniquePtr<Buffer> m_UniformBuffer{
		Buffer::Create(
			sizeof(UniformLayout), 
			BufferMemoryType::DynamicVRAM, 
			BufferUsageBits::TransferDestination | BufferUsageBits::UniformBuffer
		)
	};

	Vector3f m_Position{0, 0, 0};
public:
	float RotationY = 0;
	float RotationX = 0;
	
	operator const Buffer *()const{
		return m_UniformBuffer.Get();
	}

	void Move(Vector3f offset) {
		using namespace Math;
		using namespace Transform;
		Vector4f rotated_offset = RotateY<float>(Rad(-RotationY)) * Vector4f(offset.x, offset.y, offset.z, 1.0);

		m_Position += {rotated_offset.x, rotated_offset.y, rotated_offset.z};
	}

	void UploadToGPU(){
		using namespace Math;
		using namespace Transform;

		Matrix4f direction = RotateY<float>(Rad(-RotationY)) * RotateX<float>(Rad(RotationX));
		UniformLayout data{
			direction,
			m_Position
		};

		m_UniformBuffer->Copy(&data, sizeof(data));
	}
};

#endif//CAMERA_HPP