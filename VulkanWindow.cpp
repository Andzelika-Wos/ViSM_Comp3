#include "VulkanWindow.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <QApplication>
#include <QProcess>

VulkanWindow::VulkanWindow()
{
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    //Makes a new instance of the RenderWindow (our Renderer) class
    mRenderer = new Renderer(this, true); // last true == try MSAA

    //Camera is made when the renderer is made, so we know it is valid now
//We set values on the camera a lot from this class, so it is convenient to have a pointer to it
    mCamera = &dynamic_cast<Renderer*>(mRenderer)->mCamera;

    return mRenderer;
}

void VulkanWindow::setCameraSpeed(float value)
{
    mCameraSpeed += value;

    //Keep within some min and max values
    if(mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}

void VulkanWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_G)
    {
        //dynamic_cast<RenderWindow*>(mRenderWindow)->mObjects.at(mIndex)->move(-0.1f);
        if(mSelectedObject)
        {
            qDebug("Move object");
            mSelectedObject->move(-0.1f);
        }
    }
    if(event->key() == Qt::Key_F)
    {
        auto*heightmap = dynamic_cast<Renderer*>(mRenderer)->mMap["terrain"];
        if (heightmap->getDrawType() == 0){
            heightmap->setDrawType(1);

        }
        else{
            heightmap->setDrawType(0);
        }
    }
    if (event->key() == Qt::Key_Escape)
    {
        QCoreApplication::quit();       //Shuts down the whole program
    }

    if (event->key() == Qt::Key_0)
        mIndex = 0;
    if (event->key() == Qt::Key_1)
        mIndex = 1;

    if(event->key() == Qt::Key_R)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 0.0f, 0.0f, 1.0f);
    }

    //    You get the keyboard input like this
    if(event->key() == Qt::Key_W)
    {
        mInput.W = true;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = true;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = true;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = true;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = true;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = true;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = true;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = true;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = true;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = true;
    }
    if(event->key() == Qt::Key_Space)
    {
        mInput.SPACE = true;
    }
}

void VulkanWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        mInput.W = false;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = false;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = false;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = false;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = false;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = false;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = false;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = false;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = false;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = false;
    }
    if(event->key() == Qt::Key_Space)
    {
        mInput.SPACE = false;
    }
}

void VulkanWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();

    if (numDegrees.y() > 0)
        mCamera->mZoomDistance -= 1.0f;
    else if (numDegrees.y() < 0)
        mCamera->mZoomDistance += 1.0f;

    // Clamp zoom range
    if (mCamera->mZoomDistance < 5.0f)
        mCamera->mZoomDistance = 5.0f;
    if (mCamera->mZoomDistance > 100.0f)
        mCamera->mZoomDistance = 100.0f;
}

void VulkanWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void VulkanWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void VulkanWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->pos().x() - mMouseXlast;
    int dy = event->pos().y() - mMouseYlast;

    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();

    // Make it Able to Free look
    if (event->buttons() & Qt::RightButton) {
        mCamera->mYawOffset -= dx * mCameraRotateSpeed;
        mCamera->mPitchOffset -= dy * mCameraRotateSpeed;


        if (mCamera->mPitchOffset > 89.0f) mCamera->mPitchOffset = 89.0f;
        if (mCamera->mPitchOffset < -89.0f) mCamera->mPitchOffset = -89.0f;
    }
}

void VulkanWindow::handleInput(float deltaTime)
{
    mCamera->setSpeed(0.f);  // cancel last frame movement

    if (mInput.W) {
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(4)->move(0.0f, 0.0f, moveSpeed * deltaTime);
    }
    if (mInput.S) {
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(4)->move(0.0f, 0.0f, -moveSpeed * deltaTime);
    }
    if (mInput.D) {
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(4)->move(-moveSpeed * deltaTime, 0.0f, 0.0f);
    }
    if (mInput.A) {
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(4)->move(moveSpeed * deltaTime, 0.0f, 0.0f);
    }

}
