#include <iostream> 
using namespace std;
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <vector>
#include <limits>
#include "common/utils.hpp"
#include <fstream> 
#include <sstream>
#include <common/controls.hpp> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Variables 
GLFWwindow* window;
static const int window_width = 1920;
static const int window_height = 1080;
static const int n_points = 200; // minimum 2
static const float m_scale = 5;
unsigned int nIndices;

// 声明一些OpenGL所需的变量
//VAO 
GLuint VertexArrayID;
//Buffers for VAO 
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;
GLuint textureID;
GLuint programID;  // 在文件级别声明

GLuint heightTextureID;//高度图
GLuint roughnessTextureID;//岩石粗糙度图
GLuint normalMapTexturesID;//岩石法线贴图
GLuint snowTexturesID;//雪贴图
GLuint snowRoughnessTexturesID;//雪粗糙度贴图
GLuint snowNormalMapTexturesID;//雪法线贴图
GLuint grassTexturesID;//草地贴图
GLuint grassRoughnessTexturesID;//草地粗糙度贴图
GLuint grassNormalMapTexturesID;//草地法线贴图
GLuint skyboxTextureID;//天空盒贴图
GLuint skyboxVAO, skyboxVBO;
GLuint skyboxProgramID;  // 在文件级别声明


glm::vec3 lightPosition = glm::vec3(0.0f, -0.5f, -0.5f);
float heightScale = 1.0f;
// 在全局范围定义一个变量，用于记录上一帧按键的状态
int prevTKeyState = GLFW_RELEASE;
int prevGKeyState = GLFW_RELEASE;


/* -- EXERCISE #1 -- */
// 初始化GLFW，设置具有特定属性的窗口，并初始化GLEW。返回一个布尔值，指示初始化是否成功
bool initializeGL() {
	// 函数体
	// 检查GLFW初始化是否失败，如果失败，则打印错误消息并返回false
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		return false;
	}
	// 设置GLFW窗口创建的特定提示，指定了抗锯齿、OpenGL版本、向前兼容性和核心配置文件等属性。
	// 使用指定的尺寸和标题创建窗口
	glfwWindowHint(GLFW_SAMPLES, 1); // 不使用抗锯齿
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(window_width, window_height, "OpenGLRenderer", NULL, NULL);

	if (window == NULL) {
		cerr << "Failed to open GLFW window. If you have an Intel GPU, they may not be 4.5 compatible." << endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		return false;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // 包括启用粘滞性按键以简化输入处理
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// 禁用光标以进行无限鼠标移动
	glfwPollEvents(); // 轮询事件
	glfwSetCursorPos(window, window_width / 2, window_height / 2);// 将光标位置设置为屏幕中心

	//if (!GLEW_ARB_debug_output) return -1;

}

/* -- EXERCISE #2 -- */
void LoadModel() {
	// 函数体
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	// 循环生成顶点和UV坐标
	for (int i = 0; i < n_points; i++) {
		// ...
		for (int i = 0; i < n_points; i++)
		{
			float x = (m_scale) * ((i / float(n_points - 1)) - 0.5f) * 2.0f;
			for (int j = 0; j < n_points; j++)
			{
				float z = (m_scale) * ((j / float(n_points - 1)) - 0.5f) * 2.0f;
				vertices.push_back(vec3(x, 0, z));
				uvs.push_back(vec2(float(i + 0.5f) / float(n_points - 1),
					float(j + 0.5f) / float(n_points - 1)));
			}
		}
	}

	// 使用索引进行三角形绘制
	glEnable(GL_PRIMITIVE_RESTART);
	constexpr unsigned int restartIndex = std::numeric_limits<std::uint32_t>::max();
	glPrimitiveRestartIndex(restartIndex);
	int n = 0;
	for (int i = 0; i < n_points - 1; i++)
	{
		for (int j = 0; j < n_points; j++)
		{
			unsigned int topLeft = n;
			unsigned int bottomLeft = topLeft + n_points;
			indices.push_back(bottomLeft);
			indices.push_back(topLeft);
			n++;
		}
		indices.push_back(restartIndex);
	}

	// 生成Vertex Array Object (VAO)
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// 配置顶点属性
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 
		3,                  // size (we have x,y,z) 
		GL_FLOAT,           // type of each individual element 
		GL_FALSE,           // normalized? 
		0,                  // stride 
		(void*)0            // array buffer offset 
	);

	// 配置UV坐标
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Generate a buffer for the indices as well 
	// 配置索引缓冲区
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// 保存索引数量
	nIndices = indices.size();
}

