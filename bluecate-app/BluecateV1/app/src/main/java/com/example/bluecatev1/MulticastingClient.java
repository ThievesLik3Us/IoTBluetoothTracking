package com.example.bluecatev1;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;

import android.os.AsyncTask;
import android.util.Log;

public class MulticastingClient extends AsyncTask<String, Void, Void> {

    private DatagramSocket socket;
    private InetAddress group;
    private int expectedServerCount;
    private byte[] buf;


    public MulticastingClient() throws Exception {
        this.expectedServerCount = 1;
        this.socket = new DatagramSocket();
        this.group = InetAddress.getByName("224.3.29.71");
    }

    private void copyMessageOnBuffer(String msg) {
        buf = msg.getBytes();
    }

    private void multicastPacket() throws IOException {
        DatagramPacket packet = new DatagramPacket(buf, buf.length, group, 4446);
        socket.send(packet);
    }

    public void close() {
        socket.close();
    }


    private void receivePacket() throws IOException {
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        socket.receive(packet);
    }

    int receivePackets() throws IOException {
        int serversDiscovered = 0;
        while (serversDiscovered != expectedServerCount) {
            receivePacket();
            serversDiscovered++;
        }
        return serversDiscovered;
    }

    public int discoverServers(String[] msg) throws IOException {
        copyMessageOnBuffer(msg[0]);
        multicastPacket();

        return receivePackets();
    }

    protected Void doInBackground(String... msg) {
        try {
            discoverServers(msg);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}