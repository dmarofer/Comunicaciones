# Comunicaciones V1.0
Libreria para abstraer las comunicaciones MQTT para ESP32 y ESP8266 bajo Arduino

V1.0 - Cambiada la libreria MQTT a la PubSubclient

Para adaptar desde la V 0.9b (que usaba AsyncMQTTClient)

La funcion Enviar ahora hay que pasar char arrary[100] no String.

    char BufferTopic[100];
	char BufferPayload[100];
    strcpy(BufferTopic, MQTTT.c_str());
    strcpy(BufferPayload, RESP.c_str());

    MisComunicaciones.Enviar(BufferTopic, BufferPayload);

Tambien hay que meter el Miscomunicaciones.RunFast();

Mas informacion en Comunicaciones.h

Author: Diego Maroto - BilbaoMakers 2020 - info@bilbaomakers.org - dmarofer@diegomaroto.net
https://github.com/dmarofer
https://bilbaomakers.org/
Licencia: GNU General Public License v3.0