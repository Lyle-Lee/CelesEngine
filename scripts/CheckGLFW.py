import os
import sys
import shutil
from pathlib import Path
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

if sys.platform == 'win32':
    GLFW_LIB_DIR = 'lib-vc2022'
    if sys.maxsize > 2**32:
        GLFW_ROOT = 'glfw-3.4.bin.WIN64'
        GLFW_URL = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip'
    else:
        GLFW_ROOT = 'glfw-3.4.bin.WIN32'
        GLFW_URL = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN32.zip'
elif sys.platform == 'darwin':
    GLFW_LIB_DIR = 'lib-arm64'
    GLFW_ROOT = 'glfw-3.4.bin.MACOS'
    GLFW_URL = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.MACOS.zip'
else:
    GLFW_LIB_DIR = 'lib' # needs to build from source
    GLFW_ROOT = 'glfw-3.4'
    GLFW_URL = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip'

GLFWDirectory = 'CelesEngine/Dependencies/GLFW'

def installGLFW():
    print("Installing GLFW...\nDownloading", GLFW_URL)
    with urlopen(GLFW_URL) as zipresp:
        with ZipFile(BytesIO(zipresp.read())) as zfile:
            for file in zfile.namelist():
                if file.startswith((f'{GLFW_ROOT}/include/', f'{GLFW_ROOT}/{GLFW_LIB_DIR}/')):
                    zfile.extract(file, GLFWDirectory)
                    os.rename(os.path.join(GLFWDirectory, file), os.path.join(GLFWDirectory, file[len(GLFW_ROOT) + 1:]))
    
    shutil.rmtree(f'{GLFWDirectory}/{GLFW_ROOT}')
    # TODO: Build GLFW from source for Linux and other platforms.
    print('Done.')

def CheckGLFWLib():
    glfwLib = Path(f'{GLFWDirectory}/{GLFW_LIB_DIR}')
    if not glfwLib.exists():
        print(f"No GLFW libs found (Checked '{glfwLib}').")
        installGLFW()

    print(f"GLFW libs located at '{GLFWDirectory}'.")
    return True
