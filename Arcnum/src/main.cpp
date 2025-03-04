#define SPARK_USE_GL
#include "spark.hpp"

using namespace spark;
using namespace spark::math;

// Creates a square mesh and stores it in the ItemManager.
void CreateSquareMesh(Application& app)
{
    // Create a vertex layout with a single attribute: 2D position.
    VertexLayout layout;
    layout.AddAttribute<f32>("a_position", AttributeType::VEC2, false);

    // Define vertices for a unit square
    std::vector<f32> vertices = {
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f,  // Bottom right
         0.5f,  0.5f,  // Top right
        -0.5f,  0.5f   // Top left
    };

    // Define indices for two triangles
    std::vector<u32> indices = { 0, 1, 2, 2, 3, 0 };

    // Create the mesh if not present
    if (!app.HasItem<IMesh>("square_mesh"))
    {
        auto& mesh = app.AddItem<IMesh>("square_mesh");
        mesh.SetData(vertices, layout, indices);
    }
}

// Creates a shader that supports both instanced and regular rendering
void CreateSquareShader(Application& app)
{
    const std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in mat4 a_instance_transform;  // Instance data (optional)

        uniform mat4 u_viewProjection;
        uniform mat4 u_transform;  // Used for non-instanced rendering

        void main() {
            // Use instance transform if available, otherwise use uniform transform
            mat4 modelMatrix = (gl_InstanceID >= 0) ? a_instance_transform : u_transform;
            gl_Position = u_viewProjection * modelMatrix * vec4(a_position, 0.0, 1.0);
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
        shader.AddShader(ShaderStageType::VERTEX, vertexShaderSource);
        shader.AddShader(ShaderStageType::FRAGMENT, fragmentShaderSource);
        shader.Link();
    }
}

// Creates a main camera and stores it as an Item.
void CreateMainCamera(Application& app)
{
    Camera cam(
        Vec3(0.0f, 0.0f, 10.0f),   // position
        Vec3(0.0f, 0.0f, 0.0f),    // target
        45.0f,                      // fov
        1280.0f / 720.0f,          // aspect ratio
        0.1f, 1000.0f,             // near/far
        ProjectionMode::ORTHOGRAPHIC
    );
    cam.Zoom(-500.0f);
    app.AddItem<Camera>("main_camera", cam);
}

// Creates entities that will be automatically rendered by the render system
void CreateAutoRenderEntities(Application& app, Coordinator& coord)
{
    IMesh* mesh = &app.GetItem<IMesh>("square_mesh");
    IShaderProgram* shader = &app.GetItem<IShaderProgram>("square_shader");
    Camera* camera = &app.GetItem<Camera>("main_camera");
    
    if (!mesh || !shader || !camera) return;

    // Create a single entity with instance data for 5000 squares
    Entity entity = coord.CreateEntity();
    
    // Add unified renderable component
    RenderableComponent renderable;
    renderable.mesh = mesh;
    renderable.shader = shader;
    renderable.visible = true;
    renderable.depth_test = true;
    renderable.blending = true;
    renderable.wireframe = false;
    renderable.draw_mode = 0x0004; // GL_TRIANGLES

    // Set material properties
    renderable.material->SetUniform("u_viewProjection", camera->GetViewProjectionMatrix());
    renderable.material->SetUniform("u_color", Vec4(0.2f, 0.5f, 1.0f, 1.0f));

    // Add instance data for 5000 squares on the left side
    renderable.instance_transforms.reserve(5000);
    for (int i = 0; i < 5000; i++)
    {
        Mat4 transform = Mat4(1.0f);
        transform = Translate(transform, Vec3(i * 1.5f - 7500.0f, 100.0f, 0.0f));
        transform = Scale(transform, Vec3(1.0f));
        renderable.instance_transforms.push_back(transform);
    }

    coord.AddComponent<RenderableComponent>(entity, renderable);
}

// Creates another set of squares on the right side
void CreateRightSideSquares(Application& app, Coordinator& coord)
{
    IMesh* mesh = &app.GetItem<IMesh>("square_mesh");
    IShaderProgram* shader = &app.GetItem<IShaderProgram>("square_shader");
    Camera* camera = &app.GetItem<Camera>("main_camera");
    
    if (!mesh || !shader || !camera) return;

    // Create an entity for the right side squares
    Entity entity = coord.CreateEntity();
    
    RenderableComponent renderable;
    renderable.mesh = mesh;
    renderable.shader = shader;
    renderable.visible = true;
    renderable.depth_test = true;
    renderable.blending = true;
    renderable.wireframe = false;
    renderable.draw_mode = 0x0004; // GL_TRIANGLES

    // Set material properties including view projection
    renderable.material->SetUniform("u_viewProjection", camera->GetViewProjectionMatrix());
    renderable.material->SetUniform("u_color", Vec4(1.0f, 0.5f, 0.2f, 1.0f));

    // Add instance data for 5000 squares on the right side
    renderable.instance_transforms.reserve(5000);
    for (int i = 0; i < 5000; i++)
    {
        Mat4 transform = Mat4(1.0f);
        transform = Translate(transform, Vec3(i * 1.5f, -100.0f, 0.0f));
        transform = Scale(transform, Vec3(1.0f));
        renderable.instance_transforms.push_back(transform);
    }

    coord.AddComponent<RenderableComponent>(entity, renderable);
}

spark::i32 main()
{
    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);

    // Register initialization systems
    app.RegisterSystems(CreateSquareMesh, CreateSquareShader, CreateMainCamera, CreateAutoRenderEntities, 
        SystemSettings{.phase = SystemPhase::ON_START});
    
    app.RegisterSystem(CreateRightSideSquares,
        SystemSettings{.phase = SystemPhase::ON_START});

    app.Start();
    app.Run();

    return 0;
}
