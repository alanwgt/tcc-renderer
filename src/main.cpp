// GLAD precisa ser incluído ANTES do GLFW porque ele possui as definições das
// funções do OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <cstdlib>
#include <string>

#include "camera.h"
#include "my_math.h"
#include "shader.h"
#include "chunk.h"
#include "height_generator.h"
#include "screen.h"

#define HEIGHT 1050
#define WIDTH  1920
#define TITLE  "TCC - Alan Weingartner @ UFSM"
#define BACKGROUND 0.0f, 0.0f, 0.0f, 1.0f
#define PANIC(x) { std::cerr << x << "\n"; exit(-1); }
#define ASSERT(x, y) { if(!(x)) { std::string s = std::string("ASSERTION ") + std::string(#x) + std::string(" FAILED!\nERROR_CODE: "); std::cerr << y; PANIC(s);} }
// macro para checar o estado do OpenGL a cada chamada da API
#define GL_CHECK(x) do\
{\
    x;\
    GLenum err = glGetError();\
    ASSERT(err == GL_NO_ERROR, err);\
} while (0)

// calback chamado toda vez que a janela é redimensionada
void FramebufferSizeCallback(GLFWwindow* w, int width, int height)
{
	glViewport(0, 0, width, height);
}

static const int TERRAIN_VERTEX_COUNT = 512;
static const double TERRAIN_MOVEMENT_STEP = 0.1;

static const float FOV = 70.f;
static const float NEAR_PLANE = 0.1f;
static const float FAR_PLANE = 2000.0f;

static bool s_wireframe_mode = false;

static GLuint s_vao;
static GLuint s_vbo;
static GLuint s_n_vbo;
static GLuint s_ibo;
static wega::Chunk* s_chunk;

static glm::vec3 s_position;
static float s_rx, s_ry, s_rz;
static float s_scale;
static char s_movement_forward = 0;
static char s_movement_left = 0;

static int window_drag_active = 0;
static double cursor_x_pos, cursor_y_pos;

void Translate(float dx, float dy, float dz)
{
	s_position.x += dx;
	s_position.y += dy;
	s_position.z += dz;
}

void Rotate(float dx, float dy, float dz)
{
	s_rx += dx;
	s_ry += dy;
	s_rz += dz;
}

void ProcessInput(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	// fecha a aplicação com o ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(w, true);
	// alterna entre renderização WIREFRAME e FILL
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		s_wireframe_mode = !s_wireframe_mode;
		if (s_wireframe_mode)
		{
			std::cout << "CHANGED TO WIREFRAME MODE\n";
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			std::cout << "CHANGED TO FILL MODE\n";
		}
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		std::string name = wega::Screen::CaptureFromOpenGL(WIDTH, HEIGHT);
		wega::Screen::HeightMapToPNG(s_chunk, name);
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && s_movement_forward == 0)
		s_movement_forward = -1;
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && s_movement_forward == 0)
		s_movement_forward = 1;
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS && s_movement_left == 0)
		s_movement_left = 1;
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && s_movement_left == 0)
		s_movement_left = -1;
}

void ProcessMouseInput(GLFWwindow* w, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			window_drag_active = 1;
			double x, y;
			glfwGetCursorPos(w, &x, &y);
			cursor_x_pos = x;
			cursor_x_pos = y;
		}
		else if (action == GLFW_RELEASE)
		{
			window_drag_active = 0;
		}
	}
}

void SendChunkDataToGPU()
{
	static bool s_buffer_initialized = false;
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_chunk->GetIndicesSize() * sizeof(GLuint), s_chunk->GetIndices(), GL_STATIC_DRAW));

	// seleciona o Buffer
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, s_vbo));
	// copia os vértices da memória RAM para a memória da GPU
	if (!s_buffer_initialized)
	{
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, s_chunk->GetVerticesSize() * sizeof(GLdouble), s_chunk->GetVertices(), GL_STATIC_DRAW));
		s_buffer_initialized = true;
	}
	else
		GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, s_chunk->GetVerticesSize() * sizeof(GLdouble), s_chunk->GetVertices()));
	
	GL_CHECK(glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, static_cast<void*>(0)));
	GL_CHECK(glEnableVertexAttribArray(0));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, s_n_vbo));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, s_chunk->GetNormalsSize() * sizeof(GLdouble), s_chunk->GetNormals(), GL_STATIC_DRAW));

	// normaliza as normais
	GL_CHECK(glVertexAttribPointer(1, 3, GL_DOUBLE, GL_TRUE, 0, (void*)0));
	GL_CHECK(glEnableVertexAttribArray(1));
}

