#pragma once

/**
 * \ingroup rwraster
 * \ref RwRasterLockMode represents the options available for locking 
 * a raster so that it may be modified (see API function \ref RwRasterLock). An 
 * application may wish to write to the raster, read from the raster or
 * simultaneously write and read a raster (rwRASTERLOCKWRITE | rwRASTERLOCKREAD).
 */
enum RwRasterLockMode
{
    rwRASTERLOCKWRITE = 0x01,   /**<Lock for writing */
    rwRASTERLOCKREAD = 0x02,    /**<Lock for reading */
    rwRASTERLOCKNOFETCH = 0x04, /**<When used in combination with
                                 *  rwRASTERLOCKWRITE, asks the driver not to
                                 *  fetch the pixel data. This is only useful
                                 *  if it is known that ALL the raster data is
                                 *  going to be overwritten before the raster
                                 *  is unlocked, i.e. from an 
                                 *  \ref RwRasterSetFromImage call. This flag
                                 *  is not supported by all drivers. */
    rwRASTERLOCKRAW = 0x08,    /**<When used in combination with
                                   rwRASTERLOCKWRITE or rwRASTERLOCKREAD
                                   allows access to the raw platform specific
                                   pixel format */
    rwRASTERLOCKMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

#define rwRASTERLOCKREADWRITE   (rwRASTERLOCKREAD|rwRASTERLOCKWRITE)

typedef enum RwRasterLockMode RwRasterLockMode;

enum RwRasterFlipMode
{
    rwRASTERFLIPDONTWAIT = 0,   /**<Don't wait for VSync */
    rwRASTERFLIPWAITVSYNC = 1,  /**<Flip on VSync */
    rwRASTERFLIPMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterFlipMode RwRasterFlipMode;

enum RwRasterType
{
    rwRASTERTYPENORMAL = 0x00,          /**<Normal */
    rwRASTERTYPEZBUFFER = 0x01,         /**<Z Buffer */
    rwRASTERTYPECAMERA = 0x02,          /**<Camera */
    rwRASTERTYPETEXTURE = 0x04,         /**<Texture */
    rwRASTERTYPECAMERATEXTURE = 0x05,   /**<Camera texture */
    rwRASTERTYPEMASK = 0x07,            /**<Mask for finding type */

    rwRASTERPALETTEVOLATILE = 0x40,        /**<If set, hints that the palette will change often */
    rwRASTERDONTALLOCATE = 0x80,        /**<If set the raster is not allocated */
    rwRASTERTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterType RwRasterType;

/**
 * \ingroup rwraster
 * \ref RwRasterFormat is a set of values and flags which may be combined to
 * specify a raster format. The format chosen for a particular raster depends
 * on the hardware device and the raster type specified at creation time
 * (see API function \ref RwRasterCreate). The format may be retrieved using
 * API function \ref RwRasterGetFormat.
 *
 * The raster format is a packed set of bits which contains the following
 * four pieces of information (these may be combined with bitwise OR):
 *
 * <ol>
 * <li> The pixel color format corresponding to one of the following values:
 *      <ul>
 *      <li> rwRASTERFORMAT1555
 *      <li> rwRASTERFORMAT565
 *      <li> rwRASTERFORMAT4444
 *      <li> rwRASTERFORMATLUM8
 *      <li> rwRASTERFORMAT8888
 *      <li> rwRASTERFORMAT888
 *      <li> rwRASTERFORMAT16
 *      <li> rwRASTERFORMAT24
 *      <li> rwRASTERFORMAT32
 *      <li> rwRASTERFORMAT555
 *      </ul>
 *      This value may be masked out of the raster format using
 *      rwRASTERFORMATPIXELFORMATMASK.
 * <li> The palette depth if the raster is palettized:
 *      <ul> 
 *      <li> rwRASTERFORMATPAL4
 *      <li> rwRASTERFORMATPAL8
 *      </ul>
 *      In these cases, the color format refers to that of the palette.
 * <li> Flag rwRASTERFORMATMIPMAP. Set if the raster contains mipmap levels.
 * <li> Flag rwRASTERFORMATAUTOMIPMAP. Set if the mipmap levels were generated
 *      automatically by RenderWare.
 * </ol>
 */
enum RwRasterFormat
{
    rwRASTERFORMATDEFAULT = 0x0000, /* Whatever the hardware likes best */

