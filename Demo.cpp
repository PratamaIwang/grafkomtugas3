#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

	BuildColoredCube();

	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);

}

void Demo::Update(double deltaTime) {
	angle += (float)((deltaTime * 0.5f) / 1000);
}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(0, 1, 0));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	DrawColoredCube();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildColoredCube() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("wood2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		// Kaki Kanan
		0.4, -0.5, 0.5, 0, 0,  // 0
		0.5, -0.5, 0.5, 1, 0,  // 1
		0.5,  0.5, 0.5, 1, 1,  // 2
		0.4,  0.5, 0.5, 0, 1,  // 3
		// Kaki Kiri
		-0.4, -0.5, 0.5, 0, 0,  // 4
		-0.5, -0.5, 0.5, 1, 0,  // 5
		-0.5,  0.5, 0.5, 1, 1,  // 6
		-0.4,  0.5, 0.5, 0, 1,  // 7
		// Alas
		-0.5,  0.5, 0.5, 0, 0,  // 8
		 0.5,  0.5, 0.5, 1, 0,  // 9
		 0.5,  0.6, 0.5, 1, 1,  // 10
		-0.5,  0.6, 0.5, 0, 1,  // 11
		// Lengan Kiri
		-0.4, 0.6, -0.5, 0, 0,  // 12
		-0.5, 0.6, -0.5, 1, 0,  // 13
		-0.5, 1.3, -0.5, 1, 1,  // 14
		-0.4, 1.3, -0.5, 0, 1,  // 15
		// Lengan Kanan
		0.4,  0.6, -0.5, 0, 0,  // 16
		0.5,  0.6, -0.5, 1, 0,  // 17
		0.5,  1.3, -0.5, 1, 1,  // 18
		0.4,  1.3, -0.5, 0, 1,  // 19
		// Bantalan
		-0.5,  1.3, -0.5, 0, 0,  // 20
		 0.5,  1.3, -0.5, 1, 0,  // 21
		 0.5,  1.7, -0.5, 1, 1,  // 22
		-0.5,  1.7, -0.5, 0, 1,  // 23
		// Kaki Kanan Belakang
		0.4, -0.5, -0.5, 0, 0,  // 24
		0.5, -0.5, -0.5, 1, 0,  // 25
		0.5,  0.5, -0.5, 1, 1,  // 26
		0.4,  0.5, -0.5, 0, 1,  // 27
		// Kaki Kiri Belakang
		-0.4, -0.5, -0.5, 0, 0,  // 28
		-0.5, -0.5, -0.5, 1, 0,  // 29
		-0.5,  0.5, -0.5, 1, 1,  // 30
		-0.4,  0.5, -0.5, 0, 1,  // 31

		//rigth
		// Kaki Kanan
		0.5, -0.5, 0.5, 0, 0,  // 32
		0.5, -0.5, 0.4, 1, 0,  // 33
		0.5,  0.5, 0.4, 1, 1,  // 34
		0.5,  0.5, 0.5, 0, 1,  // 35
		// Kaki Kanan Belakang
		0.5, -0.5, -0.5, 0, 0,  // 36
		0.5, -0.5, -0.6, 1, 0,  // 37
		0.5,  0.5, -0.6, 1, 1,  // 38
		0.5,  0.5, -0.5, 0, 1,  // 39
		//Alas
		0.5,  0.5, 0.5, 0, 0,   // 40
		0.5,  0.5, -0.6, 1, 0,  // 41
		0.5,  0.6, -0.6, 1, 1,  // 42
		0.5,  0.6, 0.5, 0, 1,   // 43
		// Bantalan
		0.5,  1.3, -0.5, 0, 0,  // 44
		0.5,  1.3, -0.6, 1, 0,  // 45
		0.5,  1.7, -0.6, 1, 1,  // 46
		0.5,  1.7, -0.5, 0, 1,  // 47
		// Lengan Kanan
		0.5,  0.6, -0.5, 0, 0,  // 48
		0.5,  0.6, -0.6, 1, 0,  // 49
		0.5,  1.3, -0.6, 1, 1,  // 50
		0.5,  1.3, -0.5, 0, 1,  // 51
		// Kaki Kiri
		-0.4, -0.5, 0.5, 0, 0,  // 52
		-0.4, -0.5, 0.4, 1, 0,  // 53
		-0.4,  0.5, 0.4, 1, 1,  // 54
		-0.4,  0.5, 0.5, 0, 1,  // 55
		// Kaki Kiri Belakang
		-0.4, -0.5, -0.5, 0, 0, // 56
		-0.4, -0.5, -0.6, 1, 0, // 57
		-0.4,  0.5, -0.6, 1, 1, // 58
		-0.4,  0.5, -0.5, 0, 1, // 59
		// Lengan Kiri
		-0.4, 0.6, -0.5, 0, 0,  // 60
		-0.4, 0.6, -0.6, 1, 0,  // 61
		-0.4, 1.3, -0.6, 1, 1,  // 62
		-0.4, 1.3, -0.5, 0, 1,  // 63

		//left
		// Kaki Kanan
		0.4, -0.5, 0.5, 0, 0,  // 64
		0.4, -0.5, 0.4, 1, 0,  // 65
		0.4,  0.5, 0.4, 1, 1,  // 66
		0.4,  0.5, 0.5, 0, 1,  // 67
		// Kaki Kanan Belakang
		0.4, -0.5, -0.5, 0, 0,  // 68
		0.4, -0.5, -0.6, 1, 0,  // 69
		0.4,  0.5, -0.6, 1, 1,  // 70
		0.4,  0.5, -0.5, 0, 1,  // 71
		//Alas
		-0.5, 0.5, 0.5, 0, 0,   // 72
		-0.5, 0.5, -0.6, 1, 0,  // 73
		-0.5, 0.6, -0.6, 1, 1,  // 74
		-0.5, 0.6, 0.5, 0, 1,   // 75
		// Bantalan
		-0.5,  1.3, -0.5, 0, 0,  // 76
		-0.5,  1.3, -0.6, 1, 0,  // 77
		-0.5,  1.7, -0.6, 1, 1,  // 78
		-0.5,  1.7, -0.5, 0, 1,  // 79
		// Lengan Kanan
		0.4, 0.6, -0.5, 0, 0,  // 80
		0.4, 0.6, -0.6, 1, 0,  // 81
		0.4, 1.3, -0.6, 1, 1,  // 82
		0.4, 1.3, -0.5, 0, 1,  // 83
		// Kaki Kiri
		-0.5, -0.5, 0.5, 0, 0,  // 84
		-0.5, -0.5, 0.4, 1, 0,  // 85
		-0.5,  0.5, 0.4, 1, 1,  // 86
		-0.5,  0.5, 0.5, 0, 1,  // 87
		// Kaki Kiri Belakang
		-0.5, -0.5, -0.5, 0, 0, // 88
		-0.5, -0.5, -0.6, 1, 0, // 89
		-0.5,  0.5, -0.6, 1, 1, // 90
		-0.5,  0.5, -0.5, 0, 1, // 91
		// Lengan Kiri
		-0.5, 0.6, -0.5, 0, 0,  // 92
		-0.5, 0.6, -0.6, 1, 0,  // 93
		-0.5, 1.3, -0.6, 1, 1,  // 94
		-0.5, 1.3, -0.5, 0, 1,  // 95

		// back
		// Kaki Kanan
		0.4, -0.5, 0.4, 0, 0, // 96
		0.5, -0.5, 0.4, 1, 0, // 97
		0.5,  0.5, 0.4, 1, 1, // 98
		0.4,  0.5, 0.4, 0, 1, // 99
		// Kaki Kiri
		-0.4, -0.5, 0.4, 0, 0, // 100
		-0.5, -0.5, 0.4, 1, 0, // 101
		-0.5,  0.5, 0.4, 1, 1, // 102
		-0.4,  0.5, 0.4, 0, 1, // 103
		// Alas
		-0.5, 0.5, -0.6, 0, 0, // 104
		0.5, 0.5, -0.6, 1, 0,  // 105
		0.5, 0.6, -0.6, 1, 1,  // 106
		-0.5, 0.6, -0.6, 0, 1, // 107
		// Lengan Kiri
		-0.4, 0.6, -0.6, 0, 0, // 108
		-0.5, 0.6, -0.6, 1, 0, // 109
		-0.5, 1.3, -0.6, 1, 1, // 110
		-0.4, 1.3, -0.6, 0, 1, // 111
		// Lengan Kanan
		0.4, 0.6, -0.6, 0, 0,  // 112
		0.5, 0.6, -0.6, 1, 0,  // 113
		0.5, 1.3, -0.6, 1, 1,  // 114
		0.4, 1.3, -0.6, 0, 1,  // 115
		// Bantalan
		-0.5, 1.3, -0.6, 0, 0, // 116
		0.5, 1.3, -0.6, 1, 0,  // 117
		0.5, 1.7, -0.6, 1, 1,  // 118
		-0.5, 1.7, -0.6, 0, 1, // 119
		// Kaki Kanan Belakang
		0.4, -0.5, -0.6, 0, 0,  // 120
		0.5, -0.5, -0.6, 1, 0,  // 121
		0.5, 0.5, -0.6, 1, 1,   // 122
		0.4, 0.5, -0.6, 0, 1,   // 123
		// Kaki Kiri Belakang
		-0.4, -0.5, -0.6, 0, 0, // 124
		-0.5, -0.5, -0.6, 1, 0, // 125
		-0.5, 0.5, -0.6, 1, 1,  // 126
		-0.4, 0.5, -0.6, 0, 1,  // 127

		//up
		// Alas
		-0.5, 0.6,  0.5, 0, 0, // 128
		0.5, 0.6,  0.5, 1, 0,  // 129
		0.5, 0.6, -0.6, 1, 1,  // 130
		-0.5, 0.6, -0.6, 0, 1, // 131
		// Bantalan
		-0.5, 1.3, -0.5, 0, 0, // 132
		0.5, 1.3, -0.5, 1, 0,  // 133
		0.5, 1.7, -0.6, 1, 1,  // 134
		-0.5, 1.7, -0.6, 0, 1, // 135

		//down
		// Alas
		-0.5, 0.5, 0.5, 0, 0, // 136
		0.5, 0.5, 0.5, 1, 0,  // 137
		0.5, 0.5, -0.6, 1, 1,  // 138
		-0.5, 0.5, -0.6, 0, 1, // 139
		// Bantalan
		-0.5, 1.3, -0.5, 0, 0, // 140
		0.5, 1.3, -0.5, 1, 0,  // 141
		0.5, 1.7, -0.6, 1, 1,  // 142
		-0.5, 1.7, -0.6, 0, 1, // 143
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10,  8,  10,  11,  12,  13,  14,  12,  14,  15,  16,  17,  18,  16,  18,  19,  20,  21,  22,  20,  22,  23,  24,  25,  26,  24,  26,  27,  28,  29,  30,  28,  30,  31,// front
		32, 33, 34, 32, 34, 35, 36, 37, 38, 36, 38, 39, 40, 41, 42, 40, 42, 43, 44, 45, 46, 44, 46, 47, 48, 49, 50, 48, 50, 51, 52, 53, 54, 52, 54, 55, 56, 57, 58, 56, 58, 59, 60, 61, 62, 60, 62, 63,//right
		64, 65, 66, 64, 66, 67, 68, 69, 70, 68, 70, 71, 72, 73, 74, 72, 74, 75, 76, 77, 78, 76, 78, 79, 80, 81, 82, 80, 82, 83, 84, 85, 86, 84, 86, 87, 88, 89, 90, 88, 90, 91, 92, 93, 94, 92, 94, 95,//left
		96, 97, 98, 96, 98, 99, 100, 101, 102, 100, 102, 103, 104, 105, 106, 104, 106, 107, 108, 109, 110, 108, 110, 111, 112, 113, 114, 112, 114, 115, 116, 117, 118, 116, 118, 119, 120, 121, 122, 120, 122, 123, 124, 125, 126, 124, 126, 127,//back
		128, 129, 130, 128, 130, 131, 132, 133, 134, 132, 134, 135,//up
		136, 137, 138, 136, 138, 139, 140, 141, 142, 140, 142, 143,//down
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawColoredCube()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glm::mat4 model;
	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	
	glDrawElements(GL_TRIANGLES, 200, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}


void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.001f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}


int main(int argc, char** argv) {
	RenderEngine& app = Demo();
	app.Start("Camera: Free Camera Implementation", 800, 600, false, false);
}