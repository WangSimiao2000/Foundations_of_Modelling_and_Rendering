//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
////////////////////////////////////////////////////////////////////////


#include <math.h>
#include <random>
#include <QTimer>
// include the header file
#include "RaytraceRenderWidget.h"
#include "RGBAImage.h"
#include "Scene.h"

#define N_THREADS 16
#define N_LOOPS 100
#define N_BOUNCES 5
#define TERMINATION_FACTOR 0.35f

// constructor
RaytraceRenderWidget::RaytraceRenderWidget
    (
        // the geometric object to show
        std::vector<ThreeDModel>      *newTexturedObject,
        // the render parameters to use
        RenderParameters    *newRenderParameters,
        // parent widget in visual hierarchy
        QWidget             *parent
        )
    // the : indicates variable instantiation rather than arbitrary code
    // it is considered good style to use it where possible
    :
    // start by calling inherited constructor with parent widget's pointer
    QOpenGLWidget(parent),
    // then store the pointers that were passed in
    texturedObjects(newTexturedObject),
    renderParameters(newRenderParameters)
{ // constructor
    // 创建一个 QTimer 对象
    QTimer *timer = new QTimer(this);
    // 连接定时器的超时信号（timeout）到 forceRepaint 槽
    connect(timer, &QTimer::timeout, this, &RaytraceRenderWidget::forceRepaint);
    // 启动定时器，参数 30 表示定时间隔为 30 毫秒
    timer->start();
} // constructor


// destructor
RaytraceRenderWidget::~RaytraceRenderWidget()
{ // destructor
    // empty (for now)
    // all of our pointers are to data owned by another class
    // so we have no responsibility for destruction
    // and OpenGL cleanup is taken care of by Qt
} // destructor

// mouse-handling
void RaytraceRenderWidget::mousePressEvent(QMouseEvent *event)
{ // RaytraceRenderWidget::mousePressEvent()
    // store the button for future reference
    int whichButton = int(event->button());
    // scale the event to the nominal unit sphere in the widget:
    // find the minimum of height & width
    float size = (width() > height()) ? height() : width();
    // scale both coordinates from that
    float x = (2.0f * event->x() - size) / size;
    float y = (size - 2.0f * event->y() ) / size;


    // and we want to force mouse buttons to allow shift-click to be the same as right-click
    unsigned int modifiers = event->modifiers();

    // shift-click (any) counts as right click
    if (modifiers & Qt::ShiftModifier)
        whichButton = Qt::RightButton;

    // send signal to the controller for detailed processing
    emit BeginScaledDrag(whichButton, x,y);
} // RaytraceRenderWidget::mousePressEvent()

void RaytraceRenderWidget::mouseMoveEvent(QMouseEvent *event)
{ // RaytraceRenderWidget::mouseMoveEvent()
    // scale the event to the nominal unit sphere in the widget:
    // find the minimum of height & width
    float size = (width() > height()) ? height() : width();
    // scale both coordinates from that
    float x = (2.0f * event->x() - size) / size;
    float y = (size - 2.0f * event->y() ) / size;

    // send signal to the controller for detailed processing
    emit ContinueScaledDrag(x,y);
} // RaytraceRenderWidget::mouseMoveEvent()

void RaytraceRenderWidget::mouseReleaseEvent(QMouseEvent *event)
{ // RaytraceRenderWidget::mouseReleaseEvent()
    // scale the event to the nominal unit sphere in the widget:
    // find the minimum of height & width
    float size = (width() > height()) ? height() : width();
    // scale both coordinates from that
    float x = (2.0f * event->x() - size) / size;
    float y = (size - 2.0f * event->y() ) / size;

    // send signal to the controller for detailed processing
    emit EndScaledDrag(x,y);
} // RaytraceRenderWidget::mouseReleaseEvent()

// called when OpenGL context is set up
void RaytraceRenderWidget::initializeGL()
{ // RaytraceRenderWidget::initializeGL()
    // this should remain empty
} // RaytraceRenderWidget::initializeGL()

// called every time the widget is resized
void RaytraceRenderWidget::resizeGL(int w, int h)
{ // RaytraceRenderWidget::resizeGL()
    // resize the render image
    frameBuffer.Resize(w, h);
} // RaytraceRenderWidget::resizeGL()

