# 💡 Automação da Mangueira (ESP32)

Meu primeiro projeto integrando **código e hardware**. O sistema automatiza a iluminação da mangueira do sítio da minha família, usando um ESP32, sensores e um painel web para controle e configuração.

🎥 **Veja o projeto funcionando:** [vídeo no LinkedIn](https://lnkd.in/p/djnSxuUZ)

## Como funciona

O sistema tem dois modos de operação:

**Modo automático**
- **Lâmpada de fora:** acende sozinha quando anoitece e apaga quando amanhece, com base na leitura do sensor de luz (LDR).
- **Lâmpadas internas e freezer:** só são acionados quando o sensor de movimento (PIR) detecta alguém entrando **e** o LDR indica que é noite. Depois de um tempo configurável, apagam sozinhos.
- Existe um intervalo mínimo entre detecções para evitar acionamentos repetidos.

**Modo manual**
- Pelo painel web, é possível ligar e desligar cada grupo (lâmpadas, freezer e lâmpada de fora) ou todos de uma vez, e voltar ao modo automático quando quiser.

**Painel web (hospedado no próprio ESP32)**
- Mostra o status em tempo real: movimento, dia/noite, estado de cada saída, modo atual e leitura do LDR.
- Permite ajustar o **limite entre dia e noite** e o **tempo de luz acesa**, sem mexer no código.
- As configurações ficam salvas na memória do ESP32 (`Preferences`) e continuam após queda de energia.
- O ESP32 tenta reconectar ao Wi-Fi automaticamente se a conexão cair.

## Hardware utilizado

- ESP32
- Módulo de relés
- Sensor de movimento PIR
- Sensor de luz LDR
- LED interno para indicar que o sistema está rodando

## Tecnologias

- C++ (Arduino framework para ESP32)
- Bibliotecas: `WiFi.h`, `WebServer.h`, `Preferences.h`
- HTML e CSS embutidos no código para o painel

## Desafios que enfrentei

Por ser meu primeiro projeto, errei bastante, tanto na instalação quanto no código. Corrigi fase a fase:

**Sensor de movimento (PIR) que não funcionava direito**
O sensor às vezes não detectava nada e, quando detectava, não desligava. Precisei testá-lo isoladamente e ajustar a lógica até ele se comportar como esperado.

**Calibração do sensor de luz (LDR)**
Em vez de chutar um valor, medi a leitura real do sensor ao anoitecer, que ficou em torno de **500**. Por isso criei o ajuste do limite no painel, para regular sem precisar regravar o código.

**ESP32 reiniciando sozinho**
Ao implementar um LED piscante para indicar que o sistema estava funcionando, o ESP32 entrava em ciclo de reinicializações. A causa era um erro no meu código, que identifiquei e corrigi.

**Relés com lógica invertida**
O módulo de relés aciona com sinal `LOW` e desliga com `HIGH`, o contrário do que eu esperava. Tive que inverter os valores no código para as saídas funcionarem corretamente.

## Resultado

O sistema está instalado e funcionando no sítio.

## Melhorias futuras

- Adicionar autenticação ao painel (hoje não há login, então ele deve ser usado apenas na rede local).
- Tirar as credenciais do Wi-Fi do código, usando um arquivo de configuração fora do repositório (`config.example.h` como modelo).
- Revisar a leitura do PIR para não perder detecções entre as voltas do `loop`.
- Trocar os links de controle por requisições mais seguras para ações que alteram estado.
- Revisar o valor padrão do limite de luz no código para refletir a calibração real.

## Observação

Projeto desenvolvido por mim, com apoio de IA para orientação e revisão do código. A montagem, a instalação e os testes no local foram feitos por mim.

## Autor

**João Vitor Cardoso da Silva**
Estudante de Engenharia de Software · Maringá, PR
📧 joaovitor2008cardoso@gmail.com
