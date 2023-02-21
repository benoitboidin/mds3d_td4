#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){
    loadShaders();

    // if(!_mesh.load(DATA_DIR"/models/quad.off")) exit(1);
    if(!_mesh.load(DATA_DIR"/models/sphere.obj")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _trackball.setCamera(&_cam);

    glClearColor(0.5,0.5,0.5,1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

void Viewer::reshape(int w, int h){
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);
}

void Viewer::drawScene2D()
{

	// Matrix4f M;
	// M <<  _zoom, 0, 0, _offset.x(),
	// 		0, _zoom, 0, _offset.y(),
	// 		0, 0, _zoom, 0,
	// 		0, 0, 0, 1;

    glViewport(0, 0, _winWidth, _winHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader.activate();


    Matrix4f M, M2;

    M <<  cos(_zoom), -sin(_zoom), 0, _offset.x(),
            sin(_zoom), cos(_zoom), 0, _offset.y(),
            0, 0, 1, 0,
            0, 0, 0, 1;
        
    M2 <<  1, 0, 0, 0,
            0, 1, 0, -0.5,
            0, 0, 1, 0,
            0, 0, 0, 1;

    M = M*M2;

    glUniformMatrix4fv(_shader.getUniformLocation("M"), 1, GL_FALSE, M.data());
    _mesh.draw(_shader);

    // Petite chaise en bas à gauche.
    M <<  0.5, 0, 0, -0.5,
            0, 0.5, 0, -1,
            0, 0, 1, 0,
            0, 0, 0, 1;
    glUniformMatrix4fv(_shader.getUniformLocation("M"), 1, GL_FALSE, M.data());
    _mesh.draw(_shader);

    // Petite chaise en bas à droite.
    M <<  -0.5, 0, 0, 0.5,
            0, 0.5, 0, -1,
            0, 0, 1, 0,
            0, 0, 0, 1;
    glUniformMatrix4fv(_shader.getUniformLocation("M"), 1, GL_FALSE, M.data());
    _mesh.draw(_shader);

    _shader.deactivate();
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _shader.activate();

//   glUniform1i(_shader.getUniformLocation("wireframe"), 0);
  glUniformMatrix4fv(_shader.getUniformLocation("mat_cam"), 1, GL_FALSE, _cam.viewMatrix().data());


  _mesh.draw(_shader);

  if(_wireframe)
  {
    glUniform1i(_shader.getUniformLocation("wireframe"), 1);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDepthFunc(GL_LEQUAL);

    _mesh.draw(_shader);
    
    glUniform1i(_shader.getUniformLocation("wireframe"), 0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDepthFunc(GL_LESS);
  }

  _shader.deactivate();
}

void Viewer::drawSystem()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _shader.activate();
  glUniformMatrix4fv(_shader.getUniformLocation("mat_cam"), 1, GL_FALSE, _cam.viewMatrix().data());

    

  _mesh.draw(_shader);

  if(_wireframe)
  {
    glUniform1i(_shader.getUniformLocation("wireframe"), 1);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDepthFunc(GL_LEQUAL);

    _mesh.draw(_shader);
    
    glUniform1i(_shader.getUniformLocation("wireframe"), 0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDepthFunc(GL_LESS);
  }

  _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
    // drawScene2D();
    // drawScene();
    drawSystem(); 
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      _offset.y() += 0.05f;
    }
    else if (key==GLFW_KEY_DOWN)
    {
      _offset.y() -= 0.05f;
    }
    else if (key==GLFW_KEY_LEFT)
    {
        _offset.x() -= 0.05f;
    }
    else if (key==GLFW_KEY_RIGHT)
    {
      _offset.x() += 0.05f;
    }
    else if (key==GLFW_KEY_I)
    {
      _zoom *= 1.1f;
    }
    else if (key==GLFW_KEY_O)
    {
      _zoom /= 1.1f;
    }
    else if (key=GLFW_KEY_W)
    {
      _wireframe = !_wireframe;
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1f*y);
}

void Viewer::charPressed(int /*key*/)
{
}
