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

#pragma once
#include <kiwano/math/Math.h>
#include <kiwano/core/Time.h>
#include <kiwano/base/ObjectBase.h>
#include <kiwano/base/component/ComponentManager.h>
#include <kiwano/event/EventDispatcher.h>
#include <kiwano/utils/TaskScheduler.h>
#include <kiwano/2d/animation/Animator.h>

namespace kiwano
{
class Stage;
class Director;
class RenderContext;

/// \~chinese
/// @brief 角色列表
typedef IntrusiveList<RefPtr<Actor>> ActorList;

/**
 * \~chinese
 * \defgroup Actors 基础角色
 */

/**
 * \addtogroup Actors
 * @{
 */

/**
 * \~chinese
 * @brief 角色
 * @details
 * 角色是舞台上最基本的元素，是完成渲染、更新、事件分发等功能的最小单位，也是动画、任务、事件监听等功能的载体
 */
class KGE_API Actor
    : public ObjectBase
    , public Animator
    , public TaskScheduler
    , public EventDispatcher
    , public ComponentManager
    , protected IntrusiveListValue<RefPtr<Actor>>
{
    friend class Director;
    friend class Transition;
    friend IntrusiveList<RefPtr<Actor>>;

public:
    using IntrusiveListValue<RefPtr<Actor>>::GetNext;
    using IntrusiveListValue<RefPtr<Actor>>::GetPrev;

    /// \~chinese
    /// @brief 角色更新回调函数
    typedef Function<void(Duration)> UpdateCallback;

    Actor();

    virtual ~Actor();

    /// \~chinese
    /// @brief 更新角色
    /// @details 每帧画面刷新前调用该函数，重载该函数以实现角色的更新处理
    /// @param dt 距上一次更新的时间间隔
    virtual void OnUpdate(Duration dt);

    /// \~chinese
    /// @brief 渲染角色
    /// @details
    /// 每帧画面刷新时调用该函数，默认不进行渲染，重载该函数以实现具体渲染过程
    /// @param ctx 渲染上下文
    virtual void OnRender(RenderContext& ctx);

    /// \~chinese
    /// @brief 获取显示状态
    bool IsVisible() const;

    /// \~chinese
    /// @brief 是否启用级联透明度
    bool IsCascadeOpacityEnabled() const;

    /// \~chinese
    /// @brief 获取名称的 Hash 值
    size_t GetHashName() const;

    /// \~chinese
    /// @brief 获取 Z 轴顺序
    int GetZOrder() const;

    /// \~chinese
    /// @brief 获取坐标
    Point GetPosition() const;

    /// \~chinese
    /// @brief 获取 x 坐标
    float GetPositionX() const;

    /// \~chinese
    /// @brief 获取 y 坐标
    float GetPositionY() const;

    /// \~chinese
    /// @brief 获取大小
    virtual Size GetSize() const;

    /// \~chinese
    /// @brief 获取宽度
    float GetWidth() const;

    /// \~chinese
    /// @brief 获取高度
    float GetHeight() const;

    /// \~chinese
    /// @brief 获取缩放后的宽度
    float GetScaledWidth() const;

    /// \~chinese
    /// @brief 获取缩放后的高度
    float GetScaledHeight() const;

    /// \~chinese
    /// @brief 获取缩放后的大小
    Size GetScaledSize() const;

    /// \~chinese
    /// @brief 获取锚点
    Point GetAnchor() const;

    /// \~chinese
    /// @brief 获取 x 方向锚点
    float GetAnchorX() const;

    /// \~chinese
    /// @brief 获取 y 方向锚点
    float GetAnchorY() const;

    /// \~chinese
    /// @brief 获取透明度
    float GetOpacity() const;

    /// \~chinese
    /// @brief 获取显示透明度
    float GetDisplayedOpacity() const;

    /// \~chinese
    /// @brief 获取旋转角度
    float GetRotation() const;

    /// \~chinese
    /// @brief 获取缩放比例
    Point GetScale() const;

    /// \~chinese
    /// @brief 获取横向缩放比例
    float GetScaleX() const;

    /// \~chinese
    /// @brief 获取纵向缩放比例
    float GetScaleY() const;

    /// \~chinese
    /// @brief 获取错切角度
    Point GetSkew() const;

    /// \~chinese
    /// @brief 获取横向错切角度
    float GetSkewX() const;

    /// \~chinese
    /// @brief 获取纵向错切角度
    float GetSkewY() const;

    /// \~chinese
    /// @brief 获取变换
    Transform GetTransform() const;

    /// \~chinese
    /// @brief 获取父角色
    Actor* GetParent() const;

    /// \~chinese
    /// @brief 获取所在舞台
    Stage* GetStage() const;

    /// \~chinese
    /// @brief 获取边框
    virtual Rect GetBounds() const;

    /// \~chinese
    /// @brief 获取外切包围盒
    virtual Rect GetBoundingBox() const;

    /// \~chinese
    /// @brief 获取二维变换矩阵
    const Matrix3x2& GetTransformMatrix() const;

    /// \~chinese
    /// @brief 获取二维变换的逆矩阵
    const Matrix3x2& GetTransformInverseMatrix() const;

    /// \~chinese
    /// @brief 获取变换到父角色的二维变换矩阵
    const Matrix3x2& GetTransformMatrixToParent() const;

    /// \~chinese
    /// @brief 设置角色是否可见
    void SetVisible(bool val);

    /// \~chinese
    /// @brief 设置名称
    void SetName(StringView name);

    /// \~chinese
    /// @brief 设置坐标
    void SetPosition(const Point& point);

    /// \~chinese
    /// @brief 设置坐标
    void SetPosition(float x, float y);

    /// \~chinese
    /// @brief 设置横坐标
    void SetPositionX(float x);

    /// \~chinese
    /// @brief 设置纵坐标
    void SetPositionY(float y);

    /// \~chinese
    /// @brief 移动至坐标
    void MoveTo(const Point& p);

    /// \~chinese
    /// @brief 移动至坐标
    void MoveTo(float x, float y);

    /// \~chinese
    /// @brief 移动相对坐标
    void MoveBy(const Vec2& trans);

    /// \~chinese
    /// @brief 移动相对坐标
    void MoveBy(float trans_x, float trans_y);

    /// \~chinese
    /// @brief 设置缩放比例，默认为 (1.0, 1.0)
    void SetScale(const Vec2& scale);

    /// \~chinese
    /// @brief 设置缩放比例，默认为 (1.0, 1.0)
    void SetScale(float scalex, float scaley);

    /// \~chinese
    /// @brief 设置错切角度，默认为 (0, 0)
    void SetSkew(const Vec2& skew);

    /// \~chinese
    /// @brief 设置错切角度，默认为 (0, 0)
    void SetSkew(float skewx, float skewy);

    /// \~chinese
    /// @brief 设置旋转角度，默认为 0
    void SetRotation(float rotation);

    /// \~chinese
    /// @brief 设置锚点位置，默认为 (0, 0), 范围 [0, 1]
    void SetAnchor(const Vec2& anchor);

    /// \~chinese
    /// @brief 设置锚点位置，默认为 (0, 0), 范围 [0, 1]
    void SetAnchor(float anchorx, float anchory);

    /// \~chinese
    /// @brief 修改大小
    void SetSize(const Size& size);

    /// \~chinese
    /// @brief 修改大小
    void SetSize(float width, float height);

    /// \~chinese
    /// @brief 修改宽度
    void SetWidth(float width);

    /// \~chinese
    /// @brief 修改高度
    void SetHeight(float height);

    /// \~chinese
    /// @brief 设置透明度，默认为 1.0, 范围 [0, 1]
    void SetOpacity(float opacity);

    /// \~chinese
    /// @brief 启用或禁用级联透明度
    void SetCascadeOpacityEnabled(bool enabled);

    /// \~chinese
    /// @brief 设置二维仿射变换
    void SetTransform(const Transform& transform);

    /// \~chinese
    /// @brief 设置 Z 轴顺序，默认为 0
    void SetZOrder(int zorder);

    /// \~chinese
    /// @brief 添加子角色
    void AddChild(RefPtr<Actor> child);

    /// \~chinese
    /// @brief 添加子角色
    void AddChild(RefPtr<Actor> child, int zorder);

    /// \~chinese
    /// @brief 添加多个子角色
    void AddChildren(const Vector<RefPtr<Actor>>& children);

    /// \~chinese
    /// @brief 获取名称相同的子角色
    RefPtr<Actor> GetChild(StringView name) const;

    /// \~chinese
    /// @brief 获取所有名称相同的子角色
    Vector<RefPtr<Actor>> GetChildren(StringView name) const;

    /// \~chinese
    /// @brief 获取全部子角色
    ActorList& GetAllChildren();

    /// \~chinese
    /// @brief 获取全部子角色
    const ActorList& GetAllChildren() const;

    /// \~chinese
    /// @brief 移除子角色
    void RemoveChild(RefPtr<Actor> child);

    /// \~chinese
    /// @brief 移除所有名称相同的子角色
    void RemoveChildren(StringView child_name);

    /// \~chinese
    /// @brief 移除所有角色
    void RemoveAllChildren();

    /// \~chinese
    /// @brief 从父角色移除
    void RemoveFromParent();

    /// \~chinese
    /// @brief 暂停角色更新
    void PauseUpdating();

    /// \~chinese
    /// @brief 继续角色更新
    void ResumeUpdating();

    /// \~chinese
    /// @brief 角色更新是否暂停
    bool IsUpdatePausing() const;

    /// \~chinese
    /// @brief 设置更新时的回调函数
    void SetCallbackOnUpdate(const UpdateCallback& cb);

    /// \~chinese
    /// @brief 获取更新时的回调函数
    UpdateCallback GetCallbackOnUpdate() const;

    /// \~chinese
    /// @brief 判断点是否在角色内
    virtual bool ContainsPoint(const Point& point) const;

    /// \~chinese
    /// @brief 将世界坐标系点转换为局部坐标系点
    Point ConvertToLocal(const Point& point) const;

    /// \~chinese
    /// @brief 将局部坐标系点转换为世界坐标系点
    Point ConvertToWorld(const Point& point) const;

    /// \~chinese
    /// @brief 渲染角色边界
    void ShowBorder(bool show);

    /// \~chinese
    /// @brief 序列化
    void DoSerialize(Serializer* serializer) const override;

    /// \~chinese
    /// @brief 反序列化
    void DoDeserialize(Deserializer* deserializer) override;

    /// \~chinese
    /// @brief 设置默认锚点
    static void SetDefaultAnchor(float anchor_x, float anchor_y);

protected:
    /// \~chinese
    /// @brief 更新自身和所有子角色
    virtual void Update(Duration dt);

    void UpdateSelf(Duration dt);

    /// \~chinese
    /// @brief 渲染自身和所有子角色
    virtual void Render(RenderContext& ctx);

    /// \~chinese
    /// @brief 绘制自身和所有子角色的边界
    virtual void RenderBorder(RenderContext& ctx);

    /// \~chinese
    /// @brief 检查是否在渲染上下文的视区内
    virtual bool CheckVisibility(RenderContext& ctx) const;

    /// \~chinese
    /// @brief 渲染前初始化渲染上下文状态，仅当 CheckVisibility 返回真时调用该函数
    virtual void PrepareToRender(RenderContext& ctx);

    /// \~chinese
    /// @brief 更新自己的二维变换，并通知所有子角色
    void UpdateTransform() const;

    /// \~chinese
    /// @brief 向上追溯更新
    /// @details 对于节点树 A->B(dirty)->C->D，当对 D 执行 UpdateTransformUpwards 时会对 B、C、D 从上到下依次更新
    void UpdateTransformUpwards() const;

    /// \~chinese
    /// @brief 更新自己和所有子角色的透明度
    void UpdateOpacity();

    /// \~chinese
    /// @brief 将所有子角色按Z轴顺序排序
    void Reorder();

    /// \~chinese
    /// @brief 设置节点所在舞台
    void SetStage(Stage* stage);

    enum DirtyFlag : uint8_t
    {
        Clean                 = 0,
        DirtyTransform        = 1,
        DirtyTransformInverse = 1 << 1,
        DirtyOpacity          = 1 << 2,
        DirtyVisibility       = 1 << 3
    };

    Flag<uint8_t>& GetDirtyFlag() const;

private:
    bool         visible_;
    bool         update_pausing_;
    bool         cascade_opacity_;
    bool         show_border_;
    mutable bool visible_in_rt_;

    mutable Flag<uint8_t> dirty_flag_;

    int            z_order_;
    float          opacity_;
    float          displayed_opacity_;
    Actor*         parent_;
    Stage*         stage_;
    size_t         hash_name_;
    Point          anchor_;
    Size           size_;
    ActorList      children_;
    UpdateCallback cb_update_;
    Transform      transform_;

    mutable Matrix3x2 transform_matrix_;
    mutable Matrix3x2 transform_matrix_inverse_;
    mutable Matrix3x2 transform_matrix_to_parent_;
};

/** @} */

inline void Actor::OnUpdate(Duration dt)
{
    KGE_NOT_USED(dt);
}

inline void Actor::OnRender(RenderContext& ctx)
{
    KGE_NOT_USED(ctx);
}

inline Flag<uint8_t>& Actor::GetDirtyFlag() const
{
    return dirty_flag_;
}

inline bool Actor::IsVisible() const
{
    return visible_;
}

inline bool Actor::IsCascadeOpacityEnabled() const
{
    return cascade_opacity_;
}

inline size_t Actor::GetHashName() const
{
    return hash_name_;
}

inline int Actor::GetZOrder() const
{
    return z_order_;
}

inline Point Actor::GetPosition() const
{
    return transform_.position;
}

inline float Actor::GetPositionX() const
{
    return GetPosition().x;
}

inline float Actor::GetPositionY() const
{
    return GetPosition().y;
}

inline Point Actor::GetScale() const
{
    return transform_.scale;
}

inline float Actor::GetScaleX() const
{
    return GetScale().x;
}

inline float Actor::GetScaleY() const
{
    return GetScale().y;
}

inline Point Actor::GetSkew() const
{
    return transform_.skew;
}

inline float Actor::GetSkewX() const
{
    return GetSkew().x;
}

inline float Actor::GetSkewY() const
{
    return GetSkew().y;
}

inline float Actor::GetRotation() const
{
    return transform_.rotation;
}

inline float Actor::GetWidth() const
{
    return GetSize().x;
}

inline float Actor::GetHeight() const
{
    return GetSize().y;
}

inline Size Actor::GetSize() const
{
    return size_;
}

inline float Actor::GetScaledWidth() const
{
    return GetWidth() * GetScaleX();
}

inline float Actor::GetScaledHeight() const
{
    return GetHeight() * GetScaleY();
}

inline Size Actor::GetScaledSize() const
{
    return Size{ GetScaledWidth(), GetScaledHeight() };
}

inline Point Actor::GetAnchor() const
{
    return anchor_;
}

inline float Actor::GetAnchorX() const
{
    return GetAnchor().x;
}

inline float Actor::GetAnchorY() const
{
    return GetAnchor().y;
}

inline float Actor::GetOpacity() const
{
    return opacity_;
}

inline float Actor::GetDisplayedOpacity() const
{
    return displayed_opacity_;
}

inline Transform Actor::GetTransform() const
{
    return transform_;
}

inline Actor* Actor::GetParent() const
{
    return parent_;
}

inline Stage* Actor::GetStage() const
{
    return stage_;
}

inline void Actor::PauseUpdating()
{
    update_pausing_ = true;
}

inline void Actor::ResumeUpdating()
{
    update_pausing_ = false;
}

inline bool Actor::IsUpdatePausing() const
{
    return update_pausing_;
}

inline void Actor::SetCallbackOnUpdate(const UpdateCallback& cb)
{
    cb_update_ = cb;
}

inline Actor::UpdateCallback Actor::GetCallbackOnUpdate() const
{
    return cb_update_;
}

inline void Actor::ShowBorder(bool show)
{
    show_border_ = show;
}

inline void Actor::SetPosition(float x, float y)
{
    this->SetPosition(Point(x, y));
}

inline void Actor::SetPositionX(float x)
{
    this->SetPosition(Point(x, transform_.position.y));
}

inline void Actor::SetPositionY(float y)
{
    this->SetPosition(Point(transform_.position.x, y));
}

inline void Actor::MoveTo(const Point& p)
{
    this->SetPosition(p);
}

inline void Actor::MoveTo(float x, float y)
{
    this->SetPosition(Point(x, y));
}

inline void Actor::MoveBy(const Vec2& trans)
{
    this->SetPosition(transform_.position.x + trans.x, transform_.position.y + trans.y);
}

inline void Actor::MoveBy(float trans_x, float trans_y)
{
    this->MoveBy(Vec2(trans_x, trans_y));
}

inline void Actor::SetScale(float scalex, float scaley)
{
    this->SetScale(Vec2(scalex, scaley));
}

inline void Actor::SetAnchor(float anchorx, float anchory)
{
    this->SetAnchor(Vec2(anchorx, anchory));
}

inline void Actor::SetSize(float width, float height)
{
    this->SetSize(Size(width, height));
}

inline void Actor::SetWidth(float width)
{
    this->SetSize(Size(width, size_.y));
}

inline void Actor::SetHeight(float height)
{
    this->SetSize(Size(size_.x, height));
}

inline void Actor::SetSkew(float skewx, float skewy)
{
    this->SetSkew(Vec2(skewx, skewy));
}

}  // namespace kiwano
