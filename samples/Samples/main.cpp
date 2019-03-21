// Copyright (C) 2019 Nomango

#include "Demo1.h"
#include "Demo2.h"
#include "Demo3.h"
#include "Demo4.h"

namespace
{
	struct Demo
	{
		String title;
		ScenePtr(*Create)();
	};

	Demo s_Demos[] = {
		{ L"����ʾ��", Demo1::Create },
		{ L"����ʾ��", Demo2::Create },
		{ L"��Ƶ����ʾ��", Demo3::Create },
		{ L"֡����ʾ��", Demo4::Create },
	};
	int s_DemoIndex = 0;
	int s_DemoNum = sizeof(s_Demos) / sizeof(Demo);
}

class DemoApp
	: public Application
{
public:
	DemoApp()
	{
		ShowConsole();
		// ʹ�� Audio ���
		Use(&Audio::Instance());

		Options options(L"Easy2Dʾ������", WINDOW_WIDTH, WINDOW_HEIGHT);
		Init(options);
	}

	void OnStart() override
	{
		ChangeDemoScene();
	}

	void ChangeDemoScene()
	{
		String title = s_Demos[s_DemoIndex].title;
		GetWindow()->SetTitle(title);

		ScenePtr scene = s_Demos[s_DemoIndex].Create();
		EnterScene(scene);

		// ���Ӱ�������
		scene->AddListener(Event::KeyUp, Closure(this, &DemoApp::KeyPressed));

		// ��ʾ��ʾ����
		String intro_str = format_wstring(L"���� 1~%d ���л�ʾ��\n", s_DemoNum);
		TextPtr intro = new Text(intro_str + title);
		intro->SetFontSize(16.f);
		intro->SetPosition(10, 10);
		scene->AddChild(intro);
	}

	void KeyPressed(Event const& evt)
	{
		E2D_ASSERT(evt.type == Event::KeyUp);

		if (evt.key.code > KeyCode::Num0 &&
			evt.key.code <= (KeyCode::Num0 + s_DemoNum))
		{
			s_DemoIndex = evt.key.code - KeyCode::Num1;
			ChangeDemoScene();
		}
	}
};

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	try
	{
		DemoApp app;
		app.Run();
	}
	catch (std::exception& e)
	{
		::MessageBoxA(nullptr, e.what(), "An exception has occurred!", MB_ICONERROR | MB_OK);
	}
	return 0;
}