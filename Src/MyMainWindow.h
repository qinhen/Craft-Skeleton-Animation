#pragma once
#include "Global.h"
#include "SceneWidget.h"
namespace UI
{
	const char* languagePackData =
		"File\n""文件\n"
		"Edit\n""编辑\n"
		"View\n""视图\n"
		"Tool\n""工具\n"

		;

	gui::Translator translator;
	gui::MenuBar* menuBar;
	gui::DockArea* dockArea;

	SceneWidget* sceneWidget;
}




class MyMainWindow : public gui::MainWindow
{
private:





public:

	MyMainWindow()
	{
		UI::menuBar = new gui::MenuBar(this);
		UI::menuBar->addMenu(L"File");
		UI::menuBar->addMenu(L"Edit");
		UI::menuBar->addMenu(L"View");
		UI::menuBar->addMenu(L"Tool");

		//gui::Action action0;
		 
		craft_engine_gui_connect(this, resized, this, onResized);
		
		UI::dockArea = new gui::DockArea(this);

		UI::sceneWidget = new SceneWidget(nullptr);
		UI::dockArea->dockCenter(UI::sceneWidget);

		auto dr = UI::dockArea->dockRight(new gui::TextBox(nullptr));
		dr.mFirst->dockCenter(new gui::TextBox(nullptr));
		dr.mFirst->dockCenter(new gui::TextBox(nullptr));
		dr.mFirst->dockCenter(new gui::TextBox(nullptr));


		{
			auto widget = new gui::FrameWidget(this);
			widget->setRect({ 550,50,400,400 });
			auto pos_text = new gui::TextLine(widget);
			auto dir_text = new gui::TextLine(widget);
			auto rot_text = new gui::TextLine(widget);
			auto fra_text = new gui::TextLine(widget);

			pos_text->setRect({ 0,0,400,22 });
			dir_text->setRect({ 0,22,400,22 });
			rot_text->setRect({ 0,44,400,22 });
			fra_text->setRect({ 0,66,400,22 });


			auto f = [=]() {
				auto pos = gScene.getCamera().getPosition();
				auto str = gui::StringTool::fromUtf8("Camera Position = (" +
					std::to_string(pos.x) + ", " +
					std::to_string(pos.y) + ", " +
					std::to_string(pos.z) + ")");
				if (!pos_text->isFocus() && pos_text->getText() != str)
					pos_text->setText(str);

				math::vec3 camFront = gScene.getCamera().getFrontDir();
				str = gui::StringTool::fromUtf8("Camera Front = (" +
					std::to_string(camFront.x) + ", " +
					std::to_string(camFront.y) + ", " +
					std::to_string(camFront.z) + ")");
				if (!dir_text->isFocus() && dir_text->getText() != str)
					dir_text->setText(str);

				auto rot = gScene.getCamera().getRotation();
				str = gui::StringTool::fromUtf8("Camera Rotation = (" +
					std::to_string(rot.x) + ", " +
					std::to_string(rot.y) + ", " +
					std::to_string(rot.z) + ")");
				if (!rot_text->isFocus() && rot_text->getText() != str)
					rot_text->setText(str);

				auto rate = this->getRenderer()->getFramRate();
				str = L"fps = " + gui::StringTool::fromValue(rate);
				if (!fra_text->isFocus() && fra_text->getText() != str)
					fra_text->setText(str);

			};
			craft_engine_gui_connect_v2(this, renderStarted, f);

			auto speed_slider = new gui::HSliderFloat(widget);
			speed_slider->setRect({ 0,88,400,22 });
			craft_engine_gui_connect_v2(speed_slider, slid, [&](const float& val) {
				gAnimationSpeed = val;
			});
			speed_slider->setValue(gAnimationSpeed, 0.1f, 100.0f);

			auto animation_idx_slider = new gui::HSliderInt(widget);
			animation_idx_slider->setRect({ 0,110,400,22 });
			craft_engine_gui_connect_v2(animation_idx_slider, slid, [&](const int& val) {
				gAnimationIndex = val;
				gCurrentTimeInSeconds = 0;
			});
			animation_idx_slider->setValue(gAnimationIndex, 0, 9);


			gui::CheckBox* checkBox = new gui::CheckBox(L"Animation", widget);
			checkBox->setRect({ 0,132,400,22 });
			checkBox->getCheckButton()->setCheckState(gui::CheckButton::eCheckState_Selected);
			craft_engine_gui_connect_v2(checkBox, stateChanged, [&](int state) {
				if (state == gui::CheckButton::eCheckState_Selected)
					gUsingAnimationShader = true;
				else
					gUsingAnimationShader = false;
			});





		}


		onResized(getSize());
	}

	~MyMainWindow()
	{
		delete UI::menuBar;
	}
protected:

	void onResized(const Size&)
	{
		UI::menuBar->setRect(Rect(0, 0, getWidth(), 25));
		UI::dockArea->setRect(Rect(0, 25, getWidth(), getHeight() - 25));
	}



	void onTranslationChanged()
	{
		UI::menuBar->setMenuName(UI::translator[L"File"], 0);
		UI::menuBar->setMenuName(UI::translator[L"Edit"], 1);
		UI::menuBar->setMenuName(UI::translator[L"View"], 2);
		UI::menuBar->setMenuName(UI::translator[L"Tool"], 3);
	}

	virtual void onAcceptFiles(const gui::StringList& filelist) override
	{
		for (auto& it : filelist)
		{
			std::wcout << it << "\n";
		}
		auto file_name = gui::StringTool::toUtf8(filelist.front());
		loadModel(file_name);
	}

public:
	void loadModel(const std::string& file_name)
	{
		gRenderAssetsManager.freeModel(gModelID);
		gScene.destroyInstance(gInstanceID);
		auto model = gRenderAssetsManager.loadModel(file_name.c_str());
		if (model < 0)
			return;

		auto ins = gScene.createInstance(model);
		gModelID = model;
		gInstanceID = ins;
		auto& model_aabb = gRenderAssetsManager.getModel(model).pOglData->mAABB;
		auto scale = model_aabb.mMax - model_aabb.mMin;
		auto inverse_scale = 1.0f / math::min(scale.x, scale.y, scale.z);
		gScene.getInstance(ins).mTransform.mScale = vec3(inverse_scale);
		gScene.updateScene();
	}

};


