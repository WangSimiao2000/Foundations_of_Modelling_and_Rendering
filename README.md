## Rasterisation 光栅化

暂未截图

# Raytracing 光线追踪

## Introduce 简介

To achieve transparent materials and the Fresnel effect, I utilized the RefractionAndFresnel function in the code. 
Firstly, this function calculates the color of the surface itself, including ambient light, emissive light, and diffuse and specular reflection from the light source. 
Then, by determining whether the object is transparent, it decides whether to perform calculations for refraction and reflection.
In the case of transparency, I employ the law of refraction to calculate the refracted angle and use the Schlick approximation formula to compute the reflection and transmission coefficients. 
Subsequently, based on the size of the refracted angle, it is determined whether total internal reflection occurs. 
If the refracted angle is less than 90°, refraction calculations are performed; otherwise, total internal reflection occurs, and the diffuse color of the object is used. 
Finally, the colors obtained from reflection and refraction calculations are combined according to the weights specified by the Fresnel equation, resulting in the ultimate color.

为实现透明材质和菲涅耳效应，我在代码中使用了 `RefractionAndFresnel` 函数。
首先，该函数计算物体表面的颜色，包括环境光、发光光线，以及光源的漫反射和镜面反射。
然后，通过判断物体是否透明，决定是否进行折射和反射的计算。
对于透明的情况，我采用折射定律来计算折射角，并使用 Schlick 近似公式计算反射系数和透射系数。
接着，根据折射角的大小，判断是否发生全内反射。如果折射角小于 90°，则进行折射计算；否则，发生全内反射并使用物体的漫反射颜色。
最后，根据菲涅耳方程指定的权重，将反射和折射计算所得的颜色进行组合，生成最终的颜色。

## Screenshot 截图

显示法线方向:

<img width="812" alt="bd73770c3aee1e85d687017454186df" src="https://github.com/user-attachments/assets/81e466dc-6967-44de-939d-b776cf0b1281">

透明物体折射+反射: 

<img width="812" alt="2299f253c61a0130182372cfe33134b" src="https://github.com/user-attachments/assets/30f21059-c31d-4589-83be-ff7aaeb8fadd">

透明物体折射+反射+阴影: 

<img width="812" alt="3eb3508e2cce1b60e66617513205e0f" src="https://github.com/user-attachments/assets/3e2ba92f-f30f-4e5c-9327-e69f6b74d086">

## Contact Information 联系方式

- Email 邮箱: mickeymiao2023@163.com
- WeChat 微信: SiMiao1106
