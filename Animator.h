#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Object.h"
#include "Actor.h"
#include "Camera.h"

namespace Animator
{
	// linear interpolation between two points
	inline void lerp(glm::vec3& dest, glm::vec3& a, glm::vec3& b, const float t)
	{
		dest.x = a.x + (b.x - a.x) * t;
		dest.y = a.y + (b.y - a.y) * t;
		dest.z = a.z + (b.z - a.z) * t;
	}

	struct ControlPoint
	{
		glm::vec3 _pos;
	
		inline ControlPoint() = default;
		inline ControlPoint(glm::vec3 coords)
		{
			_pos = coords;
		}

		inline glm::vec3 GetPosition() { return _pos; }
		inline float GetX() { return _pos.x; }
		inline float GetY() { return _pos.y; }
		inline float GetZ() { return _pos.z; }
	};

	// evaluate a point on a bezier curve. t goes from 0 - 1.0 (normalised)
	inline void evaluate_bezier(glm::vec3& dest, std::vector<ControlPoint*> _control_points, const float t)
	{
		glm::vec3 ab;
		glm::vec3 bc;
		glm::vec3 cd;
		glm::vec3 abbc;
		glm::vec3 bccd;
		glm::vec3 euler;

		int test = _control_points.size();

		// 4 Control Points
		lerp(ab, _control_points[0]->GetPosition(), _control_points[1]->GetPosition(), t);
		lerp(bc, _control_points[1]->GetPosition(), _control_points[2]->GetPosition(), t);
		lerp(cd, _control_points[2]->GetPosition(), _control_points[3]->GetPosition(), t);

		lerp(abbc, ab, bc, t);
		lerp(bccd, bc, cd, t);

		lerp(dest, abbc, bccd, t);
	}

	// evaluate a point on a bezier curve. t goes from 0 - 1.0 (normalised)
	inline void evaluate_bezier_rot(glm::vec3& dest, std::vector<glm::vec3> _rot_points, const float t)
	{
		glm::vec3 ab;
		glm::vec3 bc;
		glm::vec3 cd;
		glm::vec3 abbc;
		glm::vec3 bccd;

		lerp(ab, _rot_points[0], _rot_points[1], t);
		lerp(bc, _rot_points[1], _rot_points[2], t);
		lerp(cd, _rot_points[2], _rot_points[3], t);

		lerp(abbc, ab, bc, t);
		lerp(bccd, bc, cd, t);

		lerp(dest, abbc, bccd, t);
	}

	class Bezier : public Object
	{
	private:
		bool _isPlaying;

		glm::vec3 _point_on_bezier;
		glm::vec3 _rotation_on_bezier;

		std::vector<glm::vec3> _pos_steps;
		std::vector<glm::vec3> _rot_steps;

		unsigned int _current_pos_step;
		unsigned int _current_rot_step_x;
		unsigned int _current_rot_step_y;

		std::vector<glm::vec3> _rotational_points;
		std::vector<ControlPoint*> _control_points;
	public:
		inline Bezier(std::string name)
		{
			SetName(name);	
			_isPlaying = true;
		}
		inline ~Bezier()
		{
			_control_points.clear();
		}

		inline void CreatePath()
		{
			for (unsigned int i = 0; i < 1000; i++)
			{
				float t = static_cast<float>(i) / 999.0f;

				evaluate_bezier(_point_on_bezier, _control_points, t);
				evaluate_bezier_rot(_rotation_on_bezier, _rotational_points, t);

				_pos_steps.push_back(_point_on_bezier);
				_rot_steps.push_back(_rotation_on_bezier);
			}

			_current_pos_step = 0;
			_current_rot_step_x = 0;
			_current_rot_step_y = 0;
		}

		inline void AddControlPoint(ControlPoint* _controlPoint)
		{
			_control_points.push_back(_controlPoint);
		}

		inline void AddRotationalFrame(glm::vec3 frame)
		{
			_rotational_points.push_back(frame);
		}

		inline uint16_t GetCurrentFrame() { return(0); }

		inline bool IsPlaying() { return _isPlaying; }

