const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Abre a porta serial COM4
const serialPort = new SerialPort({
  path: 'COM4',
  baudRate: 9600, 
});

// Cria um parser para ler linha a linha
const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\r\n' }));

const port = 3000;
let webSocketClient = null;

// Escutador de eventos para a porta serial
serialPort.on('open', () => {
  console.log('Porta serial COM4 aberta');
});

// Escutador de eventos para os dados
parser.on('data', data => {
    const temperature = parseFloat(data);

    if (!isNaN(temperature)&& webSocketClient) {
        console.log(`Temperatura: ${temperature} °C`);
        webSocketClient.send(temperature);

    } else {
        console.log(data)
    }
});

serialPort.on('error', err => {
  console.error('Error: ', err.message);
});

// Serve a pagina html
app.use(express.static(path.join(__dirname, 'public')));

// Lida com a conexao do websocket
wss.on('connection', (ws) => {
    console.log('Conexão websocket aberta');
    webSocketClient = ws;

    ws.on('close', () => {
        webSocketClient = null;
        console.log('Conexão websocket fechada');
    });
});

// Start the server
server.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}/`);
});