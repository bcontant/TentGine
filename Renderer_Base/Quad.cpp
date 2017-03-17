#include "precompiled.h"

#include "Quad.h"
#include "VertexBuffer.h"


//--------------------------------------------------------------------------------
struct SimpleVertex
{
	vec3 Pos;
	vec2 UV;
};

/*
Explanation for why seemingly CCW triangles in a triangle strip work out to be CW :
Source : https://en.wikipedia.org/wiki/Triangle_strip

It follows from definition that a subsequence of vertices of a triangle strip also represents a triangle strip.
However, if this substrip starts at an even(with 1 - based counting) vertex, then the resulting triangles will change their orientation.
For example a substrip BCDEF would represent triangles : BCD, CED, DEF.

So in this case {ABCD}, we get ABC (CW) and BDC (CW, instead of BCD, which would be CCW)
*/
//--------------------------------------------------------------------------------
SimpleVertex g_QuadVertices[] =
{
	{ { 1.0f, 0.0f, 0.0f },
	  { 1.0f, 0.0f } },

	{ { 1.0f, 1.0f, 0.0f },
	  { 1.0f, 1.0f } },
	
	{ { 0.0f, 0.0f, 0.0f },
	  { 0.0f, 0.0f } },
	
	{ { 0.0f, 1.0f, 0.0f },
	  { 0.0f, 1.0f } }
};

//--------------------------------------------------------------------------------
Quad::Quad(Renderer* pOwner) 
	: RendererObject(pOwner) 
{
	m_pVertexBuffer = GetOwner()->CreateVertexBuffer(4, VertexBuffer::eVB_Position | VertexBuffer::eVB_2D_TexCoords, EPrimitiveType::ePT_TriangleStrip, g_QuadVertices, eUT_Constant);
}

//--------------------------------------------------------------------------------
Quad::~Quad()
{
	m_pTexture = nullptr;

	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;
}

//--------------------------------------------------------------------------------
void Quad::SetPosition(float x, float y)
{
	m_Position.x = x;
	m_Position.y = y;
}
