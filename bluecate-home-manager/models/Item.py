import uuid
import json

class Item:

    def __init__(self, item_name="", enabled=False):
        self.item_name = item_name
        self.enabled = enabled
        self.item_id = str(uuid.uuid4())

    def itemToJSON(self):
        item_set = {
            "item id": self.item_id,
            "item name": self.item_name,
            "item status": self.enabled
        }

        ### JSON STRING
        item_json_string = json.dumps(item_set)

        return item_json_string

    def jsonToItem(self, jsonItem):
        item_json_object = json.loads(jsonItem)
        self.item_name = item_json_object["item name"]
        self.enabled = item_json_object["item status"]

        return self


"This would create first object of Item class"
item1 = Item("Item 1", True)
"This would create second object of Item class"
item2 = Item()
print("item 1 created with parameters", item1.itemToJSON())
print("item 2 created without parameters", item2.itemToJSON())
item2.jsonToItem(item1.itemToJSON())
print("item 2 after JSON for item 1 is used to populate", item2.itemToJSON())
