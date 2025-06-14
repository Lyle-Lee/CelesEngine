import os
import sys
# import shutil
from pathlib import Path
import Utils

VULKAN_SDK = os.environ.get('VULKAN_SDK')
CE_VULKAN_VERSION = '1.3.275.0'
VulkanDirectory = 'CelesEngine/Dependencies/VulkanSDK'
if sys.platform == 'win32':
    VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.275.0/windows/VulkanSDK-1.3.275.0-Installer.exe'
    VULKAN_SDK_EXE_PATH = f'{VulkanDirectory}/VulkanSDK-1.3.275.0-Installer.exe'
elif sys.platform == 'darwin':
    VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.275.0/mac/vulkansdk-macos-1.3.275.0.dmg'
    VULKAN_SDK_EXE_PATH = f'{VulkanDirectory}/vulkansdk-macos-1.3.275.0.dmg'
else:  # Linux
    VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.275.0/linux/vulkansdk-linux-x86_64-1.3.275.0.tar.xz'
    VULKAN_SDK_EXE_PATH = f'{VulkanDirectory}/vulkansdk-linux-x86_64-1.3.275.0.tar.xz' # TODO: Handle extraction for Linux

def installVulkanSDK():
    print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.downloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print('Done.')
    print('Running Vulkan SDK installer...')
    if sys.platform == 'win32':
        os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    elif sys.platform == 'darwin':
        os.system(f'open "{os.path.abspath(VULKAN_SDK_EXE_PATH)}"')
    else:
        print("Automatic installer launch is not supported on this platform. Please extract and install manually.")

    print('Please re-run this script after installation.')

def installVulkanPrompt():
    print('Would you like to install the Vulkan SDK?')
    install = Utils.yesOrNo()
    if install:
        installVulkanSDK()
        quit()

def checkVulkanSDK():
    if VULKAN_SDK is None:
        print("You don't have the Vulkan SDK installed!")
        installVulkanPrompt()
        return False
    
    print(f"Vulkan SDK located at '{VULKAN_SDK}'.")
    if CE_VULKAN_VERSION not in VULKAN_SDK:
        print(f"You don't have the correct Vulkan SDK version (Celes requires {CE_VULKAN_VERSION})!")
        installVulkanPrompt()
        return False
    
    return True

def checkVulkanSDKDebugLibs():
    if sys.platform != 'win32':
        return True

    shadercdLib = Path(f'{VULKAN_SDK}/Lib/shaderc_sharedd.lib')
    if not shadercdLib.exists():
        print(f"No Vulkan SDK debug libs found (checked '{shadercdLib}'). Please install Vulkan SDK debug libs while installing Vulkan SDK.")
        # foldersToCopy = ['Lib']
        # for folder in foldersToCopy:
        #     srcPath = os.path.join(VULKAN_SDK, folder)
        #     dstPath = os.path.join(VulkanDirectory, folder)
        #     if os.path.exists(dstPath):
        #         print(f"'{folder}' already exists in '{VulkanDirectory}'.")
        #     else:
        #         if os.path.exists(srcPath):
        #             shutil.copytree(srcPath, dstPath)
        #             print(f"Copied '{srcPath} to '{VulkanDirectory}'.")
        #         else:
        #             print(f"'{srcPath}' does not exist!")
        return False

    print(f"Vulkan SDK debug libs located at '{VULKAN_SDK}/Lib'.")
    return True
