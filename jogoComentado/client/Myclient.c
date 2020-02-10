#include "ACore.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200

#define LARGURA 1067
#define ALTURA 800
#define MAX_LOG_SIZE 17
#define LARGURA_TELA 1067
#define ALTURA_TELA 800
#define DISTANCIA_HORIZONTAL_SPRITE 66
#define LARGURA_SPRITE 57
#define ALTURAS_SPRITE 72
#define POS_ALTURA 78
#define PIXEL_PERSO_ALTURA 723




void printConnectScreen(char str[]);
void printChatLog( char chatLog[][MSG_MAX_SIZE]);
void printLobbyText(char str[]);
void printLoginScreen(char str[]);



//essa função vai colocar os obstáculos nas posições pré definidas por nós.
void InicializaMatriz (char matrizGame[][5]);
//essa função vai arrastar os obstáculos da matriz.
int ArrastaMatriz (char matrizGame[][5], int tam);
//apenas coloca as posições na tela em que cada posição da matriz 10x5 em duas matrizes distintas.
void PosicoesMatriz (int matrizIni[], int matrizJog[][5]);

enum conn_ret_t tryConnect(char IP[]) {
  char server_ip[30];
  printf("Please enter the server IP: ");
  //scanf(" %s", server_ip);
  //getchar();
  return connectToServer(IP);
}


void assertConnection(char IP[], char login[]) {

  enum conn_ret_t ans = tryConnect(IP);

  while (ans != SERVER_UP)
  {
    if (ans == SERVER_DOWN) {
      puts("Server is down!");
    } else if (ans == SERVER_FULL) {
      puts("Server is full!");
    } else if (ans == SERVER_CLOSED) {
      puts("Server is closed for new connections!");
    } else {
      puts("Server didn't respond to connection!");
    }
    printf("Want to try again? [Y/n] ");
    int res;
    while (res = tolower(getchar()), res != 'n' && res != 'y' && res != '\n'){
      puts("anh???");
    }
    if (res == 'n') {
      exit(EXIT_SUCCESS);
    }
    ans = tryConnect(IP);
  }
  //char login[LOGIN_MAX_SIZE + 4];
  //printf("Please enter your login (limit = %d): ", LOGIN_MAX_SIZE);
  //scanf(" %[^\n]", login);
  //getchar();
  int len = (int)strlen(login);
  sendMsgToServer(login, len + 1);
}

void runChat() {
  char str_buffer[BUFFER_SIZE], type_buffer[MSG_MAX_SIZE] = {0};
  char msg_history[HIST_MAX_SIZE][MSG_MAX_SIZE] = {{0}};
  int type_pointer = 0;


  while (1) {
    // LER UMA TECLA DIGITADA
    char ch = getch();
    if (ch == '\n') {
      type_buffer[type_pointer++] = '\0';
      int ret = sendMsgToServer((void *)type_buffer, type_pointer);
      if (ret == SERVER_DISCONNECTED) {
        return;
      }
      type_pointer = 0;
      type_buffer[type_pointer] = '\0';
    } else if (ch == 127 || ch == 8) {
      if (type_pointer > 0) {
        --type_pointer;
        type_buffer[type_pointer] = '\0';
      }
    } else if (ch != NO_KEY_PRESSED && type_pointer + 1 < MSG_MAX_SIZE) {
      type_buffer[type_pointer++] = ch;
      type_buffer[type_pointer] = '\0';

    }

    // LER UMA MENSAGEM DO SERVIDOR
    int ret = recvMsgFromServer(str_buffer, DONT_WAIT);
    if (ret == SERVER_DISCONNECTED) {
      return;
    } else if (ret != NO_MESSAGE) {
      int i;
      for (i = 0; i < HIST_MAX_SIZE - 1; ++i) {
        strcpy(msg_history[i], msg_history[i + 1]);
      }
      strcpy(msg_history[HIST_MAX_SIZE - 1], str_buffer);
    }

    // PRINTAR NOVO ESTADO DO CHAT
    system("clear");
    int i;
    for (i = 0; i < HIST_MAX_SIZE; ++i) {
      printf("%s\n", msg_history[i]);
    }
    printf("\nYour message: %s\n", type_buffer);
  }
}



