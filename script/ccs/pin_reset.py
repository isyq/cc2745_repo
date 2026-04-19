# Ref to: XDS110SupportReadMe.pdf

import subprocess
import os, re, sys, glob
from dotenv import load_dotenv

current_dir = os.path.dirname(os.path.abspath(__file__))
workspace_dir = os.path.abspath(os.path.join(current_dir, "../.."))

ccs_project_dir = os.path.join(workspace_dir, "ccs_project")
ccxml_file_path = os.path.join(ccs_project_dir, "CC2745R10.ccxml")

load_dotenv(os.path.join(workspace_dir, ".env"))
ccs_install_dir = os.path.abspath(os.getenv("ENV_CCS_INSTALL_DIR"))

xds110reset_path = os.path.join(ccs_install_dir, "ccs_base", "common", "uscif", "xds110", "xds110reset.exe")

# xds110reset -a toggle -d 100
subprocess.run([xds110reset_path, "-a", "toggle", "-d", "100"])
