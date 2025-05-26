package com.example.appbt

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.util.UUID
import kotlin.text.toByteArray
import kotlin.collections.isNullOrEmpty
import kotlin.collections.firstOrNull

class MainActivity : AppCompatActivity() {

    private val REQUEST_CODE = 101

    private lateinit var connectButton: Button
    private lateinit var sendButton: Button
    private lateinit var receivedDataTextView: TextView

    private val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
    private var bluetoothSocket: BluetoothSocket? = null

    // UUID SPP standard
    private val serverSocketUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        connectButton = findViewById(R.id.connectButton)
        sendButton = findViewById(R.id.sendButton)
        receivedDataTextView = findViewById(R.id.receivedData)

        // Vérifie que Bluetooth est dispo et activé
        if (bluetoothAdapter == null || bluetoothAdapter?.isEnabled == false) {
            Toast.makeText(this, "Bluetooth désactivé ou non supporté", Toast.LENGTH_SHORT).show()
            return
        }

        connectButton.setOnClickListener {
            if (hasAllPermissions()) {
                startBluetoothDiscovery()
            } else {
                requestPermissions()
            }
        }

        sendButton.setOnClickListener {
            sendData("Hello from Android!")
        }
    }

    private fun hasAllPermissions(): Boolean {
        return ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED
    }

    private fun requestPermissions() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.BLUETOOTH_CONNECT
            ),
            REQUEST_CODE
        )
    }

    override fun onRequestPermissionsResult(
        requestCode: Int, permissions: Array<out String>, grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_CODE) {
            if (grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                startBluetoothDiscovery()
            } else {
                Toast.makeText(this, "Permissions nécessaires refusées", Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun startBluetoothDiscovery() {
        val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter?.bondedDevices

        if (pairedDevices.isNullOrEmpty()) {
            Toast.makeText(this, "Aucun appareil appairé trouvé", Toast.LENGTH_SHORT).show()
            return
        }

        val device = pairedDevices.firstOrNull()
        if (device != null) {
            connectToBluetoothDevice(device)
        }
    }

    private fun connectToBluetoothDevice(device: BluetoothDevice) {
        // Vérifier la permission BLUETOOTH_CONNECT avant toute action
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            Toast.makeText(this, "Permission BLUETOOTH_CONNECT manquante", Toast.LENGTH_SHORT).show()
            // Tu peux ici demander la permission
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH_CONNECT), REQUEST_CODE)
            return
        }

        try {
            bluetoothSocket = device.createRfcommSocketToServiceRecord(serverSocketUUID)
            bluetoothSocket?.connect()
            Toast.makeText(this, "Connecté à ${device.name}", Toast.LENGTH_SHORT).show()
            listenForData()
        } catch (e: SecurityException) {
            // Gestion si la permission est quand même refusée au runtime
            Toast.makeText(this, "Permission Bluetooth refusée", Toast.LENGTH_SHORT).show()
            Log.e("BT_ERROR", "SecurityException : ${e.message}")
        } catch (e: IOException) {
            Toast.makeText(this, "Erreur de connexion Bluetooth", Toast.LENGTH_SHORT).show()
            Log.e("BT_ERROR", "IOException : ${e.message}")
        }
    }


    private fun sendData(message: String) {
        try {
            val outputStream: OutputStream = bluetoothSocket?.outputStream ?: return
            outputStream.write(message.toByteArray())
            Toast.makeText(this, "Message envoyé : $message", Toast.LENGTH_SHORT).show()
        } catch (e: IOException) {
            Toast.makeText(this, "Erreur d'envoi", Toast.LENGTH_SHORT).show()
            Log.e("BT_ERROR", "Envoi échoué : ${e.message}")
        }
    }

    private fun listenForData() {
        bluetoothSocket?.let {
            Thread {
                try {
                    val inputStream: InputStream = it.inputStream
                    val buffer = ByteArray(1024)
                    var bytes: Int

                    while (true) {
                        bytes = inputStream.read(buffer)
                        if (bytes > 0) {
                            val message = String(buffer, 0, bytes)
                            runOnUiThread {
                                receivedDataTextView.text = "Données reçues : $message"
                            }
                        }
                    }
                } catch (e: IOException) {
                    runOnUiThread {
                        Toast.makeText(this, "Erreur lecture données", Toast.LENGTH_SHORT).show()
                        Log.e("BT_ERROR", "Lecture échouée : ${e.message}")
                    }
                }
            }.start()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            bluetoothSocket?.close()
        } catch (e: IOException) {
            Log.e("BT_ERROR", "Erreur fermeture socket : ${e.message}")
        }
    }
}

