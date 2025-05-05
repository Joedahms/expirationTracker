import os
import json

def loadConfig():
    configFile = "config.json"
    configPath = os.path.join(os.path.dirname(__file__), configFile)
    with open(configPath, "r") as f:
        return json.load(f)

