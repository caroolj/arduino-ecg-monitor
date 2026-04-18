# ESP32-ecg-monitor
Este projeto foi desenvolvido como uma atividade prática durante o meu curso de Ciência da Computação. O objetivo foi projetar e implementar um protótipo funcional de eletrocardiograma (ECG) capaz de captar, processar e exibir batimentos cardíacos em tempo real.

VISÃO GERAL:
O sistema utiliza um microcontrolador Arduino para realizar a leitura analógica de sinais elétricos do coração através de sensores específicos. O código foca na estabilização do sinal e na comunicação serial para visualização gráfica dos dados.

TECNOLOGIAS E COMPONENTES:

Microcontrolador: ESP32 (Suporte a IoT e Processamento Dual-Core).

Sensor: Sensor de Pulso Óptico com cancelamento de ruído integrado.

Linguagem: C++ (Framework Arduino).

Componentes Adicionais: Protoboard 400 furos e Jumpers Dupont para montagem sem solda.

FUNCIONALIDADES TÉCNICAS:

Leitura Analógica Nativa: Utilização dos ADCs do ESP32 para alta resolução na captura do sinal.

Processamento de Sinal: Circuitos internos do sensor e lógica em código para filtragem de ruídos e estabilização da leitura.

Visualização Serial: Monitoramento via Serial Plotter para análise da onda de pulso.


