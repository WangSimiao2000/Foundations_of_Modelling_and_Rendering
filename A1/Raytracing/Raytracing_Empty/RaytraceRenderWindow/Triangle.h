#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Homogeneous4.h"
#include "Material.h"
#include "Ray.h"
#include "RenderParameters.h"

class Triangle
{
public:
    Homogeneous4 verts[3];
    Homogeneous4 normals[3];
    Homogeneous4 colors[3];
    Cartesian3 uvs[3];

    Material *shared_material;
    Triangle();

    // the render parameters to use
    RenderParameters *renderParameters;

    float intersect(Ray r);
    Cartesian3 baricentric(Cartesian3 o);
    Homogeneous4 blinnPhongShading(Cartesian3 lightPosition, Homogeneous4 lightColor, Cartesian3 barycentricPosition);
    //Homogeneous4 blinnPhongShading_shadow(bool isInShadow, Cartesian3 lightPosition, Homogeneous4 lightColor, Cartesian3 barycentricPosition);
};

#endif // TRIANGLE_H
