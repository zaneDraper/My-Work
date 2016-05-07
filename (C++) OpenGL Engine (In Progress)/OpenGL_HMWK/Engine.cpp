#include "Engine.h"

map<int, bool> keyIsDown;
map<int, bool> keyIsUp;
map<int, bool> keyWasDown;

void mouseClick(GLFWwindow * windowPtr, int button, int action, int mods) {
	keyIsDown[button] = action;
}

void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	keyIsDown[key] = action;
}

Engine::Engine()
{
	texIDs = new unsigned int[2];
	
	currentTime = 0;
	previousTime = 0;

	camera = Camera();
}

Engine::~Engine()
{
}

bool Engine::init() {
	if (glfwInit() == GL_FALSE) return false;

	GLFWwindowPtr =
		glfwCreateWindow(800, 600, "ZaneDraper DSA1 Engine", NULL, NULL);

	if (GLFWwindowPtr != nullptr)
		glfwMakeContextCurrent(GLFWwindowPtr);
	else {
		glfwTerminate();
		return false;
	}

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return false;
	}

	glfwSetMouseButtonCallback(GLFWwindowPtr, mouseClick);
	glfwSetKeyCallback(GLFWwindowPtr, keyCallback);

	return true;
}

bool Engine::bufferModels() {

	if (model.buffer("box.obj")) return true;

	return false;
}

void Engine::createObjects() {

	for (int i = 0; i < 10; i++) {
		Object temp = Object(texIDs[0]);

		temp.transform.location = vec3(-1 + (i / 5.0f), 0 + (i / 10.0f), 0);

		objects.push_back(temp);
	}

	Object temp = Object(texIDs[1]);
	temp.fileName = "textures/character.png";
	temp.colType = temp.sphere;
	temp.transform.location = vec3(0, 0, 0);
	temp.transform.size = vec3(.05, .05, .05);
	objects.push_back(temp);
}

bool Engine::gameLoop() {

	currentTime = glfwGetTime();

	while (!glfwWindowShouldClose(GLFWwindowPtr))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		update();

		glUniformMatrix4fv(3, 1, GL_FALSE, &camera.camMat[0][0]);

		//************************

		model.render(objects);

		//************************

		glfwSwapBuffers(GLFWwindowPtr);

		keyWasDown = keyIsDown;
		glfwPollEvents();

		if (keyIsDown[GLFW_KEY_ESCAPE]) {
			glfwSetWindowShouldClose(GLFWwindowPtr, true);
		}

		if (keyIsDown[GLFW_MOUSE_BUTTON_2] && !keyWasDown[GLFW_MOUSE_BUTTON_2]) {
			resetTransforms();
			cout << "yes" << endl;
		}
	}

	glDeleteTextures(1, texIDs);
	glBindTexture(GL_TEXTURE_2D, 0);
	glfwTerminate();

	return true;
}

bool Engine::useShaders() {
	if (shaderManager.loadShaders("shaders/vShader.glsl", "shaders/fShader.glsl")) {
		glUseProgram(shaderManager.getProgram());
		return true;
	}
	else return false;
}

bool Engine::loadTextures() {
	//Loading textures
	FIBITMAP* image = FreeImage_Load(FreeImage_GetFileType("textures/texture.png", 0), "textures/texture.png");
	FIBITMAP* image2 = FreeImage_Load(FreeImage_GetFileType("textures/texture2.png", 0), "textures/texture2.png");

	//if (image == nullptr) cout << "Broken";
	//else cout << "working";

	FIBITMAP* image32Bit = FreeImage_ConvertTo32Bits(image);
	FIBITMAP* image32Bit2 = FreeImage_ConvertTo32Bits(image2);

	FreeImage_Unload(image);
	FreeImage_Unload(image2);

	glGenTextures(2, texIDs);
	glBindTexture(GL_TEXTURE_2D, texIDs[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int width = FreeImage_GetWidth(image32Bit);
	int height = FreeImage_GetHeight(image32Bit);
	BYTE* address = FreeImage_GetBits(image32Bit);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)address);

	glBindTexture(GL_TEXTURE_2D, texIDs[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int width2 = FreeImage_GetWidth(image32Bit2);
	int height2 = FreeImage_GetHeight(image32Bit2);
	BYTE* address2 = FreeImage_GetBits(image32Bit2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width2, height2, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)address2);

	FreeImage_Unload(image32Bit);
	FreeImage_Unload(image32Bit2);

	createObjects();

	return true;
}

void Engine::update() {
	previousTime = currentTime;
	currentTime = glfwGetTime();
	deltaTime = currentTime - previousTime;

	for (int i = 0; i < 11; i++) {

		if (i < 10) {
			objects[i].rigidBody.force = objects[i].rigidBody.mass * .1;
			objects[i].rigidBody.velocity += (objects[i].rigidBody.force * deltaTime) / objects[i].rigidBody.mass;
			objects[i].transform.location.y -= objects[i].rigidBody.velocity;
			if (objects[i].transform.location.y <= -1) objects[i].transform.location.y = -1;
		}

		Transform tForm = objects[i].transform;
		objects[i].transform.tfMatrix = translate(tForm.location) * yawPitchRoll(tForm.rotation.x, tForm.rotation.y, tForm.rotation.z) * scale(tForm.size);
	}

	for (int i = 0; i < 10; i++) {
		if (objects[i].collidesWith(objects[10])) {
			cout << " " << i << ": Collision" << endl;
		}
	}

    camera.update(GLFWwindowPtr, keyIsDown);

}

void Engine::resetTransforms() {
	for (int i = 0; i < 10; i++) {
		objects[i].reset();
		objects[i].transform.location = vec3(-1 + (i / 5.), 0 + (i / 10.), 0);
	}
}