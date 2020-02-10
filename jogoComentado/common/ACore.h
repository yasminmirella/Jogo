#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60
#define WIDTH  640
#define HEIGHT 480
#define IP_MAX_SIZE 100

double startingTime;

ALLEGRO_DISPLAY *main_window;
ALLEGRO_EVENT_QUEUE *eventsQueue;

//========================
//FONT AND BITMAP POINTERS
ALLEGRO_FONT *ubuntu;
ALLEGRO_FONT *start;

ALLEGRO_BITMAP *objects;
ALLEGRO_BITMAP *menuScreen;
ALLEGRO_BITMAP *imagem;
ALLEGRO_BITMAP *menu;
ALLEGRO_BITMAP *jogador1;
ALLEGRO_BITMAP *jogador2;
ALLEGRO_BITMAP *jogador3;
ALLEGRO_BITMAP *foguinho;
ALLEGRO_BITMAP *pedra;
ALLEGRO_BITMAP *armadilha;
ALLEGRO_BITMAP *perdeu;
ALLEGRO_BITMAP *ganhou;
ALLEGRO_BITMAP *sacrificou;
//========================
//========================

//EXAMPLE STRUCT
typedef struct DADOS
{
    char mensagem[100];
    int start;
    int valor;

}DADOS;

//faz o que é descrito nas variaveis, guarda tais valores.
typedef struct JOGADOR
{
    int id;
    int posicao;
    int direcao;
    int contador;// usado para saber se o servidor arrastou a matriz ou não, para arrastar também no client.
    int tela;//tela é apenas a tela que vai ser mostrada para o jogador em tal situação.

}JOGADOR;

//estrutura para mandar para o servidor saber quem é o jogador que fez o movimento e a direção para que ele está indo.
typedef struct MOVIMENTO
{
    int id;
    int direcao;

}MOVIMENTO;


//MAIN ALLEGRO FUNCTIONS
bool coreInit();
bool windowInit(int W, int H, char title[]);
bool inputInit();
void allegroEnd();

//FPS CONTROL FUNCTIONS
void startTimer();
double getTimer();
void FPSLimit();

//RESOURCE LOADING FUNCTIONS
bool loadGraphics();
bool fontInit();

//INPUT READING FUNCTION
void readInput(ALLEGRO_EVENT event, char str[], int limit);


#endif
