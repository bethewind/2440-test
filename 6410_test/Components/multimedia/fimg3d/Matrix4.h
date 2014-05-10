

#ifndef __CI_MATRIX4D_H__
#define __CI_MATRIX4D_H__

#ifdef __cplusplus
extern "C" {
#endif


#define X_AXIS	0
#define Y_AXIS	1
#define Z_AXIS	2
#define M_PI 3.1415926535897932384626433832795

class  Matrix4
{
public:
	float m[4][4];

private:
	float m4cof(int r, int c, int r0, int r1, int r2, int c0, int c1, int c2);
	float m4det(void);

public:
	Matrix4();
	Matrix4(const Matrix4& mat);	
	Matrix4(const float mat[16]);
	Matrix4(float m00,float m01,float m02,float m03,
			float m10,float m11,float m12,float m13,
			float m20,float m21,float m22,float m23, 
			float m30,float m31,float m32,float m33);
	~Matrix4();

	Matrix4 operator=(const Matrix4& mat);
	Matrix4 operator*(const Matrix4& mat);
	
	void LoadIdentity(void);
	void Scale(float sx, float sy, float sz);
	void Translate(float tx, float ty, float tz);
	void Rotate(int axis, float angle);
	void Rotate(float ax, float ay, float az, float angle);
	
	void LookAtMatrix(double eyeX, double eyeY, double eyeZ, 
			          double centerX, double centerY, double centerZ,
					  double upX, double upY, double upZ);	
	void SetMatrix4(float m00,float m01,float m02,float m03,
					float m10,float m11,float m12,float m13,
					float m20,float m21,float m22,float m23, 
					float m30,float m31,float m32,float m33);	
	void SetAsOrthoMatrix(double left, double right, double bottom, double top, double nearV, double farV);
	void SetAsPerspectiveMatrix(double fovy, double aspect, double zNear, double zFar);
	

	void TransformVector(float x, float y,float z, float *pa, float *py, float *pz);

	Matrix4 Transpose();
	Matrix4 Inverse(void);
	
	unsigned int WriteMatrix4(unsigned int addr, bool bNormal = false);
	
};

#ifdef __cplusplus
}
#endif

#endif //__CI_MATRIX4D_H_
