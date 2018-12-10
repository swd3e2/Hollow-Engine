#include "Camera.h"

namespace Hollow {

	Camera::Camera()
	{
		this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->posVector = XMLoadFloat3(&this->pos);
		this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->rotVector = XMLoadFloat3(&this->rot);
		this->UpdateViewMatrix();
	}

	void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
		this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
		//this->projectionMatrix = XMMatrixOrthographicLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	const XMMATRIX & Camera::GetViewMatrix() const
	{
		return this->viewMatrix;
	}

	const XMMATRIX & Camera::GetProjectionMatrix() const
	{
		return this->projectionMatrix;
	}
	XMMATRIX Camera::GetViewMatrixCopy()
	{
		return this->viewMatrix;
	}

	XMMATRIX Camera::GetProjectionMatrixCopy()
	{
		return this->projectionMatrix;
		float fieldOfView, screenAspect;
		fieldOfView = (float)XM_PI / 2.0f;
		screenAspect = 1.0f;

		// Create the projection matrix for the light.
		return XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 1.0f, 100.0f);
	}

	const XMVECTOR & Camera::GetPositionVector() const
	{
		return this->posVector;
	}

	const XMFLOAT3 & Camera::GetPositionFloat3() const
	{
		return this->pos;
	}

	const XMVECTOR & Camera::GetRotationVector() const
	{
		return this->rotVector;
	}

	const XMFLOAT3 & Camera::GetRotationFloat3() const
	{
		return this->rot;
	}

	void Camera::SetPosition(const XMVECTOR & pos)
	{
		XMStoreFloat3(&this->pos, pos);
		this->posVector = pos;
		this->UpdateViewMatrix();
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		this->pos = XMFLOAT3(x, y, z);
		this->posVector = XMLoadFloat3(&this->pos);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustPosition(const XMVECTOR & pos)
	{
		this->posVector += pos;
		XMStoreFloat3(&this->pos, this->posVector);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustPosition(float x, float y, float z)
	{
		this->pos.x += x;
		this->pos.y += y;
		this->pos.z += z;
		this->posVector = XMLoadFloat3(&this->pos);
		this->UpdateViewMatrix();
	}

	void Camera::SetRotation(const XMVECTOR & rot)
	{
		this->rotVector = rot;
		XMStoreFloat3(&this->rot, rot);
		this->UpdateViewMatrix();
	}

	void Camera::SetRotation(float x, float y, float z)
	{
		this->rot = XMFLOAT3(x, y, z);
		this->rotVector = XMLoadFloat3(&this->rot);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustRotation(const XMVECTOR & rot)
	{
		this->rotVector += rot;
		XMStoreFloat3(&this->rot, this->rotVector);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustRotation(float x, float y, float z)
	{
		this->rot.x += x;
		this->rot.y += y;
		this->rot.z += z;
		this->rotVector = XMLoadFloat3(&this->rot);
		this->UpdateViewMatrix();
	}

	void Camera::SetLookAtPos(XMFLOAT3 lookAtPos)
	{
		//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
		if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
			return;

		lookAtPos.x = this->pos.x - lookAtPos.x;
		lookAtPos.y = this->pos.y - lookAtPos.y;
		lookAtPos.z = this->pos.z - lookAtPos.z;

		float pitch = 0.0f;
		if (lookAtPos.y != 0.0f)
		{
			const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
			pitch = atan(lookAtPos.y / distance);
		}

		float yaw = 0.0f;
		if (lookAtPos.x != 0.0f)
		{
			yaw = atan(lookAtPos.x / lookAtPos.z);
		}
		if (lookAtPos.z > 0)
			yaw += XM_PI;

		this->SetRotation(pitch, yaw, 0.0f);
	}

	void Camera::Update()
	{
		/*if (InputManager::KeyIsPressed(eKeyCodes::KEY_W) == true)
		{
			XMVECTOR vector = { 0, this->camTarget.m128_f32[1] - this->posVector.m128_f32[1] , 0.0f, 0.0f };

			AdjustPosition((vec_forward + vector * 1.5f) * cameraSpeed);
		}
		if (InputManager::KeyIsPressed(eKeyCodes::KEY_S) == true)
		{
			XMVECTOR vector = { 0, this->camTarget.m128_f32[1] - this->posVector.m128_f32[1] , 0.0f, 0.0f };
			AdjustPosition((vec_backward - vector * 1.5f) * cameraSpeed);
		}
		if (InputManager::KeyIsPressed(eKeyCodes::KEY_A) == true)
		{
			AdjustPosition(vec_left * cameraSpeed);
		}
		if (InputManager::KeyIsPressed(eKeyCodes::KEY_D) == true)
		{
			AdjustPosition(vec_right * cameraSpeed);
		}*/
	}

	XMVECTOR & Camera::getLookAtVector()
	{
		return this->camTarget;
	}

	/*bool Camera::RawMouseInput(const RawMouseEvent & arg)
	{
		if (InputManager::KeyIsPressed(eKeyCodes::KEY_RBUTTON))
		{
			AdjustRotation(arg.y * 0.001f, arg.x * 0.001f, 0.0f);
		}
		return false;
	}*/

	void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
	{
		//Calculate camera rotation matrix
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0);
		//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
		this->camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
		//Adjust cam target to be offset by the camera's current position
		this->camTarget += this->posVector;
		//Calculate up direction based on current rotation
		XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
		//Rebuild view matrix
		this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
		vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
		vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
		vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
		vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	}

}