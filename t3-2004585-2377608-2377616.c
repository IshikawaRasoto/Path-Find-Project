/*============================================================================
* CSF13 - 2021-1 - PROJETO 3
*----------------------------------------------------------------------------
* Rafael Eijy Ishikawa Rasoto - rafaelrasoto@alunos.utfpr.edu.br - RA: 2004585
* Gabriel de Almeida Spadafora -  gspadafora@alunos.utfpr.edu.br - RA: 2377608
* João Pedro dos Reis Mendes - mendes.2021@alunos.utfpr.edu.br - RA: 2377616
*============================================================================
* 3o projeto da disciplina Fundamentos de Programação
* 1o semestre de 2021, prof. Bogdan T. Nassu e Leyza E. B. Dorini,
* Universidade Tecnológica Federal do Paraná.
*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "trabalho3.h"

#define PRETO -1
#define BRANCO -2
#define DIVISOR 100

/*
*   Funcao que calcula media geral da imagem. Somamos o valor de todos os pixels e depois dividimos pelo total de pixels na imagem.
*/

unsigned int calculo_media_geral(Imagem1C* img){
    int i, j;
    unsigned int soma = 0, media, total_pixel = img->altura * img->largura; // Utilizamos tipo unsigned para evitar o risco de overflow alem de nao existir pixels com valores negativos a principio.
    for(i=0; i < (img->altura)-1; i++){
        for(j = 0; j < (img->largura)-1; j++){
            soma += img->dados[i][j];
        }
    }
    media = soma/total_pixel;
    return media;
}

/*
*   Funcao que aloca matriz de tipo int, criamos uma para cada apenas para ficar organizado.
*/

int** aloca_matriz_int (int** matriz, int altura, int largura){
    int i;
    matriz = (int**) malloc ((altura)*sizeof(int*));
    for (i = 0; i < altura; i++){
        matriz[i] = (int*) malloc ((largura) * sizeof(int));
    }
    return matriz;
}

/*
*   Funcao que aloca matriz de tipo unsigned int, criamos para ficar organizado a funcao principal.
*/

unsigned int** aloca_matriz_u_int (unsigned int** matriz, int altura, int largura){
    int i;
    matriz = (unsigned int**) malloc ((altura)*sizeof(unsigned int*));
    for (i = 0; i < altura; i++){
        matriz[i] = (unsigned int*) malloc ((largura) * sizeof(unsigned int));
    }
    return matriz;
}

/*
*   Funcao que calcula o parametro a ser usado de acordo com a media geral da imagem. A variavel ajuste sera futuramente dividida por DIVISOR, sendo assim, ela deve variar de 0 a 100.
*/

unsigned int calculo_parametros_inicializa_leitura (Imagem1C* img, int** img_leitura, unsigned int** copia_imagem, int* ajuste, unsigned int media_geral){
    int i, j;
    unsigned int media_geral_elevada;
    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            img_leitura[i][j] = 0;
            if(media_geral>=180){
                copia_imagem[i][j] = img->dados[i][j] * img->dados[i][j];
                media_geral_elevada = media_geral * media_geral;  //Elevamos ao quadrado pois a imagem possui valores altos. No geral, o preto tem valores bem diferentes do branco nessas imagens bem iluminadas, entao nao precisamos criar uma diferenca tao grande entre o branco e o preto pois ela ja existe naturalmente na imagem, apenas saturamos ela.
                (*ajuste) = 50; // Inicializamos em 50 pois eh uma porcentagem e o limiar quando elevamos ao quadrado fica por volta dos 65% aos 90% dependendo da imagem, entao como eh constantemente alterada, de ciclo em ciclo, inicializamos com valores baixos para que o proprio codigo encontre um valor ideal.
            }else{
                copia_imagem[i][j] = img->dados[i][j] * img->dados[i][j] * img->dados[i][j];
                media_geral_elevada = media_geral * media_geral * media_geral;  //Elevamos ao cubo pois a imagem possui um espectro pequeno entre branco e preto, normalmente sao imagens mal iluminadas entao a diferenca entre ambos eh bem pequeno. Por isso, elevamos seu valor ao cubo para que dessa forma a diferenca torn-se tao grande que facilita o trabalho de diferencicacao de branco e preto.
                (*ajuste) = 25;  //Inicializamos em 25 pelo mesmo motivo de inicializarmos o superior como 50, observamos que imagens mal iluminadas encontram saidas por volta de 45% a 60%.
            }
        }
    }
    return media_geral_elevada;
}

/*
*   Funcao que determina qual pixel é considerado preto e qual é considerado branco baseado nos ajustes.
*/

