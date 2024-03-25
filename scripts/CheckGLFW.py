import os
import shutil
from pathlib import Path
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

GLFW_URL = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip'
GLFWDirectory = 'CelesEngine/Dependencies/GLFW'
# TempZipFile = f'{GLFWDirectory}/glfw-3.4.bin.WIN64.zip'

def installGLFW():
    print("Installing GLFW...\nDownloading", GLFW_URL)
    with urlopen(GLFW_URL) as zipresp:
        with ZipFile(BytesIO(zipresp.read())) as zfile:
            for file in zfile.namelist():
                if file.startswith(('glfw-3.4.bin.WIN64/include/', 'glfw-3.4.bin.WIN64/lib-vc2022/')):
                    zfile.extract(file, GLFWDirectory)
                    os.rename(os.path.join(GLFWDirectory, file), os.path.join(GLFWDirectory, file[19:]))
    
    shutil.rmtree(f'{GLFWDirectory}/glfw-3.4.bin.WIN64')
    print('Done.')

def CheckGLFWLib():
    glfwLib = Path(f'{GLFWDirectory}/lib-vc2022')
    if not glfwLib.exists():
        print(f"No GLFW libs found (Checked '{glfwLib}').")
        installGLFW()

    print(f"GLFW libs located at '{GLFWDirectory}'.")
    return True
