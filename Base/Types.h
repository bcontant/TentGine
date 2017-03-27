#pragma once

#include <string>
#include "Vector.h"
#include "Matrix.h"
#include "Rect.h"

using u8 = unsigned char;
using s8 = signed char;
using u16 = unsigned short;
using s16 = signed short;
using u32 = unsigned int;
using s32 = signed int;
using u64 = unsigned long long;
using s64 = signed long long;

using vec2 = Vector2<float>;
using vec3 = Vector3<float>;
using vec4 = Vector4<float>;

using ivec2 = Vector2<int>;
using ivec3 = Vector3<int>;
using ivec4 = Vector4<int>;

using mat2 = Matrix2x2<float>;
using mat3 = Matrix3x3<float>;
using mat4 = Matrix4x4<float>;

using rect_f = Rect<float>;
using rect_i = Rect<int>;

#ifdef _UNICODE

using std_string = std::wstring;
using string_char = wchar_t;
using ustring_char = wchar_t;

#else //#ifdef _UNICODE

using std_string = std::string;
using string_char = char;
using ustring_char = unsigned char;

#endif //#ifdef _UNICODE