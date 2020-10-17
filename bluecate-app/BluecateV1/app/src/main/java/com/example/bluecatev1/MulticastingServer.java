package com.example.bluecatev1;

import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.content.Context;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;

import static android.app.PendingIntent.getActivity;
import static java.nio.charset.StandardCharsets.*;


public class MulticastingServer extends AsyncTask<String, Void, Void> {

    protected MulticastSocket socket = null;
    protected byte[] buf = new byte[256];
    protected InetAddress group = null;
    private final WifiManager.MulticastLock multicastLock;
    public static final String MULTICAST_LOCK = "KouChat multicast lock";

    public MulticastingServer(WifiManager wifiManager, Context context) throws IOException {
        this.multicastLock = wifiManager.createMulticastLock(MULTICAST_LOCK);
    }

    public boolean multicastLockIsHeld() {
        return multicastLock.isHeld();
    }

    private void acquireMulticastLock() {
        if (!multicastLockIsHeld()) {
            multicastLock.acquire();
        }
    }

    private void releaseMulticastLock() {
        if (multicastLockIsHeld()) {
            multicastLock.release();
        }
    }

    protected Void doInBackground(String... msg) {
        try {
            // Acquire multicast lock
            acquireMulticastLock();

            socket = new MulticastSocket(4446);
            socket.setReuseAddress(true);
            group = InetAddress.getByName("224.3.29.71");
            socket.joinGroup(group);

            while (true) {
                String message_1 = "Hello";
                byte[] bytes = message_1.getBytes(UTF_8);
                DatagramPacket hi = new DatagramPacket(bytes, bytes.length,
                        group, 4446);
                socket.send(hi);
                DatagramPacket packet = new DatagramPacket(buf, buf.length);
                socket.receive(packet);
                InetAddress address = packet.getAddress();
                int port = packet.getPort();
                packet = new DatagramPacket(buf, buf.length, address, port);
                String received = new String(packet.getData(), 0, packet.getLength());
                if (received.equals("end")) {
                    break;
                }
                socket.send(packet);
            }
            socket.leaveGroup(group);
            socket.close();
            releaseMulticastLock();

        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
