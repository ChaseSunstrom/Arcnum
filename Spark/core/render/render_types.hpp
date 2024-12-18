#ifndef SPARK_RENDER_TYPES_HPP
#define SPARK_RENDER_TYPES_HPP

#include <core/pch.hpp>
#include <core/util/gl.hpp>

namespace Spark {
	class GenericMesh;
	class Material;

	enum class RenderCommandType { Draw, DrawInstanced, SetState, Clear };

	struct RenderState {
		bool      depth_test  = true;
		bool      blend       = false;
		bool      cull_face   = true;
		GLenum    cull_mode   = GL_BACK;
		GLenum    blend_src   = GL_SRC_ALPHA;
		GLenum    blend_dst   = GL_ONE_MINUS_SRC_ALPHA;
		glm::vec4 clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
	};

	struct RenderCommand {
		RenderCommandType   type;
		RefPtr<GenericMesh> mesh;
		RefPtr<Material>    material;
		u32                 instance_count = 1;
		RenderState         state;
	};
} // namespace Spark

#endif // SPARK_RENDER_TYPES_HPP