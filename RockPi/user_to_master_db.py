import json
from pymongo import MongoClient

# Connect to Local MongoDB
  mongodb_client = MongoClient(port=27017)
  android_db = mongodb_client.AndroidDB
  master_db = mongodb_client.MasterDB
