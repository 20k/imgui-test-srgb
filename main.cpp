
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <imgui/imgui_internal.h>

#include <GL/glew.h>    // Initialize with glewInit()
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    bool is_srgb_enabled = true;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SRGB_CAPABLE, is_srgb_enabled);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    bool err = glewInit() != 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGui::SetStyleLinearColor(is_srgb_enabled);

    // Check that repeated applications of styling works
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Pushing a linear colour, which means that while it says we're 128, we get 50% perceptual brightness, aka 188
    ImGui::PushStyleLinearColor(ImGuiCol_WindowBg, IM_COL32(128, 0, 0, 255));
    // This is 128 in sRGB, which is not 50% brightness, but you get 128 onto the screen
    ImGui::PushStyleSRGBColor(ImGuiCol_TitleBgActive, IM_COL32(128, 0, 0, 255));

    ImVec4 window_bg_as_linear = ImGui::GetStyleLinearColor(ImGuiCol_WindowBg);
    ImVec4 window_bg_as_srgb = ImGui::GetStyleSRGBColor(ImGuiCol_WindowBg);

    printf("Window BG as linear: %f %f %f\n", window_bg_as_linear.x, window_bg_as_linear.y, window_bg_as_linear.z);
    printf("Window BG as displayed/SRGB: %f %f %f\n", window_bg_as_srgb.x, window_bg_as_srgb.y, window_bg_as_srgb.z);

    ImVec4 converted = ImGui::LinearToSRGB(window_bg_as_linear);

    float dx = window_bg_as_srgb.x - converted.x;
    float dy = window_bg_as_srgb.y - converted.y;
    float dz = window_bg_as_srgb.z - converted.z;

    assert(fabs(dx) < 0.0001);
    assert(fabs(dy) < 0.0001);
    assert(fabs(dz) < 0.0001);

    ImVec4 window_tbg_as_linear = ImGui::GetStyleLinearColor(ImGuiCol_TitleBgActive);
    ImVec4 window_tbg_as_srgb = ImGui::GetStyleSRGBColor(ImGuiCol_TitleBgActive);

    ImVec4 window_converted = ImGui::SRGBToLinear(window_tbg_as_srgb);

    float sdx = window_tbg_as_linear.x - window_converted.x;
    float sdy = window_tbg_as_linear.y - window_converted.y;
    float sdz = window_tbg_as_linear.z - window_converted.z;

    assert(fabs(sdx) < 0.0001);
    assert(fabs(sdy) < 0.0001);
    assert(fabs(sdz) < 0.0001);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Hello!");

        ImVec2 tl = ImGui::GetWindowPos();
        ImVec2 br = ImVec2(tl.x + 100, tl.y + 100);

        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(tl, br, IM_COL32(128, 0, 200, 255), 0);

        ImGui::Text("\n\n\n\n\n\n\n");

        if(ImGui::Checkbox("Srgb?", &is_srgb_enabled))
        {
            ImGui::SetStyleLinearColor(is_srgb_enabled);
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