/* -- EXERCISE #6 -- */
bool readAndCompileShader(const char* shader_path, const GLuint& id) {
	// 使用“ifstream”（输入文件流）打开一个文件
	string shaderCode;
	ifstream shaderStream(shader_path, std::ios::in);
	if (shaderStream.is_open()) {
		stringstream sstr;
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}
	else {
		cout << "Impossible to open " << shader_path << ". Are you in the right directory ? " << endl;
		return false;
	}

	// 编译
	cout << "Compiling shader :" << shader_path << endl;
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(id, 1, &sourcePointer, NULL);
	glCompileShader(id);

	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(id, InfoLogLength, NULL, &shaderErrorMessage[0]);
		cout << &shaderErrorMessage[0] << endl;
	}
	cout << "Compilation of Shader: " << shader_path << " " << (Result == GL_TRUE ? "Success" : "Failed!") << endl;
	return Result == 1;
}

/* -- EXERCISE #6 -- */
void LoadShaders(GLuint& program, const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders - tasks 1 and 2  
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	bool vok = readAndCompileShader(vertex_file_path, VertexShaderID);
	bool fok = readAndCompileShader(fragment_file_path, FragmentShaderID);

	if (vok && fok) {
		GLint Result = GL_FALSE;
		int InfoLogLength;

		cout << "Linking program" << endl;
		program = glCreateProgram();
		glAttachShader(program, VertexShaderID);
		glAttachShader(program, FragmentShaderID);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &Result);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			cout << &ProgramErrorMessage[0];
		}
		std::cout << "Linking program: " << (Result == GL_TRUE ? "Success" : "Failed!") << std::endl;
	}
	else {
		std::cout << "Program will not be linked: one of the shaders has an error" << std::endl;
	}
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

}

void UnloadModel()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void UnloadTextures() { 
	glDeleteTextures(1, &textureID); 
	glDeleteTextures(1, &skyboxTextureID);
}

void UnloadShaders() { 
	glDeleteProgram(programID); 
	glDeleteProgram(skyboxProgramID);
}

