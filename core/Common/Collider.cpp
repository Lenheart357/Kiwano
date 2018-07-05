#include "..\e2dcollider.h"
#include "..\e2dmanager.h"
#include "..\e2dnode.h"

e2d::Collider::Collider()
	: _visiable(true)
	, _color(Color::Red, 0.7)
	, _parentNode(nullptr)
	, _transformed(nullptr)
	, _geometry(nullptr)
	, _enabled(true)
	, _type(Collider::Type::None)
{
}

e2d::Collider::~Collider()
{
	SafeRelease(_transformed);
	SafeRelease(_geometry);
}

e2d::Node * e2d::Collider::getParentNode() const
{
	return _parentNode;
}

e2d::Color e2d::Collider::getColor() const
{
	return _color;
}

ID2D1Geometry * e2d::Collider::getGeometry() const
{
	return _geometry;
}

ID2D1TransformedGeometry * e2d::Collider::getTransformedGeometry() const
{
	return _transformed;
}

void e2d::Collider::setEnabled(bool enabled)
{
	_enabled = enabled;
}

void e2d::Collider::setVisiable(bool bVisiable)
{
	_visiable = bVisiable;
}

void e2d::Collider::setColor(Color color)
{
	_color = color;
}

void e2d::Collider::_render()
{
	if (_transformed && _enabled)
	{
		auto renderer = Renderer::getInstance();
		// ��ȡ��ɫ��ˢ
		ID2D1SolidColorBrush * pBrush = renderer->getSolidColorBrush();
		// ���û�ˢ��ɫ��͸����
		pBrush->SetColor(_color.toD2DColorF());
		// ���Ƽ�����ײ��
		renderer->getRenderTarget()->DrawGeometry(_transformed, pBrush);
	}
}

e2d::Collider::Relation e2d::Collider::getRelationWith(Collider * pCollider) const
{
	if (_transformed && pCollider->_transformed)
	{
		if (_enabled && pCollider->_enabled)
		{
			D2D1_GEOMETRY_RELATION relation;

			_transformed->CompareWithGeometry(
				pCollider->_transformed,
				D2D1::Matrix3x2F::Identity(),
				&relation
			);

			return Relation(relation);
		}
	}
	return Relation::Unknown;
}

void e2d::Collider::_recreate(Collider::Type type)
{
	_type = type;

	if (_parentNode)
	{
		SafeRelease(_geometry);
		SafeRelease(_transformed);

		switch (type)
		{
		case Type::Rect:
		{
			ID2D1RectangleGeometry* rectangle = nullptr;
			Renderer::getFactory()->CreateRectangleGeometry(
				D2D1::RectF(
					0,
					0,
					float(_parentNode->getRealWidth()),
					float(_parentNode->getRealHeight())),
				&rectangle
			);
			_geometry = rectangle;
		}
		break;

		case Type::Circle:
		{
			double minSide = std::min(_parentNode->getRealWidth(), _parentNode->getRealHeight());

			ID2D1EllipseGeometry* circle = nullptr;
			Renderer::getFactory()->CreateEllipseGeometry(
				D2D1::Ellipse(
					D2D1::Point2F(
						float(_parentNode->getRealWidth() / 2),
						float(_parentNode->getRealHeight() / 2)
					),
					float(minSide / 2),
					float(minSide / 2)
				),
				&circle
			);
			_geometry = circle;
		}
		break;

		case Type::Ellipse:
		{
			float halfWidth = float(_parentNode->getWidth() / 2),
				halfHeight = float(_parentNode->getHeight() / 2);

				ID2D1EllipseGeometry* ellipse = nullptr;
				Renderer::getFactory()->CreateEllipseGeometry(
					D2D1::Ellipse(
						D2D1::Point2F(
							halfWidth,
							halfHeight),
						halfWidth,
						halfHeight),
					&ellipse
				);
				_geometry = ellipse;
		}
		break;

		default:
			break;
		}
	}
}

void e2d::Collider::_transform()
{
	if (_parentNode && _enabled)
	{
		// ����������ײ��
		_recreate(_type);
		// ��ά�任
		Renderer::getFactory()->CreateTransformedGeometry(
			_geometry,
			_parentNode->_finalMatri,
			&_transformed
		);
		// ֪ͨ��ײ�������
		ColliderManager::getInstance()->__updateCollider(this);
	}
}