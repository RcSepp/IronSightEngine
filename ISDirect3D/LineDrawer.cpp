#include "Direct3D.h"
#include "LineDrawer.h"


//-----------------------------------------------------------------------------
// Name: LineDrawer()
// Desc: Constructor
//-----------------------------------------------------------------------------
LineDrawer::LineDrawer() : wnd(NULL), lineshader(NULL)
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize line drawer
//-----------------------------------------------------------------------------
Result LineDrawer::Init(OutputWindow* wnd, const FilePath shaderfilename)
{
	Result rlt;

	this->wnd = wnd;

	// Compile shader
	LPRENDERSHADER lpshd;
	CHKRESULT(wnd->CreateRenderShader(shaderfilename, &lpshd));
	lineshader = (RenderShader*)lpshd;

	// Get custom parameters
	posvar = lineshader->GetVariableBySemantic("Positions")->AsVector();
	if(!posvar->IsValid())
		return ERR("Variable with semantic Positions not found in Line.fx");
	clrvar = lineshader->GetVariableBySemantic("Colors")->AsVector();
	if(!clrvar->IsValid())
		return ERR("Variable with semantic Colors not found in Line.fx");
	worldviewprojvar = lineshader->GetVariableBySemantic("WorldViewProj")->AsMatrix();
	if(!worldviewprojvar->IsValid())
		return ERR("Variable with semantic WorldViewProj not found in Line.fx");

	// Prepare shader
	CHKRESULT(lineshader->SetTechnique(0));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawLine()
// Desc: Add transformed line to buffer
//-----------------------------------------------------------------------------
void LineDrawer::DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1)
{
	Vector4Buffer<MAX_SLOTS>* pos_buffer;
	if(transformed_positions.empty() || (pos_buffer = transformed_positions.back())->numpositions == MAX_SLOTS)
		transformed_positions.push_back(pos_buffer = new Vector4Buffer<MAX_SLOTS>());

	ColorBuffer<MAX_SLOTS>* clr_buffer;
	if(transformed_colors.empty() || (clr_buffer = transformed_colors.back())->numcolors == MAX_SLOTS)
		transformed_colors.push_back(clr_buffer = new ColorBuffer<MAX_SLOTS>());

	pos_buffer->positions[pos_buffer->numpositions++] = Vector4(&v0, 1.0f);
	pos_buffer->positions[pos_buffer->numpositions++] = Vector4(&v1, 1.0f);
	clr_buffer->colors[clr_buffer->numcolors++] = c0;
	clr_buffer->colors[clr_buffer->numcolors++] = c1;
}

//-----------------------------------------------------------------------------
// Name: DrawLine()
// Desc: Add untransformed line to buffer
//-----------------------------------------------------------------------------
void LineDrawer::DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1)
{
	Size<UINT> backbuffersize = wnd->GetBackbufferSize();
	float width = (float)backbuffersize.width;
	float height = (float)backbuffersize.height;

	Vector4Buffer<HALF_MAX_SLOTS>* pos_buffer;
	if(untransformed_positions.empty() || (pos_buffer = untransformed_positions.back())->numpositions == HALF_MAX_SLOTS)
		untransformed_positions.push_back(pos_buffer = new Vector4Buffer<HALF_MAX_SLOTS>());

	ColorBuffer<MAX_SLOTS>* clr_buffer;
	if(untransformed_colors.empty() || (clr_buffer = untransformed_colors.back())->numcolors == MAX_SLOTS)
		untransformed_colors.push_back(clr_buffer = new ColorBuffer<MAX_SLOTS>());

	pos_buffer->positions[pos_buffer->numpositions++] = Vector4(v0.x / width - 1.0f, 1.0f - v0.y / height, v1.x / width - 1.0f, 1.0f - v1.y / height);
	clr_buffer->colors[clr_buffer->numcolors++] = c0;
	clr_buffer->colors[clr_buffer->numcolors++] = c1;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw buffered lines
//-----------------------------------------------------------------------------
void LineDrawer::Render(RenderType rendertype)
{
	if(transformed_positions.empty() && untransformed_positions.empty())
		return;

	wnd->devspec->devicecontext->IASetInputLayout(NULL);

	wnd->devspec->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// Draw transformed lines
	if(wnd->cam)
		while(!transformed_positions.empty())
		{
			Vector4Buffer<MAX_SLOTS>* pos_buffer = transformed_positions.front();
			ColorBuffer<MAX_SLOTS>* clr_buffer = transformed_colors.front();

			posvar->SetFloatVectorArray((float*)pos_buffer->positions, 0, pos_buffer->numpositions);
			clrvar->SetFloatVectorArray((float*)clr_buffer->colors, 0, clr_buffer->numcolors);
			worldviewprojvar->SetMatrix(wnd->cam->viewprojmatrix);

			lineshader->PreparePass(0, 0);
			wnd->devspec->devicecontext->Draw(pos_buffer->numpositions, 0);

			delete pos_buffer, clr_buffer;
			transformed_positions.pop_front();
			transformed_colors.pop_front();
		}

	// Draw untransformed lines
	while(!untransformed_positions.empty())
	{
		Vector4Buffer<HALF_MAX_SLOTS>* pos_buffer = untransformed_positions.front();
		ColorBuffer<MAX_SLOTS>* clr_buffer = untransformed_colors.front();

		posvar->SetFloatVectorArray((float*)pos_buffer->positions, 0, pos_buffer->numpositions);
		clrvar->SetFloatVectorArray((float*)clr_buffer->colors, 0, clr_buffer->numcolors);

		lineshader->PreparePass(0, 1);
		wnd->devspec->devicecontext->Draw(pos_buffer->numpositions * 2, 0);

		delete pos_buffer, clr_buffer;
		untransformed_positions.pop_front();
		untransformed_colors.pop_front();
	}
}

//-----------------------------------------------------------------------------
// Name: LineDrawer()
// Desc: Destructor
//-----------------------------------------------------------------------------
LineDrawer::~LineDrawer()
{
}