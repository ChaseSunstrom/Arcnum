#ifndef SPARK_HPP
#define SPARK_HPP

#include "spark_pch.hpp"
#include "spark_event.hpp"
#include "spark_window.hpp"
#include "spark_bump_allocator.hpp"
#include "spark_stopwatch.hpp"
#include "spark_layer_stack.hpp"
#include "spark_vertex.hpp"
#include "spark_mesh.hpp"
#include "spark_util.hpp"
#include "spark_event.hpp"
#include "spark_command.hpp"
#include "spark_render_command.hpp"
#include "spark_camera.hpp"
#include "spark_math.hpp"
#include "spark_transform.hpp"
#include "spark_events.hpp"


#ifdef SPARK_USE_GL

	#include "opengl/spark_gl_window.hpp"
	#include "opengl/spark_gl_mesh.hpp"
	#include "opengl/spark_gl_shader.hpp"

#elif defined(SPARK_USE_DX)


#elif defined(SPARK_USE_VK)

#endif 

#include "spark_application.hpp"

#endif