void preenche_imagem_leitura (Imagem1C* img, int** img_leitura, unsigned int** copia_imagem, unsigned int media_geral_elevada, int ajuste){
    int i, j;
    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            if(copia_imagem[i][j] > (media_geral_elevada)*(ajuste)/DIVISOR){  // Utilizamos uma porcentagem da media geral da imagem elevada como parametro da divisao. Como as imagens sao saturadas a diferenca entre branco e preto aumenta exponencialmente, entao qualquer variacao na porcentagem tem um grande impacto na divisao entre preto e branco.
                img_leitura[i][j] = BRANCO;
            }else{
                img_leitura[i][j] = PRETO;
            }
        }
    }
}

/*
*   Funcao que determina a altura do pixel no qual o caminho sera iniciado.
*/

int encontra_altura_inicial (Imagem1C* img){
    int i;
    int valor_preto = 0xFF, altura_inicial; // Inicializamos valor_preto em 255 pois sabemos que o valor de cada pixel eh de um byte, ou seja, 0xFF. Dessa forma, temos um bom valor de inicializacao para irmos comparando de linha a linha para descobrirmos qual eh o pixel mais preto da primeira coluna e definirmos ele como ponto de acesso.
    for(i = 0; i < (img->altura)-1; i++){ // Claro que nesse caso assumimos que exista apenas uma entrada, mas isso eh uma especificacao do projeto.
        if(img->dados[i][0] < valor_preto){
            valor_preto = img->dados[i][0];
            altura_inicial = i; // Salvamos qual a altura do pixel mais preto atual.
        }
    }
    return altura_inicial; 
}

/*
*   Funcao que preenche a matriz leitura (preenchida com PRETO e BRANCO) com o custo de cada caminho.
*/

int preenche_leitura (Imagem1C* img, int** img_leitura){
    int i, j;
    int preencheu = 1; // Flag para saber se houve algum preenchimento no ciclo.
    int contador = 1; // Contador que determina o valor das casas.
    while(preencheu){
        preencheu = 0; // Flag em 0 pois queremos ver se nesse ciclo houve algum preenchimento, se nao houver esse estado nao eh alterado.
        for(i = 0; i < (img->altura); i++){
            for(j = 0; j < (img->largura); j++){ // Controle que percorre a matriz.
                if(img_leitura[i][j] == contador){ // Procuramos casas da img_leitura que contenham exatamente o valor de contador.
                    // Analisamos duas coisas, 1. Se eh permitido fazermos a analise de casas adjacentes para evitar erros de acesso. 2. Analisamos a casa do lado para ver se ela nao foi preenchida e se eh preta. (Se foi preenchida, tem seu valor alterado de PRETA para o do contador)
                    if((j!=(img->largura)-1)&&(img_leitura[i][j+1]==PRETO)){
                        img_leitura[i][j+1] = contador+1;
                        preencheu = 1; //Ativamos a flag
                    }
                    if((i!=0) && (img_leitura[i-1][j]==PRETO)){
                        img_leitura[i-1][j] = contador+1;
                        preencheu=1;
                    }
                    if((j!=0) && (img_leitura[i][j-1]==PRETO)){
                        img_leitura[i][j-1] = contador + 1;
                        preencheu=1;
                    }
                    if((i!=(img->altura)-1)&&(img_leitura[i+1][j]==PRETO)){
                        img_leitura[i+1][j] = contador + 1;
                        preencheu=1;
                    }
                }
            }
        }
        contador++; // Aumentamos o contador para que continue tentando preencher nos proximos ciclos.
    }
    return contador;
}

/*
*   Funcao que determina se ha alguma saida e se sim, onde.
*/

int encontra_saida (Imagem1C* img, int** img_leitura, int contador, int* altura_final, int* valor_saida){
    int i;
    *valor_saida = contador; // Utilizamos a valor_saida com valor inicial do contador pois tecnicamente o contador ele apresenta o maior valor possivel preenchido na matriz.
    //Optei em deixar valor_saida recebendo contador dentro da funcao pois inicialmente utilizava outra variavel, para economizar memoria retirei ela por ter uma funcao que poderia ser substituida por valor_saida, e como temos varias repeticoes na funcao principal de varias funcoes, mantive essa atribuicao aqui dentro para evitar qualquer problema de esquecer de atribuir fora diversas vezes.
    int find = 0; // Utilizamos uma flag que sera retornada indicando se foi encontrado ou nao uma saida.
    for(i = 0; i < img->altura; i++){
        if(((img_leitura[i][(img->largura)-1])>0) && ((img_leitura[i][((img->largura)-1)])<=(*valor_saida))){ // Caso exista mais de uma saida, escolhemos a que possui o menor custo. Utilizamos o >0 pois a matriz tem -1 e -2 como valores de PRETO e BRANCO, e dessa forma analisamos apenas caminhos que foram preenchidos.
            *altura_final = i; // Passamos por ponteiro qual a altura final atual.
            *valor_saida = img_leitura[i][img->largura - 1]; // Armazenamos tambem qual o valor da saida (quantos passos foram realizados do inicio ao final).
            find = 1; //Flag indica que uma saida foi encontrada.
        }
    } 
    return find; // Retornamos se foi ou nao encontrada uma saida.
}

