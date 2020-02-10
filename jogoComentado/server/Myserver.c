#include "ACore.h"
#include "server.h"
#include <stdio.h>
#include <string.h>


#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CHAT_CLIENTS 3




void InicializaMatriz (char matrizGame[][5]);
int ArrastaMatriz (char matrizGame[][5], int tam, JOGADOR jogadores[3]);
int ChecaMatriz (char matrizGame[][5], int posicaoJogador, int direcao, int movBuffer, int id, JOGADOR jogadores[3]);

int main()
{

  struct msg_ret_t comunicacao;
  DADOS pacote;
  MOVIMENTO mov;
  //usado para armazenar o valor da variavel movimento do client. Explicação do que ela faz no ACore.h
  int contador=0, contadorArrasta=0;
  //contador é usado pra diminuir a velocidade em que o servidor manda o broadcast, porque ele tava sobrecarregando o
  //client fazendo que o servidor arrastasse a matriz muito mais que o jogo. O contador arrasta é usado para diminuir a
  //velocidade em que a matriz é arrastada pelo servidor.
  char client_names[MAX_CHAT_CLIENTS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];
  int Start=1;
  serverInit(MAX_CHAT_CLIENTS);
  puts("Server is running!!");
  JOGADOR jogadores[3];
  int pessoas=0;
  int posicaoInicial = 0;
  // pessoas é usado para mandar o ID para o jogador
  // posicaoInicial é para guardar tal posicao na estrutura jogadores para o client saber qual posição está tal jogador.
  int game=0;
  int movBuffer =2;
  //apenas para desbugar o chat.
  int tam = 155;
  // diminui o tamanho do arrasta dinamicamente para a execução do programa ficar um pouco mais rápido
  char matrizGame[155][5];
  //a matriz do jogo.
  int contador2=0;
  //um contador para dar um tempo pros jogadores saber o que está na tela.
  jogadores[0].contador = 0;
  jogadores[1].contador = 0;
  jogadores[2].contador = 0;

  jogadores[0].tela = 0;
  jogadores[1].tela = 0;
  jogadores[2].tela = 0;


  InicializaMatriz(matrizGame);

  while (game==0)
  {
    int contador =0;
    int id = acceptConnection();
    if (id != NO_CONNECTION) {
      recvMsgFromClient(client_names[id], id, WAIT_FOR_IT);
      strcpy(str_buffer, client_names[id]);
      strcat(str_buffer, " connected to chat");
      //broadcast(str_buffer, (int)strlen(str_buffer) + 1);
      printf("%s connected id = %d\n", client_names[id], id);
      jogadores[pessoas].id = pessoas;
      jogadores[pessoas].posicao = posicaoInicial;
      fprintf(stderr, "%d\n", jogadores[pessoas].posicao);
         sendMsgToClient(&jogadores[pessoas].id, sizeof(int), pessoas);


      posicaoInicial += 2;
      pessoas++;



    }

    //checa se o maximo de pessoas esta no jogo, se tiver ele espera o jogador 0 apertar para baixo para comecar o jogo.
    if (pessoas == 3){

        if (movBuffer == 2 ){

           strcpy(str_buffer, "vai começar seus puto, primeiro jogador seta para baixo");
            broadcast(str_buffer, (int)strlen(str_buffer) + 1);
           movBuffer = 10;

        }

        recvMsgFromClient (&pacote, 0, WAIT_FOR_IT);

          broadcast(&pacote, sizeof(DADOS));
          broadcast(jogadores, sizeof(JOGADOR)*3);
          if (pacote.start == 1){

             game =1;

          }




      }

    struct msg_ret_t msg_ret = recvMsg(aux_buffer);
    if (msg_ret.status == MESSAGE_OK) {
      sprintf(str_buffer, "%s-%d: %s", client_names[msg_ret.client_id],
              msg_ret.client_id, aux_buffer);
      broadcast(str_buffer, (int)strlen(str_buffer) + 1);
    } else if (msg_ret.status == DISCONNECT_MSG) {
      sprintf(str_buffer, "%s disconnected", client_names[msg_ret.client_id]);
      printf("%s disconnected, id = %d is free\n",
             client_names[msg_ret.client_id], msg_ret.client_id);
      broadcast(str_buffer, (int)strlen(str_buffer) + 1);
    }

  }
  broadcast(jogadores, sizeof(JOGADOR)*3);
  while (game==1){



    //recebe uma mensagem do client com a estrutura de movimento.
    comunicacao = recvMsg(&mov);

      //checa se a mensagem foi recebida.
      if(comunicacao.status == MESSAGE_OK){

        //checa a direcao em que o jogador quer andar, se sim e a posicao dele for maior que 0 (posicao final esquerda), cheque
        //se o movimento é possivel.
        if (mov.direcao == 1 ){
          if (jogadores[mov.id].posicao > 0){
          jogadores[mov.id].posicao = ChecaMatriz (matrizGame, jogadores[mov.id].posicao, mov.direcao, movBuffer, mov.id, jogadores);
          movBuffer = mov.direcao;



          }//mande a informacao para os jogadores
           broadcast (jogadores, sizeof (JOGADOR)*3);



        }//checa se é possivel andar para a direita, se a posicao dele for menor que 4 (posicao final para a direita), cheque
        //se o movimento e possivel
        else if (mov.direcao == -1){

            if (jogadores[mov.id].posicao < 4){
            jogadores[mov.id].posicao = ChecaMatriz (matrizGame, jogadores[mov.id].posicao, mov.direcao, movBuffer, mov.id, jogadores);
            movBuffer = mov.direcao;


          }//mande a informacao para os jogadores
            broadcast (jogadores, sizeof (JOGADOR)*3);
        }

      }



      mov.direcao = 0;
      contador2++;
      //tive que colocar um contador porque tava sobrecarregando o client fazendo ele receber a mensagem nova alguns minutos depois
    if (contador2>4000){

      if (contador == 500){

        //se tiver em 500, mande variavel jogadores para todos os jogadores.


        if (contadorArrasta == 6){

            //essa é a parte de arrastar, se o contador estiver em 3 ele vai arrastar a matriz. depois de arrastar ele vai zerar os contadores.
            tam = ArrastaMatriz(matrizGame, tam, jogadores);
            fprintf(stderr,"arrastou\n");
            //mande a informacao para os jogadores
            broadcast (jogadores, sizeof (JOGADOR)*3);
            contadorArrasta=0;


        }
          //isso vai ser usado para igualar o tempo do arrasto da matriz no servidor e no client, quando o contador dos jogadores estiver em 6,
          //o client vai verificar se o contador está em 6 e arrastar a matriz.
          jogadores[0].contador = 0;
          jogadores[1].contador = 0;
          jogadores[2].contador = 0;
          contador=0;


          broadcast (jogadores, sizeof (JOGADOR)*3);
          jogadores[0].contador++;
          jogadores[1].contador++;
          jogadores[2].contador++;
          contadorArrasta++;
      }


      contador++;

    }


  }
}









  //aqui faz a checagem de quando o jogador anda para os lados.
  int ChecaMatriz (char grid[][5],int posicaoJogador, int direcao, int movBuffer, int id, JOGADOR jogadores[3]){



        //se a direção enviada for esquerda
         if (direcao == 1){

            //se tiver um jogador do lado, faça nada
            if (grid[0][posicaoJogador-direcao] == 'J'|| grid[0][posicaoJogador-direcao] == 'M' || grid[0][posicaoJogador-direcao] == 'N'){


              //se tiver um obstaculo ao lado, mande o jogador para a tela de derrota e retorne 6, para não printar mais sua sprite.
            }else if( grid[0][posicaoJogador-direcao] == 'X' || grid[0][posicaoJogador-direcao] == 'P' || grid[0][posicaoJogador-direcao] == 'A' ){



              jogadores[id].tela = 1;
              grid[0][posicaoJogador] = ' ';
              grid[0][posicaoJogador-1] = ' ';
              return 6;

            //troca a posicao do obstaculo de cima com o de baixo e deleta a informacao acima.
            }else {

              grid[0][posicaoJogador-direcao] = grid[0][posicaoJogador];
              grid[0][posicaoJogador] = ' ';
              posicaoJogador --;


            }



        }

        //mesma coisa que o de cima, mas para a direção direita.
        if (direcao == -1){


            if (grid[0][posicaoJogador-direcao] == 'J'|| grid[0][posicaoJogador-direcao] == 'M' || grid[0][posicaoJogador-direcao] == 'N'){



            }else if( grid[0][posicaoJogador-direcao] == 'X' || grid[0][posicaoJogador-direcao] == 'P' || grid[0][posicaoJogador-direcao] == 'A' ){


              jogadores[id].tela = 1;
              grid[0][posicaoJogador] = ' ';
              grid[0][posicaoJogador+1] = ' ';
              return 6;

            }else {

              grid[0][posicaoJogador-direcao] = grid[0][posicaoJogador];
              grid[0][posicaoJogador] = ' ';
              posicaoJogador ++;


            }



      }

    //retorne a posição nova do jogador.
    return posicaoJogador;

  }