// called every time the widget needs painting
void RaytraceRenderWidget::paintGL()
{ // RaytraceRenderWidget::paintGL()
    // set background colour to white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // and display the image
    glDrawPixels(frameBuffer.width, frameBuffer.height, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer.block);
} // RaytraceRenderWidget::paintGL()

void RaytraceRenderWidget::Raytrace()
{
    // std::cout <<"One day I will raytrace." << std::endl;
    raytracingThread= std::thread(&RaytraceRenderWidget::RaytraceThread,this);
    raytracingThread.detach();
}

//void RaytraceRenderWidget::RaytraceThread()
//{
//    frameBuffer.clear(RGBAValue(0.0f, 0.0f, 0.0f,1.0f));
//    scene->updateScene();
//#pragma omp parallel for schedule(dynamic)
//    for(int j = 0; j < frameBuffer.height; j++){
//        for(int i = 0; i < frameBuffer.width; i++){
//            //实现逐渐渲染的效果
//            //int k =10000; while(k-->0);
//            Ray r = calculateRay(i, j, !renderParameters->orthoProjection);
//            Homogeneous4 color = TraceAndShadeWithRay(r, N_BOUNCES, 1.0f);
//            //Gamma correction....
//            float gamma = 2.2f;
//            //We already calculate everything in float, so we just do gamma correction before putting it integer format.
//            color.x = pow(color.x,1/gamma);
//            color.y = pow(color.y,1/gamma);
//            color.z = pow(color.z,1/gamma);
//            frameBuffer[j][i]=RGBAValue(color.x*255.0f,color.y*255.0f,color.z*255.0f,255.0f);
//        }
//    }
//    std::cout << "Done!" << std::endl;
//}

