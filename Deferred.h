// Start of class
#ifndef __DEFERRED_H__
#define __DEFERRED_H__

#include <glm/gtx/quaternion.hpp>

#include "Engine/Rect.h"	// Get the rect object
#include "Engine/GBufferPass.h"	// Include the gbuffer
#include "Engine/GeometryPass.h"	// Get geometry pass
#include "Engine/LightPass.h"	// Get light pass
#include "Engine/SsaoPass.h"	// Get ssao pass
#include "Engine/FinalGatherPass.h"
#include "Engine/CPicker.h" // Get bloom pass
#include "Engine/PBR.h"
#include "Engine/ParticleSystem.h"
#include "Engine/Inventory.h"
#include "Manipulators.h"

// the different render views
#define VT_REALISTIC 	0 	// Final view
#define VT_UNLIT 		1 	// Albedo view
#define VT_WIRE 		2 	// Wire view
#define VT_DETAIL 		3 	// Normal detail view
#define VT_SSAO 		4 	// SSAO view

#define BLOOM_INTENSITY (float)0.6f		// Default bloom intensity

// This class contains all deffered rendering passes
class Deferred
{
public:
	static std::vector<ShaderProgram*>	shaders;	// The list of shader programs
	static std::vector<Pass*>			passes;		// The list of rendering passes
	static std::vector<PostFX*>			post_effects; // The list of post processing effects

	static CPicker* _picker;

	// TEMP
	static Fbo*							_fbo_final;		// The final frame buffer
	//

	static Inventory* _inv;

	static PBR::IBL* _ibl;

	static unsigned int current_step;

	static glm::vec3 a;
	static glm::vec3 b;
	static glm::vec3 c;
	static glm::vec3 d;
	static glm::vec3 point_on_bezier;

	static std::vector<glm::vec3> _steps;

	static float deltaTime;

	static int8_t						_current_view_t;	// The current view type
	static GLuint						_u_view_type;		// Uniform for adjusting view types
	static GLuint						_final_textures[4];		// Both final result and bright colour textures
	static GLuint						_bloom_intensity;	// Bloom intensity value
public:
	// Initialise all deferred passes
	inline static void Initialise(size_t width, size_t height)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// Initialise screen rectangles
		_screen_rect = new Rect((double)width, (double)height, 1.0f, true);

		// Initialise geometry pass shader
		shaders.push_back(new ShaderProgram);
		shaders[0]->AddShaderAttachment("GBuffer.v", GL_VERTEX_SHADER);	// Geometry vertex
		shaders[0]->AddShaderAttachment("GBuffer.f", GL_FRAGMENT_SHADER);	// Geometry fragment
		shaders[0]->LinkProgram();		// Link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[1]->AddShaderAttachment("skinning.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[1]->AddShaderAttachment("skinning.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[1]->LinkProgram();		// link both shader attachments

		passes.push_back(new GeometryPass({ shaders[0]->GetProgram(), shaders[1]->GetProgram() }));		// Initialise the geometry pass

		// Initialise light pass shader
		shaders.push_back(new ShaderProgram);
		shaders[2]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// Screen rect shader
		shaders[2]->AddShaderAttachment("Light.f", GL_FRAGMENT_SHADER);	// Light fragment
		shaders[2]->LinkProgram();		// Link both shader attachments
		passes.push_back(new LightPass(shaders[2]->GetProgram()));	// Initialise the light pass

		// Get global uniforms from light shader
		_u_view_type = glGetUniformLocation(shaders[2]->GetProgram(), "view_type");		// Buffer view type

		// Initialise GBuffer pass
		passes.push_back(new GBufferPass(shaders[2]->GetProgram(), width, height));		// Initialise the gbuffer

		// Initialise ssao pass shaders
		shaders.push_back(new ShaderProgram);
		shaders[3]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[3]->AddShaderAttachment("ssao.f", GL_FRAGMENT_SHADER);	// ssao fragment
		shaders[3]->LinkProgram();		// link both shader attachments
		shaders.push_back(new ShaderProgram);
		shaders[4]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[4]->AddShaderAttachment("ssaoblur.f", GL_FRAGMENT_SHADER);	// ssaoblur fragment
		shaders[4]->LinkProgram();		// link both shader attachments
		passes.push_back(new SsaoPass({ shaders[3]->GetProgram(), shaders[4]->GetProgram() }, width, height, 32));	// initialise the light pass

