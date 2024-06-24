#include <iostream>

#include "Harmony.h"

#include <sokol_app.h>

#define GLM_FORCE_SSE2
#define GLM_FORCE_INLINE
#define GLM_FORCE_ALIGNED
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Engine
{
public:
	void init()
	{
		// Window init
		{
			harmony::Window::Description windowDescription;
			windowDescription.width = 1024;
			windowDescription.height = 576;
			windowDescription.isFullScreen = false;
			harmony::createWindow(windowDescription);
			window = harmony::getWindow();
			window->initialize();
		}
		
		// Renderer init
		{
			renderer = harmony::getRenderer();
			renderer->initialize();
		}

		// Scene creation
		{
			harmony::Scene::Description sceneDescription;
			sceneDescription.label = "Test Scene";
			scene = harmony::createScene(sceneDescription);
			{
				harmony::component::Visual visual;
				{
					const float vertices[] = {
						// positions        colors
						-1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
						 1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
						 1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
						-1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

						-1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
						 1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
						 1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
						-1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

						-1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
						-1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
						-1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
						-1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

						1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
						1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
						1.0,  1.0,  1.0,   1.0, 0.5, 0.0, 1.0,
						1.0, -1.0,  1.0,   1.0, 0.5, 0.0, 1.0,

						-1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
						-1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
						 1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
						 1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

						-1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
						-1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
						 1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
						 1.0, 1.0, -1.0, 1.0, 0.0, 0.5, 1.0
					};
					visual.mesh.vertices = vertices;
					visual.mesh.verticesSize = sizeof(vertices);

					const uint16_t indices[] = {
						0, 1, 2,  0, 2, 3,
						6, 5, 4,  7, 6, 4,
						8, 9, 10,  8, 10, 11,
						14, 13, 12,  15, 14, 12,
						16, 17, 18,  16, 18, 19,
						22, 21, 20, 23, 22, 20
					};
					visual.mesh.indices = indices;
					visual.mesh.indicesSize = sizeof(indices);
				}
				harmony::Entity::Description entityDescription;
				{
					entityDescription.label = "Test Entity";
					entityDescription.visual = visual;
				}
				auto entity = scene->createEntity(entityDescription);
			}

			harmony::Shader::Description shaderDescription;
			{
				shaderDescription.label = "Test Shader Program";
				shaderDescription.attributesCount = 2;
				shaderDescription.attrs[0].name = "POSITION";
				shaderDescription.attrs[1].name = "COLOR";
				shaderDescription.attrs[1].index = 1;

				shaderDescription.vs.constantBlocksCount = 1;
				shaderDescription.vs.constantBlocks[0].size = sizeof(glm::mat4);
				shaderDescription.vs.source =
					"cbuffer params: register(b0) {\n"
					"  float4x4 mvp;\n"
					"};\n"
					"struct vs_in {\n"
					"  float4 pos: POSITION;\n"
					"  float4 color: COLOR1;\n"
					"};\n"
					"struct vs_out {\n"
					"  float4 color: COLOR0;\n"
					"  float4 pos: SV_Position;\n"
					"};\n"
					"vs_out main(vs_in inp) {\n"
					"  vs_out outp;\n"
					"  outp.pos = mul(mvp, inp.pos);\n"
					"  outp.color = inp.color;\n"
					"  return outp;\n"
					"};\n";

				shaderDescription.fs.source =
					"float4 main(float4 color: COLOR0): SV_Target0 {\n"
					"  return color;\n"
					"}\n";
			}

			auto shaderProgram1 = harmony::createShader(shaderDescription);
			harmony::Pipeline::Description pipelineDescription;
			{
				pipelineDescription.label = "Test Pipeline";
				pipelineDescription.layout.attributesCount = 2;
				pipelineDescription.layout.attributes[0].format = harmony::Pipeline::VertexFormat::eFLOAT3;
				pipelineDescription.layout.attributes[1].format = harmony::Pipeline::VertexFormat::eFLOAT4;
				pipelineDescription.shaderProgram = shaderProgram1;
				pipelineDescription.indexType = harmony::Pipeline::IndexType::eUINT16;
				pipelineDescription.cullMode = harmony::Pipeline::CullMode::eBACK;
				//pipelineDescription.depthBufferEnabled = true;
			}
			pipeline = harmony::createPipeline(pipelineDescription);

			harmony::RenderPass::Description passDescription;
			{
				passDescription.label = "Test Render Pass";
				passDescription.action.colors.value[0] = 1.0f;
				passDescription.action.colors.value[1] = 1.0f;
				passDescription.action.colors.value[2] = 1.0f;
				passDescription.action.colors.value[3] = 1.0f;
			}
			pass = harmony::createRenderPass(passDescription);
		}

		// Init sequence
		{
			scene->initialize();
			pipeline->initialize();
			pass->initialize();
		}
	}

	void frame()
	{
		auto& windowDescription = window->getDescription();
		glm::mat4 proj = glm::perspective(90.0f, (float)windowDescription.width / (float)windowDescription.height, 0.01f, 10.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 model(1);
		
		rx -= 1.0f;
		model *= glm::scale(model, glm::vec3(1.3f));
		model *= glm::rotate(model, glm::radians(rx), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mvp = proj * view * model;

		auto drawable = (*(scene->getVisualEntities().begin()))->getDrawable();

		pass->begin();
		pipeline->bind();
		pipeline->applyConstants(0, &mvp, sizeof(mvp));
		
		drawable->bind();
		drawable->draw(0, 36, 1);

		pass->end();
	}

	void cleanup()
	{
		scene->deinitialize();
		pipeline->deinitialize();
		pass->deinitialize();

		delete scene;
		delete pipeline;
		delete pass;

		renderer->deinitialize();
		window->deinitialize();
		delete renderer;
		delete window;
	}

private:
	float rx = 0.0f;
	
	harmony::Window* window			= nullptr;
	harmony::Renderer* renderer		= nullptr;

	harmony::Scene* scene			= nullptr;

	harmony::Pipeline* pipeline		= nullptr;
	harmony::RenderPass* pass		= nullptr;
}engine;

// this is some extra code required because of sokol GFX
namespace sokol
{
	static void init()
	{
		engine.init();
	}

	static void frame()
	{
		harmony::getWindow()->update();
		engine.frame();
	}

	static void cleanup()
	{
		engine.cleanup();
	}

	static const std::unordered_map<sapp_keycode, harmony::Inputs::Key> sg_keyMap = {
		{ sapp_keycode::SAPP_KEYCODE_INVALID, harmony::Inputs::Key::eINVALID_KEY},
		{ sapp_keycode::SAPP_KEYCODE_SPACE, harmony::Inputs::Key::eKEY_SPACE},
		{ sapp_keycode::SAPP_KEYCODE_APOSTROPHE, harmony::Inputs::Key::eKEY_APOSTROPHE},
		{ sapp_keycode::SAPP_KEYCODE_COMMA, harmony::Inputs::Key::eKEY_COMMA},
		{ sapp_keycode::SAPP_KEYCODE_MINUS, harmony::Inputs::Key::eKEY_MINUS},
		{ sapp_keycode::SAPP_KEYCODE_PERIOD, harmony::Inputs::Key::eKEY_PERIOD},
		{ sapp_keycode::SAPP_KEYCODE_SLASH, harmony::Inputs::Key::eKEY_SLASH},
		{ sapp_keycode::SAPP_KEYCODE_0, harmony::Inputs::Key::eKEY_0},
		{ sapp_keycode::SAPP_KEYCODE_1, harmony::Inputs::Key::eKEY_1},
		{ sapp_keycode::SAPP_KEYCODE_2, harmony::Inputs::Key::eKEY_2},
		{ sapp_keycode::SAPP_KEYCODE_3, harmony::Inputs::Key::eKEY_3},
		{ sapp_keycode::SAPP_KEYCODE_4, harmony::Inputs::Key::eKEY_4},
		{ sapp_keycode::SAPP_KEYCODE_5, harmony::Inputs::Key::eKEY_5},
		{ sapp_keycode::SAPP_KEYCODE_6, harmony::Inputs::Key::eKEY_6},
		{ sapp_keycode::SAPP_KEYCODE_7, harmony::Inputs::Key::eKEY_7},
		{ sapp_keycode::SAPP_KEYCODE_8, harmony::Inputs::Key::eKEY_8},
		{ sapp_keycode::SAPP_KEYCODE_9, harmony::Inputs::Key::eKEY_9},
		{ sapp_keycode::SAPP_KEYCODE_SEMICOLON, harmony::Inputs::Key::eKEY_SEMICOLON},
		{ sapp_keycode::SAPP_KEYCODE_EQUAL, harmony::Inputs::Key::eKEY_EQUAL},
		{ sapp_keycode::SAPP_KEYCODE_A, harmony::Inputs::Key::eKEY_A},
		{ sapp_keycode::SAPP_KEYCODE_B, harmony::Inputs::Key::eKEY_B},
		{ sapp_keycode::SAPP_KEYCODE_C, harmony::Inputs::Key::eKEY_C},
		{ sapp_keycode::SAPP_KEYCODE_D, harmony::Inputs::Key::eKEY_D},
		{ sapp_keycode::SAPP_KEYCODE_E, harmony::Inputs::Key::eKEY_E},
		{ sapp_keycode::SAPP_KEYCODE_F, harmony::Inputs::Key::eKEY_F},
		{ sapp_keycode::SAPP_KEYCODE_G, harmony::Inputs::Key::eKEY_G},
		{ sapp_keycode::SAPP_KEYCODE_H, harmony::Inputs::Key::eKEY_H},
		{ sapp_keycode::SAPP_KEYCODE_I, harmony::Inputs::Key::eKEY_I},
		{ sapp_keycode::SAPP_KEYCODE_J, harmony::Inputs::Key::eKEY_J},
		{ sapp_keycode::SAPP_KEYCODE_K, harmony::Inputs::Key::eKEY_K},
		{ sapp_keycode::SAPP_KEYCODE_L, harmony::Inputs::Key::eKEY_L},
		{ sapp_keycode::SAPP_KEYCODE_M, harmony::Inputs::Key::eKEY_M},
		{ sapp_keycode::SAPP_KEYCODE_N, harmony::Inputs::Key::eKEY_N},
		{ sapp_keycode::SAPP_KEYCODE_O, harmony::Inputs::Key::eKEY_O},
		{ sapp_keycode::SAPP_KEYCODE_P, harmony::Inputs::Key::eKEY_P},
		{ sapp_keycode::SAPP_KEYCODE_Q, harmony::Inputs::Key::eKEY_Q},
		{ sapp_keycode::SAPP_KEYCODE_R, harmony::Inputs::Key::eKEY_R},
		{ sapp_keycode::SAPP_KEYCODE_S, harmony::Inputs::Key::eKEY_S},
		{ sapp_keycode::SAPP_KEYCODE_T, harmony::Inputs::Key::eKEY_T},
		{ sapp_keycode::SAPP_KEYCODE_U, harmony::Inputs::Key::eKEY_U},
		{ sapp_keycode::SAPP_KEYCODE_V, harmony::Inputs::Key::eKEY_V},
		{ sapp_keycode::SAPP_KEYCODE_W, harmony::Inputs::Key::eKEY_W},
		{ sapp_keycode::SAPP_KEYCODE_X, harmony::Inputs::Key::eKEY_X},
		{ sapp_keycode::SAPP_KEYCODE_Y, harmony::Inputs::Key::eKEY_Y},
		{ sapp_keycode::SAPP_KEYCODE_Z, harmony::Inputs::Key::eKEY_Z},
		{ sapp_keycode::SAPP_KEYCODE_LEFT_BRACKET, harmony::Inputs::Key::eKEY_LEFT_BRACKET},
		{ sapp_keycode::SAPP_KEYCODE_BACKSLASH, harmony::Inputs::Key::eKEY_BACKSLASH},
		{ sapp_keycode::SAPP_KEYCODE_RIGHT_BRACKET, harmony::Inputs::Key::eKEY_RIGHT_BRACKET},
		{ sapp_keycode::SAPP_KEYCODE_GRAVE_ACCENT, harmony::Inputs::Key::eKEY_GRAVE_ACCENT},
		{ sapp_keycode::SAPP_KEYCODE_WORLD_1, harmony::Inputs::Key::eKEY_WORLD_1},
		{ sapp_keycode::SAPP_KEYCODE_WORLD_2, harmony::Inputs::Key::eKEY_WORLD_2},
		{ sapp_keycode::SAPP_KEYCODE_ESCAPE, harmony::Inputs::Key::eKEY_ESCAPE},
		{ sapp_keycode::SAPP_KEYCODE_ENTER, harmony::Inputs::Key::eKEY_ENTER},
		{ sapp_keycode::SAPP_KEYCODE_TAB, harmony::Inputs::Key::eKEY_TAB},
		{ sapp_keycode::SAPP_KEYCODE_BACKSPACE, harmony::Inputs::Key::eKEY_BACKSPACE},
		{ sapp_keycode::SAPP_KEYCODE_INSERT, harmony::Inputs::Key::eKEY_INSERT},
		{ sapp_keycode::SAPP_KEYCODE_DELETE, harmony::Inputs::Key::eKEY_DELETE},
		{ sapp_keycode::SAPP_KEYCODE_RIGHT, harmony::Inputs::Key::eKEY_ARROW_RIGHT},
		{ sapp_keycode::SAPP_KEYCODE_LEFT, harmony::Inputs::Key::eKEY_ARROW_LEFT},
		{ sapp_keycode::SAPP_KEYCODE_DOWN, harmony::Inputs::Key::eKEY_ARROW_DOWN},
		{ sapp_keycode::SAPP_KEYCODE_UP, harmony::Inputs::Key::eKEY_ARROW_UP},
		{ sapp_keycode::SAPP_KEYCODE_PAGE_UP, harmony::Inputs::Key::eKEY_PAGE_UP},
		{ sapp_keycode::SAPP_KEYCODE_PAGE_DOWN, harmony::Inputs::Key::eKEY_PAGE_DOWN},
		{ sapp_keycode::SAPP_KEYCODE_HOME, harmony::Inputs::Key::eKEY_HOME},
		{ sapp_keycode::SAPP_KEYCODE_END, harmony::Inputs::Key::eKEY_END},
		{ sapp_keycode::SAPP_KEYCODE_CAPS_LOCK, harmony::Inputs::Key::eKEY_CAPS_LOCK},
		{ sapp_keycode::SAPP_KEYCODE_SCROLL_LOCK, harmony::Inputs::Key::eKEY_SCROLL_LOCK},
		{ sapp_keycode::SAPP_KEYCODE_NUM_LOCK, harmony::Inputs::Key::eKEY_NUM_LOCK},
		{ sapp_keycode::SAPP_KEYCODE_PRINT_SCREEN, harmony::Inputs::Key::eKEY_PRINT_SCREEN},
		{ sapp_keycode::SAPP_KEYCODE_PAUSE, harmony::Inputs::Key::eKEY_PAUSE},
		{ sapp_keycode::SAPP_KEYCODE_F1, harmony::Inputs::Key::eKEY_F1},
		{ sapp_keycode::SAPP_KEYCODE_F2, harmony::Inputs::Key::eKEY_F2},
		{ sapp_keycode::SAPP_KEYCODE_F3, harmony::Inputs::Key::eKEY_F3},
		{ sapp_keycode::SAPP_KEYCODE_F4, harmony::Inputs::Key::eKEY_F4},
		{ sapp_keycode::SAPP_KEYCODE_F5, harmony::Inputs::Key::eKEY_F5},
		{ sapp_keycode::SAPP_KEYCODE_F6, harmony::Inputs::Key::eKEY_F6},
		{ sapp_keycode::SAPP_KEYCODE_F7, harmony::Inputs::Key::eKEY_F7},
		{ sapp_keycode::SAPP_KEYCODE_F8, harmony::Inputs::Key::eKEY_F8},
		{ sapp_keycode::SAPP_KEYCODE_F9, harmony::Inputs::Key::eKEY_F9},
		{ sapp_keycode::SAPP_KEYCODE_F10, harmony::Inputs::Key::eKEY_F10},
		{ sapp_keycode::SAPP_KEYCODE_F11, harmony::Inputs::Key::eKEY_F11},
		{ sapp_keycode::SAPP_KEYCODE_F12, harmony::Inputs::Key::eKEY_F12},
		{ sapp_keycode::SAPP_KEYCODE_F13, harmony::Inputs::Key::eKEY_F13},
		{ sapp_keycode::SAPP_KEYCODE_F14, harmony::Inputs::Key::eKEY_F14},
		{ sapp_keycode::SAPP_KEYCODE_F15, harmony::Inputs::Key::eKEY_F15},
		{ sapp_keycode::SAPP_KEYCODE_F16, harmony::Inputs::Key::eKEY_F16},
		{ sapp_keycode::SAPP_KEYCODE_F17, harmony::Inputs::Key::eKEY_F17},
		{ sapp_keycode::SAPP_KEYCODE_F18, harmony::Inputs::Key::eKEY_F18},
		{ sapp_keycode::SAPP_KEYCODE_F19, harmony::Inputs::Key::eKEY_F19},
		{ sapp_keycode::SAPP_KEYCODE_F20, harmony::Inputs::Key::eKEY_F20},
		{ sapp_keycode::SAPP_KEYCODE_F21, harmony::Inputs::Key::eKEY_F21},
		{ sapp_keycode::SAPP_KEYCODE_F22, harmony::Inputs::Key::eKEY_F22},
		{ sapp_keycode::SAPP_KEYCODE_F23, harmony::Inputs::Key::eKEY_F23},
		{ sapp_keycode::SAPP_KEYCODE_F24, harmony::Inputs::Key::eKEY_F24},
		{ sapp_keycode::SAPP_KEYCODE_F25, harmony::Inputs::Key::eKEY_F25},
		{ sapp_keycode::SAPP_KEYCODE_KP_0, harmony::Inputs::Key::eKEY_KP_0},
		{ sapp_keycode::SAPP_KEYCODE_KP_1, harmony::Inputs::Key::eKEY_KP_1},
		{ sapp_keycode::SAPP_KEYCODE_KP_2, harmony::Inputs::Key::eKEY_KP_2},
		{ sapp_keycode::SAPP_KEYCODE_KP_3, harmony::Inputs::Key::eKEY_KP_3},
		{ sapp_keycode::SAPP_KEYCODE_KP_4, harmony::Inputs::Key::eKEY_KP_4},
		{ sapp_keycode::SAPP_KEYCODE_KP_5, harmony::Inputs::Key::eKEY_KP_5},
		{ sapp_keycode::SAPP_KEYCODE_KP_6, harmony::Inputs::Key::eKEY_KP_6},
		{ sapp_keycode::SAPP_KEYCODE_KP_7, harmony::Inputs::Key::eKEY_KP_7},
		{ sapp_keycode::SAPP_KEYCODE_KP_8, harmony::Inputs::Key::eKEY_KP_8},
		{ sapp_keycode::SAPP_KEYCODE_KP_9, harmony::Inputs::Key::eKEY_KP_9},
		{ sapp_keycode::SAPP_KEYCODE_KP_DECIMAL = 330,},
		{ sapp_keycode::SAPP_KEYCODE_KP_DIVIDE = 331,},
		{ sapp_keycode::SAPP_KEYCODE_KP_MULTIPLY = 332,},
		{ sapp_keycode::SAPP_KEYCODE_KP_SUBTRACT = 333,},
		{ sapp_keycode::SAPP_KEYCODE_KP_ADD = 334,},
		{ sapp_keycode::SAPP_KEYCODE_KP_ENTER = 335,},
		{ sapp_keycode::SAPP_KEYCODE_KP_EQUAL = 336,},
		{ sapp_keycode::SAPP_KEYCODE_LEFT_SHIFT = 340,},
		{ sapp_keycode::SAPP_KEYCODE_LEFT_CONTROL = 341,},
		{ sapp_keycode::SAPP_KEYCODE_LEFT_ALT = 342,},
		{ sapp_keycode::SAPP_KEYCODE_LEFT_SUPER = 343,},
		{ sapp_keycode::SAPP_KEYCODE_RIGHT_SHIFT = 344,},
		{ sapp_keycode::SAPP_KEYCODE_RIGHT_CONTROL = 345,},
		{ sapp_keycode::SAPP_KEYCODE_RIGHT_ALT = 346,},
		{ sapp_keycode::SAPP_KEYCODE_RIGHT_SUPER = 347,},
		{ sapp_keycode::SAPP_KEYCODE_MENU = 348,},
	};

	static void event(const sapp_event* e)
	{

		enum sapp_keycode
		{
			SAPP_KEYCODE_INVALID = 0,
			SAPP_KEYCODE_SPACE = 32,
			SAPP_KEYCODE_APOSTROPHE = 39,  /* ' */
			SAPP_KEYCODE_COMMA = 44,  /* , */
			SAPP_KEYCODE_MINUS = 45,  /* - */
			SAPP_KEYCODE_PERIOD = 46,  /* . */
			SAPP_KEYCODE_SLASH = 47,  /* / */
			SAPP_KEYCODE_0 = 48,
			SAPP_KEYCODE_1 = 49,
			SAPP_KEYCODE_2 = 50,
			SAPP_KEYCODE_3 = 51,
			SAPP_KEYCODE_4 = 52,
			SAPP_KEYCODE_5 = 53,
			SAPP_KEYCODE_6 = 54,
			SAPP_KEYCODE_7 = 55,
			SAPP_KEYCODE_8 = 56,
			SAPP_KEYCODE_9 = 57,
			SAPP_KEYCODE_SEMICOLON = 59,  /* ; */
			SAPP_KEYCODE_EQUAL = 61,  /* = */
			SAPP_KEYCODE_A = 65,
			SAPP_KEYCODE_B = 66,
			SAPP_KEYCODE_C = 67,
			SAPP_KEYCODE_D = 68,
			SAPP_KEYCODE_E = 69,
			SAPP_KEYCODE_F = 70,
			SAPP_KEYCODE_G = 71,
			SAPP_KEYCODE_H = 72,
			SAPP_KEYCODE_I = 73,
			SAPP_KEYCODE_J = 74,
			SAPP_KEYCODE_K = 75,
			SAPP_KEYCODE_L = 76,
			SAPP_KEYCODE_M = 77,
			SAPP_KEYCODE_N = 78,
			SAPP_KEYCODE_O = 79,
			SAPP_KEYCODE_P = 80,
			SAPP_KEYCODE_Q = 81,
			SAPP_KEYCODE_R = 82,
			SAPP_KEYCODE_S = 83,
			SAPP_KEYCODE_T = 84,
			SAPP_KEYCODE_U = 85,
			SAPP_KEYCODE_V = 86,
			SAPP_KEYCODE_W = 87,
			SAPP_KEYCODE_X = 88,
			SAPP_KEYCODE_Y = 89,
			SAPP_KEYCODE_Z = 90,
			SAPP_KEYCODE_LEFT_BRACKET = 91,  /* [ */
			SAPP_KEYCODE_BACKSLASH = 92,  /* \ */
			SAPP_KEYCODE_RIGHT_BRACKET = 93,  /* ] */
			SAPP_KEYCODE_GRAVE_ACCENT = 96,  /* ` */
			SAPP_KEYCODE_WORLD_1 = 161, /* non-US #1 */
			SAPP_KEYCODE_WORLD_2 = 162, /* non-US #2 */
			SAPP_KEYCODE_ESCAPE = 256,
			SAPP_KEYCODE_ENTER = 257,
			SAPP_KEYCODE_TAB = 258,
			SAPP_KEYCODE_BACKSPACE = 259,
			SAPP_KEYCODE_INSERT = 260,
			SAPP_KEYCODE_DELETE = 261,
			SAPP_KEYCODE_RIGHT = 262,
			SAPP_KEYCODE_LEFT = 263,
			SAPP_KEYCODE_DOWN = 264,
			SAPP_KEYCODE_UP = 265,
			SAPP_KEYCODE_PAGE_UP = 266,
			SAPP_KEYCODE_PAGE_DOWN = 267,
			SAPP_KEYCODE_HOME = 268,
			SAPP_KEYCODE_END = 269,
			SAPP_KEYCODE_CAPS_LOCK = 280,
			SAPP_KEYCODE_SCROLL_LOCK = 281,
			SAPP_KEYCODE_NUM_LOCK = 282,
			SAPP_KEYCODE_PRINT_SCREEN = 283,
			SAPP_KEYCODE_PAUSE = 284,
			SAPP_KEYCODE_F1 = 290,
			SAPP_KEYCODE_F2 = 291,
			SAPP_KEYCODE_F3 = 292,
			SAPP_KEYCODE_F4 = 293,
			SAPP_KEYCODE_F5 = 294,
			SAPP_KEYCODE_F6 = 295,
			SAPP_KEYCODE_F7 = 296,
			SAPP_KEYCODE_F8 = 297,
			SAPP_KEYCODE_F9 = 298,
			SAPP_KEYCODE_F10 = 299,
			SAPP_KEYCODE_F11 = 300,
			SAPP_KEYCODE_F12 = 301,
			SAPP_KEYCODE_F13 = 302,
			SAPP_KEYCODE_F14 = 303,
			SAPP_KEYCODE_F15 = 304,
			SAPP_KEYCODE_F16 = 305,
			SAPP_KEYCODE_F17 = 306,
			SAPP_KEYCODE_F18 = 307,
			SAPP_KEYCODE_F19 = 308,
			SAPP_KEYCODE_F20 = 309,
			SAPP_KEYCODE_F21 = 310,
			SAPP_KEYCODE_F22 = 311,
			SAPP_KEYCODE_F23 = 312,
			SAPP_KEYCODE_F24 = 313,
			SAPP_KEYCODE_F25 = 314,
			SAPP_KEYCODE_KP_0 = 320,
			SAPP_KEYCODE_KP_1 = 321,
			SAPP_KEYCODE_KP_2 = 322,
			SAPP_KEYCODE_KP_3 = 323,
			SAPP_KEYCODE_KP_4 = 324,
			SAPP_KEYCODE_KP_5 = 325,
			SAPP_KEYCODE_KP_6 = 326,
			SAPP_KEYCODE_KP_7 = 327,
			SAPP_KEYCODE_KP_8 = 328,
			SAPP_KEYCODE_KP_9 = 329,
			SAPP_KEYCODE_KP_DECIMAL = 330,
			SAPP_KEYCODE_KP_DIVIDE = 331,
			SAPP_KEYCODE_KP_MULTIPLY = 332,
			SAPP_KEYCODE_KP_SUBTRACT = 333,
			SAPP_KEYCODE_KP_ADD = 334,
			SAPP_KEYCODE_KP_ENTER = 335,
			SAPP_KEYCODE_KP_EQUAL = 336,
			SAPP_KEYCODE_LEFT_SHIFT = 340,
			SAPP_KEYCODE_LEFT_CONTROL = 341,
			SAPP_KEYCODE_LEFT_ALT = 342,
			SAPP_KEYCODE_LEFT_SUPER = 343,
			SAPP_KEYCODE_RIGHT_SHIFT = 344,
			SAPP_KEYCODE_RIGHT_CONTROL = 345,
			SAPP_KEYCODE_RIGHT_ALT = 346,
			SAPP_KEYCODE_RIGHT_SUPER = 347,
			SAPP_KEYCODE_MENU = 348,
		}

		if (e->key_code == sapp_keycode::SAPP_KEYCODE_ESCAPE)
		{
			sapp_request_quit();
		}
		harmony::getWindow()->resize(e->window_width, e->window_height);
	}
}

sapp_desc sokol_main(int argc, char* argv[])
{
	sapp_desc appDesc = { 0 };
	{
		const auto& windowDesc = harmony::getWindow()->getDescription();
		appDesc.window_title = windowDesc.title;
		appDesc.width = windowDesc.width;
		appDesc.height = windowDesc.height;
		appDesc.fullscreen = windowDesc.isFullScreen;
		appDesc.high_dpi = true;
		appDesc.init_cb = sokol::init;
		appDesc.frame_cb = sokol::frame;
		appDesc.cleanup_cb = sokol::cleanup;
		appDesc.event_cb = sokol::event;
	}
	return appDesc;
}