		inline void Play(double& delta, Actor* attachment)
		{
			// Check if the mover animation is currently playing and has not reached the final keyframe
			if (_current_pos_step != _pos_steps.size() - 1)
			{
				_isPlaying = true;

				// ROTATION ------------------------------------------------------------------------------------------
				glm::vec3 Rot(attachment->_trans._rot.x, attachment->_trans._rot.y, 0.0f);

				float rot_step_len_x = glm::length(_rot_steps[_current_rot_step_x + 1].x - _rot_steps[_current_rot_step_x].x);
				float rot_step_len_y = glm::length(_rot_steps[_current_rot_step_y + 1].y - _rot_steps[_current_rot_step_y].y);

				Rot.x = _rot_steps[_current_rot_step_x].x;
				Rot.y = _rot_steps[_current_rot_step_y].y;

				float time_x = glm::length((Rot.x - _rot_steps[_current_rot_step_x + 1].x) / rot_step_len_x);
				float time_y = glm::length((Rot.y - _rot_steps[_current_rot_step_y + 1].y) / rot_step_len_y);

				if (time_x >= 1.0f)
					_current_rot_step_x++;
				if (time_y >= 1.0f)
					_current_rot_step_y++;
				// -------------------------------------------------------------------------------------------------------

				// TRANSLATION ------------------------------------------------------------------------------------------
				glm::vec3 step_dir = glm::normalize(_pos_steps[_current_pos_step + 1] - _pos_steps[_current_pos_step]);
				float step_len = glm::length(_pos_steps[_current_pos_step + 1] - _pos_steps[_current_pos_step]);

				attachment->_trans._pos += step_len * step_dir * ((float)delta * 200.0f);

				if (attachment->GetObjectType() == MESH)
					attachment->UpdateModel();

				float t = glm::length((attachment->_trans._pos - _pos_steps[_current_pos_step + 1]) / step_len);

				if (t >= 1.0f)
					_current_pos_step++;
				// -------------------------------------------------------------------------------------------------------

				if (attachment->GetObjectType() == MESH)
				{
					attachment->SetModel(glm::rotate(Rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(Rot.y, glm::vec3(0.0f, 1.0f, 0.0f))
						* glm::translate(-_pos_steps[_current_pos_step]) * glm::scale(attachment->_trans._sca));
				}
			}
			else
			{
				_isPlaying = false;
			}
		}
		inline void Pause() {}
		inline void Rewind() {} //std::reverse();
		inline void Stop() {}
	};

	class Mover
	{
	private:
		uint32 _bezier_index;
		uint32 _current_bezier;

		std::vector<Bezier*> _bezier_chain;
	public:
		inline Mover()
		{
			_bezier_index   = 0;
			_current_bezier = 0;
		}

		inline ~Mover()
		{
			_bezier_chain.clear();
		}

		inline void addPath(std::vector<glm::vec3> positions, std::vector<glm::vec3> rotations)
		{
			_bezier_index = _bezier_index + 1;

			_bezier_chain.push_back(new Bezier("bezier_" + _bezier_index));

			_bezier_chain[_bezier_chain.size() - 1]->AddControlPoint(new ControlPoint(glm::vec3(positions[0].x, positions[0].y, positions[0].z)));
			_bezier_chain[_bezier_chain.size() - 1]->AddControlPoint(new ControlPoint(glm::vec3(positions[1].x, positions[1].y, positions[1].z)));
			_bezier_chain[_bezier_chain.size() - 1]->AddControlPoint(new ControlPoint(glm::vec3(positions[2].x, positions[2].y, positions[2].z)));
			_bezier_chain[_bezier_chain.size() - 1]->AddControlPoint(new ControlPoint(glm::vec3(positions[3].x, positions[3].y, positions[3].z)));

			_bezier_chain[_bezier_chain.size() - 1]->AddRotationalFrame(glm::vec3(rotations[0].x, rotations[0].y, rotations[0].z));
			_bezier_chain[_bezier_chain.size() - 1]->AddRotationalFrame(glm::vec3(rotations[1].x, rotations[1].y, rotations[1].z));
			_bezier_chain[_bezier_chain.size() - 1]->AddRotationalFrame(glm::vec3(rotations[2].x, rotations[2].y, rotations[2].z));
			_bezier_chain[_bezier_chain.size() - 1]->AddRotationalFrame(glm::vec3(rotations[3].x, rotations[3].y, rotations[3].z));

			_bezier_chain[_bezier_chain.size() - 1]->CreatePath();
		}

		inline void Play(double& delta, Actor* attachment)
		{
			if (_bezier_chain[_current_bezier]->IsPlaying() == false)
			{
				_current_bezier = _current_bezier + 1;
			} else 
				_bezier_chain[_current_bezier]->Play(delta, attachment);
		}
	};
};


#endif
