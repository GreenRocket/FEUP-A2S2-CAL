É necessário o GraphViewerController.jar que foi submetido e está na raiz da pasta.

Para compilar, correr a makefile fornecida.

O programa utiliza argumentos para indicar quais os ficheiros a ler, a seed a utilizar para gerar pacotes e o número de pacotes a gerar.

SpeedMail [seed int] [package count uint] [node file path] [edge file path]

Ex: SpeedMail 0 10 normalizedNodes.txt normalizedEdges.txt

Os ficheiros precisam de estar no formato de:

nodes: 	1ª linha 	-> número de nodes
		Restantes 	-> (id, x, y)
		
edges: 	1ª linha 	-> número de edges
		Restantes 	-> (origem, destino)



Nota: Nas funções de teste (apenas acessiveis modificando o código fonte), a variante "testAverageRouteTimeWithRandomPackages"
utiliza uma seed aleatório e o 1º argumento indica o número de iterações a correr.

Ex: SpeedMail 5 100 normalizedNodes.txt normalizedEdges.txt
    Média de 5 iterações com 100 pacotes