int main()
{
    char matrizGame[155][5];
    int matrizIniAltura[10];
    int matrizLarg[1][5];
    int comecaJogo =0;
    int agoravai =0;
    int tam=155;
    int posXatual=0, contador=0, direcao, contadorSpriteJogador=0;
    int contadorPrintSprite =0;
    int contadorLin=1, contadorCol =0;
    int contadorWin=0;

    InicializaMatriz(matrizGame);
    PosicoesMatriz (matrizIniAltura,matrizLarg);


    //A primeira coisa a ser feita é inicializar os módulos centrais. Caso algum deles falhe, o programa já para por aí.
    if (!coreInit())
        return -1;

    //Agora vamos criar a nossa janela. Largura e Altura em pixels, o título é uma string.
    if (!windowInit(LARGURA, ALTURA, "Allegro Example - Graphical Chat"))
        return -1;

    //Agora inicializamos nosso teclado e mouse, para que a janela responda às entradas deles
    if (!inputInit())
        return -1;

    //Agora inicializamos nossas fontes
    if(!fontInit())
      return -1;
    //E por fim todas as imagens que vamos utilizar no programa.
    if(!loadGraphics()){
        puts("Falha carregando os graficos");
        return -1;
    }



    //para sair do jogo
    bool lobby = true;
    bool connectScreen = true;
    bool loginScreen = true;
    bool game=true;

    //Criamos uma nova estrutura que será enviada e recebida do servidor.
    DADOS pacote;

    //
    JOGADOR player[3];

    MOVIMENTO mov;


    //Exemplo de como colocar uma carga inicial no pacote
    char msg[]={"Oi Servidor!"};
    strcpy(pacote.mensagem, msg);
    pacote.valor = 42;

    //Esse IP irá nos conectar a "nós mesmos", apenas para efeito de testes.
    char ServerIP[30]={"127.0.0.1"};

    //para guardar o log do chat
    char chatLog[MAX_LOG_SIZE][MSG_MAX_SIZE] = {{0}};
    //mensagem pra ser mandada no lobby
    char lobbyMessage[110]={0};
    //para guardar o login
    char loginMsg[BUFFER_SIZE]={0};


    int i;

    //DEsenha a imagem inicial na tela
    al_draw_bitmap(objects,0,0,0);

    //atualiza a tela
    al_flip_display();

    //Segura a dela por 20 segundos
    al_rest(1);

    while(connectScreen)
    {
        startTimer();

        while(!al_is_event_queue_empty(eventsQueue))
        {
            ALLEGRO_EVENT connectEvent;
            al_wait_for_event(eventsQueue, &connectEvent);

            readInput(connectEvent, ServerIP, IP_MAX_SIZE);

            if (connectEvent.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(connectEvent.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ENTER:
                        connectScreen = false;
                        break;
                }
            }

            if(connectEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                connectScreen = false;
                loginScreen = false;
                lobby = false;
                //quit = true;
            }
        }

        printConnectScreen(ServerIP);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
        FPSLimit();
    }

    while(loginScreen)
    {
        startTimer();


        while(!al_is_event_queue_empty(eventsQueue))
        {
            ALLEGRO_EVENT loginEvent;
            al_wait_for_event(eventsQueue, &loginEvent);

            readInput(loginEvent, loginMsg, LOGIN_MAX_SIZE);

            if (loginEvent.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(loginEvent.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ENTER:
                        loginScreen = false;
                        break;
                }
            }

            if(loginEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                loginScreen = false;
                lobby = false;
            }
        }


        printLoginScreen(loginMsg);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
        FPSLimit();
    }

    //Realiza conexão com o server
    assertConnection(ServerIP, loginMsg);
    //recebe o id dele pera estrutura movimento
    recvMsgFromServer (&mov.id, WAIT_FOR_IT);
    fprintf(stderr," %d", mov.id);


    //aqui entra a estrutura

    while(lobby)
    {
        startTimer();

        int rec = recvMsgFromServer(&pacote, DONT_WAIT);
        if (pacote.start==1){

            recvMsgFromServer(player, WAIT_FOR_IT);
            lobby = false;

            break;
        }

        //recebe as mensagens do servidor (jogador se conectou, avisos, etc)
        if(rec != NO_MESSAGE){

                //printf("RECEIVED: %s\n", pacote.mensagem);

                for(i = 0; i < MAX_LOG_SIZE - 1; ++i)
                    strcpy(chatLog[i], chatLog[i+1]);

                strcpy(chatLog[MAX_LOG_SIZE - 1], pacote.mensagem);

                pacote.mensagem[0  ]='\0';
        }

        while(!al_is_event_queue_empty(eventsQueue))
        {
            ALLEGRO_EVENT lobbyEvent;
            al_wait_for_event(eventsQueue, &lobbyEvent);

            readInput(lobbyEvent, lobbyMessage, MSG_MAX_SIZE);



            if (lobbyEvent.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(lobbyEvent.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ENTER:
                        strcpy(pacote.mensagem, lobbyMessage);
                        sendMsgToServer(&pacote, sizeof(DADOS));
                        lobbyMessage[0]='\0';
                        break;
                    case ALLEGRO_KEY_DOWN:
                        pacote.start = 1;
                        sendMsgToServer(&pacote, sizeof(DADOS));
                        pacote.start = 0;
                        break;
                }
            }

            if(lobbyEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){


            }
        }

        //printa a tela e da o flip para aparecer a mensagem nela.
        printLobbyText(lobbyMessage);
        printChatLog(chatLog);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
        FPSLimit();
    }



    //DEsenha a imagem de instrucoes na tela
    al_draw_bitmap(imagem,0,0,0);

    //atualiza a tela
    al_flip_display();

    //Segura a dela por 20 segundos
    al_rest(1.0);

    //desenha imagem no display
    al_draw_bitmap(menuScreen, 0, 0, 0);

    //atualiza tela
    al_flip_display();

       while(game)
    {

        //utilizado para igualar o tempo entre o servidor e o client no arrasto da tela.
        if (recvMsgFromServer(player, DONT_WAIT) !=NO_MESSAGE){

            //se o contador que foi retornado estiver em 6, arraste a matriz.
            if (player[mov.id].contador == 6){

                tam = ArrastaMatriz(matrizGame, tam);
                //resete o movimento para 0, essa parte ta bugada porque mesmo resetando o client, em certas vezes, arrasta 2 vezes em vez de 1, como
                //o servidor.
                player[mov.id].contador = 0;
                fprintf(stderr,"arrastou\n");
                //contador para a quantidade de vezes em que a matriz foi arrastada, se chegar em 69 ele ganha o jogo.
                contadorWin++;

            }


        }
        //checa se a fila de eventos está vazia ou não.
        while(!al_is_event_queue_empty(eventsQueue))
        {
             ALLEGRO_EVENT gameEvent;

            al_wait_for_event(eventsQueue, &gameEvent);

            //
            if (gameEvent.type == ALLEGRO_EVENT_KEY_DOWN){

                switch(gameEvent.keyboard.keycode){

                     case ALLEGRO_KEY_LEFT:

                            mov.direcao = 1;
                            //manda mensagem pro servidor

                            sendMsgToServer(&mov, sizeof(MOVIMENTO));
                            recvMsgFromServer(player,DONT_WAIT);
                            al_flush_event_queue(eventsQueue);

                            //servidor checa as posições
                            //o servidor manda de volta o vetor jogadores
                            //atualiza tela




                        break;

                     case ALLEGRO_KEY_RIGHT:

                            mov.direcao = -1;

                            sendMsgToServer(&mov, sizeof(MOVIMENTO));

                            recvMsgFromServer(player,DONT_WAIT);

                            al_flush_event_queue(eventsQueue);


                         break;



                }




            }
        }



        //aqui é a lógica para printar os obstáculos e os jogadores na tela, ele vai percorrer 10x5 da matriz pra achar as posicoes
        //dos obstáculos e depois printar na posição que está nas matrizes (matrizLarg, matrizIniAltura)
        al_draw_bitmap (menuScreen, 0, 0, 0);
            while (contadorLin<10){

                while (contadorCol<5){

                    if (matrizGame[contadorLin][contadorCol] == 'P'){

                        al_draw_bitmap_region(pedra,0,0,LARGURA_SPRITE, ALTURAS_SPRITE, matrizLarg[0][contadorCol], matrizIniAltura[contadorLin], 0);

                    }else if(matrizGame[contadorLin][contadorCol] == 'X'){

                        al_draw_bitmap_region(foguinho,0,0,LARGURA_SPRITE, ALTURAS_SPRITE, matrizLarg[0][contadorCol], matrizIniAltura[contadorLin], 0);

                    }else if(matrizGame[contadorLin][contadorCol] == 'A'){

                        al_draw_bitmap_region(armadilha,0,0,LARGURA_SPRITE, ALTURAS_SPRITE, matrizLarg[0][contadorCol], matrizIniAltura[contadorLin], 0);

                    }
                    contadorCol++;
                }
                contadorCol=0;
                contadorLin++;

            }
            contadorLin=0;

            //o print dos jogadores, se a posição deles for 6 ele não printa, que é o que é retornado pelo servidor se ele ou outro
            //jogador for atingido pelo obstáculo.

            if (player[0].posicao !=6){

                al_draw_bitmap_region(jogador3,posXatual,POS_ALTURA,LARGURA_SPRITE, ALTURAS_SPRITE, matrizLarg[0][player[0].posicao], PIXEL_PERSO_ALTURA, 0);

            }if (player[1].posicao !=6){

                al_draw_bitmap_region(jogador2,posXatual,POS_ALTURA,LARGURA_SPRITE, ALTURAS_SPRITE, matrizLarg[0][player[1].posicao], PIXEL_PERSO_ALTURA, 0);

            }if (player[2].posicao !=6){


                al_draw_bitmap_region(jogador1,posXatual,POS_ALTURA,LARGURA_SPRITE, ALTURAS_SPRITE, matrizLarg[0][player[2].posicao], PIXEL_PERSO_ALTURA, 0);

            }
            //apenas para mudar a animação da sprite.
            contadorSpriteJogador++;
        if (contadorSpriteJogador==4){
             posXatual += DISTANCIA_HORIZONTAL_SPRITE;

             contadorSpriteJogador=0;
            if (posXatual >=126 && posXatual < 192){
                contadorSpriteJogador=0;
                posXatual += DISTANCIA_HORIZONTAL_SPRITE;

            }
             if (posXatual >= 264){

                 posXatual = 0;
                 contadorSpriteJogador=0;

            }
          }
            contadorPrintSprite=0;
            al_flip_display();





        //se o player[id].tela for retornado com o valor 1 pelo servidor, acabe o jogo e mostre a tela de perder.
        if (player[mov.id].tela == 1){

            game = false;

        }
        //se o contador chegar em 69 arrastamentos da matriz, que é o final do jogo, acabe o jogo e mostre a tela de vitória.
        if (contadorWin == 69){

            game = false;

        }






        FPSLimit();
    }
    //mudança de tela
    if (player[mov.id].tela == 1){

        al_draw_bitmap (perdeu, 0, 0, 0);
        al_flip_display();
        al_rest(10);

    //mudança de tela
    }else if (contadorWin == 69){

        al_draw_bitmap (ganhou, 0, 0, 0);
        al_flip_display();
        al_rest(10);

    }



  allegroEnd();
  return 0;
}



