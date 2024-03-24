import os
from pathlib import Path
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

GLEW_URL = 'https://nchc.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip'
GLEWDirectory = 'CelesEngine/Dependencies/GLEW'
TempZipFile = f'{GLEWDirectory}/glew-2.1.0-win32.zip'

def installGLEW():
    print("Installing GLEW...\nDownloading", GLEW_URL)
    os.makedirs(f'{GLEWDirectory}/lib')
    with urlopen(GLEW_URL) as zipresp:
        with ZipFile(BytesIO(zipresp.read())) as zfile:
            zfile.extractall(GLEWDirectory, ['include'])
            zfile.extractall(f'{GLEWDirectory}/lib', ['lib/Release/x64'])
    
    os.remove(TempZipFile)

def CheckGLEWLib():
    GLEWLib = Path(f'{GLEWDirectory}/lib')
    if not GLEWLib.exists():
        print(f"No GLEW libs found (Checked '{GLEWLib}').")
        installGLEW()

    print(f"GLEW libs located at '{GLEWDirectory}'.")
    return True
