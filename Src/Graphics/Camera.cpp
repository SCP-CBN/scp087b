#include "Camera.h"

#include <PGE/Math/Math.h>

using namespace PGE;

Camera::Camera(float width, float height, float fov) : aspectRatio(width / height) {
	fieldOfView = Math::degToRad(fov);
}

void Camera::addShader(Shader& sh) {
	projConstants.emplace_back(sh.getVertexShaderConstant("projectionMatrix"));
	viewConstants.emplace_back(sh.getVertexShaderConstant("viewMatrix"));
}

const Vector3f& Camera::getPosition() const {
	return position;
}

void Camera::setPosition(const Vector3f& pos) {
	position = pos;
	invalidView = true;
}

const Vector3f& Camera::getRotation() const {
	return rotation;
}

void Camera::setRotation(const Vector3f& rot) {
	rotation = rot;
	Matrix4x4f mat = Matrix4x4f::rotate(rot);
	up = mat.transform(STANDARD_UP);
	forward = mat.transform(STANDARD_FORWARD);
	invalidView = true;
}

const Vector3f& Camera::getForward() const {
	return forward;
}

const Vector3f& Camera::getUpward() const {
	return up;
}

float Camera::getFOV() const {
	return fieldOfView;
}

void Camera::setFOV(float fov) {
	fieldOfView = fov;
	invalidProj = true;
}

void Camera::applyTransforms() const {
	Matrix4x4f mat;

	if (invalidProj) {
		mat = Matrix4x4f::constructPerspectiveMat(fieldOfView, aspectRatio, 0.01f, 2000.f);
		for (Shader::Constant& c : projConstants) { c.setValue(mat); }
		invalidProj = false;
	}

	if (invalidView) {
		mat = Matrix4x4f::constructViewMat(position, forward, up);
		for (Shader::Constant& c : viewConstants) { c.setValue(mat); }
		invalidView = false;
	}
}
