#ifndef B_TRANSFORM_H_INCLUDED
#define B_TRANSFORM_H_INCLUDED

#include <PGE/Math/Matrix.h>

class Transform {
	public:
		Transform() = default;

		void setPosition(const PGE::Vector3f& pos);
		void setRotation(const PGE::Vector3f& rot);
		void setScale(const PGE::Vector3f& scl);

		const PGE::Vector3f& getPosition() const;
		const PGE::Vector3f& getRotation() const;
		const PGE::Vector3f& getScale() const;

		const PGE::Matrix4x4f& getModelMatrix() const;
		
	private:
		mutable bool invalid = true;
		mutable PGE::Matrix4x4f modelMatrix;

		PGE::Vector3f position;
		PGE::Vector3f rotation;
		PGE::Vector3f scale = PGE::Vectors::ONE3F;
};

#endif // B_TRANSFORM_H_INCLUDED
