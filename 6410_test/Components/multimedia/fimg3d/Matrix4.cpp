// Matrix4.cpp : 

#include "Matrix4.h"
#include "Vect4D.h"
#include "register.h"

#include <math.h>

#define M2DET(x00, x01, x10, x11) \
  ((x00)*(x11) - (x01)*(x10))


#define M3DET(x00, x01, x02, x10, x11, x12, x20, x21, x22) \
  (  (x00) * M2DET((x11), (x12), (x21), (x22)) \
   - (x01) * M2DET((x10), (x12), (x20), (x22)) \
   + (x02) * M2DET((x10), (x11), (x20), (x21))) 

Matrix4::Matrix4()
{
	for(int i=0; i<4 ; i++)
		for(int j=0; j<4 ; j++)
			m[i][j] = 0;

	for(int i=0; i<4 ; i++)
		m[i][i] = 1;

}

Matrix4::Matrix4(const Matrix4& mat)
{
	for(int i=0; i<4 ; i++)
		for(int j=0; j<4 ; j++)
			m[i][j] = mat.m[i][j];
}

Matrix4::Matrix4(float m00,float m01,float m02,float m03,
				 float m10,float m11,float m12,float m13,
				 float m20,float m21,float m22,float m23, 
				 float m30,float m31,float m32,float m33)
{
	m[0][0] = m00; m[0][1]=m01; m[0][2]=m02;  m[0][3]=m03;
	m[1][0] = m10; m[1][1]=m11; m[1][2]=m12; m[1][3]=m13;
	m[2][0] = m20; m[2][1]=m21; m[2][2]=m22; m[2][3]=m23;
	m[3][0] = m30; m[3][1]=m31; m[3][2]=m32; m[3][3]=m33;
}

Matrix4::Matrix4(const float mat[16])
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] = mat[i+4*j];
}

Matrix4::~Matrix4(void)
{
}

float Matrix4::m4cof(int r, int c, int r0, int r1, int r2, int c0, int c1, int c2)
{
	return (((r + c) % 2)? -1.0 : 1.0) 
		* M3DET(m[r0][c0], m[r0][c1], m[r0][c2], m[r1][c0], m[r1][c1], m[r1][c2], m[r2][c0], m[r2][c1], m[r2][c2]);
}

float Matrix4::m4det()
{
	float d;
	
	d =	(m[0][0] * m4cof(0,0, 1,2,3, 1,2,3) +
		 m[0][1] * m4cof(0,1, 1,2,3, 0,2,3) +
		 m[0][2] * m4cof(0,2, 1,2,3, 0,1,3) +
		 m[0][3] * m4cof(0,3, 1,2,3, 0,1,2));
	
	return d;
}

Matrix4 Matrix4::operator=(const Matrix4& mat)
{
	for(int i=0; i<4 ; i++)
		for(int j=0; j<4 ; j++)
			m[i][j] = mat.m[i][j];

	return (*this);
}

Matrix4 Matrix4::operator*(const Matrix4& mat)
{
	Matrix4 resM(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);

	
	for(int i=0; i< 4; i++)
		for(int j=0; j< 4; j++) 
			for(int k=0; k< 4;k++) 
				resM.m[i][j] += m[i][k]*mat.m[k][j];
	
	return resM;
}

void Matrix4::SetMatrix4(float m00,float m01,float m02,float m03,
						 float m10,float m11,float m12,float m13,
						 float m20,float m21,float m22,float m23, 
						 float m30,float m31,float m32,float m33)
{
	m[0][0] = m00; m[0][1]=m01; m[0][2]=m02; m[0][3]=m03;
	m[1][0] = m10; m[1][1]=m11; m[1][2]=m12; m[1][3]=m13;
	m[2][0] = m20; m[2][1]=m21; m[2][2]=m22; m[2][3]=m23;
	m[3][0] = m30; m[3][1]=m31; m[3][2]=m32; m[3][3]=m33;
}

void Matrix4::LoadIdentity(void)
{
	for(int i=0; i<4 ; i++)
		for(int j=0; j<4 ; j++)
			m[i][j] = 0;

	for(int i=0; i<4 ; i++)
		m[i][i] = 1;
}

void Matrix4::Scale(float sx, float sy, float sz)
{
	Matrix4 tempM(sx,0,0,0, 0,sy,0,0, 0,0,sz,0, 0,0,0,1);
	*this = tempM*(*this);
}

