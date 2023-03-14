#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#include "VectorImage.h"

namespace CleanSVG {

VectorImage::VectorImage(NSVGimage* handle)
    : m_handle(handle)
{}

VectorImage::~VectorImage()
{
    if (m_handle) {
        nsvgDelete(m_handle);
        m_handle = nullptr;
    }
}

std::unique_ptr<VectorImage> VectorImage::load(const char* filename)
{
    NSVGimage* handle = nsvgParseFromFile(filename, "px", 96.0f);
	if (!handle)
    {
        return nullptr;
	}
    return std::make_unique<VectorImage>(handle);
}

void VectorImage::savePng(const char* filename)
{
    auto raster = toRaster();
    if (raster.width != 0 && raster.height != 0) {
        stbi_write_png(filename, raster.width, raster.height, 4, raster.data.data(), raster.width * 4);
    }
}

CleanSVG::RasterImage VectorImage::toRaster() const
{
    RasterImage image;
    if (!m_handle) {
        return image;
    }

    NSVGrasterizer* rast = nsvgCreateRasterizer();
	if (!rast) {
        return image;
	}

    image.width = int(m_handle->width);
	image.height = int(m_handle->height);
    image.data.resize(image.width * image.height * 4, ' ');
    nsvgRasterize(rast, m_handle, 0, 0, 1, image.data.data(), image.width, image.height, image.width * 4);

    nsvgDeleteRasterizer(rast);

    return image;
}

} // namespace CleanSVG