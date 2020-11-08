package com.example.bluecatev1;

import org.json.JSONException;
import org.json.JSONObject;
import org.json.*;

import java.util.UUID;

public class Item {
    private UUID item_id;
    private String item_name;
    private boolean enabled;

    public Item(){
        this.item_id = UUID.randomUUID();
    }

    public Item(String itemName, boolean enabled){
        this.item_id = UUID.randomUUID();
        this.item_name = itemName;
        this.enabled = enabled;
    }

    public UUID getItemID() {
        return item_id;
    }

    public void setItemID(UUID item_id) {
        this.item_id = item_id;
    }

    public String getItemName() {
        return item_name;
    }

    public void setItemName(String item_name) {
        this.item_name = item_name;
    }

    public boolean getEnabledStatus() {
        return enabled;
    }

    public void setEnabledStatus(boolean enabled) {
        this.enabled = enabled;
    }

    public JSONObject toJSON() throws JSONException {
        JSONObject itemJSON = new JSONObject();

        itemJSON.put("item id", this.item_id.toString());
        itemJSON.put("item name", this.item_name);
        itemJSON.put("item status", this.enabled);

        return itemJSON;
    }

    public Item fromJSON(JSONObject jsonToBeParsed) throws JSONException {
        this.item_name = jsonToBeParsed.getString("item name");
        this.enabled = Boolean.parseBoolean(jsonToBeParsed.getString("item status"));

        return this;
    }
}
