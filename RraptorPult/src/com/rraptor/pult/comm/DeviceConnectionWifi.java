package com.rraptor.pult.comm;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.concurrent.TimeoutException;

public class DeviceConnectionWifi implements DeviceConnection {
    /**
     * Таймаут для чтения ответа на команды - сервер должет прислать ответ на
     * запрос за 5 секунд, иначе он будет считаться отключенным.
     */
    public static final int SERVER_SO_TIMEOUT = 5000;

    /**
     * Максимальное время неактивности пользователя, если пользователь не
     * отправлял команды на сервер роботу 5 секунд, приложение само отправит
     * команду ping, чтобы держать подключение открытым.
     */
    public final static long MAX_IDLE_TIMEOUT = 5000;// 5000;

    // public static final String DEFAULT_SERVER_HOST = "192.168.43.115";
    public static final String DEFAULT_SERVER_HOST = "192.168.1.115";
    public static final int DEFAULT_SERVER_PORT = 44114;

    /**
     * Таймаут на получение ответа от устройства, миллисекунды.
     */
    private static final int RESPONSE_TIMEOUT_MS = 15000;

    private Socket socket;
    private OutputStreamWriter out;
    private InputStreamReader in;

    public DeviceConnectionWifi() {
    }

    @Override
    public void connectToDevice(final String address, final int port)
            throws IOException {
        System.out.println("Connecting to device...");
        socket = new Socket(address, port);
        out = new OutputStreamWriter(socket.getOutputStream());
        in = new InputStreamReader(socket.getInputStream());
        System.out.println("Connection to device success");
    }

    @Override
    public String readFromDevice() throws IOException, TimeoutException {
        long startTime = System.currentTimeMillis();
        long currTime = System.currentTimeMillis();
        // ждем данные
        while (!in.ready() && (currTime - startTime < RESPONSE_TIMEOUT_MS)) {
            try {
                Thread.sleep(100);
                currTime = System.currentTimeMillis();
            } catch (InterruptedException e) {
            }
        }

        String replyLine;
        final char[] readBuffer = new char[256];
        final int readSize = in.read(readBuffer);
        if (readSize != -1) {
            replyLine = new String(readBuffer, 0, readSize - 1);
            System.out
                    .println("CMD reply: " + replyLine + ", size=" + readSize);
        } else {
            throw new TimeoutException("Wait response timeout");
        }

        return replyLine;
    }

    private void write(String cmd) throws IOException {
        out.write(cmd);
        out.flush();
    }

    @Override
    public void writeToDevice(final String cmd) throws IOException {
        boolean connected = false;
        if (out != null) {
            try {
                System.out.println(cmd);
                write(cmd);
                connected = true;
            } catch (IOException e) {
                System.out.println("Failed connection, try reconnect.");
            }
        }

        // переподключиться
        if (!connected) {
            connectToDevice(DEFAULT_SERVER_HOST, DEFAULT_SERVER_PORT);
            write(cmd);
        }
    }
}
