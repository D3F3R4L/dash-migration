## Sobre o programa
O programa é baseado em um framework dash para NS-3 (@haraldott/dash). Sua principal função é simular transmissões dash em diferentes cenários criados a partir do NS-3, podendo adicionar diferentes algoritmos de adaptação, ou utilizar um dos algoritmos presentes no framework (Tobasco, Panda e Festive).

## Parâmetros
Os seguintes parâmetros devem ser especificados para executar o programa:
simlationId: o ID da simulação, para obter diferentes logs em simulações que utilizam o mesmo cenário (mesmo algoritmo e mesmo número de clientes).
- numberOfClients: O número de clientes usado na simulação.
- segmentDuration: A duração de cada segmento de vídeo (em microsegundos).
- adaptationAlgo: O algoritmo de adaptção que o cliente utiliza. Os algoritmos pré-instalados são: tobasco, panda e festive.
- segmentSizeFile: O arquivo contendo o tamanho dos segmentos de vídeo. A estrutura do arquivo é uma matriz (n,m), com n sendo o número de níveis de representação e m sendo o número de segmentos. Por exemplo, um vídeo codificado em três níveis de representação e dividido em 2 segmentos pode ser descrito através de um arquivo:

1564 22394

1627 46529

1987 121606

## Como executar

Para executar o programa é necessário seguir o seguintes passos:

1. Acessar a pasta ns-3.29
2. Habilitar os testes e o exemplos através do comando:

  ./waf configure --enable-tests --enable-examples

3. Construir e linkar as dependências e configurações estabelicidas através do comando:

  ./waf

4. Rodar o script dash-migration, localizado na pasta scratch do ns-3.29, passando os parâmetros necessários descritos anteriormente. Exemplo:

  ./waf --run="dash-migration --simulationId=1 --numberOfClients=3 --adaptationAlgo=panda --segmentDuration=2000000 --segmentSizeFile=contrib/dash/segmentSizes.txt"

## Resultados
Os resultados podem ser obtidos através dos logs dentro da pasta "dash-log-files"
