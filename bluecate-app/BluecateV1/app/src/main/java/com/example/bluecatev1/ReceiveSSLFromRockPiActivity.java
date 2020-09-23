package com.example.bluecatev1;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.google.crypto.tink.Aead;
import com.google.crypto.tink.KeyTemplate;
import com.google.crypto.tink.KeysetHandle;
import com.google.crypto.tink.aead.AeadConfig;
import com.google.crypto.tink.aead.AeadKeyTemplates;
import com.google.crypto.tink.config.TinkConfig;
import com.google.crypto.tink.hybrid.HybridConfig;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.URL;
import java.net.UnknownHostException;
import java.security.GeneralSecurityException;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.UnrecoverableKeyException;
import java.security.cert.CertificateException;
import java.util.Dictionary;
import java.util.Hashtable;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocket;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;

import static com.google.crypto.tink.KeyTemplate.OutputPrefixType.TINK;

//import com.google.crypto.tink.Aead;
//import com.google.crypto.tink.CleartextKeysetHandle;
//import com.google.crypto.tink.HybridDecrypt;
//import com.google.crypto.tink.HybridEncrypt;
//import com.google.crypto.tink.KeyManager;
//import com.google.crypto.tink.KeysetHandle;
//import com.google.crypto.tink.PrimitiveSet;
//import com.google.crypto.tink.Registry;
//import com.google.crypto.tink.aead.AeadConfig;
//import com.google.crypto.tink.aead.AeadFactory;
//import com.google.crypto.tink.aead.AeadKeyTemplates;
//import com.google.crypto.tink.JsonKeysetWriter;
//import com.google.crypto.tink.config.TinkConfig;
//import com.google.crypto.tink.hybrid.HybridConfig;
//import com.google.crypto.tink.hybrid.HybridEncryptWrapper;
//import com.google.crypto.tink.hybrid.HybridDecryptWrapper;
//import com.google.crypto.tink.proto.KeyTemplate;
//import com.google.crypto.tink.shaded.protobuf.ByteString;


//import static com.google.crypto.tink.KeyTemplate.OutputPrefixType.TINK;
//import static com.google.crypto.tink.KeyTemplate.toProto;
//import static java.net.InetAddress.*;

public class ReceiveSSLFromRockPiActivity extends AppCompatActivity {
    Button btnStart;
    TextView tvConnectionStatus;
    TextView tvClientMessages;
    TextView tvServerMessages;
    TextView tvIP;
    TextView tvPort;

    private static final String TAG = "SendSSLToRockPiActivity";

    // Server Address & Port
    public static String SERVER_IP = "10.0.0.167";

    public static final int SERVER_PORT = 8082;
    public static final String KEYSTORE_PASSWORD = "bluecate";

    // Client certificates and key files
//    public static final String CLIENT_SSL_CERT = "client_android.crt'";
//    public static final String CLIENT_SSL_KEY = "client_android.key";
    public static final String CA_SSL_P12 = "ca_app.p12";
    public static final String CLIENT_MESSAGE = "Hello, world! From Android Client";

