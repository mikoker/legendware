#pragma once
#include "../../data.h"
enum RenderToRTStage_t
{
	RENDER_TO_RT_STAGE_CREATED = 0,
	RENDER_TO_RT_STAGE_STARTED,
	RENDER_TO_RT_STAGE_WAITING_FOR_READ_BACK,
	RENDER_TO_RT_STAGE_WAITING_FOR_RESULT,
	RENDER_TO_RT_STAGE_DONE,

	RENDER_TO_RT_STAGE_UNDEFINED = -1
};



struct VtfProcessingOptions
{
	uint32 cbSize;					// Set to sizeof( VtfProcessingOptions )

	//
	// Flags0
	//
	enum Flags0
	{
		// Have a channel decaying to a given decay goal for the given last number of mips
		OPT_DECAY_R = 0x00000001,	// Red decays
		OPT_DECAY_G = 0x00000002,	// Green decays
		OPT_DECAY_B = 0x00000004,	// Blue decays
		OPT_DECAY_A = 0x00000008,	// Alpha decays

		OPT_DECAY_EXP_R = 0x00000010,	// Channel R decays exponentially (otherwise linearly)
		OPT_DECAY_EXP_G = 0x00000020,	// Channel G decays exponentially (otherwise linearly)
		OPT_DECAY_EXP_B = 0x00000040,	// Channel B decays exponentially (otherwise linearly)
		OPT_DECAY_EXP_A = 0x00000080,	// Channel A decays exponentially (otherwise linearly)

		OPT_NOCOMPRESS = 0x00000100,	// Use uncompressed image format
		OPT_NORMAL_DUDV = 0x00000200,	// dU dV normal map
		OPT_FILTER_NICE = 0x00000400,	// Use nice filtering
		OPT_SRGB_PC_TO_360 = 0x00000800,	// Perform srgb correction for colormaps

		OPT_SET_ALPHA_ONEOVERMIP = 0x00001000,	// Alpha  = 1/miplevel
		OPT_PREMULT_COLOR_ONEOVERMIP = 0x00002000,	// Color *= 1/miplevel
		OPT_MIP_ALPHATEST = 0x00004000,			// Alpha-tested mip generation
		OPT_NORMAL_GA = 0x00008000,			// Use Green-Alpha normal compression
		OPT_PREMULT_ALPHA_MIPFRACTION = 0x00010000,	// Alpha *= miplevel/mipcount 
		OPT_COMPUTE_GRADIENT = 0x00020000			// Compute normalized du du gradients of green channel and store in r and b
	};

	uint32 flags0;					// A combination of "Flags0"

	//
	// Decay settings
	//
	unsigned char clrDecayGoal[4];			// Goal colors for R G B A
	unsigned char numNotDecayMips[4];		// Number of first mips unaffected by decay (0 means all below mip0)
	float fDecayExponentBase[4];	// For exponential decay the base number (e.g. 0.75)

	//
	// Alpha fading with mip levels
	//
	unsigned char fullAlphaAtMipLevel;
	unsigned char minAlpha;
};
class IVTFTexture
{
public:
	virtual ~IVTFTexture() { }
	// Initializes the texture and allocates space for the bits
	// In most cases, you shouldn't force the mip count.
	virtual bool Init(int nWidth, int nHeight, int nDepth, ImageFormat fmt, int nFlags, int iFrameCount, int nForceMipCount = -1) = 0;

	// Methods to set other texture fields
	virtual void SetBumpScale(float flScale) = 0;
	virtual void SetReflectivity(const Vector& vecReflectivity) = 0;

	// Methods to initialize the low-res image
	virtual void InitLowResImage(int nWidth, int nHeight, ImageFormat fmt) = 0;

	// set the resource data (for writers). pass size=0 to delete data. if pdata is not null,
	// the resource data will be copied from *pData
	virtual void* SetResourceData(uint32 eType, void const* pData, size_t nDataSize) = 0;

	// find the resource data and return a pointer to it. The data pointed to by this pointer will
	// go away when the ivtftexture does. retruns null if resource not present
	virtual void* GetResourceData(uint32 eType, size_t* pDataSize) const = 0;

