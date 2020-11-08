package com.example.bluecatev1;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;

public class Routine {
    private UUID routine_id;
    private String routine_name;
    private String location_alias;
    private String command;
    private boolean enabled;

    public Routine(){
        this.routine_id = UUID.randomUUID();
    }

    public Routine(String routineName, String locationAlias,
                   String command, boolean enabled){
        this.routine_id = UUID.randomUUID();
        this.routine_name = routineName;
        this.location_alias = locationAlias;
        this.command = command;
        this.enabled = enabled;
    }

    public UUID getRoutineID() {
        return routine_id;
    }

    public void setRoutineID(UUID routine_id) {
        this.routine_id = routine_id;
    }

    public String getRoutineName() {
        return routine_name;
    }

    public void setRoutineName(String routine_name) {
        this.routine_name = routine_name;
    }

    public String getLocationAlias() {
        return location_alias;
    }

    public void setLocationAlias(String location_alias) {
        this.location_alias = location_alias;
    }

    public String getCommand() {
        return command;
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public boolean getEnabledStatus() {
        return enabled;
    }

    public void setEnabledStatus(boolean enabled) {
        this.enabled = enabled;
    }

    public JSONObject toJSON() throws JSONException {
        JSONObject routineJSON = new JSONObject();

        routineJSON.put("routine id", this.routine_id.toString());
        routineJSON.put("routine name", this.routine_name);
        routineJSON.put("routine location alias", this.location_alias);
        routineJSON.put("routine command", this.command);
        routineJSON.put("routine status", this.enabled);

        return routineJSON;

    }

    public Routine fromJSON(JSONObject jsonToBeParsed) throws JSONException {
        this.routine_name = jsonToBeParsed.getString("routine name");
        this.location_alias = jsonToBeParsed.getString("routine location alias");
        this.command = jsonToBeParsed.getString("routine command");
        this.enabled = Boolean.parseBoolean(jsonToBeParsed.getString("routine status"));

        return this;
    }
}
