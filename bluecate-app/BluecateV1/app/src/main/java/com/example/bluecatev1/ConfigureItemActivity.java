package com.example.bluecatev1;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

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
                processSavedItem();
            }
        });

    }

    private void processSavedItem() {
        EditText itemNameEditText = (EditText) findViewById(R.id.EditTextItemName);
        String itemName = itemNameEditText.getText().toString().trim();
        CheckBox statusCheckBox = (CheckBox) findViewById(R.id.StatusCheckBox);
        Boolean status = statusCheckBox.isChecked();
        Log.i("FORM DATA: ", String.format("Item name: %s", itemName));
        Log.i("FORM DATA: ", String.format("Item status: %s", status));
    }


}
