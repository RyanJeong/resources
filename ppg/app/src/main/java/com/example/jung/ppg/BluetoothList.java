package com.example.jung.ppg;

import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;

import java.util.Set;

/**
 * Created by jung on 2017. 3. 11..
 */
public class BluetoothList extends DialogFragment {
    private final static int DEVICES_DIALOG = 1;
    private final static int ERROR_DIALOG = 2;

    public BluetoothList() {

    }

    public static BluetoothList newInstance(int id, String text) {
        BluetoothList list = new BluetoothList();
        Bundle args = new Bundle();
        args.putString("Content", text);
        args.putInt("id", id);
        list.setArguments(args);

        return list;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        String              content = getArguments().getString("content");
        int                 id      = getArguments().getInt("id");
        AlertDialog.Builder alertDialogBuilder = null;

        switch (id) {
            case DEVICES_DIALOG:
                alertDialogBuilder = new AlertDialog.Builder(getActivity());
                alertDialogBuilder.setTitle("Select device");

                Set<BluetoothDevice> pairedDevices = MainActivity.getPairedDevices();
                final BluetoothDevice[] devices = pairedDevices.toArray(new BluetoothDevice[0]);
                String[] items = new String[devices.length];
                for (int i = 0; i < devices.length; i++) {
                    items[i] = devices[i].getName();
                }
                alertDialogBuilder.setItems(items, new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        ((MainActivity) MainActivity.mContext).doConnect(devices[which]);
                    }
                });
                alertDialogBuilder.setCancelable(false);
                break;
            case ERROR_DIALOG:
                alertDialogBuilder = new AlertDialog.Builder(getActivity());
                alertDialogBuilder.setTitle("ERROR");
                alertDialogBuilder.setMessage(content);
                alertDialogBuilder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        ((MainActivity) MainActivity.mContext).finish();
                    }
                });
                break;
        }

        return alertDialogBuilder.create();
    }
}