void RaytraceRenderWidget::RaytraceThread()
{
    frameBuffer.clear(RGBAValue(0.0f, 0.0f, 0.0f,1.0f));
    scene->updateScene();
#pragma omp parallel for schedule(dynamic)
    for(int j = 0; j < frameBuffer.height; j++){
        for(int i = 0; i < frameBuffer.width; i++){

            //实现逐渐渲染的效果
            //int k =10000; while(k-->0);

            //计算光线
            Ray ray = calculateRay(i, j, !renderParameters->orthoProjection);
            //找到第一个击中的三角形
            Scene::CollisionInfo closestTri = scene->closestTriangle(ray);
            //计算交点
            Cartesian3 p = ray.origin + ray.direction * closestTri.t;
            //计算重心坐标
            Cartesian3 bcp = closestTri.tri.baricentric(p);
            //颜色
            Homogeneous4 color(0,0,0,1);

            //            bool flag = false;
            //            if(i>500&&j<300&&i<502&&j>298){
            //                flag = true;
            //            }
            if(closestTri.t > 0){
                //环境光
                Homogeneous4 ambientColor(closestTri.tri.shared_material->ambient.x, closestTri.tri.shared_material->ambient.y, closestTri.tri.shared_material->ambient.z, 1);
                //自发光
                Homogeneous4 emittedColor(closestTri.tri.shared_material->emissive.x, closestTri.tri.shared_material->emissive.y, closestTri.tri.shared_material->emissive.z, 1);
                /* -- task4 -- */
                if(renderParameters->interpolationRendering){
                    //重心插值求法线
                    Cartesian3 normal = (closestTri.tri.normals[0].Vector() * bcp.x+
                                         closestTri.tri.normals[1].Vector() * bcp.y+
                                         closestTri.tri.normals[2].Vector() * bcp.z).unit();
                    color = Homogeneous4(abs(normal.x),abs(normal.y),abs(normal.z),1);
                }

                /* -- task5 -- */
                if(renderParameters->phongEnabled){
                    //遍历所有光线
                    for(size_t num = 0; num < renderParameters->lights.size(); num++){
                        if(renderParameters->lights[num]->enabled == true){
                            Homogeneous4 lightColor = renderParameters->lights[num]->GetColor();
                            Homogeneous4 lightPosition = renderParameters->lights[num]->GetPositionCenter();
                            Homogeneous4 lightPosition_fix = scene->getModelview() * lightPosition;
                            color = closestTri.tri.blinnPhongShading(lightPosition_fix.Point(), lightColor, bcp) + color;
                        }
                    }
                    //加上自发光颜色
                    color = emittedColor + ambientColor + color;//
                }

                /* -- task6 -- */
                if(renderParameters->shadowsEnabled){
                    //遍历所有光线
                    for(size_t num = 0; num < renderParameters->lights.size(); num++){
                        //判断光源是否启用
                        if(renderParameters->lights[num]->enabled == true){
                            //阴影射线
                            //重心坐标插值 P
                            Cartesian3 shadowRayOrigin = p;
                            //阴影射线的方向需要调整为modelview
                            Cartesian3 lightPosition = (scene->getModelview() * renderParameters->lights[num]->GetPositionCenter()).Point();
                            Cartesian3 shadowRayDirection = lightPosition - shadowRayOrigin;
                            //重心插值求法线
                            Cartesian3 normal = (closestTri.tri.normals[0].Vector() * bcp.x+
                                                 closestTri.tri.normals[1].Vector() * bcp.y+
                                                 closestTri.tri.normals[2].Vector() * bcp.z).unit();
                            //将射线的初始位置向阴影射线方向位移一小部分,以避免Intersection Problems
                            //shadowRayOrigin = shadowRayOrigin + shadowRayDirection*0.005;
                            shadowRayOrigin = shadowRayOrigin + normal*0.005;
                            Ray shadowRay(shadowRayOrigin,shadowRayDirection.unit());
                            //找到阴影射线击中的第一个三角形
                            Scene::CollisionInfo shadowClosestTri = scene->closestTriangle(shadowRay);
                            if(shadowClosestTri.t > 0){
                                //判断是否光源
                                if(!shadowClosestTri.tri.shared_material->isLight()){
                                    //color = emittedColor + ambientColor;
                                    continue;
                                }
                                else{
                                    if(renderParameters->lights[num]->enabled == true){
                                        Homogeneous4 lightColor = renderParameters->lights[num]->GetColor();
                                        Homogeneous4 lightPosition = renderParameters->lights[num]->GetPositionCenter();
                                        Homogeneous4 lightPosition_fix = scene->getModelview() * lightPosition;
                                        color = closestTri.tri.blinnPhongShading(lightPosition_fix.Point(), lightColor, bcp) + color;
                                    }
                                }
                            }
                        }
                    }
                    color = emittedColor + color + ambientColor;
                }

                /* -- task7 -- */
                if(renderParameters->reflectionEnabled){
                    //                    if(flag){
                    //                        color = TraceAndShadeWithRay(ray, N_BOUNCES, 1.0f);
                    //                    }
                    color = TraceAndShadeWithRay(ray, N_BOUNCES, 1.0f);
                }

                if(renderParameters->fresnelRendering){
                    //                    if(flag){
                    //                        color = TraceAndShadeWithRay(ray, N_BOUNCES, 1.0f);
                    //                    }
                    color = RefractionAndFresnel(ray, 1.0f, N_BOUNCES, 1.0f);
                }

                /* -- task3 -- */
                if(!renderParameters->fresnelRendering && !renderParameters->shadowsEnabled && !renderParameters->phongEnabled && !renderParameters->interpolationRendering && !renderParameters->reflectionEnabled){
                    //射线碰到了物体, 返回白色
                    color = Homogeneous4(255,255,255,1);
                    //std::cout<<color.x<<" "<<color.y<<" "<<color.z<<std::endl;
                    frameBuffer[j][i]=RGBAValue(color.x,color.y,color.z,255.0f);
                }
                //Gamma correction....
                float gamma = 2.2f;
                //We already calculate everything in float, so we just do gamma correction before putting it integer format.
                color.x = pow(color.x,1/gamma);
                color.y = pow(color.y,1/gamma);
                color.z = pow(color.z,1/gamma);
                frameBuffer[j][i]=RGBAValue(color.x*255.0f,color.y*255.0f,color.z*255.0f,255.0f);
            }
            //Homogeneous4 color(i/float(frameBuffer.height),j/float(frameBuffer.width),0);
        }
    }
    std::cout << "Done!" << std::endl;
}

