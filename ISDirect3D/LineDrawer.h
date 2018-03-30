#ifndef __LINEDRAWER_H
#define __LINEDRAWER_H


#define MAX_SLOTS				256 // Must be in sync with line shader
#define HALF_MAX_SLOTS			128


//-----------------------------------------------------------------------------
// Name: LineDrawer
// Desc: Class rendering lines in bufferless drawing mode
//-----------------------------------------------------------------------------
class LineDrawer
{
private:
	template<int count> struct Vector4Buffer
	{
		Vector4 positions[count];
		UINT numpositions;
		Vector4Buffer() : numpositions(0) {}
	};
	template<int count> struct ColorBuffer
	{
		Color colors[count];
		UINT numcolors;
		ColorBuffer() : numcolors(0) {}
	};

	OutputWindow* wnd;
	RenderShader* lineshader;
	LPD3D11EFFECTVECTORVARIABLE posvar, clrvar;
	LPD3D11EFFECTMATRIXVARIABLE worldviewprojvar;
	std::list<Vector4Buffer<MAX_SLOTS>*> transformed_positions;
	std::list<Vector4Buffer<HALF_MAX_SLOTS>*> untransformed_positions;
	std::list<ColorBuffer<MAX_SLOTS>*> transformed_colors;
	std::list<ColorBuffer<MAX_SLOTS>*> untransformed_colors;

public:
	LineDrawer();
	~LineDrawer();

	Result Init(OutputWindow* wnd, const FilePath shaderfilename);
	void DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1);
	void DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1);
	void Render(RenderType rendertype);
};

#endif