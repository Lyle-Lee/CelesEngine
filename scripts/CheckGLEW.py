import os
import sys
import shutil
import subprocess
import stat
from pathlib import Path
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

GLEWDirectory = 'CelesEngine/Dependencies/GLEW'
if sys.platform == 'win32':
    GLEW_URL = 'https://nchc.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip'
else:  # Assuming Linux or macOS
    GLEW_URL = 'https://nchc.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0.zip'

def fixPermissions(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            filepath = os.path.join(root, file)
            if file.endswith(('.sh', '.guess', '.sub')):
                current_permissions = os.stat(filepath).st_mode
                os.chmod(filepath, current_permissions | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

def installGLEW():
    print("Installing GLEW...\nDownloading", GLEW_URL)
    # os.makedirs(f'{GLEWDirectory}/lib')
    with urlopen(GLEW_URL) as zipresp:
        with ZipFile(BytesIO(zipresp.read())) as zfile:
            if sys.platform == 'win32':
                for file in zfile.namelist():
                    if file.startswith('glew-2.1.0/include/'):
                        zfile.extract(file, GLEWDirectory)
                        os.rename(os.path.join(GLEWDirectory, file), os.path.join(GLEWDirectory, file[11:]))
                    elif file.startswith('glew-2.1.0/lib/Release/x64/'):
                        zfile.extract(file, GLEWDirectory)
                        os.rename(os.path.join(GLEWDirectory, file), os.path.join(f'{GLEWDirectory}/lib', file[23:]))
            else:  # Linux or macOS
                tempDir = f'{GLEWDirectory}/glew-2.1.0'
                zfile.extractall(GLEWDirectory)
                fixPermissions(tempDir)
                os.chdir(tempDir)
                try:
                    subprocess.run(['make', 'install'], check=True)
                    print("GLEW compiled and installed successfully.")
                except subprocess.CalledProcessError as e:
                    print("Error occurred while compiling GLEW:", e)
                    os.chdir('../../../../')
                    return False
                
                os.chdir('../../../../')
                os.rename(f'{tempDir}/include', f'{GLEWDirectory}/include')
                os.rename(f'{tempDir}/lib', f'{GLEWDirectory}/lib')

    shutil.rmtree(f'{GLEWDirectory}/glew-2.1.0')
    print('Done.')
    return True

def CheckGLEWLib():
    GLEWLib = Path(f'{GLEWDirectory}/lib')
    if not GLEWLib.exists():
        print(f"No GLEW libs found (Checked '{GLEWLib}').")
        if not installGLEW():
            return False

    print(f"GLEW libs located at '{GLEWDirectory}'.")
    return True
