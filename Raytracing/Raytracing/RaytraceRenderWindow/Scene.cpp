#include "Scene.h"

Scene::Scene(std::vector<ThreeDModel> *texobjs,RenderParameters *renderp)
{
    objects = texobjs;
    rp = renderp;
    Cartesian3 ambient = Cartesian3(0.5f,0.5f,0.5f);
    Cartesian3 diffuse = Cartesian3(0.5f,0.5f,0.5f);
    Cartesian3 specular = Cartesian3(0.5f,0.5f,0.5f);
    Cartesian3 emissive = Cartesian3(0,0,0);
    float shininess = 1.0f;
    default_mat = new Material(ambient,diffuse,specular,emissive,shininess);
}

void Scene::updateScene()
{
    // 清空了场景的三角形列表，以便后续重新填充
    // Clear the list so it can be populated again
    triangles.clear();
    std::cout << "Running updateScene()" << std::endl;

    Matrix4 modelview = getModelview();  // 获取模型视图矩阵

    //遍历所有的对象（objects），并从每个对象中获取面（faceVertices）信息。
    for (int i = 0;i< int(objects->size());i++)
    {
        typedef unsigned int uint;
        ThreeDModel obj = objects->at(uint(i));
        //针对每个面, 进一步遍历三角形, 确保每个面可以生成多个三角形，以便在渲染中更精细地表示模型
        for (uint face = 0; face < obj.faceVertices.size(); face++)
        {
            for (uint triangle = 0; triangle < obj.faceVertices[face].size()-2; triangle++)
            {
                //在内部循环中，创建了一个新的三角形对象（Triangle t;），并为该三角形的三个顶点（vertex）设置了坐标、法线、纹理坐标以及颜色属性
                Triangle t;
                for (uint vertex = 0; vertex < 3; vertex++)
                {
                    uint faceVertex = 0;
                    if (vertex != 0)
                        faceVertex = triangle + vertex;
                    //this is our vertex before any transformations. (world space)
                    // 获取顶点坐标
                    Homogeneous4 v = Homogeneous4(obj.vertices[obj.faceVertices   [face][faceVertex]].x,
                                                  obj.vertices[obj.faceVertices   [face][faceVertex]].y,
                                                  obj.vertices[obj.faceVertices   [face][faceVertex]].z);
                    v = modelview * v;
                    t.verts[vertex] = v;

                    // 获取法线信息
                    Homogeneous4 n = Homogeneous4(obj.normals[obj.faceNormals   [face][faceVertex]].x,
                                                  obj.normals[obj.faceNormals   [face][faceVertex]].y,
                                                  obj.normals[obj.faceNormals   [face][faceVertex]].z,
                                                  0.0f);
                    n = modelview * n;
                    t.normals[vertex] = n;

                    // 获取纹理坐标
                    Cartesian3 tex = Cartesian3(obj.textureCoords[obj.faceTexCoords[face][faceVertex]].x,
                                                obj.textureCoords[obj.faceTexCoords[face][faceVertex]].y,
                                                0.0f);
                    t.uvs[vertex] = tex;

                    // 设置颜色属性
                    t.colors[vertex] = Cartesian3( 0.7f, 0.7f, 0.7f);
                }
                if (obj.material == nullptr) {
                    t.shared_material = default_mat; // 使用默认材质
                } else {
                    t.shared_material = obj.material; // 使用模型的材质
                }
                // 添加三角形到场景
                triangles.push_back(t);
            }
        }
    }
}

/* -- task1 -- */
//viewMatrix * modelMatrix
Matrix4 Scene::getModelview()
{
    Matrix4 result;
    result.SetIdentity();

    // TODO: Grab all the necessary matrices to
    Matrix4 viewMatrix;
    viewMatrix.SetTranslation(Cartesian3(rp->xTranslate, rp->yTranslate, rp->zTranslate));

    Matrix4 modelMatrix;
    modelMatrix = (rp -> rotationMatrix);

    result = viewMatrix * modelMatrix;

    // build your modelview. And return from this function.
    return result;
}

/* -- task3 -- */
//scenCPP
//计算最近的三角形及其离射线起点的距离
Scene::CollisionInfo Scene::closestTriangle(Ray r){
    //TODO: method to find the closest triangle!
    Scene::CollisionInfo ci;
    ci.t = 100000;
    //遍历所有的三角形
    for(size_t i = 0; i < triangles.size(); i++){
        float t = triangles[i].intersect(r);
        //std::cout<<"t:"<<t<<std::endl;
        if(t < ci.t && t >= 0){
            ci.t = t;
            ci.tri = triangles[i];
        }
    }
    //如果距离无限远, 表示未击中, 设置为负值
    if(ci.t == 100000){
        ci.t = -1;
    }
    return ci;
}