void Matrix4::Translate(float tx, float ty, float tz)
{
	Matrix4 tempM(1,0,0,tx, 0,1,0,ty, 0,0,1,tz, 0,0,0,1);
	*this = tempM*(*this);
}

void Matrix4::Rotate(int axis, float angle)
{
	Matrix4 tempM;
	float radian=(angle/180.)*M_PI;

	switch(axis){
	case X_AXIS:
		tempM.m[1][1] = cos(radian);  tempM.m[2][1] = sin(radian);
		tempM.m[1][2] = -sin(radian); tempM.m[2][2] = cos(radian);
		break;

	case Y_AXIS:
		tempM.m[0][0] = cos(radian); tempM.m[2][0] = -sin(radian);
		tempM.m[0][2] = sin(radian); tempM.m[2][2] = cos(radian);
		break;

	case Z_AXIS:
		tempM.m[0][0] = cos(radian); tempM.m[1][0] = sin(radian);
		tempM.m[0][1] = -sin(radian);tempM.m[1][1] = cos(radian);
		break;
	}
	
	*this = tempM*(*this);
}

void Matrix4::Rotate(float ax, float ay, float az, float angle)
{
	Matrix4 tempM;
	
	Vect4D vtAxis = Vect4D(ax, ay, az);
	vtAxis.Normalize();
	float radian=(angle/180.)*M_PI;
	
	tempM.m[0][0] = vtAxis.m[0] * vtAxis.m[0] * (1 - cos(radian)) + cos(radian);
	tempM.m[0][1] = vtAxis.m[0] * vtAxis.m[1] * (1 - cos(radian)) - vtAxis.m[2] * sin(radian);
	tempM.m[0][2] = vtAxis.m[0] * vtAxis.m[2] * (1 - cos(radian)) + vtAxis.m[1] * sin(radian);
	tempM.m[1][0] = vtAxis.m[1] * vtAxis.m[0] * (1 - cos(radian)) + vtAxis.m[2] * sin(radian);
	tempM.m[1][1] = vtAxis.m[1] * vtAxis.m[1] * (1 - cos(radian)) + cos(radian);
	tempM.m[1][2] = vtAxis.m[1] * vtAxis.m[2] * (1 - cos(radian)) - vtAxis.m[0] * sin(radian);
	tempM.m[2][0] = vtAxis.m[0] * vtAxis.m[2] * (1 - cos(radian)) - vtAxis.m[1] * sin(radian);
	tempM.m[2][1] = vtAxis.m[1] * vtAxis.m[2] * (1 - cos(radian)) + vtAxis.m[0] * sin(radian);
	tempM.m[2][2] = vtAxis.m[2] * vtAxis.m[2] * (1 - cos(radian)) + cos(radian);	
	
	*this = tempM * (*this);
}

void Matrix4::LookAtMatrix(double eyeX, double eyeY, double eyeZ, 
					   	   double centerX, double centerY, double centerZ,
					   	   double upX, double upY, double upZ)
{
	Matrix4	matTranslate, matLookAt;
	Vect4D vtF, vtUp, vtS, vtU;
	
	vtF = Vect4D(centerX - eyeX, centerY - eyeY, centerZ - eyeZ);
	vtUp = Vect4D(upX, upY, upZ);
	vtF.Normalize();
	vtUp.Normalize();
	vtS = vtF.CrossProduct(vtUp);
	vtU = vtS.CrossProduct(vtF);
	vtS.Normalize();
	vtU.Normalize();
	
	matLookAt.m[0][0] = vtS.m[0];	matLookAt.m[0][1] = vtS.m[1]; 	matLookAt.m[0][2] = vtS.m[2];	matLookAt.m[0][3] = 0;
	matLookAt.m[1][0] = vtU.m[0];	matLookAt.m[1][1] = vtU.m[1]; 	matLookAt.m[1][2] = vtU.m[2];	matLookAt.m[1][3] = 0;
	matLookAt.m[2][0] = -vtF.m[0];	matLookAt.m[2][1] = -vtF.m[1]; 	matLookAt.m[2][2] = -vtF.m[2];	matLookAt.m[2][3] = 0;
	matLookAt.m[3][0] = 0;			matLookAt.m[3][1] = 0;			matLookAt.m[3][2] = 0;			matLookAt.m[3][3] = 1;
	
	matTranslate.Translate(-eyeX, -eyeY, -eyeZ);
	*this = matTranslate * (*this);
	*this = matLookAt * (*this);
}

