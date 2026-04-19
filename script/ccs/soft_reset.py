import os, re, sys, glob

from dotenv import load_dotenv

current_dir = os.path.dirname(os.path.abspath(__file__))
workspace_dir = os.path.abspath(os.path.join(current_dir, "../.."))

ccs_project_dir = os.path.join(workspace_dir, "ccs_project")
ccxml_file_path = os.path.join(ccs_project_dir, "CC2745R10.ccxml")

load_dotenv(os.path.join(workspace_dir, ".env"))
ccs_install_dir = os.path.abspath(os.getenv("ENV_CCS_INSTALL_DIR"))

sys.path.append(os.path.join(ccs_install_dir, "ccs", "scripting", "python", "site-packages"))
import scripting

ds = scripting.initScripting(scripting.ScriptingOptions(ccsRoot=ccs_install_dir))

ds.configure(ccxml_file_path)
session = ds.openSession(re.compile(".*cortex.*", re.IGNORECASE))

session.target.connect()
session.target.reset()
session.target.disconnect()

ds.shutdown()
