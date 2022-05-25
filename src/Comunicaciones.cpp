
#include <Comunicaciones.h>
#include <Arduino.h>
#include <ArduinoJson.h>				// ATENCION: Implementada con version 5, mas concretamente version 5.13.4
#include <PubSubClient.h>

#ifdef ESP32
  #include <WiFi.h>
#else
 #include <ESP8266WiFi.h>          
#endif

WiFiClient espClient;
PubSubClient ClienteMQTT(espClient);

Comunicaciones::Comunicaciones(){

    strcpy(mqttserver,"127.0.0.1");
    strcpy(mqttport,"1883");
    strcpy(mqtttopic, "NOTOPIC");
    strcpy(mqttusuario, "mosquitto");
    strcpy(mqttpassword, "nopasswd");
    strcpy(mqttclientid, "noclientid");
    SubTeleTopicSet = false;
    KeepAliveSecs = 5;
    
}

Comunicaciones::~Comunicaciones(){


}

void Comunicaciones::SetEventoCallback(TipoCallbackEvento ref){

    MiCallbackEventos = (TipoCallbackEvento)ref;

}

void Comunicaciones::SetMqttServidor(char l_mqttserver[40]){

    strcpy(mqttserver, l_mqttserver);
    this->Desonectar();
    
}

void Comunicaciones::SetMqttUsuario(char l_mqttusuario[20]){

    strcpy(mqttusuario, l_mqttusuario);
    this->Desonectar();

}

void Comunicaciones::SetMqttPassword(char l_mqttpassword[20]){

    strcpy(mqttpassword, l_mqttpassword);
    this->Desonectar();

}

void Comunicaciones::SetMqttTopic(char l_mqtttopic[20]){

    strcpy(mqtttopic, l_mqtttopic);
    this->FormaEstructuraTopics();
    this->Desonectar();

}

void Comunicaciones::SetMqttClientId(char l_mqttclientid[20]){

    strcpy(mqttclientid, l_mqttclientid);
    this->Desonectar();

}

void Comunicaciones::SetSubTeleTopic(char l_SubTeleTopic[75]){

    strcpy(SubTeleTopic, l_SubTeleTopic);
    SubTeleTopicSet = true;
    this->Desonectar();

}

void Comunicaciones::SetKeepAlive(uint16_t l_KeepAliveTimeSecs){

    this->Desonectar();
    KeepAliveSecs = l_KeepAliveTimeSecs;
    ClienteMQTT.setKeepAlive(KeepAliveSecs);

}

void Comunicaciones::FormaEstructuraTopics(){

    strcpy(cmndTopic, "cmnd/");
    strcat(cmndTopic, mqtttopic);
    strcat(cmndTopic, "/#");
    
    strcpy(lwtTopic, "tele/");
    strcat(lwtTopic, mqtttopic);
    strcat(lwtTopic, "/LWT");
        
}

bool Comunicaciones::IsConnected(){

    return ClienteMQTT.connected();

}

