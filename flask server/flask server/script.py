from roboflow import Roboflow
import os

from roboflow import Roboflow
import os

rf = Roboflow(api_key="2PUyjHKQ4JQsYGv8eWJv")  # Replace with your Roboflow API key
project = rf.workspace("middle-east-tech-university").project("fire-and-smoke-detection-hiwia")

# Get all versions
versions = project.versions()
print("Available versions:", [v.version for v in versions])

# Get the latest version number (e.g., "fire-and-smoke-detection-hiwia/X")
latest_version = max([int(v.version.split('/')[-1]) for v in versions])
version = project.version(latest_version)
dataset = version.download("yolov8")

print(f"Downloaded model to: {dataset.location}")
print(f"best.pt path: {os.path.join(dataset.location, 'weights', 'best.pt')}")

# from roboflow import Roboflow


# rf = Roboflow(api_key="2PUyjHKQ4JQsYGv8eWJv")
# project = rf.workspace("custom-thxhn").project("fire-wrpgm")
# dataset = project.version(8).download("yolov8")