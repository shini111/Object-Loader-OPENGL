#include "Shader.h"
#include "Object.h"

#include <glad/glad.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraHeight = 1.0f;

float lastX = 400, lastY = 300;
float pitch = 0.f;
float yaw = -90.f;
float fov = 45.f;

static void processKeyboard(float deltaTime) {
	float cameraSpeed = 5.f * deltaTime;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keyState[SDL_SCANCODE_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keyState[SDL_SCANCODE_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyState[SDL_SCANCODE_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyState[SDL_SCANCODE_Q])
		cameraHeight += 0.05f;
	if (keyState[SDL_SCANCODE_E])
		cameraHeight -= 0.05f;

	cameraPos.y = cameraHeight;
}

static void processMouse(SDL_Event ev, float deltaTime) {
	if (ev.type == SDL_MOUSEMOTION) {
		float xoffset = static_cast<float>(ev.motion.xrel);
		float yoffset = static_cast<float>(-ev.motion.yrel);

		float sensitivity = 0.05f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraFront = glm::normalize(front);

		//std::cout << "Yaw: " << yaw << ", Pitch: " << pitch << std::endl;
	}

	if (ev.type == SDL_MOUSEWHEEL) {
		if (fov >= 1.0f && fov <= 45.0f)
			fov -= ev.wheel.y;
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= 45.0f)
			fov = 45.0f;
	}
}

int main(int argc, char** argv) {

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Init(SDL_INIT_VIDEO);

	float screenWidth = 1920/2;
	float screenHeight = 1080/2;

	SDL_Window* window = SDL_CreateWindow("3D Scene Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(screenWidth), static_cast<int>(screenHeight), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	std::vector<Object> objects;

	Shader shader("texture.shader");
	
	Object alley = Object("Models/Alley/Enviorment.obj",false, shader);

	Object street = Object("Models/Street/street.obj",false, shader);
	street.Translate(glm::vec3(0.f, 0.f, 0.f));

	Object grass = Object("Models/Grass/Gras.obj",false, shader);
	grass.Translate(glm::vec3(5.31f, -1.5f, -1.f));

	Object house = Object("Models/Casa/casat.obj",false, shader);
	house.Translate(glm::vec3(0.f, 0.f, 5.f));

	Object dino = Object("Models/Dino/chart.obj",false, shader);
	dino.Translate(glm::vec3(0.f, 0.45f, 0.f));
	dino.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

	objects.push_back(alley);
	objects.push_back(house);
	objects.push_back(dino);
	objects.push_back(grass);
	objects.push_back(street);

	glm::mat4 projection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);

	bool running = true;
	SDL_Event event;
	Uint32 lastTime = SDL_GetTicks(), currentTime;

	glClearColor(0.68f, 0.85f, 0.90f, 1.0f); // Light steel blue

	while (running) {
		currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				running = false;
			processMouse(event, deltaTime);
		}
		processKeyboard(deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Bind();

		// Inside the Shader class constructor
		GLuint modelLoc = glGetUniformLocation(shader.GetRendererID(), "model");
		GLuint viewLoc = glGetUniformLocation(shader.GetRendererID(), "view");
		GLuint projectionLoc = glGetUniformLocation(shader.GetRendererID(), "projection");

		if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1) {
			std::cout << "Error: One or more uniforms could not be found in the shader!" << std::endl;
		}

		//hf.SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), deltaTime);

		glm::mat4 projection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shader.SetUniformMat4f("model", model);

		for (Object object : objects)
		{
			object.Draw(shader);
		}
		shader.Bind();

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
