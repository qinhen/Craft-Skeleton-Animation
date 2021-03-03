#pragma once

#include "CraftEngine/graphics/ModelImport.h"
#include "CraftEngine/graphics/ModelOGL.h"
using namespace CraftEngine;
class RenderAssetsManager
{
public:
	struct Model
	{
		graphics::ModelFile* pFile;
		graphics::ModelOGL*  pOglData;
	};
private:
	std::unordered_map<int, Model> m_modelFileList;
	std::unordered_map<int, opengl::ImageTexture> m_imageFileList;
	std::unordered_map<int, graphics::Material> m_matFileList;
	int m_modelFileID = 0;
	int m_imageFileID = 0;
	int m_matFileID = 0;
public:


	int loadModel(const char* path)
	{
		Model model;
		model.pFile = new graphics::ModelFile;
		auto success = graphics::importModel(path, *model.pFile);
		if (!success)
			return -1;
		model.pOglData = new graphics::ModelOGL;
		success = model.pOglData->load(*model.pFile);

		if (!success)
			return -1;
		int id = m_modelFileID++;
		m_modelFileList.emplace(std::make_pair(id, model));
		return id;
	}

	void freeModel(int id)
	{
		auto it = m_modelFileList.find(id);
		if (it == m_modelFileList.end())
			return;
		it->second.pOglData->clear();
		delete it->second.pOglData;
		delete it->second.pFile;
		m_modelFileList.erase(it);
	}

	const Model& getModel(int id) const
	{
		return m_modelFileList.find(id)->second;
	}

	int loadImage(const char* path)
	{
		core::Bitmap file;	
		auto success = file.loadFromFile(path);
		if (!success)
			return -1;
		opengl::ImageTexture image;
		image = opengl::createTexture2D(file.width(), file.height(), GL_RGBA, file.data(), file.size(), false);
		int id = m_imageFileID++;
		m_imageFileList.emplace(std::make_pair(id, image));
		return id;
	}

	void freeImage(int id)
	{
		opengl::destroyTexture(m_imageFileList.find(id)->second);
		m_imageFileList.erase(id);
	}

	const opengl::ImageTexture& getImage(int id) const
	{
		return m_imageFileList.find(id)->second;
	}

	


};
