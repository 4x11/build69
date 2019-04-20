#include "../../main.h"
#include "RenderWare.h"

RsGlobalType* RsGlobal;

/* rwcore.h */
RwCamera* (*RwCameraBeginUpdate)(RwCamera* camera);
RwCamera* (*RwCameraEndUpdate)(RwCamera* camera);
RwCamera* (*RwCameraShowRaster)(RwCamera * camera, void *pDev, RwUInt32 flags);

RwRaster* 	(*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool 		(*RwRasterDestroy)(RwRaster * raster);
RwRaster* 	(*RwRasterGetOffset)(RwRaster *raster, RwInt16 *xOffset, RwInt16 *yOffset);
RwInt32 	(*RwRasterGetNumLevels)(RwRaster * raster);
RwRaster* 	(*RwRasterSubRaster)(RwRaster * subRaster, RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterRenderFast)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRender)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRenderScaled)(RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterPushContext)(RwRaster * raster);
RwRaster* 	(*RwRasterPopContext)(void);
RwRaster* 	(*RwRasterGetCurrentContext)(void);
RwBool 		(*RwRasterClear)(RwInt32 pixelValue);
RwBool 		(*RwRasterClearRect)(RwRect * rpRect, RwInt32 pixelValue);
RwRaster* 	(*RwRasterShowRaster)(RwRaster * raster, void *dev, RwUInt32 flags);
RwUInt8* 	(*RwRasterLock)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlock)(RwRaster * raster);
RwUInt8* 	(*RwRasterLockPalette)(RwRaster * raster, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlockPalette)(RwRaster * raster);
RwImage* 	(*RwImageCreate)(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool 		(*RwImageDestroy)(RwImage * image);
RwImage* 	(*RwImageAllocatePixels)(RwImage * image);
RwImage* 	(*RwImageFreePixels)(RwImage * image);
RwImage* 	(*RwImageCopy)(RwImage * destImage, const RwImage * sourceImage);
RwImage* 	(*RwImageResize)(RwImage * image, RwInt32 width, RwInt32 height);
RwImage* 	(*RwImageApplyMask)(RwImage * image, const RwImage * mask);
RwImage* 	(*RwImageMakeMask)(RwImage * image);
RwImage* 	(*RwImageReadMaskedImage)(const RwChar * imageName, const RwChar * maskname);
RwImage* 	(*RwImageRead)(const RwChar * imageName);
RwImage* 	(*RwImageWrite)(RwImage * image, const RwChar * imageName);
RwImage* 	(*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);
RwRaster* 	(*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);
RwRaster* 	(*RwRasterRead)(const RwChar *filename);
RwRaster* 	(*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);
RwImage* 	(*RwImageFindRasterFormat)(RwImage *ipImage, RwInt32 nRasterType, RwInt32 *npWidth, RwInt32 *npHeight, RwInt32 *npDepth,RwInt32 *npFormat);

/* rwlpcore.h */
RwReal 		(*RwIm2DGetNearScreenZ)(void);
RwReal 		(*RwIm2DGetFarScreenZ)(void);
RwBool 		(*RwRenderStateGet)(RwRenderState state, void *value);
RwBool 		(*RwRenderStateSet)(RwRenderState state, void *value);
RwBool 		(*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool 		(*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
RwBool 		(*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
RwBool 		(*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices, RwImVertexIndex *indices, RwInt32 numIndices);

/* rtpng.h */
RwImage*	(*RtPNGImageWrite)(RwImage* image, const RwChar* imageName);
RwImage* 	(*RtPNGImageRead)(const RwChar* imageName);

void InitRenderWareFunctions()
{
	Log("Initializing RenderWare..");

	/* skeleton.h */
	RsGlobal = (RsGlobalType*)(g_libGTASA+0x95B068);

	/* rwCore.h */
	*(void**)(&RwCameraBeginUpdate) 			= (void*)(g_libGTASA+0x1AD6C8+1);
	*(void**)(&RwCameraEndUpdate) 				= (void*)(g_libGTASA+0x1AD6B8+1);
	*(void**)(&RwCameraShowRaster)				= (void*)(g_libGTASA+0x1AD8C4+1);

	*(void **)(&RwRasterCreate) 				= (void*)(g_libGTASA+0x1B0778+1);
	*(void **)(&RwRasterDestroy) 				= (void*)(g_libGTASA+0x1B059C+1);
	*(void **)(&RwRasterGetOffset) 				= (void*)(g_libGTASA+0x1B0460+1);
	*(void **)(&RwRasterGetNumLevels) 			= (void*)(g_libGTASA+0x1B06B4+1);
	*(void **)(&RwRasterSubRaster) 				= (void*)(g_libGTASA+0x1B0724+1);
	*(void **)(&RwRasterRenderFast)				= (void*)(g_libGTASA+0x1B0500+1);
	*(void **)(&RwRasterRender)					= (void*)(g_libGTASA+0x1B054C+1);
	*(void **)(&RwRasterRenderScaled)			= (void*)(g_libGTASA+0x1B0440+1);
	*(void **)(&RwRasterPushContext)			= (void*)(g_libGTASA+0x1B05E4+1);
	*(void **)(&RwRasterPopContext)				= (void*)(g_libGTASA+0x1B0674+1);
	*(void **)(&RwRasterGetCurrentContext)		= (void*)(g_libGTASA+0x1B0414+1);
	*(void **)(&RwRasterClear)					= (void*)(g_libGTASA+0x1B0498+1);
	*(void **)(&RwRasterClearRect)				= (void*)(g_libGTASA+0x1B052C+1);
	*(void **)(&RwRasterShowRaster)				= (void*)(g_libGTASA+0x1B06F0+1);
	*(void **)(&RwRasterLock)					= (void*)(g_libGTASA+0x1B0814+1);
	*(void **)(&RwRasterUnlock)					= (void*)(g_libGTASA+0x1B0474+1);
	*(void **)(&RwRasterLockPalette)			= (void*)(g_libGTASA+0x1B0648+1);
	*(void **)(&RwRasterUnlockPalette)			= (void*)(g_libGTASA+0x1B0578+1);
	*(void **)(&RwImageCreate)					= (void*)(g_libGTASA+0x1AF338+1);
	*(void **)(&RwImageDestroy)					= (void*)(g_libGTASA+0x1AF44C+1);
	*(void **)(&RwImageAllocatePixels)			= (void*)(g_libGTASA+0x1AF38C+1);
	*(void **)(&RwImageFreePixels)				= (void*)(g_libGTASA+0x1AF420+1);
	*(void **)(&RwImageCopy)					= (void*)(g_libGTASA+0x1AFA50+1);
	*(void **)(&RwImageResize)					= (void*)(g_libGTASA+0x1AF490+1);
	*(void **)(&RwImageApplyMask)				= (void*)(g_libGTASA+0x1AFBB0+1);
	*(void **)(&RwImageMakeMask)				= (void*)(g_libGTASA+0x1AF5CC+1);
	*(void **)(&RwImageReadMaskedImage)			= (void*)(g_libGTASA+0x1AFCF8+1);
	*(void **)(&RwImageRead)					= (void*)(g_libGTASA+0x1AF74C+1);
	*(void **)(&RwImageWrite)					= (void*)(g_libGTASA+0x1AF980+1);
	*(void **)(&RwImageSetFromRaster)			= (void*)(g_libGTASA+0x1B023C+1);
	*(void **)(&RwRasterSetFromImage)			= (void*)(g_libGTASA+0x1B0260+1);
	*(void **)(&RwRasterRead)					= (void*)(g_libGTASA+0x1B035C+1);
	*(void **)(&RwRasterReadMaskedRaster)		= (void*)(g_libGTASA+0x1B03CC+1);
	*(void **)(&RwImageFindRasterFormat)		= (void*)(g_libGTASA+0x1B0284+1);

	/* rwlpcore.h */
	*(void **)(&RwIm2DGetNearScreenZ)			= (void*)(g_libGTASA+0x1B8038+1);
	*(void **)(&RwIm2DGetFarScreenZ)			= (void*)(g_libGTASA+0x1B8054+1);
	*(void **)(&RwRenderStateGet)				= (void*)(g_libGTASA+0x1B80A8+1);
	*(void **)(&RwRenderStateSet)				= (void*)(g_libGTASA+0x1B8070+1);
	*(void **)(&RwIm2DRenderLine)				= (void*)(g_libGTASA+0x1B80C4+1);
	*(void **)(&RwIm2DRenderTriangle)			= (void*)(g_libGTASA+0x1B80E0+1);
	*(void **)(&RwIm2DRenderPrimitive)			= (void*)(g_libGTASA+0x1B80FC+1);
	*(void **)(&RwIm2DRenderIndexedPrimitive)	= (void*)(g_libGTASA+0x1B8118+1);

	/* rtpng.h */
	*(void **)(&RtPNGImageWrite)				= (void*)(g_libGTASA+0x1D6CEC+1);
	*(void **)(&RtPNGImageRead)					= (void*)(g_libGTASA+0x1D6F84+1);
}