#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include <core/render/renderer.hpp>

namespace Spark {
	class GLRenderer : public Renderer {
	  public:
		GLRenderer(GraphicsAPI gapi, Framebuffer& framebuffer, Manager<Resource>& resource_manager)
			: Renderer(gapi, framebuffer, resource_manager) {}

		void BeginFrame() override {
			glViewport(0, 0, m_window_width, m_window_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void Submit(const RenderCommand& command) override {
			switch (command.type) {
				case RenderCommandType::Draw:
					if (command.material && command.mesh) {
						command.material->Apply();
						command.mesh->Bind();
						command.mesh->Draw();
						command.mesh->Unbind();
					}
					break;

				case RenderCommandType::DrawInstanced:
					if (command.material && command.mesh) {
						command.material->Apply();
						command.mesh->Bind();
						command.mesh->DrawInstanced(command.instance_count);
						command.mesh->Unbind();
					}
					break;

				case RenderCommandType::SetState:
					ApplyRenderState(command.state);
					break;
			}
		}

	  private:
		void ApplyRenderState(const RenderState& state) {
			state.depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
			state.blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
			if (state.blend) {
				glBlendFunc(state.blend_src, state.blend_dst);
			}
			state.cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
			if (state.cull_face) {
				glCullFace(state.cull_mode);
			}
		}
	};
} // namespace Spark

#endif // SPARK_GL_RENDERER_HPP