/* -- task1 -- */
void LoadHeightTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("mountains_height.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &heightTextureID);
	glBindTexture(GL_TEXTURE_2D, heightTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- EXERCISE #3 -- */
void LoadTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task3 -- */
void LoadRoughnessTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks-r.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &roughnessTextureID);
	glBindTexture(GL_TEXTURE_2D, roughnessTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task4 -- */
void LoadNormalMapTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks-n.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &normalMapTexturesID);
	glBindTexture(GL_TEXTURE_2D, normalMapTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task5 -- */
void LoadSnowTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &snowTexturesID);
	glBindTexture(GL_TEXTURE_2D, snowTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadSnowRoughnessTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow-r.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &snowRoughnessTexturesID);
	glBindTexture(GL_TEXTURE_2D, snowRoughnessTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadSnowNormalMapTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow-n.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &snowNormalMapTexturesID);
	glBindTexture(GL_TEXTURE_2D, snowNormalMapTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadGrassTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &grassTexturesID);
	glBindTexture(GL_TEXTURE_2D, grassTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadGrassRoughnessTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass-r.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &grassRoughnessTexturesID);
	glBindTexture(GL_TEXTURE_2D, grassRoughnessTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadGrassNormalMapTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass-n.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &grassNormalMapTexturesID);
	glBindTexture(GL_TEXTURE_2D, grassNormalMapTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);// 生成mipmaps

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task6 -- */
// 重新加载着色器
void reloadAndRecompileShaders() {
	// 删除旧的着色器程序
	// 重新创建和编译着色器程序
	glDeleteProgram(programID);
	programID = glCreateProgram();
	LoadShaders(programID, "./Basic.vert", "./Texture.frag");

	glDeleteProgram(skyboxProgramID);
	skyboxProgramID = glCreateProgram();
	LoadShaders(skyboxProgramID, "./skybox.vert", "./skybox.frag");

	// 重新设置新的着色器程序
	glUseProgram(programID);
	glUseProgram(skyboxProgramID);

	// 重新传递变量和纹理等信息
	// ... （根据你的需求重新传递其他Uniform变量和纹理）

	// 重新启用线框渲染状态，以防之前按下了空格键
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

/* -- task6 -- */
// 在按下 W A S D 键时更新光源的位置
void updateLightPosition() {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		lightPosition.y += 1.0f;  // 向上移动
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		lightPosition.x -= 1.0f;  // 向左移动
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		lightPosition.y -= 1.0f;  // 向下移动
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		lightPosition.x += 1.0f;  // 向右移动
	}
}

/* -- task6 -- */
// 按下 T / G 使地面 升高 / 降低
void updateTerrainScale() {
	// 在渲染循环中
	int currentTKeyState = glfwGetKey(window, GLFW_KEY_T);
	int currentGKeyState = glfwGetKey(window, GLFW_KEY_G);

	// 只在当前帧按下按键而上一帧未按下时执行缩放操作
	if (currentTKeyState == GLFW_PRESS && prevTKeyState == GLFW_RELEASE && heightScale < 2.0f) {
		// 按下T键时，增加缩放因子
		heightScale += 0.1f;
	}

	if (currentGKeyState == GLFW_PRESS && prevGKeyState == GLFW_RELEASE && heightScale > 0.0f) {
		// 按下G键时，减小缩放因子
		heightScale -= 0.1f;
	}

	// 更新上一帧按键的状态
	prevTKeyState = currentTKeyState;
	prevGKeyState = currentGKeyState;
}

void InitializeSkybox() {
	// 天空盒顶点数据
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// 设置天空盒VAO和VBO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(skyboxVertices),
		skyboxVertices,
		GL_STATIC_DRAW);

	// 其余的OpenGL设置和顶点属性指针等步骤...
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//// 解绑缓冲和VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int skytextureID;
	glGenTextures(1, &skytextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skytextureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return skytextureID;
}

void loadSkybox() {
	vector<std::string> faces{
		"./skyboxPic/right.jpg",
		"./skyboxPic/left.jpg",
		"./skyboxPic/top.jpg",
		"./skyboxPic/bottom.jpg",
		"./skyboxPic/front.jpg",
		"./skyboxPic/back.jpg",
	};

	skyboxTextureID = loadCubemap(faces);
}

int main() {
	if (!initializeGL()) return -1;

	LoadModel();
	LoadHeightTextures();

	LoadTextures();
	LoadRoughnessTextures();
	LoadNormalMapTextures();

	LoadSnowTextures();
	LoadSnowRoughnessTextures();
	LoadSnowNormalMapTextures();

	LoadGrassTextures();
	LoadGrassRoughnessTextures();
	LoadGrassNormalMapTextures();

	InitializeSkybox();
	loadSkybox();

	programID = glCreateProgram();
	LoadShaders(programID, "./Basic.vert", "./Texture.frag");

	skyboxProgramID = glCreateProgram();
	LoadShaders(skyboxProgramID, "./skybox.vert", "./skybox.frag");

	glClearColor(0.7f, 0.8f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	do {
		// Clear the screen 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Compute the MVP matrix from keyboard and mouse input 
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;



		//we will add more code here 
		//First pass: Base mesh 
		glBindVertexArray(VertexArrayID);
		glUseProgram(programID);

		// Get a handle for our uniforms 
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Task3: 传入矩阵
		// 获取ViewMatrix的Uniform位置
		GLuint ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
		// 将ViewMatrix传递给片元着色器
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		// 获取ViewMatrix的Uniform位置
		GLuint ModelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
		// 将ViewMatrix传递给片元着色器
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		// Task1: 加载高度图
		//glGetUniformLocation: 在着色器中找到名为"HeightTexture"的Uniform变量
		//glUniform1i: 传值给指定的Uniform, 将其值设置为1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightTextureID);
		GLuint heightID = glGetUniformLocation(programID, "HeightTexture");
		glUniform1i(heightID, 0);// 纹理单元的索引0

		// Task2: 把场景的边长传值给顶点着色器
		GLuint edgeLength = glGetUniformLocation(programID, "sceneEdgeLength");
		glUniform1f(edgeLength, float(n_points));

		//加载本身的颜色
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Task3: 加载粗糙度图
		// glGetUniformLocation: 在着色器中找到名为"HeightTexture"的Uniform变量
		// glUniform1i: 传值给指定的Uniform, 将其值设置为1
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, roughnessTextureID);
		// 设置纹理S坐标（水平方向）的环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// 设置纹理T坐标（垂直方向）的环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Task4: 加载法线贴图
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMapTexturesID);

		// Task5: 加载雪和草地(粗糙度, 法线)贴图
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, snowTexturesID);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, snowRoughnessTexturesID);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, snowNormalMapTexturesID);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, grassTexturesID);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, grassRoughnessTexturesID);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, grassNormalMapTexturesID);


		// Task6: 线框渲染
		// 在按下空格键时启用线框渲染
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			// 重新加载和编译着色器
			// 注意：这里需要实现一个函数，用于重新加载和编译你的着色器程序
			reloadAndRecompileShaders();
		}

		//fix problem

		glDrawElements(
			GL_TRIANGLE_STRIP,      // mode 
			(GLsizei)nIndices,    // count 
			GL_UNSIGNED_INT,	// type 
			(void*)0           // element array buffer offset 
		);

		// Task6: 使用WASD键控制光源的移动
		updateLightPosition();
		// 传递光源位置给顶点着色器
		GLuint lightPositionID = glGetUniformLocation(programID, "LightPosition");
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);

		// Task6: 改变高度
		updateTerrainScale();
		// 传递场景高度缩放值给顶点着色器
		GLuint TerrainScaleID = glGetUniformLocation(programID, "heightScale");
		glUniform1f(TerrainScaleID, heightScale);


		// A2: 天空盒 Skybox 
		// Ensure that the Sky Box is drawn after other objects in the scene
		glDepthFunc(GL_LEQUAL);
		glUseProgram(skyboxProgramID);
		// The translation matrix was removed from the view matrix. Ensure that the Sky Box renders only relative to the direction of the camera, without considering camera position, to create an infinitely distant sky effect
		glm::mat4 skyViewMatrix = glm::mat4(glm::mat3(ViewMatrix));
		// Projection Matrix
		glm::mat4 skyProjectionMatrix = ProjectionMatrix;
		// Set the corresponding view and projection matrix as uniform variables in the shader
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "skyViewMatrix"), 1, GL_FALSE, &skyViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "skyProjectionMatrix"), 1, GL_FALSE, &skyProjectionMatrix[0][0]);
		// Bind the skybox VAO
		glBindVertexArray(skyboxVAO);
		// Activate the cubemap texture
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
		// Disable depth writing to draw the skybox behind everything else
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Re-enable depth writing
		glDepthFunc(GL_LESS);

		// Swap buffers 
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	UnloadModel();
	UnloadShaders();
	UnloadTextures();
	glfwTerminate();

	return 0;
}