#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include <core/pch.hpp>
#include <core/window/framebuffer.hpp>
#include <core/api.hpp>
#include "mesh.hpp"
#include "material.hpp"
#include "render_types.hpp"

namespace Spark {
	class Renderer {
	  public:
		Renderer(GraphicsAPI gapi, Framebuffer& framebuffer)
			: m_gapi(gapi)
			, m_framebuffer(framebuffer)
			, m_window_width(framebuffer.GetWidth())
			, m_window_height(framebuffer.GetHeight()) {}

		virtual ~Renderer() = default;

		virtual void BeginFrame() { SetViewport(m_window_width, m_window_height); }

		virtual void EndFrame() {}

		virtual void Submit(const RenderCommand& command) = 0;

		void SetViewport(i32 width, i32 height) {
			m_window_width  = width;
			m_window_height = height;
		}

		// Helper functions to create render commands
		static RenderCommand CreateDrawCommand(RefPtr<GenericMesh> mesh, RefPtr<Material> material) {
			RenderCommand cmd;
			cmd.type     = RenderCommandType::Draw;
			cmd.mesh     = mesh;
			cmd.material = material;
			return cmd;
		}

		static RenderCommand CreateInstancedDrawCommand(RefPtr<GenericMesh> mesh, RefPtr<Material> material, u32 count) {
			RenderCommand cmd;
			cmd.type           = RenderCommandType::DrawInstanced;
			cmd.mesh           = mesh;
			cmd.material       = material;
			cmd.instance_count = count;
			return cmd;
		}

		static RenderCommand CreateStateCommand(const RenderState& state) {
			RenderCommand cmd;
			cmd.type  = RenderCommandType::SetState;
			cmd.state = state;
			return cmd;
		}

	  protected:
		GraphicsAPI        m_gapi;
		Framebuffer&       m_framebuffer;
		i32                m_window_width;
		i32                m_window_height;
	};

	template <typename _Ty>
	concept IsRenderer = DerivedFrom<_Ty, Renderer>;
} // namespace Spark

#endif // SPARK_RENDERER_HPP