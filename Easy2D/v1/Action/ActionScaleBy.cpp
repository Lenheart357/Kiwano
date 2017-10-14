#include "..\easy2d.h"
#include "..\Win\winbase.h"

ActionScaleBy::ActionScaleBy(float duration, float scaleX, float scaleY) :
	Animation(duration)
{
	m_nVariationX = scaleX;
	m_nVariationY = scaleY;
}

ActionScaleBy::~ActionScaleBy()
{
}

void ActionScaleBy::_init()
{
	Animation::_init();
	m_nBeginScaleX = m_pTargetSprite->getScaleX();
	m_nBeginScaleY = m_pTargetSprite->getScaleY();
}

void ActionScaleBy::_exec(steady_clock::time_point nNow)
{
	while (Animation::_isDelayEnough(nNow))
	{
		// �����ƶ�λ��
		float scale = float(m_nDuration) / m_nTotalDuration;
		// �ƶ� Sprite
		m_pTargetSprite->setScale(m_nBeginScaleX + m_nVariationX * scale, m_nBeginScaleX + m_nVariationX * scale);
		// �ж϶����Ƿ����
		if (_isEnd())
		{
			this->stop();
			break;
		}
	}
}

void ActionScaleBy::_reset()
{
	Animation::_reset();
}

ActionScaleBy * ActionScaleBy::copy() const
{
	return new ActionScaleBy(m_nAnimationInterval / 1000.0f, m_nVariationX, m_nVariationY);
}

ActionScaleBy * ActionScaleBy::reverse() const
{
	return new ActionScaleBy(m_nTotalDuration / 1000.0f, -m_nVariationX, -m_nVariationY);
}