    rwRASTERFORMAT1555 = 0x0100,    /**<16 bits - 1 bit alpha, 5 bits red, green and blue */
    rwRASTERFORMAT565 = 0x0200,     /**<16 bits - 5 bits red and blue, 6 bits green */
    rwRASTERFORMAT4444 = 0x0300,    /**<16 bits - 4 bits per component */
    rwRASTERFORMATLUM8 = 0x0400,    /**<Gray scale */
    rwRASTERFORMAT8888 = 0x0500,    /**<32 bits - 8 bits per component */
    rwRASTERFORMAT888 = 0x0600,     /**<24 bits - 8 bits per component */
    rwRASTERFORMAT16 = 0x0700,      /**<16 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT24 = 0x0800,      /**<24 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT32 = 0x0900,      /**<32 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT555 = 0x0a00,     /**<16 bits - 5 bits red, green and blue */

    rwRASTERFORMATAUTOMIPMAP = 0x1000, /**<RenderWare generated the mip levels */

    rwRASTERFORMATPAL8 = 0x2000,    /**<8 bit palettised */
    rwRASTERFORMATPAL4 = 0x4000,    /**<4 bit palettised */

    rwRASTERFORMATMIPMAP = 0x8000,  /**<Mip mapping on */

    rwRASTERFORMATPIXELFORMATMASK = 0x0f00, /**<The pixel color format 
                                             *  (excluding palettised bits) */
    rwRASTERFORMATMASK = 0xff00     /**<The whole format */ ,
    rwRASTERFORMATFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterFormat RwRasterFormat;

enum RwRasterPrivateFlag
{
    rwRASTERGAMMACORRECTED = 0x01,
    rwRASTERPIXELLOCKEDREAD = 0x02, /* pixels are locked for reading */
    rwRASTERPIXELLOCKEDWRITE = 0x04, /* pixels are locked for writing */
    rwRASTERPALETTELOCKEDREAD = 0x08, /* palette is locked for reading */
    rwRASTERPALETTELOCKEDWRITE = 0x10, /* palette is locked for writing */
    rwRASTERPIXELLOCKEDRAW = 0x20, /* the pixels are in platform specific
                                      format, used in combination with
                                      rwRASTERPIXELLOCKEDREAD &
                                      rwRASTERPIXELLOCKEDWRITE */
    rwRASTERPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterPrivateFlag RwRasterPrivateFlag;

#define rwRASTERPIXELLOCKED     (rwRASTERPIXELLOCKEDREAD | rwRASTERPIXELLOCKEDWRITE)
#define rwRASTERPALETTELOCKED   (rwRASTERPALETTELOCKEDREAD | rwRASTERPALETTELOCKEDWRITE)
#define rwRASTERLOCKED          (rwRASTERPIXELLOCKED|rwRASTERPALETTELOCKED)

/* How big is my stack!!! */
#define rwRASTERCONTEXTSTACKSIZE 10

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwraster
 * \struct RwRaster 
 * Raster containing device-dependent pixels. 
 * This should be considered an opaque type.
 * Use the RwRaster API functions to access.
 */
typedef struct RwRaster RwRaster;

struct RwRaster
{
    RwRaster           *parent; /* Top level raster if a sub raster */
    RwUInt8            *cpPixels; /* Pixel pointer when locked */
    RwUInt8            *palette; /* Raster palette */
    RwInt32             width, height, depth; /* Dimensions of raster */
    RwInt32             stride; /* Lines bytes of raster */
    RwInt16             nOffsetX, nOffsetY; /* Sub raster offset */
    RwUInt8             cType;  /* Type of raster */
    RwUInt8             cFlags; /* Raster flags */
    RwUInt8             privateFlags; /* Raster private flags */
    RwUInt8             cFormat; /* Raster format */

