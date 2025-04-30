import React, { useState, useEffect } from 'react';
import axios from 'axios';

const OrionMonitor = () => {
  const [lastUpdate, setLastUpdate] = useState(null);

  const [commands, setCommands] = useState({
    red: '',
    blue: '',
    green: '',
    yellow: '',
    white: ''
  });

  useEffect(() => {
    const interval = setInterval(() => {
      axios.get('http://52.237.23.203:1026/v2/entities/urn:ngsi-ld:mednotify:001?options=keyValues', {
        headers: {
          'fiware-service': 'smart',
          'fiware-servicepath': '/',
          'Accept': 'application/json'
        }
      })
      .then(response => {
        const data = response.data;
        if (data.message && data.message !== lastUpdate) {
          setLastUpdate(data.message);
          alert(`Nova mensagem recebida: ${data.message}`);
        }
      })
      .catch(error => {
        console.error("Erro ao consultar Orion:", error);
      });
    }, 3000);

    return () => clearInterval(interval);
  }, [lastUpdate]);

  const handleChange = (e) => {
    const { name, value } = e.target;
    setCommands(prev => ({ ...prev, [name]: value }));
  };

  const sendCommandToOrion = async (attribute, value) => {
    try {
      await axios.patch( // Note que usei PATCH em vez de POST
        'http://52.237.23.203:1026/v2/entities/urn:ngsi-ld:mednotify:001/attrs',
        {
          [attribute]: {  // Notação de colchetes para usar o parâmetro como chave
            type: "Text",
            value: value
          }
        },
        {
          headers: {
            'Content-Type': 'application/json',
            'Fiware-Service': 'smart',
            'Fiware-ServicePath': '/'
          }
        }
      );
      alert(`Atributo ${attribute} atualizado para: ${value}`);
    } catch (error) {
      console.error(`Erro ao atualizar ${attribute}:`, error.response?.data || error.message);
      alert(`Erro ao atualizar ${attribute}`);
    }
  };
  
  
  const handleSubmit = (e) => {
    e.preventDefault();
    console.log("Comandos enviados:", commands['red']);
    sendCommandToOrion('b1', commands['red']);
    sendCommandToOrion('b2', commands['blue']);
    sendCommandToOrion('b3', commands['green']);
    sendCommandToOrion('b4', commands['yellow']);
    sendCommandToOrion('b5', commands['white']);
  };

  return (
    <div>
      <h2>Monitoramento FIWARE Orion</h2>
      <p>Última mensagem: {lastUpdate || "Nenhuma"}</p>

      <form onSubmit={handleSubmit}>
        <input type="text" name="red" value={commands.red} onChange={handleChange} placeholder="Comandos do Botão Vermelho" />
        <input type="text" name="blue" value={commands.blue} onChange={handleChange} placeholder="Comandos do Botão Azul" />
        <input type="text" name="green" value={commands.green} onChange={handleChange} placeholder="Comandos do Botão Verde" />
        <input type="text" name="yellow" value={commands.yellow} onChange={handleChange} placeholder="Comandos do Botão Amarelo" />
        <input type="text" name="white" value={commands.white} onChange={handleChange} placeholder="Comandos do Botão Branco" />
        <button type="submit">Enviar Comandos</button>
      </form>
      <button type="submit" onClick={sendCommandToOrion}>Teste</button>
    </div>
  );
};

export default OrionMonitor;