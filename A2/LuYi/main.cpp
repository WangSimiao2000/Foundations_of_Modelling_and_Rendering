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

//VAO 
//用于存储一个VAO的ID
GLuint VertexArrayID;
//Buffers for VAO 
//这是一个VBO（Vertex Buffer Object），用于存储顶点数据，如顶点坐标
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;

GLuint textureID;
GLuint programID;

//>>>>>>>>>>>TASK1
GLuint heighttextrueID;
//>>>>>>>>>>>TASK3
GLuint roughtextureID;
//>>>>>>>>>>>TASK4
GLuint normaltextureID;
//>>>>>>>>>>>TASK5
GLuint snow_texturesID;
GLuint snow_roughtextureID;
GLuint snow_normaltextureID;

GLuint grass_texturesID;
GLuint grass_roughtextureID;
GLuint grass_normaltextureID;

//>>>>>>>>>>TASK6
bool wireframeMode = false;//表示线框模式
bool shaderRecompileRequested = false;
bool rotateLight = false;
float terrainScale = 1.0f;

//TASK : SKYBOX
//vao
GLuint skyboxVAO, skyboxVBO;

GLuint skyboxProgramID;
GLuint skybox_textureID;


//设置一个新的uniform，其中包含一个方向光，你将在计算中使用它
glm::vec3 lightPos = glm::vec3(0.0f, -0.5f, -0.5f);

//E1
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

//E
void LoadModel() {
	// 函数体
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	//这里使用两个嵌套循环生成矩形平面上的顶点坐标和UV坐标。每个顶点都包含一个3D坐标和一个2D的UV坐标
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
				uvs.push_back(vec2(float(i + 0.5f) / float(n_points - 1),float(j + 0.5f) / float(n_points - 1)));
			}
		}
	}

	// 使用索引进行三角形绘制
	//用于启用 OpenGL 的原始重启（Primitive Restart）功能。原始重启是一项功能，允许你在绘制图元时通过在索引缓冲区中插入特定的索引值来重新启动图元的绘制
	glEnable(GL_PRIMITIVE_RESTART);
	//将 std::numeric_limits<std::uint32_t>::max()（UINT32_MAX）作为原始重启的索引值。当OpenGL在绘制时遇到这个特殊的索引值，它将重新启动一个新的图元的绘制。
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
	//用于封装顶点属性和缓冲区的状态
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

//E6
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

//E6
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

void UnloadTextures() { glDeleteTextures(1, &textureID); }

void UnloadShaders() { glDeleteProgram(programID); }

//E3:
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

