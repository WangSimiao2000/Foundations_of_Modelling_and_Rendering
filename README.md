# Rasterisation 光栅化

## Setup 环境配置和初始化

可以使用 premake5 进行环境配置和初始化, 也可以直接使用Visual Studio打开.sln文件

## ScreenShots 截图

不加载法线贴图时的法线可视化:

![image](https://github.com/user-attachments/assets/a44b55fe-4fee-4faf-9987-ee261a747d93)

加载岩石法线贴图时的法线可视化:

![image](https://github.com/user-attachments/assets/8335f1b9-ca35-4327-8787-2061ede3d81a)

根据高度切换贴图颜色+加载法线贴图:

![image](https://github.com/user-attachments/assets/0fdaff86-85db-48ba-bfca-49d09878b565)

显示三角网格:

![image](https://github.com/user-attachments/assets/056de2e6-fde2-4893-b275-9a3a59342745)

## Key Features Implementation Summary 重点功能实现总结

### 1. 高度图转换为顶点坐标

在顶点着色器中, 获取高度图的像素值, 其RGB值表示高度, 将r乘以256*256, g乘以256, b相加, 再乘以高度缩放系数, 得到高度值

```glsl
vec3 heightColor = texture(HeightTexture, UV).rgb;
float height_weight = 2500.0;
height = (heightColor.r*256*256 + heightColor.g*256 + heightColor.b)*heightScale/height_weight;
```

将以高度图为基础计算出的高度值加到原始的均匀分布的顶点网格坐标上, 得到新的顶点坐标(此时仍是在对象坐标系下)

```glsl
vec3 vertexPosition = vertexPosition_ocs + vec3(0, height, 0);
```

将新的顶点坐标通过乘以MVP矩阵转换到裁剪坐标系下

```glsl
gl_Position = MVP * vec4(vertexPosition,1.0);
```

### 2. 法线贴图

TBN矩阵是由法线, 切线, 副切线构成的矩阵, 用于将切线空间下的法线转换到世界空间下的法线

```glsl
vec3 normal_TBN = normalDir_VCS; 
vec3 tangent_TBN =  vec3(1,0,0); 
vec3 bitangent_TBN = vec3(0,0,1);
```

Gram-Schmidt 正交化方法

```glsl
normal_TBN = normal_TBN;
tangent_TBN = normalize(tangent_TBN - dot(normal_TBN,tangent_TBN));//tangent_TBN * 
bitangent_TBN = normalize(cross(tangent_TBN,normal_TBN));

mat3 TBN = mat3(tangent_TBN,bitangent_TBN,normal_TBN);
```

法线贴图的信息是在切线空间下的, 通过切线空间下的法线贴图, 将法线贴图的RGB值转换为切线空间下的法线值

注意: 法线贴图的RGB值范围是[0,1], 而切线空间中和法线的坐标范围是[-1,1], 所以要通过乘以2再减去1, 得到[-1,1]的范围

```glsl
vec3 normalMapColor = texture(NormalMapTexture, tiledUV ).rgb;
vec3 normalMapValue = normalMapColor * 2.0 - 1.0;
```

将法线贴图的法线值转换到世界空间下的法线值

```glsl
vec3 normalDir_VCS = normalize(TBN * normalMapValue);
```

### 3. blinn-phong光照模型

blinn-phong光照模型是由环境光, 漫反射, 镜面反射(高光)三部分组成

lightColor = ambientColor + diffuseColor + specularColor

# Raytracing 光线追踪

## Setup 环境配置和初始化

1. 下载QT及 QT Creater, 链接: https://www.qt.io/download-dev?hsLang=en
2. 安装过程注意选择 QT 5.13.0 或接近的稳定版本, 
3. 双击打开 Raytracing\RaytraceRenderWindow 路径下的 RaytraceRenderWindow.pro 文件
4. 在配置项目处, 选择 Desktop Qt 5.13.0 MinGW 32-bit (或其他版本), 勾选 Debug 和 Release, 点击Configure Project
5. 点击QT Creater左侧项目扳手图标, 在构建和运行处, 选择 Desktop Qt 5.13.0 MinGW 32-bit (或其他版本), 选择运行, 在Command line arguments处输入.obj文件和.mtl文件的路径(该文件在与RaytraceRenderWindow文件夹同级的objects文件夹中), 用空格隔开
6. 点击左侧绿色三角形按钮, 运行程序
7. 在出现的窗口中, 勾选中间选框, 点击右侧 Raytrace 按钮, 即可开始渲染光线追踪画面

## Functionality 已实现的按钮功能 

1. Interpolation: 显示法线
2. Phong: 光照模型
3. Shadow: 阴影
4. Reflection: 反射
5. Refraction: 折射
6. Fresnel: 菲涅尔效应
7. Monte Carlo: 蒙特卡洛采样
8. Orthographic: 正交投影

>注意: 此仓库代码在上传前由于丢失, 折射及以后部分只有截图保存, 代码已经实现, 但无法展示

To achieve transparent materials and the Fresnel effect, I utilized the RefractionAndFresnel function in the code. Firstly, this function calculates the color of the surface itself, including ambient light, emissive light, and diffuse and specular reflection from the light source. Then, by determining whether the object is transparent, it decides whether to perform calculations for refraction and reflection. 

为了实现透明材质和菲涅尔效应，我在代码中使用了 `RefractionAndFresnel` 函数。首先，该函数计算表面自身的颜色，包括环境光、自发光，以及来自光源的漫反射和镜面反射。然后，通过判断物体是否透明，决定是否进行折射和反射的计算。

In the case of transparency, I employ the law of refraction to calculate the refracted angle and use the Schlick approximation formula to compute the reflection and transmission coefficients. Subsequently, based on the size of the refracted angle, it is determined whether total internal reflection occurs. If the refracted angle is less than 90°, refraction calculations are performed; otherwise, total internal reflection occurs, and the diffuse color of the object is used. Finally, the colors obtained from reflection and refraction calculations are combined according to the weights specified by the Fresnel equation, resulting in the ultimate color.

在透明的情况下，我应用了折射定律来计算折射角，并使用 Schlick 近似公式计算反射和透射系数。接着，依据折射角的大小，判断是否发生全内反射。如果折射角小于 90°，则进行折射计算；否则，将发生全内反射，并使用物体的漫反射颜色。最后，依据菲涅尔方程中指定的权重，结合反射和折射计算得到的颜色，从而生成最终的颜色效果。

## ScreenShots 截图

显示法线方向:

![Normal](Pic/Normal.png)

左右两侧墙壁反射+球体反射+阴影:

![Reflection_3](Pic/Reflection_Wall_Ball.png)

透明物体折射+反射: 

![Reflection_Refraction](Pic/Reflection_Refraction.png)

透明物体折射+反射+阴影: 

![Reflection_Refraction_Shadow](Pic/Reflection_Refraction_Shadow.png)

## Key Features Implementation Summary 重点功能实现总结

### 1. 计算相机射线的方向

计算屏幕比例, 转换到标准化设备坐标系(NDCS)下, 根据屏幕比例调整坐标

光线的起点是相机位置, 方向是从相机位置到屏幕上的点

如果是正交投影, 则光线方向是固定的, 与相机位置无关

### 2. 找到与光线相交的最近的三角形及距离

初始化一个最近距离为无穷大的变量, 遍历所有三角形, 计算光线与三角形的交点, 如果交点在三角形内部, 且距离小于最近距离, 则更新最近距离和最近三角形

如果未击中任何三角形, 则返回的距离值为-1

### 3. 光线与三角形相交检测

光线可以表示为: P(t) = r.origin + t * r.direction

- P(t)是光线上的点
- r.origin是光线的起点
- r.direction是光线的方向
- t是光线的长度

三角形所在平面的方程: (P-P0)*N = 0

- P是平面上的任意点
- P0是平面上的一个已知点(可以用三角形的其中一个顶点)
- N是平面的法向量

将光线方程代入平面方程, 得到光线与平面的交点

(r.origin + t * r.direction - P0) * N = 0

t = (P0 - r.origin) * N / (r.direction * N)

P = r.origin + t * r.direction

```cpp
float result = (verts[0].Point() - r.origin).dot(nVector) / r.direction.dot(nVector);
Cartesian3 P = r.origin + r.direction * result;
```

计算三角形的边向量, 将其与顶点指向P的向量进行叉乘, 如果结果同向(点乘大于0), 则P在三角形内部

```cpp
if(nAPAB.dot(nBPBC) >= 0 && nBPBC.dot(nCPCA) >= 0){ return result;}
else{ return -1.0f;}
```

### 4. 反射光线的计算

反射光线的方向是入射光线与法线的夹角的反射

反射光线的方向等于入射光线的方向减去其在法线方向上的分量的两倍

rOut = rIn - 2 * (rIn * n) * n

注意: 将反射光线的起始点进行微小偏移, 防止光线与发生该反射的三角形再次相交

### 4. 光线的递归追踪

当最近的三角形距离大于0(前面方法提到若未击中则返回-1)时说明击中了三角形

当三角形的材质反射率大于0时, 说明该三角形是反射的

将反射光线作为参数, N-1次递归调用TraceRay函数, 得到反射光线的颜色

### 5. 蒙特卡洛方法 & 斯涅尔定律 & 菲涅尔方程

蒙特卡洛方法: 
- 将环境光成分替换为蒙特卡洛采样计算的值
- 蒙特卡洛积分: 通过随机采样的方式, 估计积分值
- 每次循环只需一个间接光线, 但仍然应该使用直接光线估计
- 注意不要重复计算光源

斯涅尔定律: 
- n1 * sin(theta1) = n2 * sin(theta2)
- n1和n2分别是两种介质的折射率
- theta1和theta2分别是入射角和折射角

菲涅尔方程:
- 当对象透明时, 计算折射光线和反射光线的颜色
- 施利克近似公式: R(theta) = R0 + (1 - R0) * (1 - cos(theta))^5
- R(theta)是反射率
- R0是入射光线垂直时的反射率, 计算公式: ((n1 - n2) / (n1 + n2))^2, n1和n2分别是两种介质的折射率
- theta是入射角 

# Contact Information 联系方式

- Email 邮箱: mickeymiao2023@163.com
- WeChat 微信: SiMiao1106
