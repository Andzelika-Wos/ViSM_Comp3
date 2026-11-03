#include "Camera.h"

Camera::Camera()
{
    init();
}

void Camera::init()
{
    mProjectionMatrix.setToIdentity();
    mViewMatrix.setToIdentity();
}
void Camera::perspective(int degrees, double aspect, double nearplane, double farplane)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(degrees, aspect, nearplane, farplane);


}

void Camera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mEye = eye;
    mAt = at;
    mUp = up;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
}

void Camera::followPlayer(QVector3D &playerPosition, QVector3D &offset)
{
    QMatrix4x4 rotate;
    rotate.rotate(mYawOffset, 0.f, 1.f, 0.f);
    rotate.rotate(mPitchOffset, 1.f, 0.f, 0.f);

    QVector3D rotatedOffset = rotate.map(offset);

    rotatedOffset = rotatedOffset.normalized() * mZoomDistance;


    mEye = playerPosition + rotatedOffset + mManualOffset;
    mAt = playerPosition;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
}



void Camera::pitch(float degrees)
{
    mPitch += degrees;
}

void Camera::yaw(float degrees)
{
    mYaw += degrees;
}

void Camera::update()
{
    //Set ViewMatrix to Identity, then add the new position and rotations
    mViewMatrix.setToIdentity();
    mPosition.setZ(mPosition.z() + mSpeed);
    //mViewMatrix.translate(mPosition);               //Makes rotation work around World Origo
    mViewMatrix.rotate(mYaw, 0.f, 1.f, 0.f);
    mViewMatrix.rotate(mPitch, 1.f, 0.f, 0.f);
    //mViewMatrix.rotate(mYaw, 0.f, 1.f, 0.f);      //pitch then yaw makes camera wonkey
    mViewMatrix.translate(mPosition);             //Makes rotation work around Camera Origo
}

void Camera::setPosition(const QVector3D& position)
{
    mPosition = position;
    update();
}

void Camera::setSpeed(float speed)
{
    mSpeed = speed;
}

void Camera::moveRight(float delta)
{
    mPosition.setX( mPosition.x() + delta);
}

QVector3D Camera::getForward() const {
    QVector3D forward;
    forward.setX(cos(qDegreesToRadians(mYaw)) * cos(qDegreesToRadians(mPitch)));
    forward.setY(sin(qDegreesToRadians(mPitch)));
    forward.setZ(sin(qDegreesToRadians(mYaw)) * cos(qDegreesToRadians(mPitch)));
    return forward.normalized();
}

QVector3D Camera::getRight() const {
    return QVector3D::crossProduct(getForward(), mUp).normalized();
}

void Camera::updateHeigth(float deltaHeigth)
{
    mPosition.setY(mPosition.y() + deltaHeigth);
}

//Translate camera in world coordinates
void Camera::translate(float dx, float dy, float dz)
{
    mViewMatrix.translate(dx, dy, dz);
}

void Camera::rotate(float t, float x, float y, float z)
{
    mViewMatrix.rotate(t,x,y,z);
}

//QMatrix4x4 Camera::cMatrix()
//{
//    return mProjectionMatrix * mViewMatrix;
//}
