#include "Forms.h"
#include <Shlwapi.h>

#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "shlwapi.lib")
//#pragma comment(lib, "comctl32.lib")

// Indices of file types
#define INDEX_WORDDOC 1
#define INDEX_WEBPAGE 2
#define INDEX_TEXTDOC 3

// Controls
#define CONTROL_GROUP           2000
#define CONTROL_RADIOBUTTONLIST 2
#define CONTROL_RADIOBUTTON1    1
#define CONTROL_RADIOBUTTON2    2       // It is OK for this to have the same ID as CONTROL_RADIOBUTTONLIST, because it is a child control under CONTROL_RADIOBUTTONLIST

/* File Dialog Event Handler *****************************************************************************************************/

class CDialogEventHandler : public IFileDialogEvents, public IFileDialogControlEvents
{
public:
	// IUnknown methods
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(CDialogEventHandler, IFileDialogEvents),
			QITABENT(CDialogEventHandler, IFileDialogControlEvents),
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (!cRef)
			delete this;
		return cRef;
	}

	// IFileDialogEvents methods
	IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
	IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
	IFACEMETHODIMP OnTypeChange(IFileDialog *pfd);
	IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

	// IFileDialogControlEvents methods
	IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem);
	IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
	IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
	IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

	CDialogEventHandler() : _cRef(1) { };

private:
	~CDialogEventHandler() { };
	long _cRef;
};

// IFileDialogEvents methods
// This method gets called when the file-type is changed (combo-box selection changes).
// For sample sake, let's react to this event by changing the properties show.
HRESULT CDialogEventHandler::OnTypeChange(IFileDialog *pfd)
{
	IFileSaveDialog *pfsd;
	HRESULT hr = pfd->QueryInterface(&pfsd);
	if (SUCCEEDED(hr))
	{
		UINT uIndex;
		hr = pfsd->GetFileTypeIndex(&uIndex);   // index of current file-type
		if (SUCCEEDED(hr))
		{
			IPropertyDescriptionList *pdl = NULL;

			switch (uIndex)
			{
			case INDEX_WORDDOC:
				// When .doc is selected, let's ask for some arbitrary property, say Title.
				hr = PSGetPropertyDescriptionListFromString(L"prop:System.Title", IID_PPV_ARGS(&pdl));
				if (SUCCEEDED(hr))
				{
					// FALSE as second param == do not show default properties.
					hr = pfsd->SetCollectedProperties(pdl, FALSE);
					pdl->Release();
				}
				break;

			case INDEX_WEBPAGE:
				// When .html is selected, let's ask for some other arbitrary property, say Keywords.
				hr = PSGetPropertyDescriptionListFromString(L"prop:System.Keywords", IID_PPV_ARGS(&pdl));
				if (SUCCEEDED(hr))
				{
					// FALSE as second param == do not show default properties.
					hr = pfsd->SetCollectedProperties(pdl, FALSE);
					pdl->Release();
				}
				break;

			case INDEX_TEXTDOC:
				// When .txt is selected, let's ask for some other arbitrary property, say Author.
				hr = PSGetPropertyDescriptionListFromString(L"prop:System.Author", IID_PPV_ARGS(&pdl));
				if (SUCCEEDED(hr))
				{
					// TRUE as second param == show default properties as well, but show Author property first in list.
					hr = pfsd->SetCollectedProperties(pdl, TRUE);
					pdl->Release();
				}
				break;
			}
		}
		pfsd->Release();
	}
	return hr;
}

// IFileDialogControlEvents
// This method gets called when a dialog control item selection happens (radio-button selection. etc).
// For sample sake, let's react to this event by changing the dialog title.
HRESULT CDialogEventHandler::OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem)
{
	IFileDialog *pfd = NULL;
	HRESULT hr = pfdc->QueryInterface(&pfd);
	if (SUCCEEDED(hr))
	{
		if (dwIDCtl == CONTROL_RADIOBUTTONLIST)
		{
			switch (dwIDItem)
			{
			case CONTROL_RADIOBUTTON1:
				hr = pfd->SetTitle(L"Longhorn Dialog");
				break;

			case CONTROL_RADIOBUTTON2:
				hr = pfd->SetTitle(L"Vista Dialog");
				break;
			}
		}
		pfd->Release();
	}
	return hr;
}

// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
	*ppv = NULL;
	CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
	HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = pDialogEventHandler->QueryInterface(riid, ppv);
		pDialogEventHandler->Release();
	}
	return hr;
}



//-----------------------------------------------------------------------------
// Name: CreateButton()
// Desc: DLL API for creating an instance of a push button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPOPENFILEDIALOG __cdecl CreateOpenFileDialog()
{
	return new OpenFileDialog();
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize File Open Dialog
//-----------------------------------------------------------------------------
Result OpenFileDialog::Init()
{
	HRESULT hr;

	// CoCreate the File Open Dialog object
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
	if(SUCCEEDED(hr))
	{
		// Create an event handling object and hook it up to the dialog
		hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&dialogevents));
		if(SUCCEEDED(hr))
			// Hook up the event handler.
			hr = dialog->Advise(dialogevents, &cookie);
	}

	if(FAILED(hr))
	{
		if(dialogevents)
		{
			if(cookie)
				dialog->Unadvise(cookie);
			dialogevents->Release();
		}
		RELEASE(dialog);
		return Error(hr);
	}

	// Set options
	FILEOPENDIALOGOPTIONS opts;
	hr = dialog->GetOptions(&opts);
	if(SUCCEEDED(hr))
		hr = dialog->SetOptions(opts | FOS_FORCEFILESYSTEM);

	return Error(hr);
}

//-----------------------------------------------------------------------------
// Name: SetFileTypes()
// Desc: Set file types to include in the file query. Example: description = "Text Document", extension = "txt"
//-----------------------------------------------------------------------------
Result OpenFileDialog::SetFileTypes(FileType* filetypes, UINT numfiletypes, UINT defaultindex)
{
	if(!dialog)
		return ERR("The dialog has not been initialized");

	COMDLG_FILTERSPEC* filterspec = new COMDLG_FILTERSPEC[numfiletypes];
	CHKALLOC(filterspec);

	for(UINT i = 0; i < numfiletypes; i++)
	{
		filterspec[i].pszName = (filetypes[i].description << String(" (*.") << filetypes[i].extension << String(")")).ToWCharString();
		filterspec[i].pszSpec = (String("*.") << filetypes[i].extension).ToWCharString();
	}

	HRESULT hr = dialog->SetFileTypes(numfiletypes, filterspec);

	// Free memory
	for(UINT i = 0; i < numfiletypes; i++)
	{
		delete[] filterspec[i].pszName;
		delete[] filterspec[i].pszSpec;
	}
	delete[] filterspec;

	if(SUCCEEDED(hr)  && defaultindex)
		hr = dialog->SetFileTypeIndex(defaultindex + 1);

	return Error(hr);
}

//-----------------------------------------------------------------------------
// Name: SetDefaultExtension()
// Desc: Set default file extension. Example: "txt" or "doc;docx"
//-----------------------------------------------------------------------------
Result OpenFileDialog::SetDefaultExtension(String extension)
{
	if(!dialog)
		return ERR("The dialog has not been initialized");

	LPWSTR ext = extension.ToWCharString();
	HRESULT hr = dialog->SetDefaultExtension(ext);
	delete[] ext;
	return Error(hr);
}

//-----------------------------------------------------------------------------
// Name: Show()
// Desc: Show dialog
//-----------------------------------------------------------------------------
Result OpenFileDialog::Show(HWND parent)
{
	if(!dialog)
		return ERR("The dialog has not been initialized");

	IShellItem* showresult;
	HRESULT hr;
	Result rlt;

	files = NULL;
	numfiles = 0;

	// Show the dialog
	if(FAILED(hr = dialog->Show(parent)))
	{
		if(hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
			return R_OK;
		return Error(hr);
	}
	CHKRESULT(dialog->GetResult(&showresult));

	// We are just going to print out the name of the file for sample sake.
	PWSTR pszFilePath = NULL;
	hr = showresult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if(SUCCEEDED(hr))
	{
		files = new FilePath(pszFilePath);
		numfiles = 1;
		CoTaskMemFree(pszFilePath);
	}
	showresult->Release();

	return Error(hr);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void OpenFileDialog::Release()
{
	if(dialogevents)
	{
		if(cookie)
			dialog->Unadvise(cookie);
		dialogevents->Release();
	}
	RELEASE(dialog);
	REMOVE(files);

	delete this;
}