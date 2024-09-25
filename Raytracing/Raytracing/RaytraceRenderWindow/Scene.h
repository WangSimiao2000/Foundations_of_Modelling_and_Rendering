#ifndef SCENE_H
#define SCENE_H

#include "ThreeDModel.h"
#include "Triangle.h"
#include "Ray.h"

class Scene
{
public:
    // 在 Scene 类的头文件（.h）中添加 default_mat 成员变量的声明
    Material* default_mat;

    std::vector<ThreeDModel>* objects;
    RenderParameters* rp;
    std::vector<Triangle> triangles;

    Scene(std::vector<ThreeDModel> *texobjs,RenderParameters *renderp);

    void updateScene();
    Matrix4 getModelview();
    Scene();

    struct CollisionInfo{
        Triangle tri;
        float t;
    };
    CollisionInfo closestTriangle(Ray r);


};



#endif // SCENE_H
