#include <ESP_EEPROM.h>
#include <CTBot.h>
/*
#include <CTBotDataStructures.h>
#include <CTBotDefines.h>
#include <CTBotInlineKeyboard.h>
#include <CTBotReplyKeyboard.h>
#include <CTBotSecureConnection.h>
#include <CTBotStatusPin.h>
#include <CTBotWifiSetup.h>
#include <Utilities.h>
*/
// Hay que poner los numeros de los GPIO
#define ROJO D4//5//2 cable azul en D4
#define AZUL  D2//0 //4 cable naranja en D2
#define SENSOR D8//14 // 15 cable naranja en D8
#define ACTIVADOR D6//13// 12 cable rojo a D6 que alimenta el sensor
// declaro el bot
CTBot myBot;

String ssid="MIWIFI_2G_JNbu";
String pass="6yswxgsb3xgy";
String token="7477140690:AAHRMdDJ93KAFaquLwnpuHywar3BhohRyZo";
long tiempo,ahora,duracion,tipo;
boolean estaAbierta,estadoAnterior,mensaje;
struct myEEPROM{
  int intervalo; // Tiempo entre tomas
  int tipo;      // Tipo duokopt o antibiotico
  int intensidad; // Intensidad del led 0 a 10
} datos;

void setup() {

//Serial.begin(115200);
//Serial.println("Arrancando bot");
  myBot.wifiConnect(ssid,pass);
  myBot.setTelegramToken(token);
/*
  // Ojo con esto. produce un fallo cuando entra en el loop
  if(myBot.testConnection())
    //Serial.println("\nConexion OK");
  else
    //Serial.print("Error en conexion");
*/
  //myBot.sendMessage(5510838625,"Iniciada caja" ); // Tal vez produce error
  estaAbierta = false;
  estadoAnterior = true;
  mensaje=false;
  datos.intensidad=8; //intensidad del led 
  datos.intervalo=12;
  datos.tipo=1;
  //Serial.print("intervalo ");
  //Serial.println(intervalo);
  EEPROM.begin(sizeof(datos)); // Guarda el sitio para los datos
  EEPROM.put(0,datos); // intervalo inicial 12 horas
  EEPROM.commit(); // tipo de cuenta 1-la del duokopt
  //Serial.println(EEPROM[0]);
  datos.intervalo = 0;
  EEPROM.get(0,datos); //tiempo entre dosis en horas NO LO LEE !!!
  //EEPROM.get(9,tipo);
  //Serial.print("intervalo leido");
  //Serial.println(datos.intervalo);

  myBot.sendMessage(5510838625,"Iniciada la caja"); //OJO CON ESTO
}

