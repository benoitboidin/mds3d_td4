
#include "camera.h"
#include "opengl.h"

using namespace Eigen;

Camera::Camera()
{
  mViewMatrix.setIdentity();
  setPerspective(float(M_PI/2.),0.1f,10000.f);
  setViewport(0,0);
}

/*
position : position of camera. 
target : point camera is looking at.
up : up vector of camera.
*/
void Camera::lookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up)
{
  mTarget = target;

  // TODO
  Matrix4f mViewMatrix;
  // Opposé du vecteur de visée.
  Vector3f z = (position - target).normalized(); 
  // Vecteur dans le plan défini par l'orientation verticale up 
  // et la direction de visée qui est orthogonal au premier axe.
  Vector3f x = up.cross(z).normalized(); 
  // Obtenu par produit vectoriel entre les deux vecteurs précédents.
  Vector3f y = z.cross(x).normalized();

  mViewMatrix << x.x(), x.y(), x.z(), -x.dot(position),
                   y.x(), y.y(), y.z(), -y.dot(position),
                   z.x(), z.y(), z.z(), -z.dot(position),
                   0, 0, 0, 1;
}

void Camera::setPerspective(float fovY, float near, float far)
{
  m_fovY = fovY;
  m_near = near;
  m_far = far;
}

void Camera::setViewport(int width, int height)
{
  mVpWidth = width;
  mVpHeight = height;
}

void Camera::zoom(float x)
{
  Vector3f t = Affine3f(mViewMatrix) * mTarget;
  mViewMatrix = Affine3f(Translation3f(Vector3f(0,0,x*t.norm())).inverse()) * mViewMatrix;
}

void Camera::rotateAroundTarget(float angle, Vector3f axis)
{
  // TODO
  Vector3f t = Affine3f(mViewMatrix) * mTarget;
  mViewMatrix = Affine3f(Translation3f(t) * AngleAxisf(angle, axis) * Translation3f(-t)) * mViewMatrix;
}

Camera::~Camera()
{
}

const Matrix4f& Camera::viewMatrix() const
{
  return mViewMatrix;
}

Matrix4f Camera::projectionMatrix() const
{
  float aspect = float(mVpWidth)/float(mVpHeight);
  float theta = m_fovY*0.5f;
  float range = m_far - m_near;
  float invtan = 1./std::tan(theta);

  Matrix4f projMat;
  projMat.setZero();
  projMat(0,0) = invtan / aspect;
  projMat(1,1) = invtan;
  projMat(2,2) = -(m_near + m_far) / range;
  projMat(2,3) = -2 * m_near * m_far / range;
  projMat(3,2) = -1;

  return projMat;
}
