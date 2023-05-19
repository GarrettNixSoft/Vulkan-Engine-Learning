echo Compiling shaders...
cd "C:\Users\Garrett\Visual Studio Workspaces\Vulkan Engine\shaders"
C:/VulkanSDK/1.3.246.1/Bin/glslc.exe simple_shader.vert -o vert.spv
C:/VulkanSDK/1.3.246.1/Bin/glslc.exe simple_shader.frag -o frag.spv
echo Shaders compiled.