void RaytraceRenderWidget::forceRepaint(){
    update();
}

/* -- task2 -- */
//计算光线追踪的射线的起点以及方向
Ray RaytraceRenderWidget::calculateRay(int pixelx, int pixely, bool perspective){
    //计算屏幕比例
    float w = frameBuffer.width;
    float h = frameBuffer.height;
    float aspect = w/h;

    //convert to NDCS
    float x = ((pixelx/w) - 0.5)*2;
    float y = ((pixely/h) - 0.5)*2;

    if(aspect > 1.0f){
        //wide window
        x = x * aspect;
    }
    else if(aspect < 1.0f){
        //tall window
        y = y / aspect;
    }

    Cartesian3 direction;
    Cartesian3 origin;

    if(perspective){
        //透视
        origin = Cartesian3(0.0f, 0.0f, 1.0f);
        direction = Cartesian3(x,y,-1).unit();
    }
    else{
        //正交
        origin = Cartesian3(x, y, 0.0f);
        direction = Cartesian3(0,0,-1).unit();
    }

    return Ray(origin, direction);
}


Homogeneous4 RaytraceRenderWidget::TraceAndShadeWithRay(Ray ray, int N, float energy){
    //颜色
    Homogeneous4 color(0,0,0,1);

    // ------ ↓ 计算当前表面本身的颜色 ↓ ------ //
    //找到第一个击中的三角形
    Scene::CollisionInfo closestTri = scene->closestTriangle(ray);

    //计算摄像机射线与场景三角形交点
    Cartesian3 p = ray.origin + ray.direction * closestTri.t;
    //计算该交点在该三角形上的重心坐标
    Cartesian3 bcp = closestTri.tri.baricentric(p);
    //重心插值求法线
    Cartesian3 normal = (closestTri.tri.normals[0].Vector() * bcp.x+
                         closestTri.tri.normals[1].Vector() * bcp.y+
                         closestTri.tri.normals[2].Vector() * bcp.z).unit();
    //重心坐标插值 P
    Cartesian3 Position = closestTri.tri.verts[0].Point() * bcp.x+
                          closestTri.tri.verts[1].Point() * bcp.y+
                          closestTri.tri.verts[2].Point() * bcp.z;
    //遍历所有光线
    if(closestTri.t > 0){
        //环境光
        Homogeneous4 ambientColor(closestTri.tri.shared_material->ambient.x, closestTri.tri.shared_material->ambient.y, closestTri.tri.shared_material->ambient.z, 1);
        //自发光
        Homogeneous4 emittedColor(closestTri.tri.shared_material->emissive.x, closestTri.tri.shared_material->emissive.y, closestTri.tri.shared_material->emissive.z, 1);
        for(size_t num = 0; num < renderParameters->lights.size(); num++){
            if(renderParameters->lights[num]->enabled == true){
                Homogeneous4 lightColor = renderParameters->lights[num]->GetColor();
                //重心坐标插值 P
                Cartesian3 shadowRayOrigin = p;
                Cartesian3 lightPosition = (scene->getModelview() * renderParameters->lights[num]->GetPositionCenter()).Point();
                color = closestTri.tri.blinnPhongShading(lightPosition, lightColor, bcp) + color;
                //阴影射线
                Cartesian3 shadowRayDirection = lightPosition - shadowRayOrigin;
                //将射线的初始位置向阴影射线方向位移一小部分,以避免Intersection Problems
                //shadowRayOrigin = shadowRayOrigin + shadowRayDirection*0.005;
                shadowRayOrigin = shadowRayOrigin + normal*0.005;
                Ray shadowRay(shadowRayOrigin,shadowRayDirection.unit());
                //找到阴影射线击中的第一个三角形
                Scene::CollisionInfo shadowClosestTri = scene->closestTriangle(shadowRay);
                if(shadowClosestTri.t > 0){
                    //判断是否光源
                    if(!shadowClosestTri.tri.shared_material->isLight()){
                        continue;
                    }
                    else{
                        if(renderParameters->lights[num]->enabled == true){
                            Homogeneous4 lightColor = renderParameters->lights[num]->GetColor();
                            Homogeneous4 lightPosition = renderParameters->lights[num]->GetPositionCenter();
                            Homogeneous4 lightPosition_fix = scene->getModelview() * lightPosition;
                            color = closestTri.tri.blinnPhongShading(lightPosition_fix.Point(), lightColor, bcp) + color;
                        }
                    }
                }
            }
        }
        color = emittedColor + color + ambientColor;
    }
    // ------ ↑ 计算当前表面本身的颜色 ↑ ------ //

    // 判断递归终止条件
    if (N <= 0) {
        return color;  // 或者其他适当的颜色值
    }

    if(closestTri.t > 0){
        if(closestTri.tri.shared_material->reflectivity>0){
            //反射光线
            Ray reflectedRay = reflectRay(ray, normal, Position);

            Homogeneous4 reflectedColor = TraceAndShadeWithRay(reflectedRay, N-1, energy);

            color = color * (1.0- closestTri.tri.shared_material->reflectivity) + reflectedColor * closestTri.tri.shared_material->reflectivity;
        }
    }

    color = color * energy;

    return color;
}


