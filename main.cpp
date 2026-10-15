#include <iostream>
#include <memory>
#include <string>
using namespace std; 
struct Resultado {
    double valorOriginal;
    string tipoSensor;
    string estado;
    bool alerta;
};

class ProcesadorLectura {
public:
    virtual ~ProcesadorLectura() {}
    virtual Resultado procesar(double lectura) = 0;
};

class ProcesadorGas : public ProcesadorLectura {
public:
    Resultado procesar(double lectura) override {
        bool alerta = (lectura > 0.75);
        cout << "[Gas] Analizando lectura: " << lectura << endl;
        return {lectura, "Gas", alerta ? "Peligro" : "Normal", alerta};
    }
};

class ProcesadorTemp : public ProcesadorLectura {
public:
    Resultado procesar(double lectura) override {
        bool alerta = (lectura > 0.85);
        cout << "[Temp] Analizando lectura: " << lectura << endl;
        return {lectura, "Temperatura", alerta ? "Alerta" : "Estable", alerta};
    }
};

class ProcesadorPresencia : public ProcesadorLectura {
public:
    Resultado procesar(double lectura) override {
        bool presencia = (lectura > 0.5);
        cout << "[Presencia] Analizando lectura: " << lectura << endl;
        return {lectura, "Presencia", presencia ? "Movimiento" : "Sin movimiento", presencia};
    }
};

class ProcesadorRuido : public ProcesadorLectura {
public:
    Resultado procesar(double lectura) override {
        bool alerta = (lectura > 0.8);
        cout << "[Ruido] Analizando lectura: " << lectura << endl;
        return {lectura, "Ruido", alerta ? "Alto" : "Silencioso", alerta};
    }
};

class ProtocoloEnvio {
public:
    virtual ~ProtocoloEnvio() {}
    virtual bool enviar(Resultado res) = 0;
};

class ProtocoloMQTT : public ProtocoloEnvio {
public:
    bool enviar(Resultado res) override {
        cout << "  >> [MQTT] El sensor esta conectado y funcionando" << endl;
        return true;
    }
};

class ProtocoloHTTP : public ProtocoloEnvio {
public:
    bool enviar(Resultado res) override {
        cout << "  >> [HTTP] El sensor esta conectado y funcionando" << endl;
        return true;
    }
};

class ProtocoloBluetooth : public ProtocoloEnvio {
public:
    bool enviar(Resultado res) override {
        cout << "  >> [Bluetooth] El sensor esta conectado y funcionando" << endl;
        return true;
    }
};

class ProtocoloLoRa : public ProtocoloEnvio {
public:
    bool enviar(Resultado res) override {
        cout << "  >> [LoRa] El sensor esta conectado y funcionando" << endl;
        return true;
    }
};

class NodoIoT {
private:
    unique_ptr<ProcesadorLectura> procesador;
    unique_ptr<ProtocoloEnvio> protocolo;

public:
    NodoIoT(unique_ptr<ProcesadorLectura> p, unique_ptr<ProtocoloEnvio> pr) 
        : procesador(move(p)), protocolo(move(pr)) {}

    void set_procesador(unique_ptr<ProcesadorLectura> p) {
        procesador = move(p);
    }

    void set_protocolo(unique_ptr<ProtocoloEnvio> pr) {
        protocolo = move(pr);
    }

    bool realizar_lectura(double val) {
        Resultado r = procesador->procesar(val);
        return protocolo->enviar(r);
    }
};

int main() {
    // Creación de nodos (más de los 3 requeridos, usando todas las estrategias)
    NodoIoT nodoAula(unique_ptr<ProcesadorLectura>(new ProcesadorTemp()), 
                     unique_ptr<ProtocoloEnvio>(new ProtocoloMQTT()));

    NodoIoT nodoLab(unique_ptr<ProcesadorLectura>(new ProcesadorGas()), 
                    unique_ptr<ProtocoloEnvio>(new ProtocoloHTTP()));

    NodoIoT nodoPasillo(unique_ptr<ProcesadorLectura>(new ProcesadorPresencia()), 
                        unique_ptr<ProtocoloEnvio>(new ProtocoloBluetooth()));

    NodoIoT nodoOficina(unique_ptr<ProcesadorLectura>(new ProcesadorRuido()), 
                        unique_ptr<ProtocoloEnvio>(new ProtocoloLoRa()));

    cout << "PRUEBA DE NODOS INICIALES"<< endl;
    nodoAula.realizar_lectura(0.9);
    nodoLab.realizar_lectura(0.3);
    nodoPasillo.realizar_lectura(0.6);
    nodoOficina.realizar_lectura(0.85);

    cout << endl << "DEMOSTRACION DE CAMBIO DINAMICO DE PROTOCOLO" << endl;
    cout << "Cambiando protocolo del Nodo Lab (de HTTP a LoRa)..." << endl;
    nodoLab.set_protocolo(unique_ptr<ProtocoloEnvio>(new ProtocoloLoRa()));
    
    cout << "Ejecutando de nuevo el Nodo Lab con el nuevo protocolo:" << endl;
    nodoLab.realizar_lectura(0.8);

    return 0;
}
