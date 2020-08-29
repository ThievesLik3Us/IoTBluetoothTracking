import uuid
import json

class Routine:

    def __init__(self, routine_name="", location_alias="", command="", enabled=False):
        self.routine_id = str(uuid.uuid4())
        self.routine_name = routine_name
        self.location_alias = location_alias
        self.command = command
        self.enabled = enabled

    def routineToJSON(self):
        routine_set = {
            "routine id": self.routine_id,
            "routine name": self.routine_name,
            "routine location alias": self.location_alias,
            "routine command": self.command,
            "routine status": self.enabled
        }

        ### JSON STRING
        routine_json_string = json.dumps(routine_set)

        return routine_json_string

    def jsonToRoutine(self, jsonRoutine):
        routine_json_object = json.loads(jsonRoutine)
        self.routine_name = routine_json_object["routine name"]
        self.location_alias = routine_json_object["routine location alias"]
        self.command = routine_json_object["routine command"]
        self.enabled = routine_json_object["routine status"]

        return self


"This would create first object of Routine class"
routine1 = Routine("Routine 1", "Living Room", "Turn on the TV", True)
"This would create second object of Routine class"
routine2 = Routine()
print("routine 1 created with parameters", routine1.routineToJSON())
print("routine 2 created without parameters", routine2.routineToJSON())
routine2.jsonToRoutine(routine1.routineToJSON())
print("routine 2 after JSON for routine 1 is used to populate", routine2.routineToJSON())
