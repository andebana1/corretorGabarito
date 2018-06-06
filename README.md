# corretorGabarito
Um corretor simples de gabarito escrito em c++ usando opencv. A simple multiple choice scanner written in c++ using opencv.

O corretor foi implementado com a seguinte ideia: Uma imagem do gabarito é carregada e informa quais questões estão corretas e erradas e a pontuação. Levando em consideração um conjunto de respostas já cadastradas.

# Etapas:


# A primeira e mais importante parte foi a escolha do modelo de gabarito. modeloGabarito.png.

# Preparação da imagem: 
Etapa básica, onde a imagem passa por conversão para tons de cinza, borrada para tirar ruídos e binarizada.

# Pré-processamento: 
Erosão na imagem.

# Detecção das linhas:
Algoritmo de Hough para encontrar linhas.

# Encontrar os contornos
Encontrar os contornos (respostas marcadas) e encontrar seus centros. Infelizmente, o Hough Circles não conseguiu encontrar os círculos devido o seu tamanho muito pequeno. Foi necessário usar a função findCountours().

# Fim
Um vez que já se sabe as coordenadas de todos as respostas e de todas as linhas/colunas, o que sobra é verificar onde os pontos se encaixam e marcá-los como respostas em uma matriz. O restante é apenas checar várias respostas, linhas vazias, etc. E, por fim, calcular e mostrar a pontuação.