void Comunicaciones::Conectar(){

    this->FormaEstructuraTopics();

    ClienteMQTT.setServer(mqttserver, 1883);
    ClienteMQTT.setCallback(std::bind(&Comunicaciones::RxCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    ClienteMQTT.setKeepAlive(KeepAliveSecs);
    ClienteMQTT.setSocketTimeout(5);
        
    char Mensaje[100];
    strcpy(Mensaje, "Intentando conectar a ");
    strcat(Mensaje, mqttserver);
    this->MiCallbackEventos(EVENTO_CONECTANDO, Mensaje);
    
    if (ClienteMQTT.connect(mqttclientid,mqttusuario,mqttpassword,lwtTopic,2,true,"Offline",true)){

        ClienteMQTT.publish(lwtTopic,"Online",true);
        
        strcpy(Mensaje, "Conectado al servidor MQTT");
        this->MiCallbackEventos(EVENTO_CONECTANDO, Mensaje);		

        char Mensaje[100];

        if (ClienteMQTT.subscribe(cmndTopic, 1)) {
        
            strcpy(Mensaje, "Suscrito al topic de comandos: ");
            strcat(Mensaje, cmndTopic);
            MiCallbackEventos(EVENTO_CONECTADO, Mensaje);			

        }
        
        else {

            strcpy(Mensaje, "ERROR al suscribirse al topic de comandos: ");
            strcat(Mensaje, cmndTopic);
            MiCallbackEventos(EVENTO_CONECTANDO, Mensaje);		

        }

        if (SubTeleTopicSet){


           if (ClienteMQTT.subscribe(SubTeleTopic, 1)) {
        
                strcpy(Mensaje, "Suscrito al topic Telemetria: ");
                strcat(Mensaje, SubTeleTopic);
                MiCallbackEventos(EVENTO_CONECTADO, Mensaje);			

            }
            
            else {

                strcpy(Mensaje, "ERROR al suscribirse al topic de Telemetria: ");
                strcat(Mensaje, SubTeleTopic);
                MiCallbackEventos(EVENTO_CONECTANDO, Mensaje);		

            } 


        }
            
    }

    else {

        strcpy(Mensaje, "ERROR conectando al servidor MQTT: ");
        String t_State = String(ClienteMQTT.state());
        strcat(Mensaje, t_State.c_str());
        MiCallbackEventos(EVENTO_CONECTANDO, Mensaje);		

    }

    
}

void Comunicaciones::Enviar(char Topic[75], char Payload[200]){

    if (ClienteMQTT.connected()){ClienteMQTT.publish(Topic, Payload, false);}

}

void Comunicaciones::enviarRetained(char Topic[75], char Payload[200]){

    if (ClienteMQTT.connected()){ClienteMQTT.publish(Topic, Payload, true);}

}

void Comunicaciones::Desonectar(){

    // No lanzo evento mio porque se lanza en el callback del objeto MQTT
    ClienteMQTT.disconnect();
        
}

void Comunicaciones::RxCallback(char* topic, byte* payload, unsigned int length) {
  
    // Toda esta funcion me la dispara la libreria MQTT cuando recibe algo y me pasa todo lo de arriba
    // Aqui me la cocino yo a mi gusto, miro si es cmnd/#, y construyo un Json muy bonito con el comando
    // Despues a su vez yo paso este JSON mi comando MiCallbackMsgRecibido al main para que haga con ella lo que guste
    // Que generalmente sera enviarlo a la tarea de evaluacion de comandos para ver que hacer.
    
    // El Payload me viene en un array de bytes (logico porque el MQTT transporta Bytes, si yo codifico ASCII porque soy Humano es mi problema)
    // Ademas el bufer que se usa es el mismo que para publicar asi que me viene con Kakafu de otros mensajes, de ahi el Length

    // Pero cojo y le meto en la ultima posicion el de final de cadena
    payload[length] = '\0';

    // Y con esta cosa tan bonita y simple el tio listo ya sabe hasta donde llenarme el char array c_payload, hasta el \0
    char *c_payload = (char *) payload;

    // El topic lo paso a String para poder manejar mejor
    String s_topic = String(topic);

    // Sacamos el prefijo del topic, o sea lo que hay delante de la primera /
    int Indice1 = s_topic.indexOf("/");
    String Prefijo = s_topic.substring(0, Indice1);
    
    // Si el prefijo es cmnd o tele lo procesamos
    if (Prefijo == "cmnd" || Prefijo == "tele") { 

        // Sacamos el "COMANDO" del topic, o sea lo que hay detras de la ultima /
        int Indice2 = s_topic.lastIndexOf("/");
        String Comando = s_topic.substring(Indice2 + 1);

        // Vamos generando el JSON que pasaremos en la funcion
        DynamicJsonBuffer jsonBuffer;
        JsonObject& ObjJson = jsonBuffer.createObject();
        ObjJson.set("COMANDO",Comando);
        ObjJson.set("PAYLOAD",c_payload);

        char JSONmessageBuffer[200];
        ObjJson.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        

        if (Prefijo == "cmnd"){

            MiCallbackEventos(Comunicaciones::EVENTO_CMND_RX, JSONmessageBuffer);

        }

        else if (Prefijo == "tele"){

            MiCallbackEventos(Comunicaciones::EVENTO_TELE_RX, JSONmessageBuffer);

        }
                    
    }

		
}

void Comunicaciones::RunFast(){

    ClienteMQTT.loop();

}