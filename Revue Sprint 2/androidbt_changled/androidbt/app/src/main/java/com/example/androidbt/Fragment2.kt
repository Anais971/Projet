package com.example.androidbt


import android.annotation.SuppressLint
import android.graphics.Bitmap
import android.graphics.drawable.BitmapDrawable
import android.os.Bundle
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.core.content.ContentProviderCompat.requireContext
import androidx.fragment.app.Fragment

// TODO: Rename parameter arguments, choose names that match
// the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
private const val ARG_PARAM1 = "param1"
private const val ARG_PARAM2 = "param2"
private lateinit var bitmap: Bitmap
private lateinit var hexColor: String


/**
 * A simple [Fragment] subclass.
 * Use the [Fragment2.newInstance] factory method to
 * create an instance of this fragment.
 */
class Fragment2 : Fragment() {
    // TODO: Rename and change types of parameters
    private var param1: String? = null
    private var param2: String? = null
    private lateinit var vue : View

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        arguments?.let {
            param1 = it.getString(ARG_PARAM1)
            param2 = it.getString(ARG_PARAM2)
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        // Inflate the layout for this fragment
        vue = inflater.inflate(R.layout.fragment_2, container, false)


        val sbt : Button = vue.findViewById(R.id.sendbt)
        val tMess : TextView = vue.findViewById(R.id.Mess)
        val color : ImageView = vue.findViewById(R.id.myImageView2)
        val couleur : View = vue.findViewById(R.id.cc)


        color.post {
            val drawable = color.drawable
            if (drawable is BitmapDrawable) {
                bitmap = drawable.bitmap
            } else {
                Toast.makeText(requireContext(), "Image non supportée", Toast.LENGTH_SHORT).show()
            }
        }



        color.setOnTouchListener { v, event ->
            // On détecte l'appui (ou glissement) du doigt sur l'image
            if (event.action == MotionEvent.ACTION_DOWN || event.action == MotionEvent.ACTION_MOVE) {

                // Coordonnées du point touché dans l'ImageView (en pixels)
                val colorX = event.x.toInt()
                val colorY = event.y.toInt()

                // Adaptation des coordonnées si l'image a été redimensionnée
                val scaledX = (colorX * bitmap.width) / color.width
                val scaledY = (colorY * bitmap.height) / color.height

                // Vérifie que les coordonnées sont valides dans le bitmap
                if (scaledX in 0 until bitmap.width && scaledY in 0 until bitmap.height) {
                    // Récupération de la couleur du pixel à ces coordonnées
                    val pixel = bitmap.getPixel(scaledX, scaledY)

                    // Conversion du pixel (int) en code couleur hexadécimal
                    hexColor = String.format("#%06X", 0xFFFFFF and pixel)

                    couleur.setBackgroundColor(pixel)

                    tMess.text = hexColor
                }
            }
            true // Retourne true pour indiquer qu'on gère l'événement tactile
        }

        sbt.setOnClickListener {
            val message = hexColor.toString()
            if (message.isNotEmpty()) {
                (activity as? MainActivity)?.envoyerMessage(message)

                Toast.makeText(requireContext(), "Message envoyé", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(requireContext(), "Message vide", Toast.LENGTH_SHORT).show()
            }
        }

        return vue
    }





















    companion object {
        /**
         * Use this factory method to create a new instance of
         * this fragment using the provided parameters.
         *
         * @param param1 Parameter 1.
         * @param param2 Parameter 2.
         * @return A new instance of fragment Fragment2.
         */
        // TODO: Rename and change types and number of parameters
        @JvmStatic fun newInstance(param1: String, param2: String) =
                Fragment2().apply {
                    arguments = Bundle().apply {
                        putString(ARG_PARAM1, param1)
                        putString(ARG_PARAM2, param2)
                    }
                }
    }
}