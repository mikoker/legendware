#pragma once
class IDirect3DTexture9;

struct FileData_t
{
	FileData_t(char* pFileBuffer, int nFileSize)
	{
		m_Memory.m_pMemory = pFileBuffer;
		m_Memory.m_nAllocationCount = nFileSize;
		m_Memory.m_nGrowSize = 0;
		m_Get = 0;
		m_Put = nFileSize;
		m_Error = (char)0;
		m_Flags = (char)0;
		m_Reserved = (char)204;
		m_nTab = 0;
		m_nMaxPut = nFileSize;
		m_nOffset = 0;
	}

	struct MemData_t
	{
		char* m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
	} m_Memory;

	int m_Get;
	int m_Put;

	unsigned char m_Error;
	unsigned char m_Flags;
	unsigned char m_Reserved;

	int m_nTab;
	int m_nMaxPut;
	int m_nOffset;
};


namespace ImageLoader
{
	// Tools
	void FreeImage(char* pImageBuffer);
	void ConvertBGRtoRGB(char* pImageBuffer, int nSize);

	// Create d3d9 texture by using clean image data
	bool CreateTexture(char* pImageBuffer, int iWidth, int iHeight, IDirect3DTexture9** ppTexture);

	// Get clean image PNG
	char* ReadPNGFromMemory(char* pFileBuffer, int nFileSize,  int* iWidth,  int* iHeight);
	char* ReadPNGFromFile(const char* szFilePath,  int* iWidth,  int* iHeight);
	char* ReadPNGFromVPK(const char* szVPKPath,  int* iWidth,  int* iHeight);

	// Get clean image JPG/JPEG
	char* ReadJPGFromFile(const char* szFilePath,  int* iWidth,  int* iHeight);

	// Get clean image TGA
	char* ReadTGAFromFile(const char* szFilePath,  int* iWidth,  int* iHeight);

	// Get clean image VTF
	char* ReadVTFFromVPK(const char* szVPKPath,  int* iWidth,  int* iHeight);

	// Get clean image (auto extension detection)
	char* ReadFromFile(const char* szFilePath,  int* iWidth,  int* iHeight);
}