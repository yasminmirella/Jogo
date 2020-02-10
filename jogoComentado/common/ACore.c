#include "ACore.h"


/*
INITIALIZATION OF BASIC CORE MODULES:
--ALLEGRO
--ALLEGRO IMAGE
--ALLEGRO FONTS
--ALLEGRO TTF FONT SUPPORT
--ALLEGRO PRIMITIVES
--EVENT QUEUE
*/
bool coreInit()
{
    //modules and add-ons initialization
	if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return false;
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return false;
    }

    if (!al_init_font_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_font.\n");
        return false;
    }

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_primitives.\n");
        return false;
    }

    eventsQueue = al_create_event_queue();
    if (!eventsQueue)
    {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        return false;
    }

 	

    return true;
}


//FOR INITIALIZING A WINDOW OF WxH SIZE WITH TEXT "title[]"
bool windowInit(int W, int H, char title[])
{
    //window and event queue creation
    main_window = al_create_display(W, H);
    if (!main_window)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }
    al_set_window_title(main_window, title);

    //registra janela na fila de eventos
    al_register_event_source(eventsQueue, al_get_display_event_source(main_window));

    return true;
}


//FOR INITIALIZING MAIN EXTERNAL INPUTS (KEYBOARD, MOUSE AND CURSOR)
bool inputInit()
{
	/*------------------------------MOUSE--------------------------------*/
	//Inicializa Mouse
	if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao inicializar o mouse.\n");
        al_destroy_display(main_window);
        return false;
    }

    // Atribui o cursor padrão do sistema para ser usado
    if (!al_set_system_mouse_cursor(main_window, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        al_destroy_display(main_window);
        return false;
    }
    /*------------------------------MOUSE--------------------------------*/

    /*------------------------------KEYBOARD------------------------------*/
    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    /*------------------------------KEYBOARD------------------------------*/


    //Registra mouse e teclado na fila de eventos
    al_register_event_source(eventsQueue, al_get_mouse_event_source());
    al_register_event_source(eventsQueue, al_get_keyboard_event_source());

    return true;
}


//FOR READING KEYBOARD INPUT WITH MAX SIZE = LIMIT AND SAVING AT STR[]
void readInput(ALLEGRO_EVENT event, char str[], int limit)
{
    if (event.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) <= limit)
        {
            char temp[] = {event.keyboard.unichar, '\0'};
            if (event.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= '!' &&
                     event.keyboard.unichar <= '?')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'A' &&
                     event.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'a' &&
                     event.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
    }
}


//FPS CONTROL (FPS IS DEFINED IN ACORE.H)
void startTimer()
{
    startingTime = al_get_time();
}

double getTimer()
{
    return al_get_time() - startingTime;
}

void FPSLimit()
{
    if (getTimer() < 1.0/FPS)
    {
        al_rest((1.0 / FPS) - getTimer());
    }
}


//FOR DEALLOCATING ALL ALLEGRO STUFF
void allegroEnd()
{
    al_destroy_display(main_window);
    al_destroy_event_queue(eventsQueue);
}


//MODIFY THIS TO LOAD YOUR OWN FONTS (FONT POINTERS ARE DEFINED AT ACORE.H)
bool fontInit()
{
    /*------------------------------FONTE--------------------------------*/
    start = al_load_font("resources/pressStart.ttf", 16, 0);
    if (!start)
    {
        fprintf(stderr, "Falha ao carregar \"resources/pressStart.ttf\".\n");
        return false;
    }

    ubuntu = al_load_font("resources/Ubuntu-R.ttf", 32, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Ubuntu-R.ttf\".\n");
        return false;
    }

    return true;
}


//MODIFY THIS TO LOAD YOUR OWN GRAPHICS (BITMAP POINTERS ARE DEFINED AT ACORE.H)
bool loadGraphics()
{
    menuScreen = al_load_bitmap("resources/jogo1.png");
    if (!menuScreen)
    {
        fprintf(stderr, "nao foi possivel carregar /jogo1.png\n");
        return false;
    }
    objects = al_load_bitmap("resources/menu.png");
    if (!objects){
        fprintf(stderr, "nao foi possivel carregar resources/menu.png\n");
        return false;
    }

   imagem = al_load_bitmap("resources/rsrs.png");
   if(!imagem)
   {
       fprintf(stderr, "nao foi possivel carregar resources/rsrs.png\n");
       return false;
   }

   jogador1 = al_load_bitmap("resources/anaoNovo.png");
   if(!jogador1)
   {
       fprintf(stderr, "nao foi possivel carregar resources/anaoNovo.png\n");
       return false;
   }
    
    jogador2 = al_load_bitmap("resources/elfoNovo.png");
   if(!jogador2)
   {
       fprintf(stderr, "nao foi possivel carregar resources/elfoNovo.png\n");
       return false;
   }

    jogador3 = al_load_bitmap("resources/hobbitNovo.png");
   if(!jogador3)
   {
       fprintf(stderr, "nao foi possivel carregar resources/hobbitNovo.png\n");
       return false;
   }
    
    foguinho = al_load_bitmap("resources/inimigo.png");
   if(!foguinho)
   {
       fprintf(stderr, "nao foi possivel carregar resources/inimigo.png\n");
       return false;
   }

    perdeu = al_load_bitmap("resources/youLose.png");
   if(!perdeu)
   {
       fprintf(stderr, "nao foi possivel carregar resources/youLose.png\n");
       return false;
   }

    ganhou = al_load_bitmap("resources/youWin.png");
   if(!ganhou)
   {
       fprintf(stderr, "nao foi possivel carregar resources/youWin.png\n");
       return false;
   }

    sacrificou = al_load_bitmap("resources/youHave.png");
   if(!sacrificou)
   {
       fprintf(stderr, "nao foi possivel carregar resources/youHave.png\n");
       return false;
   }

       pedra = al_load_bitmap("resources/pedra.png");
   if(!pedra)
   {
       fprintf(stderr, "nao foi possivel carregar resources/pedra.png\n");
       return false;
   }

       armadilha = al_load_bitmap("resources/armadilha.png");
   if(!armadilha)
   {
       fprintf(stderr, "nao foi possivel carregar resources/armadilha.png\n");
       return false;
   }
   
   return true;
}