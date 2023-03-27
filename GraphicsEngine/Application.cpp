#include "Application.h"

std::vector<gps::Light> lights;

DebugFB debugFB;

bool pressedKeys[1024];
bool pressedMouseButtons[2];
gps::Camera camera;
const char* name;
int windowWidth;
int windowHeight;
int retinaWidth;
int retinaHeight;
GLFWwindow* window = NULL;
Location location;
gps::SkyBox skybox;
gps::Shader defaultShader;
gps::Shader skyboxShader;
gps::Shader shadowShader;
gps::Shader shadowCubemapShader;
gps::Model3D test;

float cameraSpeed = 0.1f;
float fov = 45.0f;
float farPlane = 100.0f;
float closePlane = 0.1f;
int renderMode = 0;
float angle = 0.0f;

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024){
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
		if (pressedKeys[GLFW_KEY_F] && action == GLFW_PRESS) {
			if(renderMode == 0)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else if (renderMode == 1)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			renderMode = (renderMode + 1) % 3;
		}
		if (pressedKeys[GLFW_KEY_X] && action == GLFW_PRESS) {
			std::cout << camera.cameraPosition.x << " " << camera.cameraPosition.y << " " << camera.cameraPosition.z << "\n";
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos){
	camera.rotate(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			pressedMouseButtons[0] = true;
		}
		if (action == GLFW_RELEASE) {
			pressedMouseButtons[0] = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			pressedMouseButtons[1] = true;
		}
		if (action == GLFW_RELEASE) {
			pressedMouseButtons[1] = false;
		}
	}
}

void incrementFOV(float increment) {
	fov -= increment;
	if (fov < 5.0f)
		fov = 5.0f;
	if (fov > 45.0f)
		fov = 45.0f;
	defaultShader.useShaderProgram();
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)retinaWidth / (float)retinaHeight, closePlane, farPlane);
	glUniformMatrix4fv(location.projection, 1, GL_FALSE, glm::value_ptr(projection));
	skybox.setProjection(projection);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(pressedKeys[GLFW_KEY_LEFT_CONTROL])
		incrementFOV((float)yoffset * 5.0f);
}

bool initOpenGLWindow(){
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(windowWidth, windowHeight, name, NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);//VSYNC, 0 for ultra fps, 1 for around 150
	glewExperimental = GL_TRUE;
	glewInit();
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	glfwGetFramebufferSize(window, &retinaWidth, &retinaHeight);

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	return true;
}

void initOpenGLState(){
	std::srand((unsigned)time(NULL));
	glClearColor(0.44f, 0.56f, 0.66f, 1.0);
	glViewport(0, 0, retinaWidth, retinaHeight);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_MULTISAMPLE);
}

void initSkyBox() {
	std::vector<const GLchar*> faces;
	faces.push_back("assets/textures/skybox/right.png");
	faces.push_back("assets/textures/skybox/left.png");
	faces.push_back("assets/textures/skybox/top.png");
	faces.push_back("assets/textures/skybox/bottom.png");
	faces.push_back("assets/textures/skybox/back.png");
	faces.push_back("assets/textures/skybox/front.png");
	skybox.load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)retinaWidth / (float)retinaHeight, closePlane, farPlane);
	skybox.setProjection(projection);
}

void initShaders() {
	defaultShader.loadShader("shaders/defaultShader.vert", "shaders/defaultShader.geom", "shaders/defaultShader.frag");
	shadowShader.loadShader("shaders/shadowMap.vert", "shaders/shadowMap.frag");
	shadowCubemapShader.loadShader("shaders/shadowCubemap.vert", "shaders/shadowCubemap.geom", "shaders/shadowCubemap.frag");
}

void initUniforms() {
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)retinaWidth / (float)retinaHeight, closePlane, farPlane);

	defaultShader.useShaderProgram();
	location.projection = glGetUniformLocation(defaultShader.shaderProgram, "projection");
	glUniformMatrix4fv(location.projection, 1, GL_FALSE, glm::value_ptr(projection));
	location.model = glGetUniformLocation(defaultShader.shaderProgram, "model");
	location.view = glGetUniformLocation(defaultShader.shaderProgram, "view");
	location.cameraPos = glGetUniformLocation(defaultShader.shaderProgram, "cameraPos");
}

void cleanup() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void loadModels() {
	test.loadModel("assets/models/test/test.obj");
}

