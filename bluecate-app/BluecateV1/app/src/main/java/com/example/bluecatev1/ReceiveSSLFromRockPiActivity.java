package com.example.bluecatev1;

import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.example.bluecatev1.MulticastingClient;

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
import java.net.DatagramSocket;
import java.net.MulticastSocket;
import java.net.DatagramPacket;
import java.net.URL;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.GeneralSecurityException;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.UnrecoverableKeyException;
import java.security.cert.CertificateException;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocket;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;

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

    private MulticastingClient multicast_client;
    private MulticastingServer multicast_server;

    private static final String TAG = "SendSSLToRockPiActivity";

    // Server Address & Port
    public static String SERVER_IP = "User-PC";//"10.0.0.167";
    //public static String SERVER_IP = ""; //""127.0.0.1";
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
                try {
                    ConnectToRockPiServer();
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

    }

    private String getLocalIpAddress() throws UnknownHostException {
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
        assert wifiManager != null;
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        int ipInt = wifiInfo.getIpAddress();
        return InetAddress.getByAddress(ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(ipInt).array()).getHostAddress();
    }

    private BufferedReader input;

    private void ConnectToRockPiServer() throws Exception {
        Log.i(TAG, "Start Multicast...");

        String localIP = getLocalIpAddress();
        Log.i(TAG, localIP);
        //multicast_client = new MulticastingClient();
        //multicast_client.execute(localIP);
        Log.i(TAG, "After Multicast...");

        //multicast_server = new MulticastingServer();
       // multicast_server.execute(localIP);
        Log.i(TAG, "Start SSL Client...");
        Thread clientThread = new Thread(new Thread1());
        clientThread.start();

    }

    class Thread1 implements Runnable {
        @Override
        public void run() {
            try {
                JSONObject testJSONObject = createSampleRoutine();
                StartClient(testJSONObject);
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
            }
        }
    }

//    private void EncryptClientMessage(String message) throws GeneralSecurityException {
//        AeadConfig.register();
//        TinkConfig.register();
//        HybridConfig.register();
//
//        com.google.crypto.tink.KeyTemplate keyTemplate = com.google.crypto.tink.KeyTemplate
//                .create("www.bluecate.com", message.getBytes(), TINK);
//
//        KeysetHandle privateKeysetHandle = KeysetHandle.generateNew(keyTemplate);
//        Aead aeadValue = new Aead() {
//            @Override
//            public byte[] encrypt(byte[] plaintext, byte[] associatedData) throws GeneralSecurityException {
//                return new byte[0];
//            }
//
//            @Override
//            public byte[] decrypt(byte[] ciphertext, byte[] associatedData) throws GeneralSecurityException {
//                return new byte[0];
//            }
//        };
//
//
//        Aead aead = AeadFactory.getPrimitive(privateKeysetHandle);
//
//        // Encryption
//        byte[] ciphertext = aead.encrypt(message.getBytes(), aad.getBytes());
//
//        // Decryption
//        byte[] decrypted = aead.decrypt(ciphertext, aad.getBytes());
//    }
//
//    //HybridEncryptWrapper isn't PUBLIC!!!
//    public static HybridEncrypt getPrimitive(
//            KeysetHandle keysetHandle, final KeyManager<HybridEncrypt> keyManager)
//            throws GeneralSecurityException {
//        Registry.registerPrimitiveWrapper(new HybridEncryptWrapper());
//        final PrimitiveSet<HybridEncrypt> primitives =
//                Registry.getPrimitives(keysetHandle, keyManager, HybridEncrypt.class);
//        return Registry.wrap(primitives);
//    }
//
//    public static HybridEncrypt getPrimitive(KeysetHandle keysetHandle)
//            throws GeneralSecurityException {
//        return getPrimitive(keysetHandle,null);
//    }


    private JSONObject createSampleRoutine() throws JSONException {
        Routine testRoutine = new Routine("test routine",
                "test location alias",
                "test command",
                true);
        JSONObject routineJSON = testRoutine.toJSON();
        System.out.println(routineJSON);

        return routineJSON;
    }

    private void StartClient(JSONObject clientObject) throws KeyStoreException, UnrecoverableKeyException, NoSuchAlgorithmException, KeyManagementException{
        SSLSocket clientSocket=null;
        DataInputStream inputDataStream=null;
        DataOutputStream outputDataStream=null;

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

            for(String protocol : clientSocket.getEnabledProtocols()){
                Log.i(TAG, protocol);
            }

            try{
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvConnectionStatus.setText("Connected\n");                }
                });
                outputDataStream = new DataOutputStream(clientSocket.getOutputStream());
                input = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                final String clientJSONAsString = clientObject.toString();
                byte[] serverMessageBytes = clientJSONAsString.getBytes();
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

