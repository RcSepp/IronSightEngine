#include "HaruPdf.h"
#include <hpdf.h>

//-----------------------------------------------------------------------------
// Name: CreateHaruPdf()
// Desc: DLL API for creating an instance of HaruPdf
//-----------------------------------------------------------------------------
HARUPDF_EXTERN_FUNC LPHARUPDF __cdecl CreateHaruPdf()
{
	return CheckHaruPdfSupport(NULL) ? new HaruPdf() : NULL;
}

//-----------------------------------------------------------------------------
// Name: CheckHaruPdfSupport()
// Desc: DLL API for checking support for HaruPdf on the target system
//-----------------------------------------------------------------------------
HARUPDF_EXTERN_FUNC bool __cdecl CheckHaruPdfSupport(LPTSTR* missingdllname)
{
	if(!Engine::FindDLL("libhpdf.dll", missingdllname))
		return false;

   return true;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void HaruPdf::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

void __stdcall error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
cle->PrintLine(String("ERROR: error_no = ") << String((int)error_no) << String(", detail_no = ") << String((int)detail_no));
}

//-----------------------------------------------------------------------------
// Name: Open()
// Desc: EDIT
//-----------------------------------------------------------------------------
Result HaruPdf::Open(const FilePath& filename/*, IDatabase** database*/)
{
	Result rlt;

	/**database = NULL;

	Database* newdatabase;
	CHKALLOC(newdatabase = new Database(this));
	IFFAILED(newdatabase->Init(filename))
	{
		delete newdatabase;
		return rlt;
	}

	databases.push_back(newdatabase);
	*database = newdatabase;*/

HPDF_Doc pdf = HPDF_New(error_handler, NULL);
if(!pdf)
	return ERR("Error creating pdf");

HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

HPDF_Page page = HPDF_AddPage(pdf);
HPDF_Page_SetWidth(page, 550);
HPDF_Page_SetHeight(page, 500);

byte* buf = new byte[100 * 100 * 4];

HPDF_Image image = HPDF_LoadRawImageFromMem(pdf, buf, 100, 100, HPDF_CS_DEVICE_RGB, 8);

HPDF_Page_DrawImage(page, image, 0.0f, 0.0f, 100.0f, 100.0f);

HPDF_SaveToFile(pdf, "out.pdf");

HPDF_Free(pdf);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void HaruPdf::Release()
{
	/*while(databases.size())
		databases.front()->Release();*/

	delete this;
}