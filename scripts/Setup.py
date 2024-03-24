import os
import CheckPython

# Make sure everything we need is installed
CheckPython.validatePackages()

import CheckGLEW
import CheckGLFW
import CheckVulkan

# Change to root directory
os.chdir('../')

if not CheckGLEW.CheckGLEWLib():
    print('GLEW is not installed.')

if not CheckGLFW.CheckGLFWLib():
    print('GLFW is not installed.')

if not CheckVulkan.checkVulkanSDK():
    print('Vulkan SDK is not installed.')

if not CheckVulkan.checkVulkanSDKDebugLibs():
    print('Vulkan SDK debug libs not found.')
