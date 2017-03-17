#pragma once

#pragma warning (disable:4201)

template <class T>
class Matrix2x2
{
	union
	{
		struct
		{
			T _11, _12;
			T _21, _22;
		};

		T m[2][2];
	};
};

template <class T>
class Matrix3x3
{
	union 
	{
		struct
		{
			T _11, _12, _13;
			T _21, _22, _23;
			T _31, _32, _33;
		};

		T m[3][3];
	};
};

template <class T>
class Matrix4x4
{
	union
	{
		struct
		{
			T _11, _12, _13, _14;
			T _21, _22, _23, _24;
			T _31, _32, _33, _34;
			T _41, _42, _43, _44;
		};

		T m[4][4];
	};
};

#pragma warning (default:4201)