package com.example.androidbt

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.ListView
import android.widget.TextView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.material3.Button
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import java.io.IOException
import java.util.UUID

private lateinit var bluetoothManager : BluetoothManager
private lateinit var bluetoothAdapter : BluetoothAdapter
private val appareilsAppaires = mutableListOf<String>()
private var connectedSocket: BluetoothSocket? = null






class MainActivity : AppCompatActivity() {
    @RequiresApi(Build.VERSION_CODES.M)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)


        val act : Button = findViewById(R.id.actbt)
        val perm : Button = findViewById(R.id.permbt)
        val app : Button = findViewById(R.id.appas)
        val eff : Button = findViewById(R.id.effas)
        val lisas : ListView = findViewById(R.id.app)
        val TMess = findViewById<EditText>(R.id.Mess)
        val sBt = findViewById<Button>(R.id.sendbt)
        val page1: Button = findViewById(R.id.pag1)
        val frag1 = Fragment2()
        val page: Button = findViewById(R.id.pag)
        val frag = Fragment1()
        val text = "NYEUH  !°µ°"
        val duration = Toast.LENGTH_SHORT







        val arrayAdapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, appareilsAppaires)
        lisas.adapter = arrayAdapter

        val requetePermissions: ActivityResultLauncher<Array<String>> =
            registerForActivityResult(
                ActivityResultContracts.RequestMultiplePermissions()) { permissions ->
                if (permissions.values.all { it }) {
                    act.isEnabled = true
                    Toast.makeText(this, "Permissions OK", Toast.LENGTH_SHORT).show()
                }
            }

        val requeteActivationBT =
            registerForActivityResult(ActivityResultContracts.StartActivityForResult()) {
                    result ->
                if (result.resultCode == RESULT_OK) {
                    Toast.makeText(this, "L'utilisateur a activé le BT",
                        Toast.LENGTH_SHORT).show()
//activer un bouton (A COMPLETER PLUS TARD)
                }
                else {
                    Toast.makeText(this, "L'utilisateur a refusé",
                        Toast.LENGTH_SHORT).show()
                }
            }

        bluetoothManager = this?.getSystemService(BluetoothManager::class.java)!!
        bluetoothAdapter = bluetoothManager.adapter
        if (bluetoothAdapter == null) {
// Device doesn't support Bluetooth
            Toast.makeText(this, "La machine ne possède pas le Bluetooth",
                Toast.LENGTH_SHORT).show();
        } else {
            perm.isEnabled = true
            Toast.makeText(this, "Interface BT existe", Toast.LENGTH_SHORT).show()
        }

        act.isEnabled = false


        class ConnectThread(device: BluetoothDevice) : Thread() {
            private val uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
            private val mmSocket: BluetoothSocket = device.createRfcommSocketToServiceRecord(uuid)
            override fun run() {
// Cancel discovery because it otherwise slows down the connection.
                bluetoothAdapter.cancelDiscovery()
                try {
                    Log.i("BT", "début attente connexion")
                    mmSocket.connect()
                    Log.i("BT", "fin attente connexion - connexion OK");
                    connectedSocket = mmSocket  // Sauvegarde le socket pour l'utiliser plus tard
                }
                catch (e: IOException) {
                    Log.e("BT", "Erreur", e)
                }
            }
        }






        perm.setOnClickListener {

            lateinit var BT_PERMISSIONS : Array<String>
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                BT_PERMISSIONS = arrayOf(
                    android.Manifest.permission.BLUETOOTH_CONNECT,
                    android.Manifest.permission.BLUETOOTH_SCAN
                )
            }
            else
            {
                BT_PERMISSIONS = arrayOf(
                    android.Manifest.permission.BLUETOOTH_ADMIN,
                    android.Manifest.permission.BLUETOOTH,
                    android.Manifest.permission.ACCESS_FINE_LOCATION
                )
            }
            requetePermissions.launch(BT_PERMISSIONS)
        }




        act.setOnClickListener {
            if (bluetoothAdapter.isEnabled) {
                Toast.makeText(this, "BT déjà activé", Toast.LENGTH_SHORT).show()
//activer un bouton (A COMPLETER PLUS TARD)
            } else {
                val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                requeteActivationBT.launch(enableBtIntent)
            }
        }

        app.setOnClickListener {
            appareilsAppaires.clear()  // Vide la liste avant de la remplir
            val pairedDevices: Set<BluetoothDevice> = bluetoothAdapter.bondedDevices
            for (device in pairedDevices) {
                val deviceName = device.name ?: "Nom inconnu"
                val deviceAddress = device.address
                appareilsAppaires.add("$deviceName\n$deviceAddress")
            }
            arrayAdapter.notifyDataSetChanged()  // Met à jour la ListView

            lisas.setOnItemClickListener { parent, view, pos, id ->
                val address: String = appareilsAppaires[pos].split('\n')[1]
                val device: BluetoothDevice? = bluetoothAdapter.getRemoteDevice(address)
                if (device != null) {
                    Log.i("BT", "Device choisi - ${device.name} - $address")
                    ConnectThread(device).start()
                    Toast.makeText(this, "Appareil connecté", Toast.LENGTH_SHORT).show()
                } else {
                    Toast.makeText(this, "Erreur : périphérique introuvable", Toast.LENGTH_SHORT).show()
                }
            }
        }


        eff.setOnClickListener {
            appareilsAppaires.clear()
            arrayAdapter.notifyDataSetChanged()
        }



        sBt.setOnClickListener {
            val message = TMess.text.toString()
            val socket = connectedSocket
            if (socket != null && socket.isConnected) {
                try {
                    val outputStream = socket.outputStream
                    outputStream.write((message + "\n").toByteArray())
                    Toast.makeText(this, "Message envoyé", Toast.LENGTH_SHORT).show()
                } catch (e: IOException) {
                    Toast.makeText(this, "Erreur d'envoi", Toast.LENGTH_SHORT).show()
                    Log.e("BT", "Erreur envoi", e)
                }
            } else {
                Toast.makeText(this, "Aucun appareil connecté", Toast.LENGTH_SHORT).show()
            }
        }


        page1.setOnClickListener {
            remplaceFragment(frag1)


            act .isEnabled = false
            perm .isEnabled = false
            app .isEnabled = false
            eff  .isEnabled = false
            lisas .isEnabled = false
            TMess .isEnabled = false
            sBt .isEnabled = false



            val toast = Toast.makeText(this, text, duration) // in Activity
            toast.show()
        }

        page.setOnClickListener {
            remplaceFragment(frag)
            perm .isEnabled = true
            app .isEnabled = true
            eff  .isEnabled = true
            lisas .isEnabled = true
            TMess .isEnabled = true
            sBt .isEnabled = true
            val toast = Toast.makeText(this, text, duration) // in Activity
            toast.show()
        }
    }


    private fun remplaceFragment(fragment : Fragment) {

        val fragMan: FragmentManager = supportFragmentManager
        val fragTran = fragMan.beginTransaction()
        fragTran.replace(R.id.fragmentContainerView, fragment)
        fragTran.commit()
    }

    fun envoyerMessage(message: String) {
        val socket = connectedSocket
        try {
            if (socket != null && socket.isConnected) {
                val out = socket.outputStream
                out.write((message + "\n").toByteArray()) // \n pour retour à la ligne
            }
        } catch (e: IOException) {
            Log.e("BT", "Erreur d'envoi", e)
        }
    }

}