//========================================================FUNCTIONS========================================================
void printConnectScreen(char str[])
{
    al_draw_bitmap(menuScreen,0,0,0);

    al_draw_text(start, al_map_rgb(255, 255, 255), WIDTH / 2, 30, ALLEGRO_ALIGN_CENTRE, "Digite o IP do server:");

    if (strlen(str) > 0)
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "0.0.0.0");
    }
}


void printChatLog( char chatLog[][MSG_MAX_SIZE])
{
    int i;
    int initialY = 60;
    int spacing = al_get_font_ascent(start)+5;

    for(i = 0; i < MAX_LOG_SIZE; ++i)
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), 40, initialY + (i*spacing), ALLEGRO_ALIGN_LEFT, chatLog[i]);
    }


}

void printLobbyText(char str[])
{
    al_draw_bitmap(menuScreen,0,0,0);

    al_draw_text(start, al_map_rgb(255, 255, 255), 20, 30, ALLEGRO_ALIGN_LEFT, "ConverCINha - Beta 1.0");

    if (strlen(str) > 0)
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), 20,
                     (HEIGHT - al_get_font_ascent(start)) - 20,
                     ALLEGRO_ALIGN_LEFT, str);
    }
    else
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), 20,
                     (HEIGHT - al_get_font_ascent(start)) - 20,
                     ALLEGRO_ALIGN_LEFT, "type anything to chat...");
    }
}

