import subprocess
from importlib import metadata

def install(package):
    print(f"Installing python module '{package}'...")
    subprocess.check_call(['python', '-m', 'pip', 'install', package])

def validatePackage(package):
    required = { package }
    installed = { dist.metadata["Name"].lower() for dist in metadata.distributions() }
    missing = required - installed
    if missing:
        install(package)

def validatePackages():
    validatePackage('requests')
    validatePackage('fake-useragent')
