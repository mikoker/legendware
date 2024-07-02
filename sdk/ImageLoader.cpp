#include  "ImageLoader.h"
#include "../globals.h"


//-----------------------------------------------------------------------------
// Tools
//-----------------------------------------------------------------------------

void ImageLoader::FreeImage(char* pImageBuffer)
{
	memalloc->Free(pImageBuffer);
}

void ImageLoader::ConvertBGRtoRGB(char* pImageBuffer, int nSize)
{
	if (!pImageBuffer)
		return;

	for (int i = 0; i < nSize; i += 4)
	{
		BYTE temp = pImageBuffer[i + 0];
		pImageBuffer[i + 0] = pImageBuffer[i + 2];
		pImageBuffer[i + 2] = temp;
	}
}


//-----------------------------------------------------------------------------
// Create d3d9 texture by using clean image data
//-----------------------------------------------------------------------------

bool ImageLoader::CreateTexture(char* pImageBuffer, int iWidth, int iHeight, OUT IDirect3DTexture9** ppTexture)
{
	if (!pImageBuffer)
		return false;
	HRESULT hr = device->CreateTexture(iWidth, iHeight, 0, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, ppTexture, 0);
	if (!*ppTexture)
		return false;

	D3DLOCKED_RECT Rect;
	IDirect3DTexture9* pTexture = *ppTexture;
	pTexture->LockRect(0, &Rect, 0, D3DLOCK_DISCARD);
	memcpy(Rect.pBits, pImageBuffer, iWidth * iHeight * 4);
	pTexture->UnlockRect(0);

	return true;
}


//-----------------------------------------------------------------------------
// PNG
//-----------------------------------------------------------------------------

char* ImageLoader::ReadPNGFromMemory(char* pFileBuffer, int nFileSize, OUT int* iWidth, OUT int* iHeight)
{
	// int __usercall ImgUtl_ReadPNGAsRGBAFromBuffer@<eax>(_DWORD *width@<edx>, int *buffer@<ecx>, int *height, _DWORD *errcode)
	static auto pImgUtl_ReadPNGAsRGBAFromBuffer = signatures_manager->signatures[SIGNATURE_IMGUTL_READ_PNG_AS_RGBA_FROM_BUFFER];

	// Build correct file data (CUtlBuffer based)
	FileData_t fileData(pFileBuffer, nFileSize);

	// Call ImgUtl_ReadPNGAsRGBAFromBuffer
	int error;
	char* pImage;
	__asm
	{
		lea     ecx, fileData

		mov		edx, iWidth

		lea     eax, error
		push	eax

		push	iHeight

		call	pImgUtl_ReadPNGAsRGBAFromBuffer
		add     esp, 0x8
		mov		pImage, eax
	}

	// Fix colors
	ConvertBGRtoRGB(pImage, (*iWidth) * (*iHeight) * 4);

	return pImage;
}

char* ImageLoader::ReadPNGFromFile(const char* szFilePath, OUT int* iWidth, OUT int* iHeight)
{
	// Open file
	std::ifstream fs(szFilePath, std::fstream::binary);
	if (!fs.is_open())
		return NULL;

	// Read file
	std::stringstream ss;
	ss << fs.rdbuf();
	std::string sBuf = ss.str();

	// Get image
	return ReadPNGFromMemory((char*)sBuf.c_str(), sBuf.size(), iWidth, iHeight);
}

char* ImageLoader::ReadPNGFromVPK(const char* szVPKPath, OUT int* iWidth, OUT int* iHeight)
{
	// Open VPK
	auto fileHandle = basefilesys->Open(szVPKPath, ("r"), ("GAME"));
	if (!fileHandle)
		return NULL;

	// Get file size and alloc memory
	int fileSize = basefilesys->Size(fileHandle);
	void* fileData = memalloc->Alloc(fileSize);

	// Read file and close VPK
	basefilesys->Read(fileData, fileSize, fileHandle);
	basefilesys->Close(fileHandle);

	// Get image
	char* pImage = ReadPNGFromMemory((char*)fileData, fileSize, iWidth, iHeight);

	memalloc->Free(fileData);
	return pImage;
}


//-----------------------------------------------------------------------------
// JPG/JPEG
//-----------------------------------------------------------------------------

