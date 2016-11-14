# Import libraries
import os
import sys
import subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
# SUMO Folder
SUMOFolder = os.environ.get('SUMO_HOME', '.')
# Current environment
currentEnvironmentFile = open(
    SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
# Get path to netEdit app
neteditApp = currentEnvironmentFile.readline().replace("\n", "")
# Get SandBox folder
textTestSandBox = currentEnvironmentFile.readline().replace("\n", "")
# Get resources depending of the current Operating system
currentOS = currentEnvironmentFile.readline().replace("\n", "")
netEditResources = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/"
currentEnvironmentFile.close()
#****#

# Open netedit
netEditProcess = subprocess.Popen([neteditApp,
                                   '--window-size', '800,600',
                                   '--sumo-net-file', textTestSandBox + "/input_net.net.xml",
                                   '--sumo-additionals-file', textTestSandBox + "/input_additionals.add.xml",
                                   '--additionals-output', textTestSandBox + "/additionals.xml"],
                                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# Wait 10 seconds to netedit main windows
try:
    match = wait(netEditResources + "neteditToolbar.png", 10)
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'neteditToolbar.png' not found")

# focus netEdit window
click(match.getTarget().offset(0, -20))

# save additionals
click(match.getTarget().offset(-375, 5))
click(match.getTarget().offset(-350, 265))

# quit
type("q", Key.CTRL)
try:
    find(netEditResources + "confirmClosingNetworkDialog.png")
    type("y", Key.ALT)
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'confirmClosingNetworkDialog.png' not found")