    // Server certificates and key files
    public static final String SERVER_SSL_CERT = "server.crt";
    public static final String SERVER_SSL_KEY = "server.key";
    public static final String SERVER_SNI_HOSTNAME = "bluecate.com";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_receive_from_rockpi);
        btnStart = findViewById(R.id.btnStart);
        tvIP = findViewById(R.id.tvIP);
        tvPort = findViewById(R.id.tvPort);
        tvClientMessages = findViewById(R.id.tvClientMessages);
        tvServerMessages = findViewById(R.id.tvServerMessages);
        tvConnectionStatus = findViewById(R.id.tvConnectionStatus);
        tvConnectionStatus.setText("Not connected");
        tvPort.setText("Port: " + String.valueOf(SERVER_PORT));
        tvIP.setText("IP: " + SERVER_IP);

        btnStart.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.O)
            @Override
            public void onClick(View v) {
                ConnectToRockPiServer();
            }
        });

    }

    private BufferedReader input;

    private void ConnectToRockPiServer() {

        Log.i(TAG, "Start SSL Client...");
        Thread clientThread = new Thread(new Thread1());
        clientThread.start();

    }

    class Thread1 implements Runnable {
        @Override
        public void run() {
            try {
                JSONObject testJSONObject = createSampleRoutine();
                String routine = getIntent().getStringExtra("routine");
                StartClient(routine);
            } catch (KeyStoreException e) {
                e.printStackTrace();
            } catch (UnrecoverableKeyException e) {
                e.printStackTrace();
            } catch (NoSuchAlgorithmException e) {
                e.printStackTrace();
            } catch (KeyManagementException e) {
                e.printStackTrace();
            } catch (JSONException e) {
                e.printStackTrace();
            } catch (GeneralSecurityException e) {
                e.printStackTrace();
            }
        }
    }


    private byte[] DecryptServerMessage(KeysetHandle keysetHandle, byte[] encryptedMessage) throws GeneralSecurityException {

        // 2. Get the primitive.
        Aead aead = keysetHandle.getPrimitive(Aead.class);

        // ... or to decrypt a ciphertext.
        byte[] decrypted = aead.decrypt(encryptedMessage, null);

        return decrypted;
    }

    private byte[] EncryptClientMessage(KeysetHandle keysetHandle, String message) throws GeneralSecurityException {

        // 2. Get the primitive.
        Aead aead = keysetHandle.getPrimitive(Aead.class);

        // 3. Use the primitive to encrypt a plaintext,
        byte[] ciphertext = aead.encrypt(message.getBytes(), null);

        return ciphertext;
    }

    private JSONObject createSampleRoutine() throws JSONException {
        Routine testRoutine = new Routine("test routine",
                "test location alias",
                "test command",
                true);
        JSONObject routineJSON = testRoutine.toJSON();

        return routineJSON;
    }

    private void StartClient(String clientObject) throws GeneralSecurityException {
        SSLSocket clientSocket=null;
        DataInputStream inputDataStream=null;
        DataOutputStream outputDataStream=null;

        AeadConfig.register();
        TinkConfig.register();
        HybridConfig.register();

        // 1. Generate the key material.
        KeysetHandle keysetHandle = KeysetHandle.generateNew(
                AeadKeyTemplates.AES128_GCM);

        byte[] encryptedMessage = EncryptClientMessage(keysetHandle, clientObject);

        try{
            //Create a SSLServersocket
            KeyStore keyStore = KeyStore.getInstance("PKCS12");
            char[] pwdArray = KEYSTORE_PASSWORD.toCharArray();

//            InputStream inputStream = this.getResources().openRawResource(R.raw.client_new);
            InputStream inputStream = this.getResources().openRawResource(R.raw.ca_app);

            keyStore.load(inputStream, pwdArray);

            TrustManagerFactory trustManagerFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            trustManagerFactory.init(keyStore);

            KeyManagerFactory keyManagerFactory = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
            keyManagerFactory.init(keyStore, pwdArray);

            SSLContext context = SSLContext.getInstance("TLS");//"SSL" "TLS"
            context.init(keyManagerFactory.getKeyManagers(), trustManagerFactory.getTrustManagers(), null);

            SSLSocketFactory factory = context.getSocketFactory();
            clientSocket = (SSLSocket) factory.createSocket(SERVER_IP, SERVER_PORT);

            try{
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvConnectionStatus.setText("Connected\n");                }
                });
                outputDataStream = new DataOutputStream(clientSocket.getOutputStream());
                input = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                final String clientJSONAsString = clientObject;
                byte[] serverMessageBytes = clientObject.getBytes();
                outputDataStream.write(serverMessageBytes);
                outputDataStream.flush();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvClientMessages.setText("client sends: " + clientJSONAsString);
                    }
                });

                final String bufferInput = input.readLine();
                Log.i(TAG, String.format("bufferInput: %s", bufferInput));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvServerMessages.setText("server: " + bufferInput);
                    }
                });
//                    Log.i(TAG, String.format("dataInput: %s", dataInput));

            }
            catch (Exception e){
                e.printStackTrace();
            }

        }
        catch(IOException | CertificateException e)
        {
            System.out.print(e);
        }
        finally{
            if( clientSocket!= null){
                try {
                    clientSocket.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }

            if( inputDataStream!= null){
                try {
                    inputDataStream.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }

            if( outputDataStream != null){
                try {
                    outputDataStream.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
    }
}

