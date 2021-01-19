package com.example.bluecatev1;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import org.json.JSONException;
import org.json.JSONObject;

public class ConfigureItemActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configure_item);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        // Sets the Toolbar to act as the ActionBar for this Activity window.
        // Make sure the toolbar exists in the activity and is not null
        setSupportActionBar(toolbar);

        Button save_item_button = (Button) findViewById(R.id.button_save_item_config);
        save_item_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    processSavedItem();
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });

    }

    private void processSavedItem() throws JSONException {
        EditText itemNameEditText = (EditText) findViewById(R.id.EditTextItemName);
        String itemName = itemNameEditText.getText().toString().trim();
        CheckBox statusCheckBox = (CheckBox) findViewById(R.id.StatusCheckBox);
        Boolean status = statusCheckBox.isChecked();

        Item savedItem = new Item(itemName, status);

        Log.i("FORM DATA: ", String.format("Item id: %s", savedItem.getItemID()));
        Log.i("FORM DATA: ", String.format("Item name: %s", savedItem.getItemName()));
        Log.i("FORM DATA: ", String.format("Item status: %s", savedItem.getEnabledStatus()));

        JSONObject itemJSON = savedItem.toJSON();
        System.out.println("savedItem " + itemJSON);

        Item newItem = new Item();
        newItem.fromJSON(itemJSON);
        System.out.println("newItem name " + newItem.getItemName());

        goToSendSSLToRockPiActivity(itemJSON);


    }

    private void goToSendSSLToRockPiActivity(JSONObject item) {
        Intent intent = new Intent(this, SendSSLToRockPiActivity.class);
        intent.putExtra("item", item.toString());
        startActivity(intent);
    }

//    private void goToSendToRockPiActivity() {
//        Intent intent = new Intent(this, SendToRockPiActivity.class);
//        startActivity(intent);
//    }
//
//    private void goToReceiveFromRockPiActivity() {
//        Intent intent = new Intent(this, ReceiveFromRockPiActivity.class);
//        startActivity(intent);
//    }

}
