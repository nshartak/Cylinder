#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.2f, -4.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float fov = 90;//by default field of view is 90 degrees, but can be changed by mouse roller

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//changing FOV by rolling mouse scroller
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 120.0f)
		fov = 120.0f;
}

//changing camera location by using WASD keys to move forward, left, back, right
//use the key Z to go up and X to go down
void processInput(GLFWwindow* window)
{

	const float cameraSpeed = 0.05f; //the speed of camera movement when using keys
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		cameraPos += glm::normalize(cameraUp) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cameraPos -= glm::normalize(cameraUp) * cameraSpeed;
}

//Now we will orbit around the cylinder using Euler angles
bool firstMouse = true;
float yaw = 90.0f;
float pitch = 0.0f;
float lastX = 1024.0 / 2.0;
float lastY = 768.0 / 2.0;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}


//a utility function that loads, compiles and links the shaders

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {




	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}




int main()
{
	constexpr unsigned n = 168;//the number of triangles
	float r = 1;//the radius of the cylinder
	std::cout << "Please give the radius of the cylinder[1]: ";
	std::cin >> r;
	//initializing glfw, glew
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return 1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creating the window
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "CG", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to create window");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW");
		return 1;
	}
	glfwSetCursorPosCallback(window, mouse_callback);//setting mouse callback to update camera
	glfwSetScrollCallback(window, scroll_callback);//setting scroll callback to update camera
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glViewport(0, 0, 1024, 768);//specifying the screen size for the viewport transform

	
	float Z = 4;//the height of the cylinder
	float data[3 * (n + 1)];//this array will hold the vertices of the up circle
	float alfa = 6.28f / n;//2pi/n to get the angle
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = Z;
	for (int i = 0; i < n; ++i)
	{
		float x = r * cos(i * alfa);
		float y = r * sin(i * alfa);
		float z = Z;
		unsigned index = (i + 1) * 3;
		data[index++] = x;
		data[index++] = y;
		data[index++] = z;
	}
	//for (auto i : data)std::cout << i << " ";
	//std::cout << std::endl;
	unsigned index[3 * n];//this array will hold the indexes to generate the up circle
	for (int i = 0; i < 3 * n; ++i)
	{
		index[i++] = 0;
		index[i] = 1 + (i + 1) / 3;
		++i;
		if (i != 3 * n - 1)
			index[i] = index[i - 1] + 1;
		else
			index[i] = 1;

	}
	//for (auto i : index)std::cout << i << " ";
	//changing the names of the abovementioned arrays to more convinent ones
	unsigned indices[std::size(index)];
	std::copy(std::begin(index), std::end(index), indices);
	float vertices[std::size(data)];
	std::copy(std::begin(data), std::end(data), vertices);

	/*for (auto i : indices)std::cout << i << " ";
	std::cout << std::endl;
	for (auto i : vertices)std::cout << i << " ";*/

	////
	//lets load this data to openGL by generating and binding appropriate buffers
	//when drawing we will bind the appropriate VAO for up circle, bottom circle and the walls of cylinder
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//the data is "preserved" in VAO so we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//doing the same thing for the bottom circle

	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	for (int i = 0; i < n; ++i)
	{
		float x = r * cos(i * alfa);
		float y = r * sin(i * alfa);
		float z = 0.0;
		unsigned index = (i + 1) * 3;
		data[index++] = x;
		data[index++] = y;
		data[index++] = z;
	}

	float bvertices[std::size(data)];
	std::copy(std::begin(data), std::end(data), bvertices);

	unsigned int bVBO, bVAO, bEBO;
	glGenVertexArrays(1, &bVAO);
	glGenBuffers(1, &bVBO);
	glGenBuffers(1, &bEBO);

	glBindVertexArray(bVAO);

	glBindBuffer(GL_ARRAY_BUFFER, bVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bvertices), bvertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Now the VAO for bottom circle is ready
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//loading vertex data for the walls(which consists of vertices of bottom and up circles except the central ones)
	float walls[6 * n];
	auto end = std::copy(std::begin(vertices) + 3, std::end(vertices), walls);
	std::copy(std::begin(bvertices) + 3, std::end(bvertices), end);

	//generating indices for the walls by "crossing" appropriate vertices
	std::vector<int> strips;
	for (int i = 0; i < n; ++i)
	{
		int a = i;
		int b = i + 1;
		if (i == n - 1)b = 0;
		int c = a + n;
		int d = b + n;
		strips.push_back(a);
		strips.push_back(c);
		strips.push_back(d);
		strips.push_back(a);
		strips.push_back(b);
		strips.push_back(d);

	}
	int triangles[6 * n];//converting the above vector to an array in order to give it to openGL
	std::copy(strips.begin(), strips.end(), triangles);

	//again generating and binding appropriate buffers
	unsigned int wVBO, wVAO, wEBO;
	glGenVertexArrays(1, &wVAO);
	glGenBuffers(1, &wVBO);
	glGenBuffers(1, &wEBO);

	glBindVertexArray(wVAO);

	glBindBuffer(GL_ARRAY_BUFFER, wVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(walls), walls, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);




	//the (commented)call below is very useful when debugging(to use the line strips)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);//enable z buffer test


	GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	//creating model view and projection matrices(identity in the beginning but will change later)
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);//the light is pure white
	do {

		processInput(window);



		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(fov), (float)1024 / (float)768, 0.1f, 100.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		//passing (possibly modified by key presses/mouse) matrices to shaders
		int modelLoc = glGetUniformLocation(programID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		modelLoc = glGetUniformLocation(programID, "view");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(view));
		modelLoc = glGetUniformLocation(programID, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//giving the light color to shader
		modelLoc = glGetUniformLocation(programID, "lightColor");
		glUniform3f(modelLoc, 1.0f, 1.0f, 1.0f);
		//giving the light position to the shader
		modelLoc = glGetUniformLocation(programID, "lightPos");
		glUniform3f(modelLoc, 0.0f, 0.0f, 0.0f);


		modelLoc = glGetUniformLocation(programID, "viewPos");
		glUniform3f(modelLoc, 0.0f, 0.0f, 0.0f);
		// drawing the bottom circle by binding appropriate VAO
		glBindVertexArray(bVAO);

		modelLoc = glGetUniformLocation(programID, "flag");
		glUniform1i(modelLoc, 0);//hint to generate normals

		glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		// drawing the up circle by binding appropriate VAO
		glBindVertexArray(VAO);

		modelLoc = glGetUniformLocation(programID, "flag");
		glUniform1i(modelLoc, 1);//hint to generate normals

		glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// drawing the walls by binding appropriate VAO
		glBindVertexArray(wVAO);

		modelLoc = glGetUniformLocation(programID, "flag");
		glUniform1i(modelLoc, 2);//hint to generate normals
		glDrawElements(GL_TRIANGLES, std::size(triangles), GL_UNSIGNED_INT, 0);

		//swap buffers, etc.
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);



	glfwTerminate();
	return 0;
}