void initLights() {
	lights.push_back(gps::Light(gps::DIRECTIONAL, glm::normalize(glm::vec3(7.23767f, 17.15592f, -25.4074f)), glm::vec3(1.0f), &shadowShader, 8192));
	//lights.push_back(gps::Light(gps::DIRECTIONAL, glm::normalize(glm::vec3(-7.23767f, 17.15592f, +25.4074f)), glm::vec3(0.0f, 1.0f, 0.0f), &shadowShader, 4096));
	lights.push_back(gps::Light(gps::POINT, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 10.0f, 0.0f), &shadowCubemapShader, 1024));
	lights.push_back(gps::Light(gps::POINT, glm::vec3(10.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 10.0f), &shadowCubemapShader, 1024));
	lights.push_back(gps::Light(gps::POINT, glm::vec3(-10.0f, 3.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f), &shadowCubemapShader, 1024));
}

int init() {
	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}
	initOpenGLState();
	initShaders();
	initSkyBox();
	initUniforms();
	loadModels();
	debugFB.init();
	initLights();
}

void processMovement() {

	if (pressedKeys[GLFW_KEY_W]) {
		camera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		camera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		camera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		camera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1;
	}

	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		camera.move(gps::MOVE_DOWN, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_SPACE]) {
		camera.move(gps::MOVE_UP, cameraSpeed);
	}
}

void appInit(const char* appName, int appWindowWidth, int appWindowHeight) {
	name = appName;
	windowWidth = appWindowWidth;
	windowHeight = appWindowHeight;
	camera = gps::Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.05f);
	init();
}

void setLightUniforms() {
	if (lights.size() == 0)
		return;

	defaultShader.useShaderProgram();
	glm::mat4 lightProjections[2];
	glm::vec3 lightPos[5];
	glm::vec3 lightColor[5];
	GLint lightType[5];
	int nrDir = 0, nrPoint = 0, iter= 0;
	for (gps::Light light : lights) {
		lightPos[iter] = light.pos;
		lightColor[iter] = light.color;
		if (light.type == gps::DIRECTIONAL) {
			lightProjections[nrDir] = light.projection;
			glUniform1i(glGetUniformLocation(defaultShader.shaderProgram, strIntCat("shadowMap", nrDir)), light.index);
			nrDir++;
			lightType[iter] = 0;
		}
		else if (light.type == gps::POINT) {
			glUniform1i(glGetUniformLocation(defaultShader.shaderProgram, strIntCat("shadowCubemap", nrPoint)), light.index);
			nrPoint++;
			lightType[iter] = 1;
		}
		iter++;
	}
	glActiveTexture(GL_TEXTURE0 + iter);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gps::Light::defaultCubemap);
	for (int i = nrPoint; i < 5; i++) {
		glUniform1i(glGetUniformLocation(defaultShader.shaderProgram, strIntCat("shadowCubemap", i)), iter);
	}
	glUniform1i(glGetUniformLocation(defaultShader.shaderProgram, "lightNr"), lights.size());
	glUniform1f(glGetUniformLocation(defaultShader.shaderProgram, "farPlane"), lights.at(0).shadowFar);
	glUniformMatrix4fv(glGetUniformLocation(defaultShader.shaderProgram, "lightProjection"), nrDir, GL_FALSE, glm::value_ptr(lightProjections[0]));
	glUniform1i(glGetUniformLocation(defaultShader.shaderProgram, "nrDir"), nrDir);
	glUniform3fv(glGetUniformLocation(defaultShader.shaderProgram, "lightPos"), lights.size(), glm::value_ptr(lightPos[0]));
	glUniform3fv(glGetUniformLocation(defaultShader.shaderProgram, "lightColor"), lights.size(), glm::value_ptr(lightColor[0]));
	glUniform1iv(glGetUniformLocation(defaultShader.shaderProgram, "lightType"), lights.size(), lightType);
}

void renderScene() {
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 view = camera.getViewMatrix();

	for (gps::Light light : lights) 
		light.draw(&test, model);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, retinaWidth, retinaHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	defaultShader.useShaderProgram();
	setLightUniforms();
	glUniformMatrix4fv(location.model, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(location.view, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(location.cameraPos, 1, glm::value_ptr(camera.cameraPosition));

	test.draw(defaultShader);
	//skyboxShader.useShaderProgram();
	//glUniform1i(glGetUniformLocation(skyboxShader.shaderProgram, "skybox"), 2);
	skybox.draw(skyboxShader, view);
	//debugFB.draw(0);
}

int start() {
	double frameTime1 = glfwGetTime();
	double frameTime2;
	int nrOfFrames = 0;

	while (!glfwWindowShouldClose(window)) {
		glCheckError();

		processMovement();
		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(window);

		frameTime2 = glfwGetTime();
		nrOfFrames++;
		if (nrOfFrames == 30) {
			std::string newTitle = name + std::string(" - FPS: ") + std::to_string(1.0f / (frameTime2 - frameTime1));
			glfwSetWindowTitle(window, newTitle.c_str());
			nrOfFrames = 0;
		}
		frameTime1 = frameTime2;
	}
	cleanup();
	return 0;
}
