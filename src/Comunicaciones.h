
#include <Arduino.h>


class Comunicaciones
{


private:

    // Esto en cristiano moderno lo vamos a llamar por su nombre. Manejadores de eventos.
    // Voy a hacer un unico manejador de eventos que pase un evento de un tipo definido aqui, y un char array con informacion
    // Este envia el comando recibido en el fomrato JSON que yo me he inventado en un char array de 100 junto con un int tipo de evento 
    typedef void(*TipoCallbackEvento)(unsigned int Evento_Comunicaciones, char Info[200]);  // Definir el tipo (pantilla) de la funcion callback que me tendran que pasar (aqui yo tengo que saber como es aunque no la tenga aun para usarla claro)
                                                                                            // typedef crea un alias de un tipo. En este caso crea el typo Callback Mensaje recibido que es "puntero a una funcion void X (String, String)"
    TipoCallbackEvento MiCallbackEventos = nullptr;                                         // Instanaciar aqui el nuevo tipo vacio (nullptr porque recordemos que es puntero)
                                                                   
    
    // Funciones internas de esta clase para pasarle al Objeto MQTT como manejador de sus eventos)
    // Luego veremos que hay que hacer malabares para pasarselas
    // En esta libreria PubSubclient solo hay una funcion que se ejecuta cuando llega un mensaje y es asi.
    void RxCallback (char* topic, byte* payload, unsigned int length);
    
    // Variables internas de configuracion para el objeto MQTT
    char mqttserver[40];
	char mqttport[6];
	char mqtttopic[20];
	char mqttusuario[20];
	char mqttpassword[20];
    char mqttclientid[20];
    
    char cmndTopic[75];
	char lwtTopic[75];
    
    char SubTeleTopic[75];
    bool SubTeleTopicSet;

    uint16_t KeepAliveSecs;

    // Funcion para crear la estructura de los topics.
    void FormaEstructuraTopics();
   

public:
    
    // Enum para definir los tipos de eventos que devolvera nuestro callback
    enum Tipo_Evento_Comunicaciones {

        EVENTO_CONECTANDO,              // Cuando iniciamos una conexion MQTT
		EVENTO_CONECTADO,				// Cuando se conecta al broker
		EVENTO_DESCONECTADO,	    	// Cuando se desconecta del Broker
		EVENTO_CMND_RX,	            	// Cuando recibo un comando
        EVENTO_TELE_RX,                 // Cuando se recibe un mensaje de Telemetria de un dispositivo remoto
		EVENTO_MSG_TX,					// Cuando se ha finalaziado la transmision de un mensaje
		
	};
    
    // Constructor y destructor
    Comunicaciones();
    ~Comunicaciones();
    
    // Funciones de configuracion
    void SetMqttServidor(char l_mqttserver[40]);             // Configurar el servidor MQTT
    void SetMqttUsuario(char l_mqttusuario[20]);             // Configurar el usuario
    void SetMqttPassword(char l_mqttpassword[20]);           // Configurar la contraseña
    void SetMqttTopic(char l_mqtttopic[20]);                 // Configurar el topic base
    void SetMqttClientId(char l_mqttclientid[20]);           // Configurar el topic base
    void SetSubTeleTopic(char l_SubTeleTopic[75]);           // Para Suscribirse a un topic para recibir info
    void SetKeepAlive(uint16_t l_KeepAliveTimeSecs);                   // Para configurar el timeout de la conexion
    
    void SetEventoCallback(TipoCallbackEvento ref);          // Para pasarme el manejador de eventos
    
    void Enviar(char Topic[75], char Payload[200]);                 // Funcion para publicar un payload en un topic sin retain
    void Enviar(char Topic[75], char Payload[200], bool retained);  // Sobrecarga para incluir el flag retained

    void Conectar();                                         // Funcion para Conectar al servidor MQTT y publicar el LWT
    void Desonectar();                                       // Funcion para desconectar del servidor MQTT
    
    bool IsConnected();                                      // Para saber si esta conectado
    
    void RunFast();                                          // Loop de vida
    
};


