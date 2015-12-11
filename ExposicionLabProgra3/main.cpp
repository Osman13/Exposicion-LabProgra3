#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <vector>
#include <string>
#include <sstream>

#include "Math.h"
#include "rotateTexture.h"

#include <fstream>
#include <string>
#include <sstream>

#define WHITE makecol(255, 255, 255)

using namespace std;

//EVENTOS Y TIMERS
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *fondo = NULL;
ALLEGRO_EVENT ev;
ALLEGRO_TIMEOUT timeout;

ALLEGRO_SAMPLE *music = NULL;
ALLEGRO_SAMPLE *effect = NULL;
ALLEGRO_SAMPLE *game = NULL;
ALLEGRO_SAMPLE_INSTANCE *songInstance;
ALLEGRO_SAMPLE_ID imusic;
ALLEGRO_SAMPLE_ID ieffect;
ALLEGRO_SAMPLE_ID igame;

ALLEGRO_KEYBOARD_STATE keystate;

ALLEGRO_MOUSE_STATE state;

ALLEGRO_BITMAP *bouncer = NULL;

ALLEGRO_BITMAP *personajes[8];

int width = 500, height = 250;
const float FPS = 10;
const int BOUNCER_SIZE = 32;
int seconds = 1, timer2 = 0, velocity = 5, x = 50, y = 50, personaje = 0;
string  edittext;                         // an empty string for editting
string::iterator iter = edittext.begin(); // string iterator
int caret  = 0;                       // tracks the text caret
bool insert = true;
double delta_x, delta_y;
double angle;
double mposx,mposy;
rotateTexture* rt = new rotateTexture();               // true of should text be inserted

int initAllegro()
{
    al_init();

    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    display = al_create_display(width, height);

    event_queue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
        cout<<"Failed to create timer!"<<endl;
    }

    if(!al_install_keyboard())
    {
        cout<<"Failed to initialize the keyboard!"<<endl;
    }

    if(!al_install_audio() || !al_init_acodec_addon() || !al_reserve_samples(2))
    {
        cout<<"Failed to initialize Audio!"<<endl;
    }
    if(!al_install_mouse())
    {
        cout<<"Failed to initialize the mouse!\n"<<endl;
        return -1;
    }

    al_init_primitives_addon();
    cout<<"Llego hasta aqui"<<endl;


    al_register_event_source(event_queue, al_get_display_event_source(display));//registrar eventos del display
    al_register_event_source(event_queue, al_get_timer_event_source(timer));//registrar eventos del timer
    al_register_event_source(event_queue, al_get_keyboard_event_source());//registrar eventos del teclado
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);
    al_init_timeout(&timeout, 0.06);

}

int main()
{
    initAllegro();

    enum Direction { DOWN = 0, LEFT = 2, RIGHT = 4, UP = 6 };
    int dir = DOWN, prevDir, index = 0;
    bool done = false, draw = true, active = false;

    for(int i =0; i < 8; i++)
        {
            std::stringstream str;
            str << "Personaje/" << i + 1 << ".png";
            personajes[i] = al_load_bitmap(str.str().c_str());
        }

    fondo = al_load_bitmap("fondo.jpg");

    ALLEGRO_COLOR white_color = al_map_rgb(0xff,0xff,0xff);
    ALLEGRO_FONT *font = al_load_ttf_font("OpenSans.ttf",24,0 );

    music=al_load_sample("Temple.wav");
    effect=al_load_sample("soundeffect.wav");
    songInstance=al_create_sample_instance(music);
    al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());
    al_play_sample_instance(songInstance);

    float bouncer_x = width / 2.0 - BOUNCER_SIZE / 2.0;
    float bouncer_y = height / 2.0 - BOUNCER_SIZE / 2.0;

    while(!done)
    {
        stringstream convertx;
        convertx << (state.x);
        std::string strx = convertx.str();
        const char* labelsx = strx.c_str();

        stringstream converty;
        converty << (state.y);
        std::string stry = converty.str();
        const char* labelsy = stry.c_str();

        al_draw_bitmap(fondo, 0, 0, 0);

        al_draw_filled_rectangle(200.0, 200.0, 150.0, 150.0, white_color);

        al_draw_text(font, al_map_rgb(255,255,255), 400, 5, ALLEGRO_ALIGN_CENTRE, labelsx);
        al_draw_text(font, al_map_rgb(255,255,255), 450, 5, ALLEGRO_ALIGN_CENTRE, labelsy);

        bool get_event = al_wait_for_event_until(event_queue, &ev, &timeout);

        al_get_keyboard_state(&keystate);
        al_get_mouse_state(&state);

        mposx = state.y;
        mposy = state.x;


        if(get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }


        else if(ev.type == ALLEGRO_EVENT_TIMER)
        {

            timer2++;
            if(timer2==60)
            {
                cout<<seconds++<<endl;
                timer2=0;
            }
            /*if(timer2%30==0)
            {
                personaje++;
            }*/


            active = true;
            prevDir = dir;

            if(state.buttons & 1){
                mposx = state.x;
                mposy = state.y;
                cout<<"x: "<<state.x<<" y: "<<state.y<<endl;
            }

            if(state.buttons & 2){
                cout<<"x: "<<state.x<<" y: "<<state.y<<endl;
            }

            if(al_key_down(&keystate, ALLEGRO_KEY_RIGHT))
            {
                x+=velocity;
                dir = RIGHT;
            }
            else if(al_key_down(&keystate, ALLEGRO_KEY_LEFT))
            {
                x-=velocity;
                dir = LEFT;
            }
            else if(al_key_down(&keystate, ALLEGRO_KEY_UP))
            {
                y-=velocity;
                dir = UP;
            }
            else if(al_key_down(&keystate, ALLEGRO_KEY_DOWN))
            {
                y+=velocity;
                dir = DOWN;
            }
            else
                active = false;

            if(active)
                index++;
            else
            index = dir + 1;

            if(index >= dir + 2)
                index = dir;

            if(dir != prevDir)
                index = dir;

            draw = true;
        }

    if(draw)
    {
        //al_draw_bitmap(personajes[index], 40, 40, NULL);
        rt->rotateBitmap(mposx,mposy,x,y);
        al_draw_rotated_bitmap(personajes[index],0,0,x,y,rt->getAngle(),NULL);
        al_flip_display();
    }

    if(x>=470)
    {
        al_play_sample(effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
        x-=velocity;
    }
    if(x<0)
    {
        al_play_sample(effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
        x+=velocity;
    }
    if(y>=220)
    {
        al_play_sample(effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
        y-=velocity;
    }
    if(y<0)
    {
        al_play_sample(effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
        y+=velocity;
    }


    al_flip_display();

    }
    return 0;
}