char* ImageLoader::ReadJPGFromFile(const char* szFilePath, OUT int* iWidth, OUT int* iHeight)
{
	// int __usercall ImgUtl_ReadJPEGAsRGBA@<eax>(char* FilePath@<ecx>, int* width@<edx>, int* height, int* errcode)
	static auto pImgUtl_ReadJPEGAsRGBA = signatures_manager->signatures[SIGNATURE_IMGUTL_READ_JPEG_AS_RGBA];

	// Call ImgUtl_ReadJPEGAsRGBA
	int error;
	char* pImage;
	__asm
	{
		lea     ecx, szFilePath

		mov     edx, iWidth

		lea     eax, error
		push	eax

		push	iHeight

		call	pImgUtl_ReadJPEGAsRGBA
		add     esp, 0x8
		mov		pImage, eax
	}

	// Fix colors
	ConvertBGRtoRGB(pImage, (*iWidth) * (*iHeight) * 4);

	return pImage;
}


//-----------------------------------------------------------------------------
// BMP (bugged)
//-----------------------------------------------------------------------------

/*char* ImageLoader::ReadBMPFromFile(const char* szFilePath, OUT int* iWidth, OUT int* iHeight)
{
	// int __usercall ImgUtl_ReadBMPAsRGBA@<eax>(char *FileName@<ecx>, _DWORD *a2@<edx>, int a3, int a4)
	static PVOID pImgUtl_ReadBMPAsRGBA = MEM::PatternScan(g_Dll_client, XOR("55 8B EC 83 EC ? 53 56 57 68 ? ? ? ? 6A ? 6A ? 6A ? 8B F1 89 55 E8"));

	// Call ImgUtl_ReadBMPAsRGBA
	int error;
	char* pImage;
	__asm
	{
		lea		ecx, szFilePath

		mov     edx, iWidth

		lea     eax, error
		push	eax

		push	iHeight

		call	pImgUtl_ReadBMPAsRGBA
		add     esp, 0x8
		mov		pImage, eax
	}

	// Fix colors
	ConvertBGRtoRGB(pImage, (*iWidth) * (*iHeight) * 4);

	return pImage;
}*/


//-----------------------------------------------------------------------------
// TGA
//-----------------------------------------------------------------------------

char* ImageLoader::ReadTGAFromFile(const char* szFilePath, OUT int* iWidth, OUT int* iHeight)
{
	// _BYTE *__usercall ImgUtl_ReadTGAAsRGBA@<eax>(char *FileName@<ecx>, _DWORD *a2@<edx>, int a3@<edi>, int a4, int a5, void *Buffer)
	static auto pImgUtl_ReadTGAAsRGBA = signatures_manager->signatures[SIGNATURE_IMGUTL_READ_TGA_AS_RGBA];
		// Call ImgUtl_ReadTGAAsRGBA
	char header[20] = {};
	int error;
	char* pImage;
	__asm
	{
		lea     ecx, szFilePath

		mov     edx, iWidth

		lea     eax, header
		push	eax

		lea     eax, error
		push	eax

		push	iHeight

		call	pImgUtl_ReadTGAAsRGBA
		add     esp, 0xC
		mov		pImage, eax
	}

	// Fix colors
	ConvertBGRtoRGB(pImage, (*iWidth) * (*iHeight) * 4);

	return pImage;
}


//-----------------------------------------------------------------------------
// VTF
//-----------------------------------------------------------------------------

char* ImageLoader::ReadVTFFromVPK(const char* szVPKPath, OUT int* iWidth, OUT int* iHeight)
{
	// void *__usercall ImgUtl_ReadVTFAsRGBA@<eax>(_DWORD *a1@<edx>, int a2@<ecx>, _DWORD *a3, _DWORD *a4)
	static auto pImgUtl_ReadVTFAsRGBA = signatures_manager->signatures[SIGNATURE_IMGUTL_READ_VTF_AS_RGBA];

	// Call ImgUtl_ReadVTFAsRGBA
	int error;
	char* pImage;
	__asm
	{
		mov     ecx, szVPKPath

		mov     edx, iWidth

		lea     eax, error
		push	eax

		push	iHeight

		call	pImgUtl_ReadVTFAsRGBA
		add     esp, 0x8
		mov		pImage, eax
	}

	// Fix colors
	ConvertBGRtoRGB(pImage, (*iWidth) * (*iHeight) * 4);

	return pImage;
}


//-----------------------------------------------------------------------------
// With extension detection
//-----------------------------------------------------------------------------

char* ImageLoader::ReadFromFile(const char* szFilePath, OUT int* iWidth, OUT int* iHeight)
{
	if (strstr(szFilePath, (".png")))
		return ReadPNGFromFile(szFilePath, iWidth, iHeight);
	else if (strstr(szFilePath, (".jpg")) || strstr(szFilePath, (".jpeg")))
		return ReadJPGFromFile(szFilePath, iWidth, iHeight);
	else if (strstr(szFilePath, (".tga")))
		return ReadTGAFromFile(szFilePath, iWidth, iHeight);

	return NULL;
}