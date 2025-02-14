#define SPARK_USE_GL
#include "spark.hpp"
#include "opengl/spark_gl_shader.hpp"

using namespace spark;
using namespace spark::opengl;
using namespace spark::math;

static i32 frame = 0;

struct MeshName
{
    std::string_view name;
};

struct ShaderName
{
    std::string_view name;
};

// Creates a square mesh and stores it in the ItemManager.
void CreateSquareMesh(Application& app)
{
    // Create a vertex layout with a single attribute: 2D position.
    VertexLayout layout;
    layout.AddAttribute<f32>("a_position", AttributeType::VEC2, false);

    // Define four vertices for a unit square (using 2D positions).
    std::vector vertices = {
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f,  // Bottom right
         0.5f,  0.5f,  // Top right
        -0.5f,  0.5f   // Top left
    };

    // Define indices for two triangles that make up the square.
    std::vector<u32> indices = { 0, 1, 2, 2, 3, 0 };

    // Create the mesh (if not already present) and set its data.
    if (!app.HasItem<IMesh>("square_mesh"))
    {
        auto& mesh = app.AddItem<IMesh>("square_mesh");
        // The templated SetDataBytes will reinterpret the float vector as bytes.
        mesh.SetData(vertices, layout, indices);
    }
}

// Creates a simple shader for drawing a square and stores it as an Item.
void CreateSquareShader(Application& app)
{
    const std::string vertexShaderSource = R"(
	    #version 330 core
	    layout (location = 0) in vec2 a_position;

	    // Assume that the static attributes (vertex positions) use location 0.
	    // The instance model matrix will use locations 1, 2, 3, and 4.
	    layout (location = 1) in mat4 a_instance_model;

	    uniform mat4 u_viewProjection;

	    void main() {
	        gl_Position = u_viewProjection * a_instance_model * vec4(a_position, 0.0, 1.0);
		}
    )";

    const std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 frag_color;
        uniform vec4 u_color;
        void main() {
            frag_color = u_color;
        }
    )";

    if (!app.HasItem<IShaderProgram>("square_shader"))
    {
        auto& shader = app.AddItem<IShaderProgram>("square_shader");
        // Add shader stages generically.
        shader.AddShader(ShaderStageType::VERTEX, vertexShaderSource);
        shader.AddShader(ShaderStageType::FRAGMENT, fragmentShaderSource);
        shader.Link();
    }
}

void CreateMeshEntities(Application& app, Coordinator& coord)
{
	for (usize i  = 0; i < 1000000; i++)
	{
        coord.CreateEntity( Translate(Mat4(1.0f), Vec3(i, 0.0f, 0.0f)) *
            Scale(Mat4(1.0f), Vec3(100.0f, 100.0f, 1.0f)));
	}
}

// Creates a main camera and stores it as an Item.
void CreateMainCamera(Application& app)
{
    // Create a Camera with orthographic projection.
    Camera cam(
        Vec3(0.0f, 0.0f, 500.0f),  // position
        Vec3(0.0f, 0.0f, 0.0f),    // target
        45.0f,                     // fov (not used in orthographic)
        1280.0f / 720.0f,          // aspect ratio
        0.1f, 1000.0f,
        ProjectionMode::ORTHOGRAPHIC
    );

    // Increase the ortho scale so that the view volume is large enough.
    // A negative value increases the scale.
    cam.Zoom(-500.0f);

    app.AddItem<Camera>("main_camera", cam);
}


void RenderEntities(Application& app, Query<Mat4> query)
{
    Camera* cam_ptr = &app.GetItem<Camera>("main_camera");
    IMesh* mesh = &app.GetItem<IMesh>("square_mesh");
    IShaderProgram* shader = &app.GetItem<IShaderProgram>("square_shader");

    RenderCommand cmd;
    cmd.shader_program = shader;
    cmd.mesh = mesh;
    cmd.instance_transforms = query.GetVector();
    cmd.camera = cam_ptr;
    // In this instanced render command, you do not set a uniform model matrix.
    // Instead, your shader uses the instanced attribute.
    cmd.set_uniforms_fn = [=](IShaderProgram& shader)
        {
            shader.SetUniformMat4("u_viewProjection", cam_ptr->GetViewProjectionMatrix());

            float r = 1.0f;
            float g = 0.0f;
            float b = 0.0f;

            shader.SetUniformVec4("u_color", Vec4(r, g, b, 1.0f));
        };

    // The renderer will call DrawInstanced(instance_count).
    app.SubmitCommand(cmd);
}

spark::i32 main()
{
    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);

    // Register systems with correct parameter passing
    app.RegisterSystems(CreateSquareMesh, CreateSquareShader, CreateMainCamera, CreateMeshEntities, spark::SystemSettings{.phase = SystemPhase::ON_START});
    app.RegisterSystems(RenderEntities);

    app.Start();
    app.Run();

    return 0;
}
