package com.example.bluecatev1;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Spinner;

public class ConfigureRoutineActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configure_routine);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        // Sets the Toolbar to act as the ActionBar for this Activity window.
        // Make sure the toolbar exists in the activity and is not null
        setSupportActionBar(toolbar);

        Button save_item_button = (Button) findViewById(R.id.button_save_routine_config);
        save_item_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                processSavedRoutine();
            }
        });
    }

    private void processSavedRoutine() {
        EditText routineNameEditText = (EditText) findViewById(R.id.EditTextRoutineName);
        String routineName = routineNameEditText.getText().toString().trim();
        EditText commandEditText = (EditText) findViewById(R.id.EditTextCommand);
        String command = commandEditText.getText().toString().trim();
        Spinner locationAliasSpinner = (Spinner) findViewById(R.id.SpinnerTriggerLocationAlias);
        String location = locationAliasSpinner.getSelectedItem().toString();
        CheckBox statusCheckBox = (CheckBox) findViewById(R.id.CheckBoxStatus);
        Boolean status = statusCheckBox.isChecked();

        Log.i("FORM DATA: ", String.format("Routine name: %s", routineName));
        Log.i("FORM DATA: ", String.format("Location Alias: %s", location));
        Log.i("FORM DATA: ", String.format("Command: %s", command));
        Log.i("FORM DATA: ", String.format("Routine status: %s", status));
    }
}
