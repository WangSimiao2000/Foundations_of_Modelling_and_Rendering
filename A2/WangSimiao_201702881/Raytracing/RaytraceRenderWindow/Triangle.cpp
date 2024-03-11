#include "Triangle.h"
#include "math.h"
#include "Scene.h"
//#include "Light.h"
//#include "RenderParameters.h""


Triangle::Triangle()
{
    shared_material= nullptr;
}


/* -- task3 -- */
// 交点距离射线起点的距离
float Triangle::intersect(Ray r){
    //PPT lecture2 35-38
    Cartesian3 AB = verts[1].Point() - verts[0].Point();
    Cartesian3 AC = verts[2].Point() - verts[0].Point();

    //Calculate n
    Cartesian3 nVector = AB.cross(AC).unit();

    //plane = {verts[0],u,w,n}
    //calculate t
    float result = (verts[0].Point() -r.origin).dot(nVector) / r.direction.dot(nVector);

    //交点坐标
    Cartesian3 P = r.origin + r.direction * result;

    //三角形三边的向量
    Cartesian3 BC = verts[2].Point() - verts[1].Point();
    Cartesian3 CA = verts[0].Point() - verts[2].Point();

    //三个顶点与交点连线的向量
    Cartesian3 AP = P - verts[0].Point();
    Cartesian3 BP = P - verts[1].Point();
    Cartesian3 CP = P - verts[2].Point();

    Cartesian3 nAPAB = AB.cross(AP);
    Cartesian3 nBPBC = BC.cross(BP);
    Cartesian3 nCPCA = CA.cross(CP);

    //交点在三角形外则设为负数
    if(nAPAB.dot(nBPBC) >= 0 && nBPBC.dot(nCPCA) >= 0){
        return result;
    }
    else{
        result = -1.0f;
        return result;
    }
}


/* -- task4 -- */
// 计算重心坐标
// 任意一点 P 满足 P = iA + jB + kC
// i + j + k = 1
// 输入是射线与三角形的交点
Cartesian3 Triangle::baricentric(Cartesian3 o){

    //方便理解
    Cartesian3 A,B,C;
    A = verts[0].Point();
    B = verts[1].Point();
    C = verts[2].Point();

    //计算AB, AC向量
    Cartesian3 vectorAB = B-A;
    Cartesian3 vectorAC = C-A;

    //计算A到交点的向量
    Cartesian3 vectorAO = o-A;

    float dot00 = vectorAB.dot(vectorAB);
    float dot01 = vectorAB.dot(vectorAC);
    float dot02 = vectorAB.dot(vectorAO);
    float dot11 = vectorAC.dot(vectorAC);
    float dot12 = vectorAC.dot(vectorAO);


    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float beta = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float gamma = (dot00 * dot12 - dot01 * dot02) * invDenom;
    float alpha = 1.0f - beta - gamma;

    // o = origin + direction * t;
    Cartesian3 bc;
    bc.x = alpha;
    bc.y = beta;
    bc.z = gamma;

    //std::cout<<alpha<<" "<<beta<<" "<<gamma<<std::endl;
    return bc;
}
\


//    //环境光
//    Homogeneous4 ambient(shared_material->ambient.x, shared_material->ambient.y, shared_material->ambient.z, 1);
//    Homogeneous4 ambientColor = (lightColor * quadraticAttenuation).modulate(ambient);


/* -- task5 -- */
Homogeneous4 Triangle::blinnPhongShading(Cartesian3 lightPosition, Homogeneous4 lightColor, Cartesian3 bcp){
    //相机坐标
    Cartesian3 camaraPosition(0.0f, 0.0f, 1.0f);

    //重心坐标插值 P
    Cartesian3 Position = verts[0].Point() * bcp.x+
                          verts[1].Point() * bcp.y+
                          verts[2].Point() * bcp.z;

    //重心坐标指向相机的向量(带长度)
    Cartesian3 p2Camara = camaraPosition - Position;

    //重心坐标指向光源的向量(带长度)
    Cartesian3 p2Light = lightPosition - Position;

    //角平分线的1/2
    Cartesian3 angularBisector = (p2Light + p2Camara)/2;

    //法向量
    Cartesian3 normal = (normals[0].Vector() * bcp.x+
                         normals[1].Vector() * bcp.y+
                         normals[2].Vector() * bcp.z).unit();

    //二次衰减
    float a = 1;
    float b = 0;
    float c = 1;
    float quadraticAttenuation = 1/(a + b * p2Light.length() + c * pow(p2Light.length(),2));

    //cosθ
    float cosSpecularTheta = normal.dot(angularBisector) / (normal.length()*angularBisector.length());

    //计算镜面反射
    Homogeneous4 specular(shared_material->specular.x, shared_material->specular.y, shared_material->specular.z, 1);
    Homogeneous4 specularColor = (lightColor * quadraticAttenuation).modulate(specular)* pow(cosSpecularTheta,shared_material->shininess);

    //cosθ
    float cosDiffuseTheta = normal.dot(p2Light) / (normal.length()*p2Light.length());

    //计算漫反射
    Homogeneous4 diffuse(shared_material->diffuse.x, shared_material->diffuse.y, shared_material->diffuse.z, 1);
    Homogeneous4 diffuseColor = (lightColor * quadraticAttenuation).modulate(diffuse) * (cosDiffuseTheta);

    //判断光源是否在另一个方向
    if(normal.dot(p2Light)<0){
        //颜色
        Homogeneous4 color(0,0,0,1);
        return color;
    }

    //颜色
    Homogeneous4 color = specularColor + diffuseColor;

    return color;
}
