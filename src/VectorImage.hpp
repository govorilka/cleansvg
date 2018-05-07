#pragma once

#include <memory>
#include <vector>

struct NSVGimage; 

namespace CleanSVG
{

struct RasterImage
{
    int width = 0;
    int height = 0;
    std::vector<unsigned char> data;
};

class VectorImage
{
public:
    VectorImage(NSVGimage* handle);
    ~VectorImage();

    static std::unique_ptr<VectorImage> load(const char* filename);

    void savePng(const char* filename);

    RasterImage toRaster() const;

private:
    NSVGimage* handle_ = nullptr;
};

} // namespace CleanSVG