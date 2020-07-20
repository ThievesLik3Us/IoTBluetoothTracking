package com.example.bluecatev1;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.Button;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        // Sets the Toolbar to act as the ActionBar for this Activity window.
        // Make sure the toolbar exists in the activity and is not null
        setSupportActionBar(toolbar);

        Button setup_button = (Button) findViewById(R.id.button_setup);
        setup_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                goToConfigActivity();
            }
        });

        Button routines_button = (Button) findViewById(R.id.button_routines);
        routines_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                goToRoutinesActivity();
            }
        });

        Button tracking_button = (Button) findViewById(R.id.button_items);
        tracking_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                goToItemsActivity();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    private void goToConfigActivity() {
        Intent intent = new Intent(this, ConfigureItemActivity.class);
        startActivity(intent);
    }

    private void goToRoutinesActivity() {
        Intent intent = new Intent(this, RoutinesActivity.class);
        startActivity(intent);
    }

    private void goToItemsActivity() {
        Intent intent = new Intent(this, ItemsActivity.class);
        startActivity(intent);
    }


}
