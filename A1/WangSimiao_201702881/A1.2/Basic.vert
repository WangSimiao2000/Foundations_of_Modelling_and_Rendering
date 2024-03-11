#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_ocs;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform sampler2D HeightTexture;// Task1: 接收从c++的main()中传入的HeightTexture
out vec3 normalVector;// Task2: 传递给片元着色器
uniform float sceneEdgeLength;// 传入场景的边长
uniform mat4 ViewMatrix;// Task3: 接受ViewMatrix
uniform mat4 ModelMatrix;// Task3: 接受ViewMatrix
out vec3 lightDir_VCS;
out vec3 viewDir_VCS;
out vec3 normalDir_VCS;
out float height;// Task5: 高度
uniform vec3 LightPosition;// 传入光源位置
uniform float heightScale;// Task6: 缩放高度

void main(){
	// Output position of the vertex, in clip space : MVP * position
	//gl_Position =  MVP * vec4(vertexPosition_ocs,1);

	// 当前顶点的纹理坐标系的坐标
	// The coordinates of the texture coordinate system of the current vertex
	UV = vertexUV;
	
	// UV of the vertex. No special space for this one.

	
	/* -- Task1 -- START -- */
	// 获取高度图在当前顶点位置的RGB颜色;
	// Obtain the RGB color of the height map at the current vertex position;
	vec3 heightColor = texture(HeightTexture, UV).rgb;
	// 计算的结果会有点过大了, 将其缩小以便正常观察, 这里就是缩小的权重
	// The calculated result may be a bit too large. Reduce it for normal observation, and here is the reduced weight
	float height_weight = 2500.0;
	// 根据RGB值计算高度
	// Calculate height based on RGB values
	height = (heightColor.r*256*256 + heightColor.g*256 + heightColor.b)*heightScale/height_weight;
	// 将计算出来的高度转换成一个vec3的值
	// Convert the calculated height into a vec3 value
	vec3 vertexPosition = vertexPosition_ocs + vec3(0, height, 0);
	// 输出经过MVP变换后的位置
	// Output the position after MVP transformation
	gl_Position = MVP * vec4(vertexPosition,1.0);
	/* -- Task1 --- END --- */


	/* -- Task2 -- START -- */
	// 当前片元为 *
	//	0 1 2
	//	3 * 5
	//	6 7 8
	// 其他片元为 0 1 2 3 5 6 7 8
	// 获得周围8个点的信息
	// Obtain information from 8 surrounding points
	vec3 heightColor_0 = texture(HeightTexture, UV + vec2(-1.0/sceneEdgeLength,1.0/sceneEdgeLength)).rgb;
	vec3 heightColor_1 = texture(HeightTexture, UV + vec2(0/sceneEdgeLength,1.0/sceneEdgeLength)).rgb;
	vec3 heightColor_2 = texture(HeightTexture, UV + vec2(1.0/sceneEdgeLength,1.0/sceneEdgeLength)).rgb;
	vec3 heightColor_3 = texture(HeightTexture, UV + vec2(-1.0/sceneEdgeLength,0.0/sceneEdgeLength)).rgb;
	vec3 heightColor_5 = texture(HeightTexture, UV + vec2(1.0/sceneEdgeLength,0.0/sceneEdgeLength)).rgb;
	vec3 heightColor_6 = texture(HeightTexture, UV + vec2(-1.0/sceneEdgeLength,-1.0/sceneEdgeLength)).rgb;
	vec3 heightColor_7 = texture(HeightTexture, UV + vec2(0.0/sceneEdgeLength,-1.0/sceneEdgeLength)).rgb;
	vec3 heightColor_8 = texture(HeightTexture, UV + vec2(1.0/sceneEdgeLength,-1.0/sceneEdgeLength)).rgb;
	// 计算各个点位的高度
	// Calculate the height of each point
	float height_0 = (heightColor_0.r*256*256 + heightColor_0.g*256 + heightColor_0.b)*heightScale/height_weight;
	float height_1 = (heightColor_1.r*256*256 + heightColor_1.g*256 + heightColor_1.b)*heightScale/height_weight;
	float height_2 = (heightColor_2.r*256*256 + heightColor_2.g*256 + heightColor_2.b)*heightScale/height_weight;
	float height_3 = (heightColor_3.r*256*256 + heightColor_3.g*256 + heightColor_3.b)*heightScale/height_weight;
	float height_5 = (heightColor_5.r*256*256 + heightColor_5.g*256 + heightColor_5.b)*heightScale/height_weight;
	float height_6 = (heightColor_6.r*256*256 + heightColor_6.g*256 + heightColor_6.b)*heightScale/height_weight;
	float height_7 = (heightColor_7.r*256*256 + heightColor_7.g*256 + heightColor_7.b)*heightScale/height_weight;
	float height_8 = (heightColor_8.r*256*256 + heightColor_8.g*256 + heightColor_8.b)*heightScale/height_weight;
	// 计算在x, z方向的法向量
	// Calculate the normal vector in the x and z directions
	float weight = 2.0;
	float Nx = ((height_0 - height_6) + weight * (height_1 - height_7) + (height_2 - height_8));
	float Ny = 0.2; //sceneEdgeLength
	float Nz = ((height_0 - height_2) + weight * (height_3 - height_5) + (height_6 - height_8));
	// 归一化法向量
	normalVector = normalize(vec3(Nx, Ny, Nz));	
	/* -- Task2 --- END --- */


	/* -- Task3 -- START -- */
	// 获得相机的世界坐标(后四位)
	// Obtain the world coordinates of the camera (last four digits)
	// vec3 viewPos_VCS = vec3(ViewMatrix[3]);
	vec3 viewPos_VCS = vec3(0,0,0);
	// 定义光照光源位置(光照随相机移动)
	// Define the position of the lighting source (lighting moves with the camera)
	// vec3 lightPos_WCS = LightPosition;
	// vec3 lightPos_VCS = lightPos_WCS + viewPos_VCS;
	vec3 lightPos_VCS = LightPosition;
	// 计算当前顶点的VCS坐标
	// Calculate the VCS coordinates of the current vertex
	vec3 vertexPos_VCS = vec3(MVP * vec4(vertexPosition,1.0));
	// 计算光照方向
	// Calculate lighting direction
	lightDir_VCS = normalize(-lightPos_VCS);
	// 计算视线方向
	// Calculate line of sight direction
	// viewDir_VCS = normalize(viewPos_VCS - vertexPos_VCS);
	viewDir_VCS = normalize(vertexPos_VCS);
	// 计算法向量方向
	// Calculate the direction of the normal vector
	normalDir_VCS = normalVector;
	/* -- Task3 --- END --- */
}