//Task1:
void LoadHeightMapTextures() {
	// 从"mountains_height.bmp"加载纹理
	int width, height;
	unsigned char* data = nullptr;
	//loadBMP_custom函数加载了一个BMP格式的纹理文件（"rocks.bmp"）。加载后，获取了纹理的宽度和高度，并将像素数据存储在data指针中。
	loadBMP_custom("mountains_height.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	//调用glGenTextures生成一个纹理对象，然后通过glBindTexture将其绑定到当前OpenGL上下文。
	//glPixelStorei用于配置像素存储模式，这里设置为4。接着，使用glTexImage2D将加载的纹理数据传递给OpenGL，并指定其格式为GL_RGB，数据类型为GL_UNSIGNED_BYTE。
	//最后，通过delete[] data释放用于存储纹理数据的内存
	glGenTextures(1, &heighttextrueID);
	glBindTexture(GL_TEXTURE_2D,heighttextrueID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// 定义我们在UV坐标中采样0-1范围之外时
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们放大图像时会发生什么”
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 当我们缩小图像时会发生什么
	glGenerateMipmap(GL_TEXTURE_2D);

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

//Task3：
void LoadRoughTextures() {
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks-r.bmp", width, height, data);

	// 生成和配置纹理对象
	glGenTextures(1, &roughtextureID);
	glBindTexture(GL_TEXTURE_2D, roughtextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// 解绑纹理
	glBindTexture(GL_TEXTURE_2D, -1);
}

//Task4:
void LoadNormalTextrues() {
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks-n.bmp", width, height, data);

	glGenTextures(1, &normaltextureID);
	glBindTexture(GL_TEXTURE_2D, normaltextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, -1);
}

//Task5:
//snow:
void LoadSnowTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow.bmp", width, height, data);

	// 生成和配置纹理对象
	glGenTextures(1, &snow_texturesID);
	glBindTexture(GL_TEXTURE_2D,snow_texturesID);
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

void LoadSnowRoughTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow-r.bmp", width, height, data);

	// 生成和配置纹理对象
	glGenTextures(1, &snow_roughtextureID);
	glBindTexture(GL_TEXTURE_2D, snow_roughtextureID);
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

void LoadSnowNormalTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow-n.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &snow_normaltextureID);
	glBindTexture(GL_TEXTURE_2D, snow_normaltextureID);
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

//grass:
void LoadGrassTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass.bmp", width, height, data);

	// 生成和配置纹理对象
	glGenTextures(1, &grass_texturesID);
	glBindTexture(GL_TEXTURE_2D, grass_texturesID);
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

void LoadGrassRoughTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass-r.bmp", width, height, data);

	// 生成和配置纹理对象
	glGenTextures(1, &grass_roughtextureID);
	glBindTexture(GL_TEXTURE_2D, grass_roughtextureID);
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

void LoadGrassNormalTextures() {
	// 加载纹理
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass-n.bmp", width, height, data);

	// 生成和配置纹理对象
	// 添加到函数
	glGenTextures(1, &grass_normaltextureID);
	glBindTexture(GL_TEXTURE_2D, grass_normaltextureID);
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

//Task6:
//重新编译你的着色器
void RecompileShaders() {
	UnloadShaders();
	LoadShaders(programID, "./Basic.vert", "./Texture.frag");
}



//TASK : SKYBOX


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
	glBindVertexArray(skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



GLuint loadCubemap(vector<std::string> faces)
{
	GLuint SkyboxtextureID;
	glGenTextures(1, &SkyboxtextureID);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxtextureID);

	int width, height, nrChannels;// 纹理的宽度和高度
	unsigned char* image; // 存储加载的图片数据

	// 遍历每个天空盒面的路径
	for (GLuint i = 0; i < faces.size(); i++) {
		
		// 使用stb_image加载每个面的图像
		image = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

		if (image) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
				width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image
			);

			// 释放图像数据
			stbi_image_free(image);
		}
		else {
			std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(image);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // 解绑立方体贴图

	return SkyboxtextureID; // 返回生成的天空盒纹理ID
}


void LoadSkybox() {
	vector<std::string> faces{
		"G:/leeds/rendering/Rasterisation_Empty/right.jpg",
		"G:/leeds/rendering/Rasterisation_Empty/left.jpg",
		"G:/leeds/rendering/Rasterisation_Empty/top.jpg",
		"G:/leeds/rendering/Rasterisation_Empty/bottom.jpg",
		"G:/leeds/rendering/Rasterisation_Empty/front.jpg",
		"G:/leeds/rendering/Rasterisation_Empty/back.jpg",
	};
	skybox_textureID = loadCubemap(faces);
}




int main() {
	if (!initializeGL()) return -1;

	LoadModel();
	LoadTextures();
	//task1
	LoadHeightMapTextures();
	//task3
	LoadRoughTextures();
	//task4
	LoadNormalTextrues();

	//task5
	LoadSnowTextures();
	LoadSnowRoughTextures();
	LoadSnowNormalTextures();

	LoadGrassTextures();
	LoadGrassRoughTextures();
	LoadGrassNormalTextures();


	programID = glCreateProgram();//这行代码创建了一个新的空着色器程序
	LoadShaders(programID, "./Basic.vert", "./Texture.frag");

	glClearColor(0.7f, 0.8f, 1.0f, 0.0f);//这行代码设置了当调用 glClear 函数清除颜色缓冲区时使用的颜色值。
	//这里设置的是一种浅蓝色，这通常被用作天空的颜色
	glEnable(GL_DEPTH_TEST);//启用深度测试,确保更远的物体不会遮挡更近的物体
	glDepthFunc(GL_LESS);//设置了深度测试的条件。
	//GL_LESS 的含义是只有当一个片段的深度值小于深度缓冲区中已有的值时，它才会被绘制
	glEnable(GL_CULL_FACE);//启用了面剔除


	//>>>>>>TASK : skybox

	InitializeSkybox();
	LoadSkybox();// 加载天空盒纹理
	
	//创建了一个新的空着色器程序,因为天空盒比较特殊
	skyboxProgramID = glCreateProgram();
	LoadShaders(skyboxProgramID, "./skybox.vert", "./skybox.frag");

	//>>>>>>END

	do {

		// Clear the screen 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Compute the MVP matrix from keyboard and mouse input 
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		
		

		//First pass: Base mesh 
		glUseProgram(programID);

		// Get a handle for our uniforms 
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		//>>>>TASK1		
		//激活并绑定高度图纹理
		//glActiveTexture 函数用于激活纹理单元。
		//GL_TEXTURE0 表示激活纹理单元0，OpenGL支持多个纹理单元，通常从0开始编号。
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture 函数用于将纹理对象绑定到当前激活的纹理单元上。
		//GL_TEXTURE_2D 表示绑定的是二维纹理。。
		glBindTexture(GL_TEXTURE_2D, heighttextrueID);

		//"heightmapTexture" 是你在着色器中声明的 uniform 变量名。
		GLuint heightID = glGetUniformLocation(programID, "heightmapTexture");
		//0 是你选择的纹理单元编号，这里是 GL_TEXTURE0。
		glUniform1i(heightID, 0);

		//>>>>END


		//>>>>TASK3
		//这段代码主要用于将视图矩阵（ViewMatrix）和模型矩阵（ModelMatrix）传递给着色器程序
		//这是因为在地形着色中，需要在视图坐标系中计算光照，所以需要传递这两个矩阵。

		GLuint ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		GLuint ModelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		//>>>>TASK3
		//加载粗糙度贴图（Roughness Texture）的。在地形着色中，你可能想要使用粗糙度贴图来调整材质的表面粗糙程度
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, roughtextureID);
		//在加载纹理时，确保设置纹理坐标的平铺模式，以防止纹理拉伸。
		//这可以通过设置纹理的GL_TEXTURE_WRAP_S和GL_TEXTURE_WRAP_T参数来完成。
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//>>>>TASK4
		//加载法线贴图
		glActiveTexture(GL_TEXTURE3);
		//glBindTexture 函数用于将纹理对象绑定到当前激活的纹理单元上。
		//GL_TEXTURE_2D 表示绑定的是二维纹理。。
		glBindTexture(GL_TEXTURE_2D, normaltextureID);

		GLuint normalID = glGetUniformLocation(programID, "normalTexture");
		glUniform1i(normalID, 3);

		//>>>>TASK5:加载雪和草贴图
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, snow_texturesID);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, snow_roughtextureID);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, snow_normaltextureID);
		GLuint snow_normalID = glGetUniformLocation(programID, "snow_normalTexture");
		glUniform1i(snow_normalID, 6);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, grass_texturesID);

		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, grass_roughtextureID);

		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, grass_normaltextureID);
		GLuint grass_normalID = glGetUniformLocation(programID, "grass_normalTexture");
		glUniform1i(grass_normalID, 9);


		//加载本身的颜色
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glDrawElements(
			GL_TRIANGLE_STRIP,      // mode 
			(GLsizei)nIndices,    // count 
			GL_UNSIGNED_INT,	// type 
			(void*)0           // element array buffer offset 
		);


		//>>>>TASK6
		// 空格线框
		// Check for key presses
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			// 切换线框模式
			if (!wireframeMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				wireframeMode = true;
			}
		}
		else {
			// 关闭线框模式
			if (wireframeMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				wireframeMode = false;
			}
		}
		//按R键进行重新着色
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			// 触发重新编译着色器
			if (!shaderRecompileRequested) {
				shaderRecompileRequested = true;
				RecompileShaders();
			}
		}
		else {
			shaderRecompileRequested = false;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			lightPos.y += 1.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			lightPos.x -= 1.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			lightPos.y -= 1.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			lightPos.x += 1.0f;
		}
		GLuint lightPosID = glGetUniformLocation(programID, "LightPos");
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			// 放大地形
			terrainScale += 0.1f;
			// 这里需要根据你的实际情况更新地形的缩放
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			// 缩小地形
			terrainScale -= 0.1f;
			// 这里需要根据你的实际情况更新地形的缩放
		}
		GLuint TerrainScaleID = glGetUniformLocation(programID, "terrainScale");
		glUniform1f(TerrainScaleID, terrainScale);


		//TASK:SKYBOX


		glDepthFunc(GL_LEQUAL);

		// Remove translation from the view matrix
		glm::mat4 view = getViewMatrix();
		glm::mat4 projection = getProjectionMatrix();

		view = glm::mat4(glm::mat3(getViewMatrix()));

		glUseProgram(skyboxProgramID);
		// Set uniforms for the view and projection matrices
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "projection"), 1, GL_FALSE, &projection[0][0]);

		// Bind the skybox VAO
		glBindVertexArray(skyboxVAO);
		// Activate the cubemap texture
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_textureID);

		// Disable depth writing to draw the skybox behind everything else
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Re-enable depth writing
		glDepthFunc(GL_LESS);

		//>>>>>>END



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