	// Locates the resource entry info if it's present, easier than crawling array types
	virtual bool HasResourceEntry(uint32 eType) const = 0;

	// Retrieve available resource types of this IVTFTextures
	//		arrTypesBuffer			buffer to be filled with resource types available.
	//		numTypesBufferElems		how many resource types the buffer can accomodate.
	// Returns:
	//		number of resource types available (can be greater than "numTypesBufferElems"
	//		in which case only first "numTypesBufferElems" are copied to "arrTypesBuffer")
	virtual unsigned int GetResourceTypes(uint32* arrTypesBuffer, int numTypesBufferElems) const = 0;

	// When unserializing, we can skip a certain number of mip levels,
	// and we also can just load everything but the image data
	// NOTE: If you load only the buffer header, you'll need to use the
	// VTFBufferHeaderSize() method below to only read that much from the file
	// NOTE: If you skip mip levels, the height + width of the texture will
	// change to reflect the size of the largest read in mip level
	virtual bool Unserialize(CUtlBuffer& buf, bool bHeaderOnly = false, int nSkipMipLevels = 0) = 0;
	virtual bool Serialize(CUtlBuffer& buf) = 0;

	// These are methods to help with optimization:
	// Once the header is read in, they indicate where to start reading
	// other data (measured from file start), and how many bytes to read....
	virtual void LowResFileInfo(int* pStartLocation, int* pSizeInBytes) const = 0;
	virtual void ImageFileInfo(int nFrame, int nFace, int nMip, int* pStartLocation, int* pSizeInBytes) const = 0;
	virtual int FileSize(int nMipSkipCount = 0) const = 0;

	// Attributes...
	virtual int Width() const = 0;
	virtual int Height() const = 0;
	virtual int Depth() const = 0;
	virtual int MipCount() const = 0;

	// returns the size of one row of a particular mip level
	virtual int RowSizeInBytes(int nMipLevel) const = 0;

	// returns the size of one face of a particular mip level
	virtual int FaceSizeInBytes(int nMipLevel) const = 0;

	virtual ImageFormat Format() const = 0;
	virtual int FaceCount() const = 0;
	virtual int FrameCount() const = 0;
	virtual int Flags() const = 0;

	virtual float BumpScale() const = 0;

	virtual int LowResWidth() const = 0;
	virtual int LowResHeight() const = 0;
	virtual ImageFormat LowResFormat() const = 0;

	// NOTE: reflectivity[0] = blue, [1] = greem, [2] = red
	virtual const Vector& Reflectivity() const = 0;

	virtual bool IsCubeMap() const = 0;
	virtual bool IsNormalMap() const = 0;
	virtual bool IsVolumeTexture() const = 0;

	// Computes the dimensions of a particular mip level
	virtual void ComputeMipLevelDimensions(int iMipLevel, int* pMipWidth, int* pMipHeight, int* pMipDepth) const = 0;

	// Computes the size (in bytes) of a single mipmap of a single face of a single frame 
	virtual int ComputeMipSize(int iMipLevel) const = 0;

	// Computes the size of a subrect (specified at the top mip level) at a particular lower mip level
	virtual void ComputeMipLevelSubRect(void* pSrcRect, int nMipLevel, void* pSubRect) const = 0;

	// Computes the size (in bytes) of a single face of a single frame
	// All mip levels starting at the specified mip level are included
	virtual int ComputeFaceSize(int iStartingMipLevel = 0) const = 0;

	// Computes the total size (in bytes) of all faces, all frames
	virtual int ComputeTotalSize() const = 0;

	// Returns the base address of the image data
	virtual unsigned char* ImageData() = 0;

	// Returns a pointer to the data associated with a particular frame, face, and mip level
	virtual unsigned char* ImageData(int iFrame, int iFace, int iMipLevel) = 0;

	// Returns a pointer to the data associated with a particular frame, face, mip level, and offset
	virtual unsigned char* ImageData(int iFrame, int iFace, int iMipLevel, int x, int y, int z = 0) = 0;

	// Returns the base address of the low-res image data
	virtual unsigned char* LowResImageData() = 0;