		shaders.push_back(new ShaderProgram);
		shaders[5]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[5]->AddShaderAttachment("GaussH.f", GL_FRAGMENT_SHADER);	// guassian blur fragment
		shaders[5]->LinkProgram();		// link both shader attachments
		shaders.push_back(new ShaderProgram);
		shaders[6]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[6]->AddShaderAttachment("GaussV.f", GL_FRAGMENT_SHADER);	// guassian blur fragment
		shaders[6]->LinkProgram();		// link both shader attachments
		shaders.push_back(new ShaderProgram);
		shaders[7]->AddShaderAttachment("firstpass.v", GL_VERTEX_SHADER);	// firstpass vertex shader
		shaders[7]->AddShaderAttachment("firstpass.f", GL_FRAGMENT_SHADER);	// firstpass fragment
		shaders[7]->LinkProgram();		// link both shader attachments

		// initialise the final gather 
		shaders.push_back(new ShaderProgram);
		shaders[8]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[8]->AddShaderAttachment("finalgather.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[8]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[9]->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[9]->AddShaderAttachment("sharpen.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[9]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[10]->AddShaderAttachment("colourID.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[10]->AddShaderAttachment("colourID.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[10]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[11]->AddShaderAttachment("light_scatter.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[11]->AddShaderAttachment("light_scatter.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[11]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[12]->AddShaderAttachment("radial_blur.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[12]->AddShaderAttachment("radial_blur.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[12]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[13]->AddShaderAttachment("HDR_Sky.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[13]->AddShaderAttachment("HDR_Sky.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[13]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[14]->AddShaderAttachment("EQU_HDR_Sky.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[14]->AddShaderAttachment("EQU_HDR_Sky.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[14]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[15]->AddShaderAttachment("convolution.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[15]->AddShaderAttachment("convolution.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[15]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[16]->AddShaderAttachment("convolution.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[16]->AddShaderAttachment("prefilter.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[16]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[17]->AddShaderAttachment("brdf.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[17]->AddShaderAttachment("brdf.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[17]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[18]->AddShaderAttachment("Particle.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[18]->AddShaderAttachment("Particle.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[18]->LinkProgram();		// link both shader attachments

		shaders.push_back(new ShaderProgram);
		shaders[19]->AddShaderAttachment("UI.v", GL_VERTEX_SHADER);	// screen rect shader
		shaders[19]->AddShaderAttachment("UI.f", GL_FRAGMENT_SHADER);	// final gather fragment
		shaders[19]->LinkProgram();		// link both shader attachments

		Manipulators::Create(shaders[10]->GetProgram());

		post_effects.push_back(new Bloom({ shaders[5]->GetProgram(), shaders[6]->GetProgram() }, width, height, BLOOM_INTENSITY));
		post_effects.push_back(new ColorCorrection(shaders[8]->GetProgram(), 1.5f, -0.02f));
		post_effects.push_back(new Shadowmapping({ shaders[7]->GetProgram(), shaders[5]->GetProgram(), shaders[6]->GetProgram() }, SHADOW_QUALITY));
		post_effects.push_back(new VolumetricLightPass({ shaders[11]->GetProgram(), shaders[12]->GetProgram(), shaders[5]->GetProgram(), shaders[6]->GetProgram() }, 1920, 1080));

		_picker = new CPicker(shaders[10]->GetProgram());

		_fbo_final = new Fbo(width, height, { new FboAttachment(width, height, GL_RGBA, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);

		passes.push_back(new FinalGatherPass(post_effects, { shaders[8]->GetProgram() }));

		_ibl = new PBR::IBL("Res/Content/HDRI/Etnies_Park_Center_Env.hdr", { shaders[14]->GetProgram(), shaders[15]->GetProgram(), shaders[16]->GetProgram(), shaders[17]->GetProgram() });

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// Clear background colour
	}

	// Destroy all allocated memory
	inline static void Destroy()
	{
		passes.clear();		// Destroy gbuffer data
		shaders.clear();	// Delete all shader programs
		post_effects.clear();

		delete _ibl;
		delete _fbo_final;

		if (_screen_rect) delete _screen_rect;	// Delete screen rectangle
	}

	// Update some deferred passes
	inline static void Update(double delta)
	{
		deltaTime = (float)delta;

		passes[GEOMETRY_PASS]->Update(delta);		// Update the geometry pass
		//_inv->Update(delta);
	}

	// Render all deferred passes
	inline static void Render()
	{
		glDisable(GL_BLEND);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear last buffers

		//// ------------------------- SHADOWING PASS -------------------------- // 
		static_cast<Shadowmapping*>(post_effects[2])->Render();
		//// ------------------------------------------------------------------- //

		// ----------------------------- GEOMETRY ----------------------------- //
		passes[GBUFFER_PASS]->GetFbos()[0]->Bind();		// Bind the world gbuffer frame buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear last buffers

		//shaders[17]->UseProgram();
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE);
		//_ps->render();
		//glDisable(GL_BLEND);

		passes[GEOMETRY_PASS]->Render();// Render geometry

		passes[GBUFFER_PASS]->GetFbos()[0]->Unbind();	// Unbind last frame buffer
		// ------------------------------------------------------------------- //

		//// ------------------------------ SSAO ------------------------------- //
		passes[SSAO_PASS]->Render();
		//// ------------------------------------------------------------------- //
		//

		static_cast<VolumetricLightPass*>(post_effects[3])->Render();

		// ------------------------------ LIGHT ------------------------------------- //
		{
			static_cast<Bloom*>(post_effects[0])->getFbo()->Bind(); // start to capture the scene into a fbo (frame buffer object)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			passes[GBUFFER_PASS]->Render();	// Render the gbuffer colour attachments
			passes[LIGHT_PASS]->Render();	// Parse the camera position

			glUniform1i(_u_view_type, _current_view_t);		// Assign global view type

			glm::mat4 shadow_bias = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * glm::translate(glm::vec3(1.0, 1.0, 1.0));
			glm::mat4 shadow_matrix = shadow_bias * static_cast<Shadowmapping*>(post_effects[2])->getLightSpaceMatrix();

			glUniformMatrix4fv(((LightPass*)passes[LIGHT_PASS])->_u_lsm, 1, GL_FALSE, glm::value_ptr(static_cast<Shadowmapping*>(post_effects[2])->getLightSpaceMatrix()));
			glUniformMatrix4fv(((LightPass*)passes[LIGHT_PASS])->_u_shadow_matrix, 1, GL_FALSE, glm::value_ptr(shadow_matrix));

			glm::mat4 model;
			for (Actor* a : Content::_map->GetActors())		// Iterate through each actor
			{
				model = a->GetModelMatrix();	// Get model matrix
				glUniformMatrix4fv(((LightPass*)passes[LIGHT_PASS])->_u_mod, 1, GL_FALSE, glm::value_ptr(model));	// Send model matrix to buffer
			}

			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, static_cast<Shadowmapping*>(post_effects[2])->_v_blur->GetAttachments()[0]->_texture);
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, ((SsaoPass*)passes[SSAO_PASS])->GetFbos()[1]->GetAttachments()[0]->_texture);

			_ibl->Render();

			_screen_rect->Render(1); // Render to screen rectangle

			static_cast<Bloom*>(post_effects[0])->getFbo()->Unbind();
			//// --------------------------------------------------------------------------- //
		}
		// -------------------------------------------------------------------------- // 

		// ------------------------------ POSTFX ------------------------------------- //
		{
			static_cast<Bloom*>(post_effects[0])->Render();
		}
		// --------------------------------------------------------------------------- //


		// TODO: Put this in post processing!!!!
		{
			// Image Sharpening
			_fbo_final->Bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaders[9]->UseProgram(); // bind the final gather shader (using additive blending to blend all the postFX needed)
			glUniform1i(_final_textures[0], 0);	// Bind the final scene texture
			glActiveTexture(GL_TEXTURE0); // unit 1
			glBindTexture(GL_TEXTURE_2D, static_cast<Bloom*>(post_effects[0])->getFbo()->GetAttachments()[0]->_texture); // bind the scene texture
			_screen_rect->Render(1); // Render to screen rectangle
			_fbo_final->Unbind();
		}

		_picker->Render();

		if (!Content::_map->GetActors().empty())
		{
			if (_picker->picked)
			{
				Content::_map->GetActors()[_picker->_selected_id]->SetSelected(true);

				if (Mouse::IsRightClick())
				{
					if (Mouse::GetPointX() < 1920.0f / 2)
					{
						Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.x -= 0.05f;
						Content::_map->GetActors()[_picker->_selected_id]->UpdateModel();
					}
					if (Mouse::GetPointX() > 1920.0f / 2)
					{
						Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.x += 0.05f;
						Content::_map->GetActors()[_picker->_selected_id]->UpdateModel();
					}
				}
				if (Mouse::IsMiddleClick() && Keyboard::GetKey('Y').down)
				{
					if (Mouse::GetPointY() > 1080.0f / 2)
					{
						Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.y -= 0.05f;
						Content::_map->GetActors()[_picker->_selected_id]->UpdateModel();
					}
					if (Mouse::GetPointY() < 1080.0f / 2)
					{
						Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.y += 0.05f;
						Content::_map->GetActors()[_picker->_selected_id]->UpdateModel();
					}
				}
				if (Mouse::IsMiddleClick() && Keyboard::GetKey('Z').down)
				{
					if (Mouse::GetPointY() < 1080.0f / 2)
					{
						Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.z -= 0.05f;
						Content::_map->GetActors()[_picker->_selected_id]->UpdateModel();
					}
					if (Mouse::GetPointY() > 1080.0f / 2)
					{
						Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.z += 0.05f;
						Content::_map->GetActors()[_picker->_selected_id]->UpdateModel();
					}
				}
			}
			else if (_picker->picked == false)
			{
				for (Actor* a : Content::_map->GetActors())
					a->SetSelected(false);
			}
		}


		// ------------------------------ FINAL GATHER ------------------------------- // TODO: Put this as a pass class
		passes[FINAL_GATHER]->Render();

		// ---------------------------- FORWARD RENDERING --------------------------------- //
		passes[GBUFFER_PASS]->GetFbos()[0]->ResolveToForward(passes[GBUFFER_PASS]->GetFbos()[0]);

		if (_picker->picked)
		{
			Manipulators::SetActive(true);
			Manipulators::SetPosition(glm::vec3(Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.x, 
												Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.y,
												Content::_map->GetActors()[_picker->_selected_id]->_trans._pos.z));
			Manipulators::Render();
		}

		//glDisable(GL_CULL_FACE);
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

		//// shader 
		//shaders[13]->UseProgram();
		//// uniforms
		//glm::mat4 view = Content::_map->GetCamera()->GetViewMatrix();
		//glm::mat4 projection = glm::perspective(Content::_map->GetCamera()->GetFov(), (float)1920.0f / (float)1080.0f, 0.1f, 100.0f);
		//glUniformMatrix4fv(glGetUniformLocation(shaders[13]->GetProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(glGetUniformLocation(shaders[13]->GetProgram(), "proj"), 1, GL_FALSE, glm::value_ptr(projection));
		//glUniform1i(glGetUniformLocation(shaders[13]->GetProgram(), "environmentMap"), 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, _ibl->GetEnvironmentMap());

		//Primitives::cube();
	}
};

// Define all static variables
std::vector<ShaderProgram*>		Deferred::shaders;
std::vector<Pass*>				Deferred::passes;		// The list of post effect passes
std::vector<PostFX*>			Deferred::post_effects; // The list of post processing effects

int8_t							Deferred::_current_view_t(0);
GLuint							Deferred::_u_view_type;
GLuint						    Deferred::_final_textures[4];
GLuint							Deferred::_bloom_intensity;
Fbo*							Deferred::_fbo_final;

PBR::IBL* Deferred::_ibl;
Inventory* Deferred::_inv;

glm::vec3 Deferred::a;
glm::vec3 Deferred::b;
glm::vec3 Deferred::c;
glm::vec3 Deferred::d;
glm::vec3 Deferred::point_on_bezier;
std::vector<glm::vec3> Deferred::_steps;
unsigned int Deferred::current_step;
float Deferred::deltaTime;

CPicker* Deferred::_picker;

#endif