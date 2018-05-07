#include "VectorImage.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

CleanSVG::VectorImage::VectorImage(NSVGimage* handle)
    : handle_(handle)
{}

CleanSVG::VectorImage::~VectorImage()
{
    if (handle_)
    {
        nsvgDelete(handle_);
        handle_ = nullptr;
    }
}

std::unique_ptr<CleanSVG::VectorImage> CleanSVG::VectorImage::load(const char* filename)
{
    NSVGimage* handle = nsvgParseFromFile(filename, "px", 96.0f);
	if (!handle)
    {
        return nullptr;
	}
    return std::make_unique<VectorImage>(handle);
}

void CleanSVG::VectorImage::savePng(const char* filename)
{
    auto raster = toRaster();
    if (raster.width != 0 && raster.height != 0)
    {
        stbi_write_png(filename, raster.width, raster.height, 4, raster.data.data(), raster.width * 4);
    }
}

CleanSVG::RasterImage CleanSVG::VectorImage::toRaster() const
{
    RasterImage image;
    if (!handle_)
    {
        return image;
    }

    NSVGrasterizer* rast = nsvgCreateRasterizer();
	if (!rast)
    {
        return image;
	}

    image.width = int(handle_->width);
	image.height = int(handle_->height);
    image.data.resize(image.width * image.height * 4, ' ');
    nsvgRasterize(rast, handle_, 0, 0, 1, image.data.data(), image.width, image.height, image.width * 4);

    nsvgDeleteRasterizer(rast);

    return image;
}