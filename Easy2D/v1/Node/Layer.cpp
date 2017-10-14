#include "..\easy2d.h"


Layer::Layer() :
	m_bBlock(true)
{
}

Layer::~Layer()
{
}

int Layer::getBlock() const
{
	return m_bBlock;
}

void Layer::setBlock(bool block)
{
	m_bBlock = block;
}

bool Layer::_exec(bool active)
{
	// ��ͼ��������Ϣ������Զȡ�û��潹��
	return BatchNode::_exec(active) || m_bBlock;
}