	// Converts the textures image format. Use IMAGE_FORMAT_DEFAULT
	// if you want to be able to use various tool functions below
	virtual	void ConvertImageFormat(ImageFormat fmt, bool bNormalToDUDV, bool bNormalToDXT5GA = false) = 0;

};

class CVTFTexture : public IVTFTexture
{
public:
	CVTFTexture();
	virtual ~CVTFTexture();

	virtual bool Init(int nWidth, int nHeight, int nDepth, ImageFormat fmt, int iFlags, int iFrameCount, int nForceMipCount);

	// Methods to initialize the low-res image
	virtual void InitLowResImage(int nWidth, int nHeight, ImageFormat fmt);

	virtual void* SetResourceData(uint32 eType, void const* pData, size_t nDataSize);
	virtual void* GetResourceData(uint32 eType, size_t* pDataSize) const;

	// Locates the resource entry info if it's present, easier than crawling array types
	virtual bool HasResourceEntry(uint32 eType) const;

	// Retrieve available resource types of this IVTFTextures
	//		arrTypesBuffer			buffer to be filled with resource types available.
	//		numTypesBufferElems		how many resource types the buffer can accomodate.
	// Returns:
	//		number of resource types available (can be greater than "numTypesBufferElems"
	//		in which case only first "numTypesBufferElems" are copied to "arrTypesBuffer")
	virtual unsigned int GetResourceTypes(uint32* arrTypesBuffer, int numTypesBufferElems) const;

	// Methods to set other texture fields
	virtual void SetBumpScale(float flScale);
	virtual void SetReflectivity(const Vector& vecReflectivity);

	// These are methods to help with optimization of file access
	virtual void LowResFileInfo(int* pStartLocation, int* pSizeInBytes) const;
	virtual void ImageFileInfo(int nFrame, int nFace, int nMip, int* pStartLocation, int* pSizeInBytes) const;
	virtual int FileSize(int nMipSkipCount = 0) const;

	// When unserializing, we can skip a certain number of mip levels,
	// and we also can just load everything but the image data
	virtual bool Unserialize(CUtlBuffer& buf, bool bBufferHeaderOnly = false, int nSkipMipLevels = 0);
	virtual bool Serialize(CUtlBuffer& buf);

	// Attributes...
	virtual int Width() const;
	virtual int Height() const;
	virtual int Depth() const;
	virtual int MipCount() const;

	virtual int RowSizeInBytes(int nMipLevel) const;
	virtual int FaceSizeInBytes(int nMipLevel) const;

	virtual ImageFormat Format() const;
	virtual int FaceCount() const;
	virtual int FrameCount() const;
	virtual int Flags() const;

	virtual float BumpScale() const;
	virtual const Vector& Reflectivity() const;

	virtual bool IsCubeMap() const;
	virtual bool IsNormalMap() const;
	virtual bool IsVolumeTexture() const;

	virtual int LowResWidth() const;
	virtual int LowResHeight() const;
	virtual ImageFormat LowResFormat() const;

	// Computes the size (in bytes) of a single mipmap of a single face of a single frame 
	virtual int ComputeMipSize(int iMipLevel) const;

	// Computes the size (in bytes) of a single face of a single frame
	// All mip levels starting at the specified mip level are included
	virtual int ComputeFaceSize(int iStartingMipLevel = 0) const;

	// Computes the total size of all faces, all frames
	virtual int ComputeTotalSize() const;

	// Computes the dimensions of a particular mip level
	virtual void ComputeMipLevelDimensions(int iMipLevel, int* pWidth, int* pHeight, int* pMipDepth) const;

	// Computes the size of a subrect (specified at the top mip level) at a particular lower mip level
	virtual void ComputeMipLevelSubRect(void* pSrcRect, int nMipLevel, void* pSubRect) const;

	// Returns the base address of the image data
	virtual unsigned char* ImageData();

	// Returns a pointer to the data associated with a particular frame, face, and mip level
	virtual unsigned char* ImageData(int iFrame, int iFace, int iMipLevel);

	// Returns a pointer to the data associated with a particular frame, face, mip level, and offset
	virtual unsigned char* ImageData(int iFrame, int iFace, int iMipLevel, int x, int y, int z);

	// Returns the base address of the low-res image data
	virtual unsigned char* LowResImageData();


};
