#version 420 core

// Input
in vec2 UV;
// Output
out vec3 color;
layout (binding=1) uniform sampler2D DiffuseTextureSampler;// 注意这里binding改为了1, 因为c++里修改了岩石纹理的层级: glGenTextures(1, &textureID);
in vec3 normalVector;// Task2: 从顶点着色器中接受传递过来的法向量?
layout (binding=2) uniform sampler2D RoughnessTextureSampler;// Task3: 获取粗糙度纹理 RoughnessTextureSampler
in vec3 lightDir_VCS;
in vec3 viewDir_VCS;
in vec3 normalDir_VCS;
in float height;// 高度
layout (binding=4) uniform sampler2D SnowTextureSampler;
layout (binding=5) uniform sampler2D SnowRoughnessTextureSampler;
layout (binding=7) uniform sampler2D GrassTextureSampler;
layout (binding=8) uniform sampler2D GrassRoughnessTextureSampler;

layout (binding=3) uniform sampler2D NormalMapTexture;// Task4: 法线贴图
layout (binding=6) uniform sampler2D SnowNormalMapTexture;//法线贴图
layout (binding=9) uniform sampler2D GrassNormalMapTexture;//法线贴图

void main(){
	// 输出岩石的颜色
	// color = texture(DiffuseTextureSampler,vec2(UV.x,UV.y)).rgb;
	

	/* -- Task2 -- START -- */
	// 输出法线的颜色, 需将其他修改颜色的注释掉
	// color = vec3(abs(normalVector.x), abs(normalVector.y), abs(normalVector.z));
	/* -- Task2 --- END --- */


	// 新建光照颜色,光照强度
	// New lighting color, lighting intensity
	vec3 LightColor = vec3(1, 1, 1);
	float LightPower = 1.5;
	// 由于需要纹理平铺而不是拉伸, 所以调整UV坐标, 并设置重复平铺的次数
	// Due to the need for texture tiling instead of stretching, adjust the UV coordinates and set the number of times to repeat tiling
	int repeatTimes = 10;
	// 使用fract函数确保UV坐标在[0,1]范围内
	// Use the fragment function to ensure that the UV coordinates are within the range of [0,1]
	vec2 tiledUV = fract(UV * repeatTimes);
	// 通过灰度图获取粗糙度Roughness的值
	// Obtaining roughness values through grayscale images
	float roughness = texture(RoughnessTextureSampler,vec2(tiledUV.x,tiledUV.y)).r;
	float snowRoughness = texture(SnowRoughnessTextureSampler,vec2(tiledUV.x,tiledUV.y)).r;
	float grassRoughness = texture(GrassRoughnessTextureSampler,vec2(tiledUV.x,tiledUV.y)).r;
	// 计算光泽度
	// Calculate shinines
	float shininess = clamp((2 / (pow(roughness, 4) + 1e-2)) - 2, 0, 500.0f);
	// 新建漫反射Diffuse颜色, 即是它本身显示的纹理的颜色, 这里只是为了代码可读性
	// Create a new diffuse Diffuse color, which is the color of the texture it displays.
	vec3 diffuseColor = texture(DiffuseTextureSampler,vec2(tiledUV.x,tiledUV.y)).rgb;
	vec3 snowDiffuseColor = texture(SnowTextureSampler,vec2(tiledUV.x,tiledUV.y)).rgb;
	vec3 grassDiffuseColor = texture(GrassTextureSampler,vec2(tiledUV.x,tiledUV.y)).rgb;
	// 定义镜面反射Specular颜色
	// Defining Specular Colors
	vec3 specularColor = vec3(0.01, 0.01, 0.01);
	// vec3 specularColor = vec3(0.5, 0.5, 0.5);


	// 由于使用的是高度图, 所以tangent是(1,0,0),bitangent是(0,0,1),Normal是Task2里计算的结果
	// Due to the use of a height map, the tangent is (1,0,0), the bigent is (0,0,1), and Normal is the result calculated in Task 2
	vec3 normal_TBN = normalDir_VCS; 
	vec3 tangent_TBN =  vec3(1,0,0); 
	vec3 bitangent_TBN = vec3(0,0,1); 
	// 使用Gram-Schmidt orthogonalization方法来使TBN矩阵正交
	// Using the Gram Schmidt orthogonalization method to make the TBN matrix orthogonal
	normal_TBN = normal_TBN;
	tangent_TBN = normalize(tangent_TBN - dot(normal_TBN,tangent_TBN));//tangent_TBN * 
	bitangent_TBN = normalize(cross(tangent_TBN,normal_TBN));
	// 构建修正后的 TBN 矩阵
	// Construct the revised TBN matrix
	mat3 TBN = mat3(tangent_TBN,bitangent_TBN,normal_TBN);


	/* -- Task4 -- START -- */
	// 获取法线贴图当前值的颜色
	// Get the color of the current value of the normal map
	vec3 normalMapColor = texture(NormalMapTexture, tiledUV ).rgb;
	// rgb值范围是[0,1], 但切线空间中和法线的坐标范围是[-1,1]
	// The range of RGB values is [0,1], but the coordinate range of the tangent space and normal is [-1,1]
	vec3 normalMapValue = normalMapColor * 2.0 - 1.0;
	// 将法线贴图的法线转换到viewModel中
	// Convert the normals of normal maps to viewModel
	vec3 normalDir_VCS = normalize(TBN * normalMapValue);
	/* -- Task4 --- END --- */
	

	/* -- Task5 -- START -- */
	// 获取雪地法线贴图的当前颜色
	// Obtain the current color of the snow normal map
	vec3 snowNormalMapColor = texture(SnowNormalMapTexture, tiledUV).rgb;
	// rgb值范围是[0,1], 但切线空间中和法线的坐标范围是[-1,1]
	// The range of RGB values is [0,1], but the coordinate range of the tangent space and normal is [-1,1]
	vec3 snowNormalMapValue = snowNormalMapColor * 2.0 - 1.0;
	// 将雪地法线贴图的法线转换到viewModel中
	// Convert the normals of the snow normal map to viewModel
	vec3 snowNormalDir_VCS = normalize(TBN * snowNormalMapValue);
	// snowNormalDir_VCS = abs(snowNormalDir_VCS);

	// 获取草地法线贴图的当前颜色
	// Obtain the current color of the grassland normal map
	vec3 grassNormalMapColor = texture(GrassNormalMapTexture,tiledUV).rgb;
	// rgb值范围是[0,1], 但切线空间中和法线的坐标范围是[-1,1]
	// The range of RGB values is [0,1], but the coordinate range of the tangent space and normal is [-1,1]
	vec3 grassNormalMapValue = grassNormalMapColor * 2.0 - 1.0;
	// 将草地法线贴图的法线转换到viewModel中
	// Convert the normals of the grassland normal map to viewModel
	vec3 grassNormalDir_VCS = normalize(TBN * grassNormalMapValue);
	// grassNormalDir_VCS = abs(grassNormalDir_VCS);
	/* -- Task5 --- END --- */

	/* -- Task3 -- START -- */
	/* -- Task4 -- START -- */
	// 实现Blinn-Phong着色
	// Implementing Blinn Phong shading
	// 计算漫反射项
	// Calculate the diffuse reflection term
	float diff = clamp(dot(normalDir_VCS, lightDir_VCS), 0, 1);
    vec3 diffuse = diffuseColor * LightColor * LightPower * diff;
	// 计算镜面反射项
	// Calculate the specular reflection term
	// 半程向量
	vec3 halfwayDir = normalize(lightDir_VCS + viewDir_VCS);
    float spec = clamp(pow(clamp(dot(normalDir_VCS, halfwayDir), 0, 1), shininess),0 ,1);
    vec3 specular = specularColor * LightColor * LightPower * spec;
	// 计算环境光
	// Calculate ambient light
	vec3 AmbientColor = vec3(0.1,0.1,0.1) * diffuseColor;
	// 计算最终颜色
	// Calculate final color
	vec3 rockColor = diffuse + specular + AmbientColor;
	//vec3 rockColor = specular;
	// color = diffuse + specular + AmbientColor;
	/* -- Task3 --- END --- */	
	/* -- Task4 --- END --- */
	

	/* -- Task5 -- START -- */
	// 计算雪地漫反射项
	// Calculate the snow diffuse reflection term
	float snowDiff = clamp(dot(snowNormalDir_VCS, lightDir_VCS), 0, 1);
    vec3 snowDiffuse = snowDiffuseColor * LightColor * LightPower * snowDiff;
	// 计算雪地镜面反射项
	// Calculate the specular reflection term on snow surfaces
	float snowSpec = clamp(pow(clamp(dot(snowNormalDir_VCS, halfwayDir), 0, 1), shininess),0 ,1);
	vec3 snowSpecular = specularColor * LightColor * LightPower * snowSpec;
	// 计算雪地环境光
	// Calculate snow ambient light
	vec3 snowAmbientColor = vec3(0.1,0.1,0.1) * snowDiffuseColor;
	// 计算雪地最终颜色
	// Calculate the final color of the snow
	vec3 snowColor = snowDiffuse + snowSpecular + snowAmbientColor;
	// vec3 snowColor = snowSpecular;

	// 计算草地漫反射项
	// Calculate the diffuse reflectance term of grassland
	float grassDiff = clamp(dot(grassNormalDir_VCS, lightDir_VCS), 0, 1);
    vec3 grassDiffuse = grassDiffuseColor * LightColor * LightPower * grassDiff;
	// 计算草地镜面反射项
	// Grass mirror reflection term
	float grassSpec = clamp(pow(clamp(dot(grassNormalDir_VCS, halfwayDir), 0, 1), shininess),0 ,1);
	vec3 grassSpecular = specularColor * LightColor * LightPower * grassSpec;
	// 计算草地环境光
	// Calculate grassland ambient light
	vec3 grassAmbientColor = vec3(0.1,0.1,0.1) * grassDiffuseColor;
	// 计算草地最终颜色
	// Calculate the final color of the grassland
	vec3 grassColor = grassDiffuse + grassSpecular + grassAmbientColor;
	// vec3 grassColor = grassSpecular;

	// 定义过度高度
	// Define excessive height
	float snowRockStart = 2;
	float snowRockEnd = 2.5;
	float rockGrassStart = 0.5;
	float rockGrassEnd = 1;
	if(height < rockGrassStart){
		//在草地上
		color = grassColor;
	}else if(height > rockGrassStart && height < rockGrassEnd){
		//在草地与岩石过度上
		color = mix(grassColor, rockColor, smoothstep(rockGrassStart, rockGrassEnd, height));
	}else if(height > rockGrassEnd && height < snowRockStart){
		//在岩石上
		color = rockColor;
	}else if(height > snowRockStart && height < snowRockEnd){
		//在岩石与雪地过度上
		color = mix(rockColor, snowColor, smoothstep(snowRockStart, snowRockEnd, height));
	}else{
		//在雪地上
		color = snowColor;
	}
	/* -- Task5 --- END --- */

	// color = normalVector;
    // color =  normalDir_VCS;
    // color =  grassNormalDir_VCS;
    // color =  snowNormalDir_VCS;
	//color = vec3(abs(normalVector.x), abs(normalVector.y), abs(normalVector.z));

}