int main(void)
{
	if (glfwInit() != GL_TRUE)
	{
		glfwTerminate();
		PANIC("GLFW não pode ser inicializado!")
	}

	// configurações do OpenGL dentro do GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
	// dá o controle para o OpenGL renderizar diretamente à janela do S.O.
	glfwMakeContextCurrent(window);
	// ativa a sincronia vertical (VSync)
	glfwSwapInterval(1);

	if (!window)
	{
		glfwTerminate();
		PANIC("Falha ao tentar criar uma janela com o GLFW!")
	}

	// inicializa o GLAD, isso é: encontra a biblioteca do OpenGL no S.O.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		PANIC("GLAD não pode ser inicializado");
	}

	// antes de começar a renderizar, precisamos dizer ao OpenGL qual é o
	// tamanho da janela
	GL_CHECK(glViewport(0, 0, WIDTH, HEIGHT));
	// adiciona o callback de redimensionamento da janela para a função
	// definida acima
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	// callback de processamento de input do teclado
	glfwSetKeyCallback(window, ProcessInput);
	// callback de processamento de inputs do mouse
	glfwSetMouseButtonCallback(window, ProcessMouseInput);

	// escopo local para gerenciamento de memória
	{
		s_chunk = new wega::Chunk(TERRAIN_VERTEX_COUNT, 0, 0);

		// inicializa o vetor de alturas com 0s
		wega::HeightGenerator height_generator{ s_chunk, TERRAIN_VERTEX_COUNT };
		 //height_generator.SetRandomValues();
		
		//height_generator.AddMidPointDisplacement(s_chunk, 6.0, 1.2);
		//height_generator.DiamondSquare();
		//height_generator.AddVoronoiLayer(5, 1.05f, 0.1f);
		
		// height_generator.SetThreshold(0.01);
		// height_generator.DisableThreshold();
		// roughness controla quão suave o resultado será
		// quando maior for dumpener, menor é o valor
		// oitavas menores resultam em terrenos mais irregulares (ruidosos)
		// octaves (1..16) / persistance

		//height_generator.AddPerlinNoiseLayer(6, 2.52f, 1.0f);
		//height_generator.AddPerlinNoiseLayer(8, 3.52f, 0.6f);

		// height_generator.AddMidPointDisplacement(s_chunk, 1.0f, 6.9f);
		// 
		// gera um Vertex Array Object
		GL_CHECK(glGenVertexArrays(1, &s_vao));
		// seleciona o Vertex Array
		GL_CHECK(glBindVertexArray(s_vao));

		// gera um Index Buffer Object
		GL_CHECK(glGenBuffers(1, &s_ibo));
		//GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo));
		//GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_chunk->GetIndicesSize() * sizeof(GLuint), s_chunk->GetIndices(), GL_STATIC_DRAW));

		// gera um Vertex Buffer Object utilizado para armazenar os vértices
		// na memória da GPU
		GL_CHECK(glGenBuffers(1, &s_vbo));
		// seleciona o Buffer
		//GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, s_vbo));
		// copia os vértices da memória RAM para a memória da GPU
		//GL_CHECK(glBufferData(GL_ARRAY_BUFFER, s_chunk->GetVerticesSize() * sizeof(GLdouble), s_chunk->GetVertices(), GL_STATIC_DRAW));

		// cria o layout do VAO (para poder ser utilizado no shader)
		//GL_CHECK(glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, (void*)0));
		//GL_CHECK(glEnableVertexAttribArray(0));

		GL_CHECK(glGenBuffers(1, &s_n_vbo));
		/*GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, s_n_vbo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, s_chunk->GetNormalsSize() * sizeof(GLdouble), s_chunk->GetNormals(), GL_STATIC_DRAW));*/

		//// normaliza as normais
		//GL_CHECK(glVertexAttribPointer(1, 3, GL_DOUBLE, GL_TRUE, 0, (void*)0));
		//GL_CHECK(glEnableVertexAttribArray(1));

		// 
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		s_position = glm::vec3{ 0.0f, -2.0f, -2.0f };
		s_scale = 1.0f;
		s_ry = 120.0f;

		// Camera
		auto* camera = new wega::Camera{ window };
		glm::mat4 view_matrix = CreateViewMatrix(camera);
		glm::mat4 projection_matrix = wega::GenerateProjectionMatrix(WIDTH, HEIGHT, FOV, NEAR_PLANE, FAR_PLANE);
		glm::mat4 transformation_matrix = wega::CreateTransformationMatrix(
			s_position,
			s_rx,
			s_ry,
			s_rz,
			s_scale
		);

		auto* shader = new wega::Shader{ "../shaders/ambient.vert", "../shaders/ambient.frag" };
		shader->Bind();
		shader->SetM4F("projection_matrix", projection_matrix);

		glm::vec3 light_color{ 1.0f, 1.0f, 1.0f };
		// dirt color
		/* glm::vec3 terrain_color{0.678f, 0.4627f, 0.3255f}; */
		glm::vec3 terrain_color{ 1.0f, 1.0f, 1.0f };
		glm::vec3 light_position{ 0.0f, 1500.0f, 0.0f };

		shader->SetV3("terrain_color", terrain_color);
		shader->SetV3("light_color", light_color);
		shader->SetV3("light_pos", light_position);

#pragma region
		
		module::RidgedMulti ridged;
		ridged.SetOctaveCount(8);
		ridged.SetFrequency(2);
		ridged.SetLacunarity(1.2);
		
		module::Billow base;
		base.SetFrequency(2.0);
		base.SetOctaveCount(8);
		base.SetLacunarity(2.5);

		module::ScaleBias flattener;
		flattener.SetSourceModule(0, base);
		flattener.SetScale(0.02);
		flattener.SetBias(-0.75);

		module::Perlin perlin;
		module::Add adder;
		module::Voronoi voronoi;
		module::Select selector;
		module::Turbulence turbulence;
		utils::NoiseMap height_map;
		utils::NoiseMapBuilderPlane height_map_builder;

		perlin.SetSeed(123456789);
		perlin.SetOctaveCount(8);
		perlin.SetFrequency(2.0);
		perlin.SetPersistence(1 / 4);
		perlin.SetLacunarity(1.5);

		voronoi.SetFrequency(1);
		voronoi.SetDisplacement(10);

		selector.SetSourceModule(0, flattener);
		selector.SetSourceModule(1, ridged);
		selector.SetControlModule(perlin);
		selector.SetBounds(0.8, 1000.0);
		selector.SetEdgeFalloff(0.9);

		turbulence.SetSourceModule(0, selector);
		turbulence.SetFrequency(2.0);
		turbulence.SetPower(1/4);

		adder.SetSourceModule(1, perlin);
		adder.SetSourceModule(0, voronoi);

		//height_map_builder.SetSourceModule(ridged);
		height_map_builder.SetSourceModule(turbulence);
		height_map_builder.SetDestNoiseMap(height_map);
		
		const auto sz = TERRAIN_VERTEX_COUNT;
		height_map_builder.SetDestSize(sz, sz);
		height_map_builder.EnableSeamless(true);

#pragma endregion HEIGHT_GEN

		auto x_lower_bound_increment = 0.0;
		auto x_upper_bound_increment = 2.0;
		auto z_lower_bound_increment = 0.0;
		auto z_upper_bound_increment = 2.0;

		height_map_builder.SetBounds(0.0, 2.0, 0.0, 2.0);
		height_map_builder.Build();

		height_generator.ApplyHeightMap(height_map);
		s_chunk->GenerateMesh();
		SendChunkDataToGPU();
		
		// loop de renderização
		while (!glfwWindowShouldClose(window))
		{
			if (s_movement_forward != 0 || s_movement_left != 0)
			{
				x_lower_bound_increment += TERRAIN_MOVEMENT_STEP * s_movement_forward;
				x_upper_bound_increment += TERRAIN_MOVEMENT_STEP * s_movement_forward;
				z_lower_bound_increment += TERRAIN_MOVEMENT_STEP * s_movement_left;
				z_upper_bound_increment += TERRAIN_MOVEMENT_STEP * s_movement_left;
				height_map_builder.SetBounds(x_lower_bound_increment, x_upper_bound_increment, z_lower_bound_increment, z_upper_bound_increment);
				height_map_builder.Build();
				height_generator.ApplyHeightMap(height_map);
				s_chunk->GenerateMesh();
				SendChunkDataToGPU();
				s_movement_left = s_movement_forward = 0;
			}
			
			// atualizar e lidar com os eventos
			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();

			// atualiza a posição do mouse na tela
			glfwGetCursorPos(window, &cursor_x_pos, &cursor_y_pos);
			
			GL_CHECK(glClearColor(BACKGROUND));
			GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

#pragma region
			camera->Update();

			if (window_drag_active)
			{
				double xpos, ypos, delta_x, delta_y;
				glfwGetCursorPos(window, &xpos, &ypos);
				delta_x = xpos - cursor_x_pos;
				delta_y = ypos - cursor_y_pos;
				cursor_x_pos = xpos;
				cursor_y_pos = ypos;
				camera->UpdateByMouseDrag(delta_x, delta_y);
			}
#pragma endregion CAMERA UPDATE
			
			view_matrix = CreateViewMatrix(camera);
			/* Rotate(0.0f, 0.2f, 0.0f); */
			transformation_matrix = wega::CreateTransformationMatrix(s_position, s_rx, s_ry, s_rz, s_scale);
			shader->SetM4F("view_matrix", view_matrix);
			shader->SetM4F("transformation_matrix", transformation_matrix);
			shader->SetV3("view_pos", camera->GetPosition());

			// 1o: gerenciar todos os inputs
			// 2o: renderizar
			/* GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, chunk->GetVerticesSize() * sizeof(GLdouble), chunk->GetVertices())); */
			GL_CHECK(glDrawElements(GL_TRIANGLES, s_chunk->GetIndicesSize(), GL_UNSIGNED_INT, 0));
			
			// 3o: trocar os buffers (troca o buffer que está sendo desenhado
			// pelo que está sendo mostrado na janela)
			glfwSwapBuffers(window);
		}

		delete s_chunk;
		delete camera;
		delete shader;
		GL_CHECK(glDeleteBuffers(1, &s_ibo));
		GL_CHECK(glDeleteBuffers(1, &s_vbo));
		GL_CHECK(glDeleteVertexArrays(1, &s_vao));
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
