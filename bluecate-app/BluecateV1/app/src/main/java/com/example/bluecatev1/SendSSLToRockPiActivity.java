package com.example.bluecatev1;

import android.annotation.SuppressLint;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Path;
import java.nio.file.Paths;
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

public class SendSSLToRockPiActivity extends AppCompatActivity {
    Button btnStart;
    TextView tvConnectionStatus;
    TextView tvClientMessages;
    TextView tvServerMessages;
    TextView tvIP;
    TextView tvPort;

    private static final String TAG = "SendSSLToRockPiActivity";

    // Server Address & Port
    public static String SERVER_IP = "";
    public static final int SERVER_PORT = 8082;
    public static final String SERVER_MESSAGE = "Hello, world! From Android Server";
    public static final String KEYSTORE_PASSWORD = "bluecate";

    // Client certificates and key files
    public static final String CLIENT_SSL_CERT = "client.crt'";
    public static final String CLIENT_SSL_KEY = "client.key";

    // Server certificates and key files
    public static final String SERVER_SSL_CERT = "server.crt";
    public static final String SERVER_SSL_KEY = "server.key";
    public static final String SERVER_SSL_P12 = "server.pkcs12";
    public static final String SERVER_SNI_HOSTNAME = "bluecate.com";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_send_to_rockpi);
        btnStart = findViewById(R.id.btnStart);
        tvIP = findViewById(R.id.tvIP);
        tvPort = findViewById(R.id.tvPort);
        tvClientMessages = findViewById(R.id.tvClientMessages);
        tvServerMessages = findViewById(R.id.tvServerMessages);
        tvConnectionStatus = findViewById(R.id.tvConnectionStatus);
        tvConnectionStatus.setText("Not connected");
        tvPort.setText("Port: " + String.valueOf(SERVER_PORT));

        try {
            SERVER_IP = getLocalIpAddress();
            tvIP.setText("IP: " + SERVER_IP);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        btnStart.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.O)
            @Override
            public void onClick(View v) {
                SendSSLToRockPiActivity();
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

    @RequiresApi(api = Build.VERSION_CODES.O)
    private void SendSSLToRockPiActivity() {

        Log.i(TAG, "Start SSL Server...");
        Thread serverThread = new Thread(new Thread1());
        serverThread.start();

    }

    class Thread1 implements Runnable {
        @Override
        public void run() {
            try {
                StartServer();
            } catch (KeyStoreException e) {
                e.printStackTrace();
            } catch (UnrecoverableKeyException e) {
                e.printStackTrace();
            } catch (NoSuchAlgorithmException e) {
                e.printStackTrace();
            } catch (KeyManagementException e) {
                e.printStackTrace();
            }
        }
    }


    private void StartServer() throws KeyStoreException, UnrecoverableKeyException, NoSuchAlgorithmException, KeyManagementException {
        SSLServerSocket serverSocket=null;
        DataInputStream inputDataStream=null;
        DataOutputStream outputDataStream=null;

        SSLSocket sslSocket = null;
        try{
            //Create a SSLServersocket
            KeyStore keyStore = KeyStore.getInstance("PKCS12");
            char[] pwdArray = KEYSTORE_PASSWORD.toCharArray();

            InputStream inputStream = this.getResources().openRawResource(R.raw.server);
            keyStore.load(inputStream, pwdArray);

            TrustManagerFactory trustManagerFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            trustManagerFactory.init(keyStore);

            KeyManagerFactory keyManagerFactory = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
            keyManagerFactory.init(keyStore, pwdArray);

            SSLContext context = SSLContext.getInstance("TLS");//"SSL" "TLS"
            context.init(keyManagerFactory.getKeyManagers(), trustManagerFactory.getTrustManagers(), null);

            SSLServerSocketFactory factory = context.getServerSocketFactory();
            serverSocket = (SSLServerSocket)factory.createServerSocket(SERVER_PORT);

            sslSocket = (SSLSocket) serverSocket.accept();
            input = new BufferedReader(new InputStreamReader(sslSocket.getInputStream()));

            try{
                input = new BufferedReader(new InputStreamReader(sslSocket.getInputStream()));
//                    inputDataStream = new DataInputStream(sslSocket.getInputStream());
                outputDataStream = new DataOutputStream(sslSocket.getOutputStream());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvConnectionStatus.setText("Connected\n");                }
                });

//                    Byte dataInput=inputDataStream.readByte();
//                    String dataString = dataInput.toString();
//                    String inputUpper=dataString.toUpperCase();

                byte[] serverMessageBytes = SERVER_MESSAGE.getBytes();
                outputDataStream.write(serverMessageBytes);
//                outputDataStream.writeUTF(SERVER_MESSAGE);
                outputDataStream.flush();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvServerMessages.setText("server: " + SERVER_MESSAGE);
                    }
                });

                final String bufferInput = input.readLine();
                Log.i(TAG, String.format("bufferInput: %s", bufferInput));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvClientMessages.setText("client: " + bufferInput);
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
            if( sslSocket!= null){
                try {
                    sslSocket.close();
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

//    private class Thread2 implements Runnable {
//        @Override
//        public void run() {
//            while (true) {
//                try {
//                    final String message = input.readLine();
//                    if (message != null) {
//                        runOnUiThread(new Runnable() {
//                            @Override
//                            public void run() {
//                                tvMessages.append("client:" + message + "\n");
//                            }
//                        });
//                    } else {
//                        Thread1 = new Thread(new Thread1());
//                        Thread1.start();
//                        return;
//                    }
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//            }
//        }
//    }
//    class Thread3 implements Runnable {
//        private String message;
//        Thread3(String message) {
//            this.message = message;
//        }
//        @Override
//        public void run() {
//            output.write(message);
//            output.flush();
//            runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    tvMessages.append("server: " + message + "\n");
//                    etMessage.setText("");
//                }
//            });
//        }
//    }

    private void StartClient() {
        try{
            SSLSocketFactory factory=(SSLSocketFactory) SSLSocketFactory.getDefault();
            SSLSocket sslsocket=(SSLSocket) factory.createSocket(SERVER_IP,SERVER_PORT);
            DataOutputStream os=new DataOutputStream(sslsocket.getOutputStream());
            DataInputStream is=new DataInputStream(sslsocket.getInputStream());

            String str="helloworld";
            os.writeBytes(str);

            String responseStr;
            if((responseStr=is.readUTF())!=null)
            {
                System.out.println(responseStr);
            }

            os.close();
            is.close();
            sslsocket.close();
        }
        catch(UnknownHostException e)
        {
            System.out.println(e.getMessage());
        }
        catch(IOException e)
        {
            System.out.println(e.getMessage());
        }
    }
}