Homogeneous4 RaytraceRenderWidget::RefractionAndFresnel(Ray ray, float n1, int N, float energy){
    //颜色
    Homogeneous4 color(0,0,0,1);

    // ------ ↓ 计算当前表面本身的颜色 ↓ ------ //
    //找到第一个击中的三角形
    Scene::CollisionInfo closestTri = scene->closestTriangle(ray);

    //计算摄像机射线与场景三角形交点
    Cartesian3 p = ray.origin + ray.direction * closestTri.t;
    //计算该交点在该三角形上的重心坐标
    Cartesian3 bcp = closestTri.tri.baricentric(p);
    //重心插值求法线
    Cartesian3 normal = (closestTri.tri.normals[0].Vector() * bcp.x+
                         closestTri.tri.normals[1].Vector() * bcp.y+
                         closestTri.tri.normals[2].Vector() * bcp.z).unit();
    //重心坐标插值 P
    Cartesian3 Position = closestTri.tri.verts[0].Point() * bcp.x+
                          closestTri.tri.verts[1].Point() * bcp.y+
                          closestTri.tri.verts[2].Point() * bcp.z;
    //遍历所有光线
    if(closestTri.t > 0){
        //环境光
        Homogeneous4 ambientColor(closestTri.tri.shared_material->ambient.x, closestTri.tri.shared_material->ambient.y, closestTri.tri.shared_material->ambient.z, 1);
        //自发光
        Homogeneous4 emittedColor(closestTri.tri.shared_material->emissive.x, closestTri.tri.shared_material->emissive.y, closestTri.tri.shared_material->emissive.z, 1);
        for(size_t num = 0; num < renderParameters->lights.size(); num++){
            if(renderParameters->lights[num]->enabled == true){
                Homogeneous4 lightColor = renderParameters->lights[num]->GetColor();
                //重心坐标插值 P
                Cartesian3 shadowRayOrigin = p;
                Cartesian3 lightPosition = (scene->getModelview() * renderParameters->lights[num]->GetPositionCenter()).Point();
                color = closestTri.tri.blinnPhongShading(lightPosition, lightColor, bcp) + color;
                //                //阴影射线
                //                Cartesian3 shadowRayDirection = lightPosition - shadowRayOrigin;
                //                //将射线的初始位置向阴影射线方向位移一小部分,以避免Intersection Problems
                //                //shadowRayOrigin = shadowRayOrigin + shadowRayDirection*0.005;
                //                shadowRayOrigin = shadowRayOrigin + normal*0.005;
                //                Ray shadowRay(shadowRayOrigin,shadowRayDirection.unit());
                //                //找到阴影射线击中的第一个三角形
                //                Scene::CollisionInfo shadowClosestTri = scene->closestTriangle(shadowRay);
                //                if(shadowClosestTri.t > 0){
                //                    //判断是否光源
                //                    if(!shadowClosestTri.tri.shared_material->isLight()){
                //                        continue;
                //                    }
                //                    else{
                //                        if(renderParameters->lights[num]->enabled == true){
                //                            Homogeneous4 lightColor = renderParameters->lights[num]->GetColor();
                //                            Homogeneous4 lightPosition = renderParameters->lights[num]->GetPositionCenter();
                //                            Homogeneous4 lightPosition_fix = scene->getModelview() * lightPosition;
                //                            color = closestTri.tri.blinnPhongShading(lightPosition_fix.Point(), lightColor, bcp) + color;
                //                        }
                //                    }
                //                }
            }
        }
        color = emittedColor + color + ambientColor;
    }
    // ------ ↑ 计算当前表面本身的颜色 ↑ ------ //

    // 判断递归终止条件
    if (N <= 0) {
        return color;  // 或者其他适当的颜色值
    }

    if(closestTri.t > 0){
        if(closestTri.tri.shared_material->transparency==1){
            float n2 = closestTri.tri.shared_material->indexOfRefraction;
            if(n2 == n1) {
                n2 = 1.0f;//内部射到外部, 相当于遇到真空
            }
            // 垂直入射分量R0​
            float R0 = std::pow(((n1-n2)/(n1+n2)),2);
            // 入射角余弦值
            float cosTheta = std::abs(normal.dot(ray.direction.unit()));
            // Schlick逼近公式 计算反射系数 R(θ) 和 透射系数 T(θ)
            float ReflectanceIndex = R0 + (1.0f - R0) * std::pow((1.0f-cosTheta),5) ;
            float TransmittanceIndex = 1 - ReflectanceIndex;

            Ray reflectedRay = reflectRay(ray, normal, Position);
            Homogeneous4 reflectedColor = RefractionAndFresnel(reflectedRay, n2, N-1, energy);
            //            Homogeneous4 reflectedColor = TraceAndShadeWithRay(reflectedRay, N-1, energy);


            // 使用折射定律计算折射角
            float n = n1 / n2;
            float sinThetaT2 = n * n * (1.0f - cosTheta * cosTheta);
            Homogeneous4 refractedColor;

            if(sinThetaT2 < 1.0f){/*折射角<90°, 未发生全内反射*/
                Ray refractedRay = refractRay(ray, normal, Position, n1, n2);
                refractedColor = RefractionAndFresnel(refractedRay, n2, N-1, energy);;
            }else{/*折射角>90°, 发生全内反射*/
                refractedColor = closestTri.tri.shared_material->diffuse;
                //                refractedColor = color;
            }
            //            std::cout<<cosTheta<<std::endl;
            //            std::cout<<ReflectanceIndex<<" "<< TransmittanceIndex << std::endl;
            color = ReflectanceIndex * reflectedColor + TransmittanceIndex * refractedColor;
        }
    }

    color = color * energy;

    return color;
}