void InicializaMatriz (char matriz[][5]){


     matriz[4][0] = 'P';
    matriz[4][2] = 'X';
    matriz[6][4] = 'P';
    matriz[7][1] = 'X';
    matriz[9][2] = 'P';
    matriz[10][0] = 'X';
    matriz[13][2] = 'P';
    matriz[13][4] = 'X';
    matriz[15][3] = 'P';
    matriz[16][1] = 'X';
    matriz[19][2] = 'X';
    matriz[21][4] = 'P';
    matriz[22][1] = 'X';
    matriz[25][2] = 'X';
    matriz[25][3] = 'P';
    matriz[28][1] = 'X';
    matriz[30][2] = 'X';
    matriz[33][1] = 'P';
    matriz[33][3] = 'X';
    matriz[36][0] = 'X';
    matriz[36][4] = 'P';
    matriz[39][2] = 'X';
    matriz[41][1] = 'A';
    matriz[41][3] = 'P';
    matriz[44][3] = 'X';
    matriz[44][4] = 'X';
    matriz[47][0] = 'X';
    matriz[47][2] = 'X';
    matriz[49][4] = 'P';
    matriz[49][3] = 'P';
    matriz[52][2] = 'X';
    matriz[55][0] = 'A';
    matriz[55][1] = 'P';
    matriz[55][2] = 'P';
    matriz[59][0] = 'X';
    matriz[59][2] = 'X';
    matriz[59][4] = 'P';
    matriz[62][2] = 'X';
    matriz[62][3] = 'A';
    matriz[65][0] = 'X';
    matriz[65][1] = 'P';
    matriz[69][0] = 'X';
    matriz[69][2] = 'P';
    matriz[69][4] = 'X';
    matriz[0][0] = 'J';
    matriz[0][2] = 'M';
    matriz[0][4] = 'N';


}


