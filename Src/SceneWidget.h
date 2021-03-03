#pragma once
#include "Global.h"
#include "DebugRenderer.h"
#include "CraftEngine/gui/widgetsext/OpenGLWidget.h"
#include "CraftEngine/graphics/guiext/CameraControllerWidget.h"
DebugRenderer gDebugRenderer;

class SceneWidget : public gui::Widget
{
private:
	gui::OpenGLWidget* m_oglWidget;
	graphics::extgui::CameraControllerWidget* m_cameraControlWidget;
	bool m_updateEveryFrame = false;
public:

	SceneWidget(Widget* parent) :Widget(parent)
	{
		m_oglWidget = new gui::OpenGLWidget(this);
		m_oglWidget->setRect(Rect(Point(0), getSize()));
		gDebugRenderer.init(getWidth(), getHeight());
		m_oglWidget->bindImage(gDebugRenderer.getImage(0));

		m_cameraControlWidget = new graphics::extgui::CameraControllerWidget(this);
		m_cameraControlWidget->setRect(Rect(Point(0), getSize()));
		m_cameraControlWidget->setCamera(&gScene.getCamera());
	}

	~SceneWidget()
	{
		gDebugRenderer.clear();
	}

	void render()
	{
		gDebugRenderer.renderScene(&gScene);
	}

	virtual void onResizeEvent(const ResizeEvent& resizEvent) override
	{
		gDebugRenderer.resize(getWidth(), getHeight());
		m_oglWidget->setRect(Rect(Point(0), getSize()));
		m_oglWidget->bindImage(gDebugRenderer.getImage(0));
		m_cameraControlWidget->setRect(Rect(Point(0), getSize()));
		m_cameraControlWidget->setChangedState(true);
	}

	virtual void onPaintEvent() override
	{
		if (m_updateEveryFrame || m_cameraControlWidget->getChangedState())
		{
			render();
			m_cameraControlWidget->setChangedState(false);
		}
		gui::Widget::onPaintEvent();
	}

	void setUpdateEveryFrame(bool enable)
	{
		m_updateEveryFrame = enable;
	}
};