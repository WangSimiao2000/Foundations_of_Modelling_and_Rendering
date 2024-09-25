#include "RenderParameters.h"

void RenderParameters::findLights(std::vector<ThreeDModel> objects)
{
    // 遍历所有3D模型对象以查找光源
    for (ThreeDModel obj : objects)
    {
        // 检查对象的材质是否为光源材质
        if (obj.material->isLight())
        {
            // 如果对象具有2个三角形，则它是矩形区域光源
            if (obj.faceVertices.size() == 2)
            {
                // TODO: 在这里添加矩形区域光源的代码
                // 遍历对象的三角形的顶点
                for (unsigned int i = 0; i < 3; i++)
                {
                    // 获取当前三角形的一个顶点ID
                    unsigned int vid = obj.faceVertices[0][i];
                    bool found = false;

                    // 检查该顶点是否在另一个三角形中，以确定是否为对角线的顶点
                    for(unsigned int j = 0; j < 3; j++)
                    {
                        if(vid == obj.faceVertices[1][j])
                        {
                            found = true;
                            break;
                        }
                    }

                    // 如果未找到对角线顶点，创建矩形面光源
                    if(!found)
                    {
                        // 获取矩形的三个顶点
                        unsigned int id1 = obj.faceVertices[0][i];
                        unsigned int id2 = obj.faceVertices[0][(i+1)%3];
                        unsigned int id3 = obj.faceVertices[0][(i+2)%3];
                        Cartesian3 v1 = obj.vertices[id1];
                        Cartesian3 v2 = obj.vertices[id2];
                        Cartesian3 v3 = obj.vertices[id3];
                        Cartesian3 vecA = v2 - v1;
                        Cartesian3 vecB = v3 - v1;
                        Homogeneous4 color = obj.material->emissive;
                        Homogeneous4 pos = v1 + (vecA/2) + (vecB/2);
                        Homogeneous4 normal = obj.normals[obj.faceNormals[0][0]];
                        Light *l = new Light(Light::Area,color,pos,normal,vecA,vecB);
                        l->enabled = true;
                        lights.push_back(l);
                    }
                }
            }
            else
            {
                // TODO: 在这里添加非矩形光源的代码
                // 计算对象的中心点
                Cartesian3 center = Cartesian3(0,0,0);
                for (unsigned int i = 0; i < obj.vertices.size(); i++)
                {
                    center = center + obj.vertices[i];
                }
                center = center / obj.vertices.size();
                Light *l = new Light(Light::Point,obj.material->emissive,center,Homogeneous4(),Homogeneous4(),Homogeneous4());
                l->enabled = true;
                lights.push_back(l);
            }
        }
    }
}
