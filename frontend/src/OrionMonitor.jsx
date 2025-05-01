import React, { useState, useEffect } from 'react';
import axios from 'axios';
import './OrionMonitor.css'

const OrionMonitor = () => {
  const [lastUpdate, setLastUpdate] = useState(null);
    //Hashmap de mensagens dos botões
  const [commands, setCommands] = useState({
    red: '',
    blue: '',
    green: '',
    yellow: '',
    white: ''
  });

  useEffect(() => {
    // Verifica a notificação a cada 3 segundos
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

  // Muda a variável commands
  const handleChange = (e) => {
    const { name, value } = e.target;
    setCommands(prev => ({ ...prev, [name]: value }));
  };
  // Manda o comando para o Orion
  const sendCommandToOrion = async (attribute, value) => {
    try {
      await axios.patch( 
        'http://52.237.23.203:1026/v2/entities/urn:ngsi-ld:mednotify:001/attrs',
        {
          [attribute]: {  
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
  
  // Submit do forms para mandar os comandos até os botões
  const handleSubmit = (e) => {
    e.preventDefault();
    console.log("Comandos enviados:", commands['red']);
    sendCommandToOrion('b1', commands['red']);
    sendCommandToOrion('b2', commands['blue']);
    sendCommandToOrion('b3', commands['green']);
    sendCommandToOrion('b4', commands['yellow']);
    sendCommandToOrion('b5', commands['white']);
  };
  // Reseta o message do orion
  const resetNotification = () => {
    sendCommandToOrion('message', 'Nenhuma notificação');
  };

  return (
    <>
    <div className='bgPart'>
    </div>
    <div id='orionMain'>
        <div id='orionNotification'>
            <h2>Notificação: </h2>
            <p>{lastUpdate || "Nenhuma"}</p>
            <button type="submit" id='orionButton' onClick={resetNotification}>Resetar Notificação</button>
        </div>


            <form onSubmit={handleSubmit} className='orionForms'>
                <div>
                    <h2>Configuração dos botões</h2>
                    <div>
                        <input type="text" name="red" value={commands.red} onChange={handleChange} placeholder="Botão Vermelho" className='orionInput' />
                        <input type="text" name="blue" value={commands.blue} onChange={handleChange} placeholder="Botão Azul" className='orionInput' />
                    </div>
                    <div>
                        <input type="text" name="green" value={commands.green} onChange={handleChange} placeholder="Botão Verde"  className='orionInput'/>
                        <input type="text" name="yellow" value={commands.yellow} onChange={handleChange} placeholder="Botão Amarelo"  className='orionInput'/>
                    </div>
                    <input type="text" name="white" value={commands.white} onChange={handleChange} placeholder="Botão Branco" className='orionInput'/>
                </div>


                <button type="submit" id='orionButton'>Enviar Comandos</button>
            </form>


    </div>
    </>
  );
};

export default OrionMonitor;