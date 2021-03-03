
#pragma once
#include "Global.h"
#include "CraftEngine/graphics/AnimationSlover.h"

class DebugRenderer : public graphics::AbstractSceneRenderer
{
private:
	using Vertex = graphics::Vertex;
	using Texture = opengl::ImageTexture;
	int m_width, m_height;
	Texture m_colorAttachment;
	Texture m_idAttachment;
	Texture m_depthAttachment;
	opengl::FrameBuffer m_framebuffer;
	opengl::Shader m_shaders[2];

	opengl::Buffer m_uniformBuffer;
	opengl::VertexArrayObject m_vertexArray;
	graphics::AnimationSlover m_AnimationSlover;
	struct UniformBuffer
	{
		mat4 view;
		mat4 proj;
		vec4 viewPos;
		vec4 surfaceColor;
		vec4 lightDir;
		vec4 lightColor;
		vec4 ambientColor;
		float shininess;
	}m_uboData;
	std::unordered_map<int, int> m_instanceIdxMap;
public:

	GLint getImage(int id)
	{
		switch (id)
		{
		case 0: return m_colorAttachment.mImage;
		case 1: return m_depthAttachment.mImage;
		case 2: return m_idAttachment.mImage;
		}
	}

	DebugRenderer()
	{


	}

	virtual void init(int w, int h)
	{
		m_width = w;
		m_height = h;
		m_framebuffer.create();

		m_colorAttachment = opengl::createColorAttachment(m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
		m_idAttachment = opengl::createColorAttachment(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		m_depthAttachment = opengl::createDepthAttachment(m_width, m_height);


		m_framebuffer.attachColorAttachment(m_colorAttachment, 0);
		m_framebuffer.attachDepthAttachment(m_depthAttachment);
		m_framebuffer.attachColorAttachment(m_idAttachment, 1);
		m_framebuffer.drawBuffers(2);

		auto success = m_framebuffer.check();
		if (!success)
			throw 0;

		m_vertexArray.create();
		m_vertexArray.format(0, 3, GL_FLOAT, offsetof(Vertex, mPosition), 0);
		m_vertexArray.format(1, 3, GL_FLOAT, offsetof(Vertex, mNormal), 0);
		m_vertexArray.format(2, 2, GL_FLOAT, offsetof(Vertex, mTexcoords[0]), 0);
		m_vertexArray.format(3, 2, GL_FLOAT, offsetof(Vertex, mTexcoords[1]), 0);
		m_vertexArray.format(4, 3, GL_FLOAT, offsetof(Vertex, mTangent), 0);
		m_vertexArray.formati(5, 4, GL_UNSIGNED_INT, offsetof(Vertex, mBoneIndices), 0);
		m_vertexArray.format(6, 4, GL_FLOAT, offsetof(Vertex, mBoneWeights), 0);

		m_shaders[0].loadShaderFromFile("./shaders/glsl/debug_skinning.vert", "./shaders/glsl/debug_skinning.frag");
		m_shaders[1].loadShaderFromFile("./shaders/glsl/debug.vert", "./shaders/glsl/debug.frag");

		m_uniformBuffer = opengl::createUniformBuffer(sizeof(m_uboData));
		m_uboData.shininess = 32.0f;
		m_uboData.lightColor = vec4(0.5f);
		m_uboData.ambientColor = vec4(0.2f);
		m_uboData.lightDir.xyz = gScene.getCamera().getFrontDir();
		m_uboData.surfaceColor = vec4(0.5, 0.2, 0.1f, 0.0f);
	}

	virtual void clear()
	{
		opengl::destroyTexture(m_colorAttachment);
		opengl::destroyTexture(m_idAttachment);
		opengl::destroyTexture(m_depthAttachment);
	}


	virtual void resize(int w, int h)
	{
		clear();

		m_width = math::max(1, w);
		m_height = math::max(1, h);

		m_colorAttachment = opengl::createColorAttachment(m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
		m_idAttachment = opengl::createColorAttachment(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		m_depthAttachment = opengl::createDepthAttachment(m_width, m_height);

		m_framebuffer.attachColorAttachment(m_colorAttachment, 0);
		m_framebuffer.attachDepthAttachment(m_depthAttachment);
		m_framebuffer.attachColorAttachment(m_idAttachment, 1);

		auto success = m_framebuffer.check();
		if (!success)
			throw 0;
		m_framebuffer.unbind();

		gScene.getCamera().setAspectRatio(float(m_width) / float(m_height));
	}

	virtual void renderScene(graphics::AbstraceScene* scene) override
	{
		gCurrentTimeInSeconds += 0.01667 * gAnimationSpeed;
		//gCurrentTimeInSeconds = 0.0;


		m_uboData.lightDir.xyz = scene->getCamera().getFrontDir();
		m_uboData.view = scene->getCamera().matrices.view;
		m_uboData.proj = scene->getCamera().matrices.perspective;
		m_uboData.viewPos.xyz = scene->getCamera().getPosition();

		m_uniformBuffer.write(&m_uboData, sizeof(m_uboData));
		m_uniformBuffer.bindBase(0);
		m_vertexArray.bind();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT, GL_FILL);
		glCullFace(GL_BACK);
		m_framebuffer.bind();
		glViewport(0, 0, m_width, m_height);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);


		auto insList = scene->getInstanceList();
		m_instanceIdxMap.clear();
		int idx = 1;
		for (auto it : insList)
		{
			const auto& ins = scene->getInstance(it);
			const auto& data = gRenderAssetsManager.getModel(ins.mModel);
			
			bool have_animation =
				data.pFile->mBoneBuffer.boneData.size() > 0 && 
				data.pFile->mAnimationBuffer.animationInfoData.size() > 0 && 
				gUsingAnimationShader == true;
			auto shader = m_shaders[0];
			if (!have_animation)
				shader = m_shaders[1];

			shader.use();

			if (have_animation)
			{
				m_AnimationSlover.bindModel(data.pFile);
				int animation_idx = gAnimationIndex;
				if (animation_idx >= data.pFile->mAnimationBuffer.animationInfoData.size())
					animation_idx = data.pFile->mAnimationBuffer.animationInfoData.size() - 1;
				m_AnimationSlover.evaluate(gCurrentTimeInSeconds, animation_idx);
			}

			shader.setUniform(0, ins.mModelMatrix);
			m_vertexArray.bindVertexBuffer(data.pOglData->mVertexBuffer, 0, sizeof(graphics::Vertex), 0);
			m_instanceIdxMap.emplace(std::make_pair(idx, it));

			for (int i = 0; i < data.pFile->mMeshBuffer.meshData.size(); i++)
			{
				auto& meshData = data.pFile->mMeshBuffer.meshData[i];
				auto& mesh = data.pOglData->mMeshList[i];
				shader.setUniform(1, float(idx++));
				shader.setUniform(2, m_AnimationSlover.mBoneTransformMatrices.data(), m_AnimationSlover.mBoneTransformMatrices.size());
				auto mat_idx = mesh.mMaterial;
				m_vertexArray.bindElementBuffer(mesh.mIndexBuffer);
				glDrawElementsBaseVertex(GL_TRIANGLES, mesh.mCount, GL_UNSIGNED_INT, 0, mesh.mBase);
			}

		}

		m_framebuffer.unbind();
	}

};