void loop() {
  //Serial.print("intervalo en el loop");
  //Serial.println(intervalo);
  TBMessage msg; 
  pinMode(ROJO,OUTPUT);
  pinMode(AZUL,OUTPUT);
  pinMode(SENSOR,INPUT);
  pinMode(ACTIVADOR,OUTPUT);
  digitalWrite(ACTIVADOR,HIGH); // Da voltaje al sensor de la puerta
  delay(3);
  
  estaAbierta = digitalRead(SENSOR);
  //Serial.println(estaAbierta);
    if (estaAbierta==true){
      //Serial.println("Detecta Abierta la caja");
      analogWrite(ROJO,LOW); //no estan negados
      analogWrite(AZUL,LOW); //No estan negados
      // tomar el tiempo
      tiempo=millis(); //coge el tiempo que hay que medir NO NECESARIO
      mensaje=false;
      if(estadoAnterior!=estaAbierta){
        //Pasa de cerrada a abierta

        //Serial.println("Se ha abierto la caja");
        myBot.sendMessage(5510838625,"Se ha abierto la caja");
        estadoAnterior=estaAbierta;
      }
      
    }
    else {
      // Esta cerrada la puerta
      //Serial.println("Detecta cerrada la puerta");
      ahora=millis();
      if(estadoAnterior!=estaAbierta){
        //Pasa de abierta a cerrada
        //Serial.println("Se ha cerrado la caja");
        myBot.sendMessage(5510838625,"Se ha cerrado la caja");
        //myBot.sendMessage(5510838625,String(ahora));
        
        estadoAnterior=estaAbierta;
        EEPROM.get(0,datos); //leo el intervalo de la eeprom porque se puede cambiar momentaneamente
        //datos.intervalo=12;
        //Serial.print("intervalo cerrada caja");
        //Serial.println(intervalo);
      }
        
      }
     // se termina con la apertura de puertas

  if (ahora>=tiempo)
    { 
      duracion = ahora-tiempo;
      Serial.println(duracion);}
  else 
    {duracion=(2147483647-tiempo)+ (2147483647+ahora);}

  if (duracion>((datos.intervalo+1)*3600000L)){
    //Serial.println("Te has pasado de la hora");
    if (mensaje==false){
      myBot.sendMessage(5510838625,"No te has echado las gotas");
      mensaje=true;
    }
    analogWrite(ROJO,(datos.intensidad*50)); //no estan negados
    analogWrite(AZUL,(datos.intensidad*50)); //No estan negados
  }
  else if (duracion>(datos.intervalo*3600000L)){
    //Es la hora
    //Serial.println("Ya es la hora");
    analogWrite(ROJO,LOW); //no estan negados
    analogWrite(AZUL,(datos.intensidad*50)); //No estan negados
    //Serial.print("intervalo enciende azul");
    //Serial.println(intervalo);
  }
  else if ((duracion)<(datos.intervalo*360000L)){
    //Aun no es la hora
    //Serial.println("Aun no es la hora");
    if (estaAbierta==false){
    analogWrite(ROJO,(datos.intensidad*50)); //no estan negados
    analogWrite(AZUL,LOW); //No estan negados
    }
  }

  if(myBot.getNewMessage(msg)) {
    if (msg.text=="Reset"){
      tiempo=millis(); //coge el tiempo que hay que medir
      EEPROM.get(0,datos); //leo los datos configurados.
      mensaje=false;
      myBot.sendMessage(5510838625,"Puesto a cero");
     //Serial.println(msg.text);
     //Serial.println(msg.sender.id);
    }
    else if(msg.text=="Administrada"){
      tiempo=millis()-3600000; //coge el tiempo una hora antes
      EEPROM.get(0,datos); // sigue con el intervalo original
      mensaje=false;
      myBot.sendMessage(5510838625,"Puesto a cero");      
    }
    else if(msg.text=="Falta"){
      String falta = String(((datos.intervalo*3600000L)-duracion)/3600000L) + ":" + String((((datos.intervalo*3600000L)-duracion)%3600000L)/60000L);
      myBot.sendMessage(5510838625,falta);      
    }
    else if(msg.text=="Ayuda"||msg.text=="Help"){
      String cero ="Estado.- Informa del estado de la caja\n";
      String uno= "Reset.- Pone valores iniciales y contador a cero \n";
      String dos= "Administrada.- Se ha administrado la dosis \n";
      String tres="Falta.- Responde con el tiempo que falta para la dosis \n";
      String cuatro="Siguiente.- Selecciona el tiempo para la proxima dosis \n";
      String cinco="Intervalo.- Cambia la frecuencia de todas las dosis \n";
      String seis= "Intensidad.- Intensidad de los leds 0 a 10\n";
      String siete="Ayuda o Help.- muestra esta ayuda\n";

      myBot.sendMessage(5510838625,cero + uno + dos + tres + cuatro + cinco + seis + siete);      
    }
    else if(msg.text.substring(0,9)=="Siguiente"){
      //Serial.println(msg.text.substring(0,9));
      long prox = long(msg.text.substring(9).toInt());
      tiempo=millis(); // empiezo a contar desde ya
      datos.intervalo=prox; //Ojo que aqui se modifica el intervalo.
      String falta="Próxima alarma a "+ String(prox) +" horas";
      myBot.sendMessage(5510838625,falta);      
    }
    else if(msg.text.substring(0,9)=="Intervalo"){
      long inter = long(msg.text.substring(9).toInt());
      String horas = String(inter);
      EEPROM.put(0,datos);
      datos.intervalo=inter; // Cambio el intervalo de la proxima dosis
      String falta="Configurado intervalo a "+ horas +" horas";
      myBot.sendMessage(5510838625,falta);      
    }
    else if(msg.text.substring(0,10)=="Intensidad"){
      datos.intensidad = msg.text.substring(10).toInt();
      EEPROM.put(0,datos);
      String falta="Intensidad "+ String(datos.intensidad);
      myBot.sendMessage(5510838625,falta);      
    }
    else if(msg.text.substring(0,10)=="Estado"){
      datos.intensidad = msg.text.substring(10).toInt();
      EEPROM.put(0,datos);
      
      String dos;
      if(estaAbierta){dos="Abierta\n";}
      else {dos="Cerrada\n";}

      String tres ="Tiempo "+String(tiempo)+"\n";
      String cuatro ="Ahora "+String(ahora)+"\n";
           
      String falta = "Estado\nPuerta " + dos + tres + cuatro;

      myBot.sendMessage(5510838625,falta);      
    }
  }

delay(5000);
}
