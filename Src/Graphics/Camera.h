#ifndef B_CAMERA_H_INCLUDED
#define B_CAMERA_H_INCLUDED

#include <PGE/Graphics/Shader.h>

class Camera {
	public:
		Camera(float width, float height, float fov);
		
		void addShader(PGE::Shader& sh);

		const PGE::Vector3f& getPosition() const;
		void setPosition(const PGE::Vector3f& pos);

		const PGE::Vector3f& getRotation() const;
		void setRotation(const PGE::Vector3f& rot);

		const PGE::Vector3f& getForward() const;
		const PGE::Vector3f& getUpward() const;

		float getFOV() const;
		void setFOV(float fov);

		void applyTransforms();

	private:
		static constexpr PGE::Vector3f STANDARD_UP = PGE::Vector3f(0.f, 1.f, 0.f);
		static constexpr PGE::Vector3f STANDARD_FORWARD = PGE::Vector3f(0.f, 0.f, 1.f);

		mutable bool invalidView = true;
		mutable bool invalidProj = true;
		mutable bool invalidPos = true;

		float fieldOfView;
		const float aspectRatio;

		PGE::ReferenceVector<PGE::Shader::Constant> projConstants;
		PGE::ReferenceVector<PGE::Shader::Constant> viewConstants;
		PGE::ReferenceVector<PGE::Shader::Constant> posConstants;

		PGE::Vector3f position;
		PGE::Vector3f rotation;

		PGE::Vector3f up = STANDARD_UP;
		PGE::Vector3f forward = STANDARD_FORWARD;
};

#endif // B_CAMERA_H_INCLUDED
