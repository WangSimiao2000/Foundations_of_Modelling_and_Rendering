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

// Variables 
GLFWwindow* window;
static const int window_width = 1920;
static const int window_height = 1080;

static const int n_points = 200; // minimum 2
static const float m_scale = 5;

unsigned int nIndices;

// ����һЩOpenGL����ı���
//VAO 
GLuint VertexArrayID;
//Buffers for VAO 
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;
GLuint textureID;
GLuint programID;  // ���ļ���������

GLuint heightTextureID;//�߶�ͼ
GLuint roughnessTextureID;//��ʯ�ֲڶ�ͼ
GLuint normalMapTexturesID;//��ʯ������ͼ
GLuint snowTexturesID;//ѩ��ͼ
GLuint snowRoughnessTexturesID;//ѩ�ֲڶ���ͼ
GLuint snowNormalMapTexturesID;//ѩ������ͼ
GLuint grassTexturesID;//�ݵ���ͼ
GLuint grassRoughnessTexturesID;//�ݵشֲڶ���ͼ
GLuint grassNormalMapTexturesID;//�ݵط�����ͼ

glm::vec3 lightPosition = glm::vec3(0.0f, -0.5f, -0.5f);
float heightScale = 1.0f;
// ��ȫ�ַ�Χ����һ�����������ڼ�¼��һ֡������״̬
int prevTKeyState = GLFW_RELEASE;
int prevGKeyState = GLFW_RELEASE;


/* -- EXERCISE #1 -- */
// ��ʼ��GLFW�����þ����ض����ԵĴ��ڣ�����ʼ��GLEW������һ������ֵ��ָʾ��ʼ���Ƿ�ɹ�
bool initializeGL() {
	// ������
	// ���GLFW��ʼ���Ƿ�ʧ�ܣ����ʧ�ܣ����ӡ������Ϣ������false
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		return false;
	}
	// ����GLFW���ڴ������ض���ʾ��ָ���˿���ݡ�OpenGL�汾����ǰ�����Ժͺ��������ļ������ԡ�
	// ʹ��ָ���ĳߴ�ͱ��ⴴ������
	glfwWindowHint(GLFW_SAMPLES, 1); // ��ʹ�ÿ����
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

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // ��������ճ���԰����Լ����봦��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// ���ù���Խ�����������ƶ�
	glfwPollEvents(); // ��ѯ�¼�
	glfwSetCursorPos(window, window_width / 2, window_height / 2);// �����λ������Ϊ��Ļ����

	//if (!GLEW_ARB_debug_output) return -1;

}

/* -- EXERCISE #2 -- */
void LoadModel() {
	// ������
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	// ѭ�����ɶ����UV����
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

	// ʹ���������������λ���
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
	
	// ����Vertex Array Object (VAO)
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// ���ö�������
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0],GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 
		3,                  // size (we have x,y,z) 
		GL_FLOAT,           // type of each individual element 
		GL_FALSE,           // normalized? 
		0,                  // stride 
		(void*)0            // array buffer offset 
	);

	// ����UV����
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Generate a buffer for the indices as well 
	// ��������������
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0],GL_STATIC_DRAW);
	
	// ������������
	nIndices = indices.size();
}