Ray RaytraceRenderWidget::reflectRay(Ray rIn, Cartesian3 normal, Cartesian3 hitPoint){
    ///反射光线方向
    Cartesian3 rInDir = rIn.direction;
    Cartesian3 rOutDir = rInDir - 2 * rInDir.dot(normal) * normal;

    //构造反射光线
    Ray rayOut(hitPoint + rOutDir * 0.001, rOutDir.unit());
    // rayOut.origin = hitPoint + rOutDir * 0.001;
    // rayOut.direction = rOutDir.unit();

    return rayOut;

}

Ray RaytraceRenderWidget::refractRay(Ray rIn, Cartesian3 normal, Cartesian3 hitPoint, float n1, float n2){
    Cartesian3 rInDir = rIn.direction;

    // 计算入射角
    float cosThetaI = std::abs(normal.dot(rIn.direction.unit()));

    // 使用折射定律计算折射角
    float n = n1 / n2;
    float sinThetaT2 = n * n * (1.0f - cosThetaI * cosThetaI);

    // 计算折射光线方向
    Cartesian3 rOutDir = n * rInDir + (n * cosThetaI - std::sqrt(1.0f - sinThetaT2)) * normal;

    // 构造折射光线
    Ray rayOut(hitPoint + rOutDir * 0.001, rOutDir.unit());
    return rayOut;
}