void printLoginScreen(char str[])
{
    al_draw_bitmap(menuScreen,0,0,0);

    al_draw_text(start, al_map_rgb(255, 255, 255), WIDTH / 2, 30, ALLEGRO_ALIGN_CENTRE, "Digite o Login desejado: ");

    if (strlen(str) > 0)
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "<login>");
    }
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

void PosicoesMatriz (int matrizIni[], int matrizJog[][5]){

      matrizJog[0][0] = 411;
      matrizJog[0][1] = 471;
      matrizJog[0][2] = 531;
      matrizJog[0][3] = 591;
      matrizJog[0][4] = 651;

      matrizIni[9] = 72;
      matrizIni[8] = 144;
      matrizIni[7] = 216;
      matrizIni[6] = 288;
      matrizIni[5] = 360;
      matrizIni[4] = 432;
      matrizIni[3] = 504;
      matrizIni[2] = 576;
      matrizIni[1] = 642;
      matrizIni[0] = 732;



}

int ArrastaMatriz (char grid[][5], int tam){




    int contadorLin=1, contadorCol=0;
    while (contadorLin < tam){


	    while (contadorCol < 5){

                //troca a posição abaixo pela de cima
                grid [contadorLin-1][contadorCol] = grid [contadorLin][contadorCol];
			    grid [contadorLin][contadorCol] = ' ';


        contadorCol++;

        }
        contadorCol=0;
    contadorLin++;


     }
     tam--;
     return tam;

}




