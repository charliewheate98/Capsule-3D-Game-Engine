#ifndef __KINEMATIC_H__
#define __KINEMATIC_H__

#include <iostream>
#include <vector>
#include "Content.h"
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glew.h>
#include "Camera.h"

typedef unsigned int uint;

enum InterpolationType
{
	LINEAR
};

class Kinematic
{
private:

	Camera *m_playerCamera;

	uint m_interpolationType;
	uint m_pathSteps;
	uint m_pathIndex;
public:
	std::vector<Camera*> m_KeyFrames;
	std::vector<Camera*> m_cameraPath;

	bool m_play;

	inline Kinematic(Camera *_camera)
	{
		m_playerCamera = _camera;
		m_play = true;
		m_interpolationType = LINEAR;
		m_pathSteps = 100;
		m_pathIndex = 0;
	}
	inline ~Kinematic()
	{
		m_KeyFrames.clear();
		m_cameraPath.clear();
	}

	inline void AddKeyFrame(Camera* camera)
	{
		m_KeyFrames.push_back(camera);
	}

	inline void BuildPath(uint m_program, uint m_type, double &deltaTime)
	{
		m_interpolationType = m_type;

		if (m_type == LINEAR)
		{
			for (uint i = 0; i < m_KeyFrames.size() - 1; i++)
			{
				for (uint j = 0; j < m_pathSteps; j++)
				{
					glm::vec3 difference = glm::lerp(m_KeyFrames[i]->_trans._pos, m_KeyFrames[i + 1]->_trans._pos, (float)j / (float)(m_pathSteps - 1));
					glm::vec3 center = m_KeyFrames[i]->_trans._pos + difference;

					difference = glm::lerp(m_KeyFrames[i]->GetFront(), m_KeyFrames[i + 1]->GetFront(), (float)j / (float)(m_pathSteps - 1));
					glm::vec3 look = m_KeyFrames[i]->GetFront() + difference;

					difference = glm::lerp(m_KeyFrames[i]->GetUp(), m_KeyFrames[i + 1]->GetUp(), (float)j / (float)(m_pathSteps - 1));
					glm::vec3 up = m_KeyFrames[i]->GetUp() + difference;

					glm::vec3 r = glm::cross(up, look);
					glm::vec3 u = glm::cross(look, r * -1.f);

					Camera *m_camera = new Camera(m_program, center, 75.0f, 0.1f, 0.0f, 0.1f, 100.0f, (float)1920 / (float)1080);
					m_camera->Update(deltaTime);

					m_cameraPath.push_back(m_camera);
				}
			}
		}
	}

	inline void Update(double &deltaTime)
	{
		if (!m_cameraPath.empty())
		{
			if (m_play)
			{
				m_playerCamera = m_cameraPath[m_pathIndex];

				if (m_pathIndex < m_cameraPath.size() - 1)
					m_pathIndex++;
			}
		}

		m_playerCamera->UpdateProjectionMatrix();
		m_playerCamera->Update(deltaTime);
	}

	inline void Render()
	{
		for (unsigned int i = 0; i < m_KeyFrames.size() - 1; i++)
			m_KeyFrames[i]->Render();
		for (unsigned int j = 0; j < m_pathSteps; j++)
			m_cameraPath[j]->Render();
	}
};

#endif