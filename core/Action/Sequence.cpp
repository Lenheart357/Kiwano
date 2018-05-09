#include "..\e2daction.h"

e2d::Sequence::Sequence()
	: _currIndex(0)
{
}

e2d::Sequence::Sequence(int number, Action * action, ...) :
	_currIndex(0)
{
	va_list args;
	va_start(args, action);

	this->add(action);
	for (int i = 1; i < number; i++)
	{
		this->add(va_arg(args, Action*));
	}

	va_end(args);
}

#ifdef HIGHER_THAN_VS2012
e2d::Sequence::Sequence(const std::initializer_list<Action*>& actions)
	: _currIndex(0)
{
	this->add(actions);
}
#endif

e2d::Sequence::~Sequence()
{
}

void e2d::Sequence::_init()
{
	Action::_init();
	// 将所有动作与目标绑定
	if (_target)
	{
		for (auto action : _actions)
		{
			action->_target = _target;
		}
	}
	// 初始化第一个动作
	_actions[0]->_init();
}

void e2d::Sequence::onDestroy()
{
	Action::onDestroy();
	for (auto action : _actions)
	{
		SafeRelease(&action);
	}
}

void e2d::Sequence::_update()
{
	Action::_update();

	auto &action = _actions[_currIndex];
	action->_update();

	if (action->_isDone())
	{
		_currIndex++;
		if (_currIndex == _actions.size())
		{
			this->stop();
		}
		else
		{
			_actions[_currIndex]->_init();
		}
	}
}

void e2d::Sequence::reset()
{
	Action::reset();
	for (auto action : _actions)
	{
		action->reset();
	}
	_currIndex = 0;
}

void e2d::Sequence::_resetTime()
{
	for (auto action : _actions)
	{
		action->_resetTime();
	}
}

void e2d::Sequence::add(Action * action)
{
	if (action)
	{
		_actions.push_back(action);
		action->retain();
	}
}

void e2d::Sequence::add(int number, Action * action, ...)
{
	va_list args;
	va_start(args, action);

	this->add(action);
	for (int i = 1; i < number; i++)
	{
		this->add(va_arg(args, Action*));
	}

	va_end(args);
}

#ifdef HIGHER_THAN_VS2012
void e2d::Sequence::add(const std::initializer_list<Action*>& actions)
{
	for (const auto &action : actions)
	{
		this->add(action);
	}
}
#endif

e2d::Sequence * e2d::Sequence::clone() const
{
	auto a = new Sequence();
	for (auto action : _actions)
	{
		a->add(action->clone());
	}
	return a;
}

e2d::Sequence * e2d::Sequence::reverse() const
{
	auto a = new Sequence();
	for (auto action : _actions)
	{
		a->add(action->reverse());
	}
	// 将动作顺序逆序排列
	a->_actions.reserve(_actions.size());
	return a;
}