    RwUInt8            *originalPixels;
    RwInt32             originalWidth;
    RwInt32             originalHeight;
    RwInt32             originalStride;
};

/****************************************************************************
 <macro/inline functionality
 */

#define RwRasterGetWidthMacro(_raster) \
    ((_raster)->width)

#define RwRasterGetHeightMacro(_raster) \
    ((_raster)->height)

#define RwRasterGetStrideMacro(_raster) \
    ((_raster)->stride)

#define RwRasterGetDepthMacro(_raster) \
    ((_raster)->depth)

#define RwRasterGetFormatMacro(_raster) \
    ((((_raster)->cFormat) & (rwRASTERFORMATMASK >> 8)) << 8)

#define RwRasterGetTypeMacro(_raster) \
    (((_raster)->cType) & rwRASTERTYPEMASK)

#define RwRasterGetParentMacro(_raster) \
    ((_raster)->parent)

#define RwRasterGetWidth(_raster)                   \
    RwRasterGetWidthMacro(_raster)

#define RwRasterGetHeight(_raster)                  \
    RwRasterGetHeightMacro(_raster)

#define RwRasterGetStride(_raster)                  \
    RwRasterGetStrideMacro(_raster)

#define RwRasterGetDepth(_raster)                   \
    RwRasterGetDepthMacro(_raster)

#define RwRasterGetFormat(_raster)                  \
    RwRasterGetFormatMacro(_raster)

#define RwRasterGetType(_raster)                  \
    RwRasterGetTypeMacro(_raster)

#define RwRasterGetParent(_raster)                  \
    RwRasterGetParentMacro(_raster)

extern RwRaster* (*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
extern RwBool (*RwRasterDestroy)(RwRaster * raster);

extern RwRaster* (*RwRasterGetOffset)(RwRaster *raster, 
                                      RwInt16 *xOffset, RwInt16 *yOffset);

extern RwInt32 (*RwRasterGetNumLevels)(RwRaster * raster);

extern RwRaster* (*RwRasterSubRaster)(RwRaster * subRaster,
                                      RwRaster * raster, RwRect * rect);

extern RwRaster* (*RwRasterRenderFast)(RwRaster * raster, RwInt32 x,
                                       RwInt32 y);
extern RwRaster* (*RwRasterRender)(RwRaster * raster, RwInt32 x,
                                       RwInt32 y);
extern RwRaster* (*RwRasterRenderScaled)(RwRaster * raster,
                                         RwRect * rect);

/* Raster rendering context */
extern RwRaster* (*RwRasterPushContext)(RwRaster * raster);
extern RwRaster* (*RwRasterPopContext)(void);
extern RwRaster* (*RwRasterGetCurrentContext)(void);

/* Clearing rasters */
extern RwBool (*RwRasterClear)(RwInt32 pixelValue);
extern RwBool (*RwRasterClearRect)(RwRect * rpRect,
                                          RwInt32 pixelValue);

/* Displaying rasters */
extern RwRaster* (*RwRasterShowRaster)(RwRaster * raster, void *dev,
                                       RwUInt32 flags);

/* Locking and releasing */
extern RwUInt8* (*RwRasterLock)(RwRaster * raster, RwUInt8 level,
                                 RwInt32 lockMode);
extern RwRaster* (*RwRasterUnlock)(RwRaster * raster);
extern RwUInt8* (*RwRasterLockPalette)(RwRaster * raster,
                                        RwInt32 lockMode);
extern RwRaster* (*RwRasterUnlockPalette)(RwRaster * raster);

/****************************************************************************
 Defines
 */

/**
 * \ingroup rwimage
 * \struct RwImage 
 * Image containing device-independent pixels. 
 * This should be considered an opaque type.
 * Use the RwImage API functions to access.
 */
typedef struct RwImage RwImage;

struct RwImage
{
        RwInt32             flags;

        RwInt32             width;  /* Device may have different ideas */
        RwInt32             height; /* internally !! */

        RwInt32             depth;  /* Of referenced image */
        RwInt32             stride;

        RwUInt8            *cpPixels;
        RwRGBA             *palette;
};

#define RwImageSetStrideMacro(_image, _stride)      \
    (((_image)->stride = (_stride)), (_image))

#define RwImageSetPixelsMacro(_image, _pixels)      \
    (((_image)->cpPixels = (_pixels)), (_image))

#define RwImageSetPaletteMacro(_image, _palette)    \
    (((_image)->palette = (_palette)), (_image))

#define RwImageGetWidthMacro(_image)                \
    ((_image)->width)

#define RwImageGetHeightMacro(_image)               \
    ((_image)->height)

#define RwImageGetDepthMacro(_image)                \
    ((_image)->depth)

#define RwImageGetStrideMacro(_image)               \
    ((_image)->stride)

#define RwImageGetPixelsMacro(_image)               \
    ((_image)->cpPixels)

#define RwImageGetPaletteMacro(_image)              \
    ((_image)->palette)


#define RwImageSetStride(_image, _stride)           \
    RwImageSetStrideMacro(_image, _stride)

#define RwImageSetPixels(_image, _pixels)           \
    RwImageSetPixelsMacro(_image, _pixels)

#define RwImageSetPalette(_image, _palette)         \
    RwImageSetPaletteMacro(_image, _palette)

#define RwImageGetWidth(_image)                     \
    RwImageGetWidthMacro(_image)

#define RwImageGetHeight(_image)                    \
    RwImageGetHeightMacro(_image)

#define RwImageGetDepth(_image)                     \
    RwImageGetDepthMacro(_image)

#define RwImageGetStride(_image)                    \
    RwImageGetStrideMacro(_image)

#define RwImageGetPixels(_image)                    \
    RwImageGetPixelsMacro(_image)

#define RwImageGetPalette(_image)                   \
    RwImageGetPaletteMacro(_image)

extern RwImage* (*RwImageCreate)(RwInt32 width, RwInt32 height,
                                  RwInt32 depth);
extern RwBool (*RwImageDestroy)(RwImage * image);

    /* Allocating */
extern RwImage* (*RwImageAllocatePixels)(RwImage * image);
extern RwImage* (*RwImageFreePixels)(RwImage * image);

    /* Converting images */
extern RwImage* (*RwImageCopy)(RwImage * destImage,
                                const RwImage * sourceImage);

    /* Resizing images */
extern RwImage* (*RwImageResize)(RwImage * image, RwInt32 width,
                                  RwInt32 height);

    /* Producing masks ! */
extern RwImage* (*RwImageApplyMask)(RwImage * image,
                                     const RwImage * mask);
extern RwImage* (*RwImageMakeMask)(RwImage * image);

    /* Helper functions */
extern RwImage* (*RwImageReadMaskedImage)(const RwChar * imageName,
                                           const RwChar * maskname);
extern RwImage* (*RwImageRead)(const RwChar * imageName);
extern RwImage* (*RwImageWrite)(RwImage * image,
                                 const RwChar * imageName);

/****************************************************************************
 Global types
 */

typedef struct RwBBox RwBBox;

struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};

/****************************************************************************
 Function prototypes
 */
 /* Images from rasters */
extern RwImage* (*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);

/* Rasters from images */
extern RwRaster* (*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);

/* Read a raster */
extern RwRaster* (*RwRasterRead)(const RwChar *filename);
extern RwRaster* (*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);

/* Finding appropriate raster formats */
extern RwImage* (*RwImageFindRasterFormat)(RwImage *ipImage,RwInt32 nRasterType,
                                        RwInt32 *npWidth,RwInt32 *npHeight,
                                        RwInt32 *npDepth,RwInt32 *npFormat);

struct RwCamera {};

extern RwCamera*    (*RwCameraBeginUpdate)(RwCamera * camera);
extern RwCamera*    (*RwCameraEndUpdate)(RwCamera * camera);
extern RwCamera*    (*RwCameraShowRaster)(RwCamera * camera, void *pDev, RwUInt32 flags);

#define rwTEXTUREBASENAMELENGTH     32

struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
typedef struct RwLLLink  RwLLLink;

struct RwTexture
{
    RwRaster           *raster; /** pointer to RwRaster with data */                      //+0
    void               *dict;   /* Dictionary this texture is in */                       //+4
    RwLLLink            lInDictionary; /* List of textures in this dictionary */          //+8

    RwChar              name[rwTEXTUREBASENAMELENGTH];  /* Name of the texture */         //+16
    RwChar              mask[rwTEXTUREBASENAMELENGTH];  /* Name of the textures mask */   //+48

    /* 31 [xxxxxxxx xxxxxxxx vvvvuuuu ffffffff] 0 */
    RwUInt32            filterAddressing; /* Filtering & addressing mode flags */         //+80

    RwInt32             refCount; /* Reference count, surprisingly enough */              //+84
};
typedef struct RwTexture RwTexture;