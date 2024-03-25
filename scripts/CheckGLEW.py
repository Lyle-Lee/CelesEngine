import os
import shutil
from pathlib import Path
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

GLEW_URL = 'https://nchc.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip'
GLEWDirectory = 'CelesEngine/Dependencies/GLEW'
# TempZipFile = f'{GLEWDirectory}/glew-2.1.0-win32.zip'

def installGLEW():
    print("Installing GLEW...\nDownloading", GLEW_URL)
    os.makedirs(f'{GLEWDirectory}/lib')
    with urlopen(GLEW_URL) as zipresp:
        with ZipFile(BytesIO(zipresp.read())) as zfile:
            for file in zfile.namelist():
                if file.startswith('glew-2.1.0/include/'):
                    zfile.extract(file, GLEWDirectory)
                    os.rename(os.path.join(GLEWDirectory, file), os.path.join(GLEWDirectory, file[11:]))
                elif file.startswith('glew-2.1.0/lib/Release/x64/'):
                    zfile.extract(file, GLEWDirectory)
                    os.rename(os.path.join(GLEWDirectory, file), os.path.join(f'{GLEWDirectory}/lib', file[23:]))
    
    shutil.rmtree(f'{GLEWDirectory}/glew-2.1.0')
    print('Done.')

def CheckGLEWLib():
    GLEWLib = Path(f'{GLEWDirectory}/lib/x64')
    if not GLEWLib.exists():
        print(f"No GLEW libs found (Checked '{GLEWLib}').")
        installGLEW()

    print(f"GLEW libs located at '{GLEWDirectory}'.")
    return True
