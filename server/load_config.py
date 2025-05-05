import os
import json

def loadConfig(configFilename):
    configPath = os.path.join(os.path.dirname(__file__), configFilename)
    with open(configPath, "r") as f:
        return json.load(f)