/* -- EXERCISE #6 -- */
bool readAndCompileShader(const char* shader_path, const GLuint& id) {
	// ʹ�á�ifstream���������ļ�������һ���ļ�
	string shaderCode;
	ifstream shaderStream(shader_path, std::ios::in);
	if (shaderStream.is_open()) {
		stringstream sstr;
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}
	else {
		cout << "Impossible to open " << shader_path << ". Are you in the right directory ? "<< endl; 
		return false;
	}

	// ����
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
	cout << "Compilation of Shader: " << shader_path << " " << (Result == GL_TRUE ? "Success" :	"Failed!") << endl;
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

void UnloadTextures() { glDeleteTextures(1, &textureID); }

void UnloadShaders() { glDeleteProgram(programID); }

/* -- task1 -- */
void LoadHeightTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("mountains_height.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &heightTextureID);
	glBindTexture(GL_TEXTURE_2D, heightTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- EXERCISE #3 -- */
void LoadTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task3 -- */
void LoadRoughnessTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks-r.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &roughnessTextureID);
	glBindTexture(GL_TEXTURE_2D, roughnessTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task4 -- */
void LoadNormalMapTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks-n.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &normalMapTexturesID);
	glBindTexture(GL_TEXTURE_2D, normalMapTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task5 -- */
void LoadSnowTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &snowTexturesID);
	glBindTexture(GL_TEXTURE_2D, snowTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadSnowRoughnessTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow-r.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &snowRoughnessTexturesID);
	glBindTexture(GL_TEXTURE_2D, snowRoughnessTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadSnowNormalMapTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("snow-n.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &snowNormalMapTexturesID);
	glBindTexture(GL_TEXTURE_2D, snowNormalMapTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadGrassTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &grassTexturesID);
	glBindTexture(GL_TEXTURE_2D, grassTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadGrassRoughnessTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass-r.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &grassRoughnessTexturesID);
	glBindTexture(GL_TEXTURE_2D, grassRoughnessTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

void LoadGrassNormalMapTextures() {
	// ��������
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("grass-n.bmp", width, height, data);

	// ���ɺ������������
	// ��ӵ�����
	glGenTextures(1, &grassNormalMapTexturesID);
	glBindTexture(GL_TEXTURE_2D, grassNormalMapTexturesID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;

	// �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// ����������UV�����в���0-1��Χ֮��ʱ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �����ǷŴ�ͼ��ʱ�ᷢ��ʲô��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// ��������Сͼ��ʱ�ᷢ��ʲô
	glGenerateMipmap(GL_TEXTURE_2D);// ����mipmaps

	// �������
	glBindTexture(GL_TEXTURE_2D, -1);
}

/* -- task6 -- */
// ���¼�����ɫ��
void reloadAndRecompileShaders() {
	// ɾ���ɵ���ɫ������
	glDeleteProgram(programID);

	// ���´����ͱ�����ɫ������
	programID = glCreateProgram();
	LoadShaders(programID, "./Basic.vert", "./Texture.frag");

	// ���������µ���ɫ������
	glUseProgram(programID);

	// ���´��ݱ������������Ϣ
	// ... ����������������´�������Uniform����������

	// ���������߿���Ⱦ״̬���Է�֮ǰ�����˿ո��
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

/* -- task6 -- */
// �ڰ��� W A S D ��ʱ���¹�Դ��λ��
void updateLightPosition() {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		lightPosition.y += 1.0f;  // �����ƶ�
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		lightPosition.x -= 1.0f;  // �����ƶ�
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		lightPosition.y -= 1.0f;  // �����ƶ�
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		lightPosition.x += 1.0f;  // �����ƶ�
	}
}

/* -- task6 -- */
// ���� T / G ʹ���� ���� / ����
void updateTerrainScale() {
	// ����Ⱦѭ����
	int currentTKeyState = glfwGetKey(window, GLFW_KEY_T);
	int currentGKeyState = glfwGetKey(window, GLFW_KEY_G);

	// ֻ�ڵ�ǰ֡���°�������һ֡δ����ʱִ�����Ų���
	if (currentTKeyState == GLFW_PRESS && prevTKeyState == GLFW_RELEASE && heightScale < 2.0f) {
		// ����T��ʱ��������������
		heightScale += 0.1f;
	}

	if (currentGKeyState == GLFW_PRESS && prevGKeyState == GLFW_RELEASE && heightScale > 0.0f) {
		// ����G��ʱ����С��������
		heightScale -= 0.1f;
	}

	// ������һ֡������״̬
	prevTKeyState = currentTKeyState;
	prevGKeyState = currentGKeyState;
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

	programID = glCreateProgram();
	LoadShaders(programID, "./Basic.vert", "./Texture.frag");

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

		//std::cout << ViewMatrix << std::endl;

		//we will add more code here 
		//First pass: Base mesh 
		glUseProgram(programID);

		// Get a handle for our uniforms 
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Task3: �������
		// ��ȡViewMatrix��Uniformλ��
		GLuint ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
		// ��ViewMatrix���ݸ�ƬԪ��ɫ��
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		// ��ȡViewMatrix��Uniformλ��
		GLuint ModelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
		// ��ViewMatrix���ݸ�ƬԪ��ɫ��
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		// Task1: ���ظ߶�ͼ
		//glGetUniformLocation: ����ɫ�����ҵ���Ϊ"HeightTexture"��Uniform����
		//glUniform1i: ��ֵ��ָ����Uniform, ����ֵ����Ϊ1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightTextureID);
		GLuint heightID = glGetUniformLocation(programID, "HeightTexture");
		glUniform1i(heightID, 0);// ����Ԫ������0

		// Task2: �ѳ����ı߳���ֵ��������ɫ��
		GLuint edgeLength = glGetUniformLocation(programID, "sceneEdgeLength");
		glUniform1f(edgeLength, float(n_points));

		//���ر������ɫ
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Task3: ���شֲڶ�ͼ
		// glGetUniformLocation: ����ɫ�����ҵ���Ϊ"HeightTexture"��Uniform����
		// glUniform1i: ��ֵ��ָ����Uniform, ����ֵ����Ϊ1
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, roughnessTextureID);
		// ��������S���꣨ˮƽ���򣩵Ļ��Ʒ�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// ��������T���꣨��ֱ���򣩵Ļ��Ʒ�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Task4: ���ط�����ͼ
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMapTexturesID);

		// Task5: ����ѩ�Ͳݵ�(�ֲڶ�, ����)��ͼ
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

		// Task6: �߿���Ⱦ
		// �ڰ��¿ո��ʱ�����߿���Ⱦ
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			// ���¼��غͱ�����ɫ��
			// ע�⣺������Ҫʵ��һ���������������¼��غͱ��������ɫ������
			reloadAndRecompileShaders();
		}

		// Task6: ʹ��WASD�����ƹ�Դ���ƶ�
		updateLightPosition();
		// ���ݹ�Դλ�ø�������ɫ��
		GLuint lightPositionID = glGetUniformLocation(programID, "LightPosition");
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);

		// Task6: �ı�߶�
		updateTerrainScale();
		// ���ݳ����߶�����ֵ��������ɫ��
		GLuint TerrainScaleID = glGetUniformLocation(programID, "heightScale");
		glUniform1f(TerrainScaleID, heightScale);

		glDrawElements(
			GL_TRIANGLE_STRIP,      // mode 
			(GLsizei)nIndices,    // count 
			GL_UNSIGNED_INT,	// type 
			(void*)0           // element array buffer offset 
		);

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