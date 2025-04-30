import React, { useState } from 'react';
import mqtt from 'mqtt';

const MQTTPublisher = () => {
  const [message, setMessage] = useState('');
  const [client, setClient] = useState(null);

  // Conexão com o broker MQTT (WebSocket)
  const connectToBroker = () => {
    const mqttClient = mqtt.connect('ws://52.237.23.203:1883'); // Ex: ws://52.237.23.203:9001
    
    mqttClient.on('connect', () => {
      console.log('Conectado ao broker MQTT!');
      setClient(mqttClient);
    });

    mqttClient.on('error', (err) => {
      console.error('Erro MQTT:', err);
    });
  };

  // Publica mensagem no tópico /TEF/mednotify001/cmd/b1
  const publishMessage = () => {
    if (client && message) {
      client.publish('/TEF/mednotify001/cmd/b1', message, (err) => {
        if (!err) {
          alert(`Mensagem enviada: ${message}`);
        } else {
          console.error('Falha ao publicar:', err);
        }
      });
    }
  };

  return (
    <div>
      <h2>Publicar Comando MQTT</h2>
      <input
        type="text"
        value={message}
        onChange={(e) => setMessage(e.target.value)}
        placeholder="Digite o comando"
      />
      <button onClick={connectToBroker}>Conectar ao Broker</button>
      <button onClick={publishMessage} disabled={!client}>
        Enviar para /b1
      </button>
    </div>
  );
};

export default MQTTPublisher;