//arrasta a matriz
 int ArrastaMatriz (char grid[][5], int tam, JOGADOR jogadores[3]){


    int contadorLin=0, contadorCol=0;
    while (contadorLin < tam){


	    while (contadorCol < 5){

            //testa se na posicao atual é um jogador, se for faça nada.
            if (grid[contadorLin][contadorCol]== 'J' || grid[contadorLin][contadorCol]== 'M' || grid[contadorLin][contadorCol]== 'N' ){



            }//testa se a posicao atual é um obstáculo.
            else if ((grid[contadorLin][contadorCol] == 'X'|| grid[contadorLin][contadorCol] == 'A' || grid[contadorLin][contadorCol] == 'P') && (grid[contadorLin-1][contadorCol] == 'J' || grid[contadorLin-1][contadorCol] == 'M' || grid[contadorLin-1][contadorCol] == 'N') ){


              //se for e abaixo for um jogador, mande o para a tela de derrota e faça ele não printar mais na tela.
          if (grid[contadorLin-1][contadorCol] == 'J'){

            jogadores[0].tela = 1;
            jogadores[0].posicao = 6;



          }else if (grid[contadorLin-1][contadorCol] == 'M'){

            jogadores[1].tela = 1;
            jogadores[1].posicao = 6;


          }else if (grid[contadorLin-1][contadorCol] == 'N'){

            jogadores[2].tela = 1;
            jogadores[2].posicao = 6;


          }
			    grid [contadorLin][contadorCol] =  ' ';
			    grid [contadorLin-1][contadorCol] =  ' ';


	         }


             else {
                //se estiver na linha 1 e abaixo for um jogador, faça nada, necessário porque iria trocar o de cima com o jogador.
                if (contadorLin == 1 && (grid[contadorLin-1][contadorCol]=='J' || grid[contadorLin-1][contadorCol]=='M' || grid[contadorLin-1][contadorCol]=='N') ){


                  //troque a posicao de cima com a de baixo.
                }else{
                 grid [contadorLin-1][contadorCol] = grid [contadorLin][contadorCol];
			           grid [contadorLin][contadorCol] = ' ';

                }

             }

        contadorCol++;

        }
        contadorCol=0;
    contadorLin++;


     }
     tam--;
     return tam;

}





  //172.20.4.34