/*
*   Funcao que determina    
*/

void define_caminho_de_volta (Imagem1C* img, Coordenada** caminho, int** img_leitura, int valor_saida){
    int qt_passos; // Variavel de controle do loop
    for(qt_passos = valor_saida - 1; qt_passos>0; qt_passos--){
        /*
        Essa etapa nao esta a mais legivel, pois estamos trabalhando com as variaveis raizes, e nao uma copia que sera enviada para caminho. 
        Basicamente o que eh feito nesses IFs eh: na encontra_saida, temos o valor_saida representando quantos passos serao utilizados caso alguma saida tenha sido encontrada, o vetor *caminho, ja alocado na funcao encontraCaminho, deve receber as coordenadas x e y de cada passo,
        sendo assim, vericicamos se a casa adjacente tem o valor da atual - 1. 

        Logo abaixo da alocacao do vetor caminho, definimos que caminho[seu tamanho - 1] recebe as coordenadas da saida, ou seja, ja preenchemos sua ultima casa, dessa forma, nessa funcao, basta preenchermos da penultima ate a primeira.
        Sendo assim, inicializamos qt_passos como valor_saida - 1, dessa forma, olharemos apenas os valores abaixos. 
        Por exemplo, supondo que valor_saida tenha dado 50. alocamos 50 casas para *caminho, e guardamos a posicao do numero 50 em (*caminho)[49]
        O mesmo eh feito aqui, inicalizariamos buscando 49 nas casas adjacentes a do 50, e guardariamos na posicao 48 ás coordenadas do valor 49.

        Para obtermos tal objetivo, primieiro comparamos se podemos olhar a casa adjacente, caso esteja em posicaos nas extremidades, impedimos a observacao de fora para evitar qualquer especie de acesso invalido da memoria. Depois, vemos se a posicao adjacente (esquerda, baixo, cima, direita, nessa ordem), possuem exatamente o valor da posicao atual - 1
        caso essa condicao seja valida preenchemos o proximo endereco de (*caminho) com as novas coordenadas observadas.

        Escolhi primeiro olhar a casa da esquerda pois estamos fazendo um caminho da direita a esquerda da imagem, entao achei plausivel olhar de cara a esquerda, em cima ou em baixo foi simplesmente arbitrario, e apos isso verificar a da direita
        */
        if((((*caminho)[qt_passos].x) != 0) && (img_leitura[(*caminho)[qt_passos].y][((*caminho)[qt_passos].x)-1] == ((img_leitura[(*caminho)[qt_passos].y][((*caminho)[qt_passos].x)]) - 1))){ // Verifica esquerda
            (*caminho)[qt_passos-1].y = (*caminho)[qt_passos].y;
            (*caminho)[qt_passos-1].x = ((*caminho)[qt_passos].x)-1;
        
        }else if((((*caminho)[qt_passos].y)!=((img->altura)-1))&&((img_leitura[((*caminho)[qt_passos].y)+1][(*caminho)[qt_passos].x])==((img_leitura[(*caminho)[qt_passos].y][((*caminho)[qt_passos].x)])-1))){ // Verifica em baixo
            (*caminho)[qt_passos-1].y = ((*caminho)[qt_passos].y)+1;
            (*caminho)[qt_passos-1].x = ((*caminho)[qt_passos].x);

        }else if((((*caminho)[qt_passos].y)!=0)&&((img_leitura[((*caminho)[qt_passos].y)-1][(*caminho)[qt_passos].x])==((img_leitura[(*caminho)[qt_passos].y][((*caminho)[qt_passos].x)])-1))){ // Verifica em cima
            (*caminho)[qt_passos-1].y = ((*caminho)[qt_passos].y) - 1;
            (*caminho)[qt_passos-1].x = (*caminho)[qt_passos].x;
            
        }else if((((*caminho)[qt_passos].x)!=((img->largura)-1))&&(img_leitura[((*caminho)[qt_passos].y)][((*caminho)[qt_passos].x)+1])==((img_leitura[(*caminho)[qt_passos].y][((*caminho)[qt_passos].x)])-1)){ // Verifica na direita
            (*caminho)[qt_passos-1].y = (*caminho)[qt_passos].y;
            (*caminho)[qt_passos-1].x = ((*caminho)[qt_passos].x)+1;
        }
    }
}

/*
*   Funcao que desaloca matriz de tipo int.
*/

void desaloca_matriz_int (unsigned int** matriz, int n){
    int i;
    for(i = 0; i < n; i++){
        free(matriz[i]);
    }
    free(matriz);
}

/*
*   Funcao que desaloca matriz de tipo unsigned int.
*/

