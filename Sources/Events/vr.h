#pragma once
#include "openvr.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

struct VRData {
	vr::IVRSystem *m_pHMD;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
	char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];
	string m_strPoseClasses;

	glm::mat4 m_mat4HMDPose;
	glm::mat4 m_mat4eyePosLeft;
	glm::mat4 m_mat4eyePosRight;

	glm::mat4 m_mat4ProjectionCenter;
	glm::mat4 m_mat4ProjectionLeft;
	glm::mat4 m_mat4ProjectionRight;

	struct FramebufferDesc
	{
		GLuint m_nDepthBufferId;
		GLuint m_nRenderTextureId;
		GLuint m_nRenderFramebufferId;
		GLuint m_nResolveTextureId;
		GLuint m_nResolveFramebufferId;
	};
	FramebufferDesc leftEyeDesc;
	FramebufferDesc rightEyeDesc;
	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;

	vr::VRControllerState_t rightControllerPreviousState;
	vr::VRControllerState_t leftControllerPreviousState;
	vr::VRControllerState_t rightControllerState;
	vr::VRControllerState_t leftControllerState;
	glm::mat4 rightMatrix;
	glm::mat4 leftMatrix;
};

extern VRData vrd; 

inline glm::mat4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	glm::mat4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}

inline glm::mat4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
	if (!vrd.m_pHMD)
		return glm::mat4();

	vr::HmdMatrix34_t matEyeRight = vrd.m_pHMD->GetEyeToHeadTransform(nEye);
	glm::mat4 matrixObj(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
	);

	return glm::inverse(matrixObj);
}

inline glm::mat4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
{
	if (!vrd.m_pHMD)
		return glm::mat4();

	vr::HmdMatrix44_t mat = vrd.m_pHMD->GetProjectionMatrix(nEye, .05f, 30.f); // near far respectively.

	return glm::mat4(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
	);
}

inline glm::mat4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
	glm::mat4 matMVP;
	if (nEye == vr::Eye_Left)
	{
		matMVP = vrd.m_mat4ProjectionLeft * vrd.m_mat4eyePosLeft * vrd.m_mat4HMDPose;
	}
	else if (nEye == vr::Eye_Right)
	{
		matMVP = vrd.m_mat4ProjectionRight * vrd.m_mat4eyePosRight *  vrd.m_mat4HMDPose;
	}

	//matMVP = glm::translate(matMVP, glm::vec3(0.0, 1.0, 0.0));
	//matMVP = glm::scale(matMVP, glm::vec3(2.0, 2.0, 2.0));
	return matMVP;
}

inline void SetupCameras()
{
	vrd.m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
	vrd.m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
	vrd.m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
	vrd.m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}

inline bool CreateFrameBuffer(int nWidth, int nHeight, VRData::FramebufferDesc &framebufferDesc)
{
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

inline bool SetupStereoRenderTargets()
{
	if (!vrd.m_pHMD)
		return false;

	vrd.m_pHMD->GetRecommendedRenderTargetSize(&vrd.m_nRenderWidth, &vrd.m_nRenderHeight);

	CreateFrameBuffer(vrd.m_nRenderWidth, vrd.m_nRenderHeight, vrd.leftEyeDesc);
	CreateFrameBuffer(vrd.m_nRenderWidth, vrd.m_nRenderHeight, vrd.rightEyeDesc);

	return true;
}

inline void updateVRData() {
	if (!vrd.m_pHMD)
		return;

	vr::VRCompositor()->WaitGetPoses(vrd.m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	int m_iValidPoseCount = 0; //Not sure  what this is used for...
	vrd.m_strPoseClasses = "";
	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (vrd.m_rTrackedDevicePose[nDevice].bPoseIsValid)
		{
			m_iValidPoseCount++;
			vrd.m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(vrd.m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			if (vrd.m_rDevClassChar[nDevice] == 0)
			{
				switch (vrd.m_pHMD->GetTrackedDeviceClass(nDevice))
				{
				case vr::TrackedDeviceClass_Controller:        vrd.m_rDevClassChar[nDevice] = 'C'; break;
				case vr::TrackedDeviceClass_HMD:               vrd.m_rDevClassChar[nDevice] = 'H'; break;
				case vr::TrackedDeviceClass_Invalid:           vrd.m_rDevClassChar[nDevice] = 'I'; break;
				case vr::TrackedDeviceClass_GenericTracker:    vrd.m_rDevClassChar[nDevice] = 'G'; break;
				case vr::TrackedDeviceClass_TrackingReference: vrd.m_rDevClassChar[nDevice] = 'T'; break;
				default:                                       vrd.m_rDevClassChar[nDevice] = '?'; break;
				}
			}
			vrd.m_strPoseClasses += vrd.m_rDevClassChar[nDevice];
		}
	}

	if (vrd.m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		vrd.m_mat4HMDPose = vrd.m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
		vrd.m_mat4HMDPose = glm::inverse(vrd.m_mat4HMDPose);
	}
}

inline void ProcessVREvent(const vr::VREvent_t & event)
{
	switch (event.eventType)
	{
	case vr::VREvent_TrackedDeviceActivated:
	{
		//SetupRenderModelForTrackedDevice(event.trackedDeviceIndex); //TODO
		printf("Device %u attached. Setting up render model.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceDeactivated:
	{
		printf("Device %u detached.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceUpdated:
	{
		printf("Device %u updated.\n", event.trackedDeviceIndex);
	}
	break;
	}
}

inline void receiveVRInput() {
	/* If the headset isn't loaded yet, don't pull VR events */
	if (vrd.m_pHMD == NULL) return;
		
	//Process SteamVR events
	vr::VREvent_t event;
	while (vrd.m_pHMD->PollNextEvent(&event, sizeof(event))) {
		ProcessVREvent(event);
	}

	// Process SteamVR controller state
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	{
		vr::TrackedPropertyError error;
		vr::VRControllerState_t state;
		if (vrd.m_pHMD->GetControllerState(unDevice, &state, sizeof(state))) {
			vr::ETrackedControllerRole role = vrd.m_pHMD->GetControllerRoleForTrackedDeviceIndex(unDevice);

			if (role == vr::ETrackedControllerRole::TrackedControllerRole_RightHand)
			{
				vrd.rightControllerPreviousState = vrd.rightControllerState;
				vrd.rightControllerState = state;
				glm::mat4 matrix = ConvertSteamVRMatrixToMatrix4(vrd.m_rTrackedDevicePose[unDevice].mDeviceToAbsoluteTracking);
				vrd.rightMatrix = matrix;// glm::inverse(matrix);
			}
			else if (role == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand)
			{
				vrd.leftControllerPreviousState = vrd.leftControllerState;
				vrd.leftControllerState = state;
				glm::mat4 matrix = ConvertSteamVRMatrixToMatrix4(vrd.m_rTrackedDevicePose[unDevice].mDeviceToAbsoluteTracking);
				vrd.leftMatrix = matrix;// glm::inverse(matrix);
			}
		}
	}
}