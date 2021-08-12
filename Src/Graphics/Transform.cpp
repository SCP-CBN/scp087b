#include "Transform.h"

using namespace PGE;

void Transform::setPosition(const Vector3f& pos) { position = pos; invalid = true; }
void Transform::setRotation(const Vector3f& rot) { rotation = rot; invalid = true; }
void Transform::setScale(const Vector3f& scl) { scale = scl; invalid = true; }

const Vector3f& Transform::getPosition() const { return position; }
const Vector3f& Transform::getRotation() const { return rotation; }
const Vector3f& Transform::getScale() const { return scale; }

const Matrix4x4f& Transform::getModelMatrix() const {
	if (invalid) {
		modelMatrix = Matrix4x4f::constructWorldMat(position, rotation, scale);
		invalid = false;
	}
	return modelMatrix;
}
