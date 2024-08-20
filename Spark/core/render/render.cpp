#include "renderer.hpp"

namespace Spark
{
	void Renderer::AddRenderable(const std::string& model_name, const Transform& transform)
	{
		m_renderables[model_name] = transform;
	}

	void Renderer::RemoveRenderables(const std::string& model_name)
	{
		auto it = m_renderables.begin();

		if (it != m_renderables.end())
		{
			m_renderables.erase(it);
		}
	}

	void Renderer::RemoveRenderable(const Transform& transform)
	{
		auto it = std::find_if(m_renderables.begin(), m_renderables.end(), [&](const auto& pair)
		{
			return pair.second == transform;
		});

		if (it != m_renderables.end())
		{
			m_renderables.erase(it);
		}
	}

	void Renderer::ClearRenderables()
	{
		m_renderables.clear();
	}
}