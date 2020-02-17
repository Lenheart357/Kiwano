// Copyright (c) 2016-2018 Kiwano - Nomango
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <kiwano/render/ShapeMaker.h>
#include <kiwano/render/Renderer.h>

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
#include <kiwano/render/DirectX/NativePtr.h>
#endif

namespace kiwano
{

ShapeMakerPtr ShapeMaker::Create()
{
    ShapeMakerPtr maker = new (std::nothrow) ShapeMaker;
    return maker;
}

ShapeMaker::ShapeMaker() {}

ShapeMaker::~ShapeMaker()
{
    Clear();
}

void ShapeMaker::Clear()
{
    CloseStream();
    ResetNativePointer();
}

ShapePtr ShapeMaker::GetShape()
{
    return shape_;
}

bool ShapeMaker::IsStreamOpened() const
{
    return IsValid();
}

void ShapeMaker::BeginPath(Point const& begin_pos)
{
    if (!IsStreamOpened())
    {
        OpenStream();
    }

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->BeginFigure(DX::ConvertToPoint2F(begin_pos), D2D1_FIGURE_BEGIN_FILLED);
#else
    // not supported
#endif
}

void ShapeMaker::EndPath(bool closed)
{
    KGE_ASSERT(IsStreamOpened());

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->EndFigure(closed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
#else
    // not supported
#endif

    this->CloseStream();
}

void ShapeMaker::AddLine(Point const& point)
{
    KGE_ASSERT(IsStreamOpened());

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->AddLine(DX::ConvertToPoint2F(point));
#else
    // not supported
#endif
}

void ShapeMaker::AddLines(Vector<Point> const& points)
{
    KGE_ASSERT(IsStreamOpened());

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->AddLines(reinterpret_cast<const D2D_POINT_2F*>(&points[0]), static_cast<uint32_t>(points.size()));
#else
    // not supported
#endif
}

void kiwano::ShapeMaker::AddLines(const Point* points, size_t count)
{
    KGE_ASSERT(IsStreamOpened());

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->AddLines(reinterpret_cast<const D2D_POINT_2F*>(points), UINT32(count));
#else
    // not supported
#endif
}

void ShapeMaker::AddBezier(Point const& point1, Point const& point2, Point const& point3)
{
    KGE_ASSERT(IsStreamOpened());

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->AddBezier(
        D2D1::BezierSegment(DX::ConvertToPoint2F(point1), DX::ConvertToPoint2F(point2), DX::ConvertToPoint2F(point3)));
#else
    // not supported
#endif
}

void ShapeMaker::AddArc(Point const& point, Size const& radius, float rotation, bool clockwise, bool is_small)
{
    KGE_ASSERT(IsStreamOpened());

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);
    native->AddArc(D2D1::ArcSegment(DX::ConvertToPoint2F(point), DX::ConvertToSizeF(radius), rotation,
                                   clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                                   is_small ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE));
#else
    // not supported
#endif
}

ShapePtr ShapeMaker::Combine(ShapePtr shape_a, ShapePtr shape_b, CombineMode mode, const Matrix3x2* matrix)
{
    ShapeMakerPtr maker = ShapeMaker::Create();
    maker->OpenStream();

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    if (shape_a && shape_b)
    {
        auto geo_a  = NativePtr::Get<ID2D1Geometry>(shape_a);
        auto geo_b  = NativePtr::Get<ID2D1Geometry>(shape_b);
        auto native = NativePtr::Get<ID2D1GeometrySink>(maker);

        HRESULT hr = geo_a->CombineWithGeometry(geo_b.Get(), D2D1_COMBINE_MODE(mode), DX::ConvertToMatrix3x2F(matrix),
                                                native.Get());

        KGE_THROW_IF_FAILED(hr, "ID2D1Geometry::CombineWithGeometry failed");
    }
#else
    // not supported
#endif

    maker->CloseStream();
    return maker->GetShape();
}

void ShapeMaker::OpenStream()
{
    if (IsStreamOpened())
        return;

    Renderer::GetInstance().CreateShapeSink(*this);

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto geometry = NativePtr::Get<ID2D1PathGeometry>(shape_);
    if (geometry)
    {
        ComPtr<ID2D1GeometrySink> native;

        HRESULT hr = geometry->Open(&native);
        if (SUCCEEDED(hr))
        {
            NativePtr::Set(this, native);
        }
        KGE_THROW_IF_FAILED(hr, "ID2D1PathGeometry::Open failed");
    }
#else
    // not supported
#endif
}

void ShapeMaker::CloseStream()
{
    if (!IsStreamOpened())
        return;

#if KGE_RENDER_ENGINE == KGE_RENDER_ENGINE_DIRECTX
    auto native = NativePtr::Get<ID2D1GeometrySink>(this);

    HRESULT hr = native->Close();
    KGE_THROW_IF_FAILED(hr, "ID2D1PathGeometry::Close failed");

    ResetNativePointer();
#else
    return;  // not supported
#endif
}

void ShapeMaker::SetShape(ShapePtr shape)
{
    shape_ = shape;
}

}  // namespace kiwano