void desaloca_matriz_u_int (int** matriz, int n){
    int i;
    for(i = 0; i < n; i++){
        free(matriz[i]);
    }
    free(matriz);
}

/*
*   Funcao principal utilizada no projeto.
*/
int encontraCaminho (Imagem1C* img, Coordenada** caminho){
    
    unsigned int media_geral = calculo_media_geral(img); // Calculamos a media geral

    // ==================================================
    //Alocacao das matrizes que serao utilizdas para manipulacao da img.

    int** img_leitura; // Matriz que contera a parte binaria da operacao (matriz custo)
    unsigned int** copia_imagem; // Matriz que contera a imagem super saturada

    //Alocao das matrizes
    img_leitura = aloca_matriz_int(img_leitura, img->altura, img->largura); 
    copia_imagem = aloca_matriz_u_int(copia_imagem, img->altura, img->largura);

    //=====================================================
    //Calculo dos parametros que futuramente serao utilizados para definir quadros brancos e pretos.

    int ajuste; // Variavel que controal o percentual que sera utilizado para separar o branco do preto, seria divida por DIVISOR. 
    unsigned int media_geral_elevada = calculo_parametros_inicializa_leitura(img, img_leitura, copia_imagem, &ajuste, media_geral);
    // Media geral elevada eh a media que sera multiplicada pelo percentual definido por ajuste para descobrirmos o limiar entre preto e branco

    //====================================================
    //Preenchimento da matriz de leitura.

    preenche_imagem_leitura(img, img_leitura, copia_imagem, media_geral_elevada, ajuste);
    //Preenchemos a img_leitura com preto e branco 

    //=========================================================
    //Achamos o ponto de inicio.

    int altura_inicial = encontra_altura_inicial(img);
    img_leitura[altura_inicial][0] = 1;
    //Ecnontramos o ponto inicial e definimos ele como 1, dessa forma difere-se do PRETO e BRANCO e sabemos assim que ali eh o inicio do caminho.

    //============================================================
    //Preenchimento da matriz com seu custo.

    int contador = preenche_leitura(img, img_leitura);
    //Preenchemos a matriz img_leitura com o caminho

    //===============================================================
    //Encontra a localizacao final.
    int altura_final, valor_saida; 
    int encontrou_saida = encontra_saida(img, img_leitura,  contador, &altura_final, &valor_saida);
    // encontrou_saida eh uma flag que quando verdadeira, encontrou, quando falsa, nao encontrou
    while(!encontrou_saida){ // 
        /*
        Caso nao tenha encontrado uma solucao, o codigo entra em um loop que vai aumentando o percentual de 5% em 5% para que haja uma maior adaptabilidade do codigo a varias imagens.
        Claro que nao aparenta ser a maneira mais eficiente de fazer o codigo. O valor 5 eh arbitrario, inicialmente foi feito de 1% em 1%, mas notou-se que era quaser certeza que ele nao pegaria o caminho mais curto da imagem, mas sim o primeiro caminho identificado para o outro lado,
        dessa forma, aumentamos a margem o que diminui bastante a chance disso ocorrer, mas diminui levemente a precisao do codigo em distinguir o preto do branco. Na pior das hipoteses, ou ele vai ter um limiar dividido por 5 que apresenta o mesmo problema de aumentar em 1 em 1, ou teremos uma diferenca de 4%,
        a qual nos testes demonstrou-se bem aceitavel, pois de todas as solucoes encontradas e testadas pela equipe, essa foi a que apresentou o menor score geral.
        */
        ajuste+=5;
        preenche_imagem_leitura(img, img_leitura, copia_imagem, media_geral_elevada, ajuste);
        altura_inicial = encontra_altura_inicial(img);
        img_leitura[altura_inicial][0] = 1;
        contador = preenche_leitura(img, img_leitura);
        encontrou_saida = encontra_saida(img, img_leitura, contador, &altura_final, &valor_saida);
    }
    //==========================================================
    //Alocacao do objeto caminho e preenchimento do final.
    (*caminho) = (Coordenada*) malloc ((valor_saida) * sizeof(Coordenada));
    // Ja fazemos com que o ultimo endereco de caminho consiga a coordenada final, facilitando a funcionalidade da funcao define_caminho_de_volta
    (*caminho)[valor_saida-1].x = img->largura - 1;
    (*caminho)[valor_saida-1].y = altura_final;

    //========================================================
    //Caminho de volta.
    //Funcao que faz o caminho de volta salvando diretamente na struct caminho
    define_caminho_de_volta(img, caminho, img_leitura, valor_saida);

    //==========================================================
    //Desalocacao de memoria.
    desaloca_matriz_int(img_leitura, img->altura);
    desaloca_matriz_u_int (copia_imagem, img->altura);

    return(valor_saida); // Retorno da quantidade de passos
}