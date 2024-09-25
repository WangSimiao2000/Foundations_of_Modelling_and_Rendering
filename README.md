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

# Contact Information 联系方式

- Email 邮箱: mickeymiao2023@163.com
- WeChat 微信: SiMiao1106