void Matrix4::SetAsOrthoMatrix(double left, double right, double bottom, double top, double nearV, double farV)
{
	double dWidth, dHeight, dDepth, dTX, dTY, dTZ;
	
	dWidth = right - left;
	dHeight = top - bottom;
	dDepth = farV - nearV;
	dTX = -(right + left) / dWidth;
	dTY = -(top + bottom) / dHeight;
	dTZ = -(farV + nearV) / dDepth;
	
	m[0][0] = 2 / dWidth;	m[0][1] = 0;			m[0][2] = 0;			m[0][3] = dTX;
	m[1][0] = 0;			m[1][1] = 2 / dHeight;	m[1][2] = 0;			m[1][3] = dTY;
	m[2][0] = 0;			m[2][1] = 0;			m[2][2] = -2 / dDepth;	m[2][3] = dTZ;
	m[3][0] = 0;			m[3][1] = 0;			m[3][2] = 0;			m[3][3] = 1;
}

void Matrix4::SetAsPerspectiveMatrix(double fovy, double aspect, double zNear, double zFar)
{
	double dCotangent, dDepth;
	double dRadian = (fovy/180.) * M_PI / 2;
	
	LoadIdentity();
	dCotangent = cos(dRadian) / sin(dRadian);
	dDepth = zNear - zFar;
	m[0][0] = dCotangent / aspect;
	m[1][1] = dCotangent;
	m[2][2] = (zFar + zNear) / dDepth;	
	m[2][3] = 2 * zFar * zNear / dDepth;
	m[3][2] = -1;
	m[3][3] = 0;	
}

void Matrix4::TransformVector(float x, float y,float z, float *px, float *py, float *pz)
{
	*px = x * m[0][0] + y * m[0][1] + z * m[0][2];
	*py = x * m[1][0] + y * m[1][1] + z * m[1][2];
	*pz = x * m[2][0] + y * m[2][1] + z * m[2][2];
}

Matrix4 Matrix4::Transpose()
{
	Matrix4 resM;
	for(int i=0; i<4; i++) {
		resM.m[0][i] = m[i][0];
		resM.m[1][i] = m[i][1];
		resM.m[2][i] = m[i][2];
		resM.m[3][i] = m[i][3];
	}
	return resM;
}

Matrix4 Matrix4::Inverse()
{
	Matrix4 resM;
	
	float d = m4det();

	if(d == 0.0)
		d = 0.00000000001;
	
	resM.m[0][0] = m4cof(0,0, 1,2,3, 1,2,3) / d;
	resM.m[1][0] = m4cof(0,1, 1,2,3, 0,2,3) / d;
	resM.m[2][0] = m4cof(0,2, 1,2,3, 0,1,3) / d;
	resM.m[3][0] = m4cof(0,3, 1,2,3, 0,1,2) / d;
	
	resM.m[0][1] = m4cof(1,0, 0,2,3, 1,2,3) / d;
	resM.m[1][1] = m4cof(1,1, 0,2,3, 0,2,3) / d;
	resM.m[2][1] = m4cof(1,2, 0,2,3, 0,1,3) / d;
	resM.m[3][1] = m4cof(1,3, 0,2,3, 0,1,2) / d;
	
	resM.m[0][2] = m4cof(2,0, 0,1,3, 1,2,3) / d;
	resM.m[1][2] = m4cof(2,1, 0,1,3, 0,2,3) / d;
	resM.m[2][2] = m4cof(2,2, 0,1,3, 0,1,3) / d;
	resM.m[3][2] = m4cof(2,3, 0,1,3, 0,1,2) / d;

	resM.m[0][3] = m4cof(3,0, 0,1,2, 1,2,3) / d;
	resM.m[1][3] = m4cof(3,1, 0,1,2, 0,2,3) / d;
	resM.m[2][3] = m4cof(3,2, 0,1,2, 0,1,3) / d;
	resM.m[3][3] = m4cof(3,3, 0,1,2, 0,1,2) / d;
	

	return resM;
}

unsigned int Matrix4::WriteMatrix4(unsigned int addr, bool bNormal)
{
	unsigned int uCount = 0;
	bNormal ? uCount = 3: uCount = 4;
	for(int i = 0; i < uCount; i++)
	{
		for(int j = 0; j < uCount; j++)
		{
			WRITEREGF(addr, m[i][j]);
			addr +=4;
		}		
		if(bNormal)
			addr += 4;
	}
	
	return addr;
}
