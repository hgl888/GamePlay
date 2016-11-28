#include "Base.h"
#include "VkRectangle.h"

namespace vk
{

VkRectangle::VkRectangle()
    : x(0), y(0), width(0), height(0)
{
}

VkRectangle::VkRectangle(float width, float height) :
    x(0), y(0), width(width), height(height)
{
}

VkRectangle::VkRectangle(float x, float y, float width, float height) :
    x(x), y(y), width(width), height(height)
{
}

VkRectangle::VkRectangle(const VkRectangle& copy)
{
    set(copy);
}

VkRectangle::~VkRectangle()
{
}

const VkRectangle& VkRectangle::empty()
{
    static VkRectangle empty;
    return empty;
}

bool VkRectangle::isEmpty() const
{
    return (x == 0 && y == 0 && width == 0 && height == 0);
}

void VkRectangle::set(const VkRectangle& r)
{
    set(r.x, r.y, r.width, r.height);
}

void VkRectangle::set(float x, float y, float width, float height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

void VkRectangle::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}

float VkRectangle::left() const
{
    return x;
}

float VkRectangle::top() const
{
    return y;
}

float VkRectangle::right() const
{
    return x + width;
}

float VkRectangle::bottom() const
{
    return y + height;
}

bool VkRectangle::contains(float x, float y) const
{
    return (x >= this->x && x <= (this->x + width) && y >= this->y && y <= (this->y + height));
}

bool VkRectangle::contains(float x, float y, float width, float height) const
{
    return (contains(x, y) && contains(x + width, y + height));
}

bool VkRectangle::contains(const VkRectangle& r) const
{
    return contains(r.x, r.y, r.width, r.height);
}

bool VkRectangle::intersects(float x, float y, float width, float height) const
{
    float t;
    if ((t = x - this->x) > this->width || -t > width)
        return false;
    if ((t = y - this->y) > this->height || -t > height)
        return false;
    return true;
}

bool VkRectangle::intersects(const VkRectangle& r) const
{
    return intersects(r.x, r.y, r.width, r.height);
}

bool VkRectangle::intersect(const VkRectangle& r1, const VkRectangle& r2, VkRectangle* dst)
{
    GP_ASSERT(dst);

    float xmin = max(r1.x, r2.x);
    float xmax = min(r1.right(), r2.right());
    if (xmax > xmin)
    {
        float ymin = max(r1.y, r2.y);
        float ymax = min(r1.bottom(), r2.bottom());
        if (ymax > ymin)
        {
            dst->set(xmin, ymin, xmax - xmin, ymax - ymin);
            return true;
        }
    }

    dst->set(0, 0, 0, 0);
    return false;
}

void VkRectangle::combine(const VkRectangle& r1, const VkRectangle& r2, VkRectangle* dst)
{
    GP_ASSERT(dst);

    dst->x = min(r1.x, r2.x);
    dst->y = min(r1.y, r2.y);
    dst->width = max(r1.x + r1.width, r2.x + r2.width) - dst->x;
    dst->height = max(r1.y + r1.height, r2.y + r2.height) - dst->y;
}

void VkRectangle::inflate(float horizontalAmount, float verticalAmount)
{
    x -= horizontalAmount;
    y -= verticalAmount;
    width += horizontalAmount * 2;
    height += verticalAmount * 2;
}

VkRectangle& VkRectangle::operator = (const VkRectangle& r)
{
    x = r.x;
    y = r.y;
    width = r.width;
    height = r.height;
    return *this;
}

bool VkRectangle::operator == (const VkRectangle& r) const
{
    return (x == r.x && width == r.width && y == r.y && height == r.height);
}

bool VkRectangle::operator != (const VkRectangle& r) const
{
    return (x != r.x || width != r.width || y != r.y || height != r.height);
}

}
