# Comunicaciones
Libreria para comunicaciones ESP32 ESP8266





Para adaptar desde la V 0.9:

La funcion Enviar ahora hay que pasar char arrary[100] no String.

    char BufferTopic[100];
	char BufferPayload[100];
    strcpy(BufferTopic, MQTTT.c_str());
    strcpy(BufferPayload, RESP.c_str());

    MisComunicaciones.Enviar(BufferTopic, BufferPayload);

Tambien hay que meter el Miscomunicaciones.RunFast();

