#include <iostream>

#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/joystick.h"
#include <vector>
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
ALLEGRO_JOYSTICK_STATE joystate;
//ALLEGRO_BITMAP *buffer = create_bitmap(320, 240); // initialize the double buffer
ALLEGRO_BITMAP *bouncer = NULL;

ALLEGRO_BITMAP *personajes[8];

int width = 500, height = 250;
const float FPS = 72;
const int BOUNCER_SIZE = 32;
int seconds = 1, timer2 = 0, velocity = 3, x = 0, y = 0, personaje = 0;
string  edittext;                         // an empty string for editting
string::iterator iter = edittext.begin(); // string iterator
int caret  = 0;                       // tracks the text caret
bool insert = true;                    // true of should text be inserted

int initAllegro()
{
    if(!al_init())
    {
        cout<<"Error: Failed to initialize allegro!\n"<<endl;
        return -1;
    }

    if(!al_init_image_addon())
    {
        cout<<"Error: Failed to initialize al_init_image_addon!"<<endl;
        return -1;
    }

    display = al_create_display(width, height);
    if(!display)
    {
        cout<<"Failed to create display!\n"<<endl;
        return -1;
    }

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout<<"Failed to create event_queue!\n"<<endl;
        al_destroy_display(display);
        return -1;
    }

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
    if(!al_install_joystick())
    {
        cout<<"Joystick FUGGED UP AAAAAA~\n"<<endl;
    }
    if (al_get_num_joysticks() == 0)
    {
        cout<<"No joysticks found.\n"<<endl;
    }


    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();// initialize the ttf (True Type Font) addon
    //cout<<"Llego hasta aqui"<<endl;


    al_register_event_source(event_queue, al_get_display_event_source(display));//registrar eventos del display
    al_register_event_source(event_queue, al_get_timer_event_source(timer));//registrar eventos del timer
    al_register_event_source(event_queue, al_get_keyboard_event_source());//registrar eventos del teclado
    //al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);
    al_init_timeout(&timeout, 0.06);
    //set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
}

int main()
{
    /*al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_REQUIRE);*/
    //al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_VSYNC, 1,ALLEGRO_REQUIRE);

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


    ALLEGRO_COLOR red_color = al_map_rgb(184, 22, 22);
    al_draw_filled_rectangle(3.0, 4.0, 17.0, 16.0, red_color);

    music=al_load_sample("Temple.wav");
    effect=al_load_sample("soundeffect.wav");
    songInstance=al_create_sample_instance(music);
    al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());
    al_play_sample_instance(songInstance);

    /*personajes.push_back(al_load_bitmap("Personaje/down1.png"));
    personajes.push_back(al_load_bitmap("Personaje/down2.png"));
    personajes.push_back(al_load_bitmap("Personaje/right1.png"));
    personajes.push_back(al_load_bitmap("Personaje/right2.png"));
    personajes.push_back(al_load_bitmap("Personaje/up1.png"));
    personajes.push_back(al_load_bitmap("Personaje/up2.png"));
    personajes.push_back(al_load_bitmap("Personaje/left1.png"));
    personajes.push_back(al_load_bitmap("Personaje/left2.png"));*/

    float bouncer_x = width / 2.0 - BOUNCER_SIZE / 2.0;
    float bouncer_y = height / 2.0 - BOUNCER_SIZE / 2.0;

     ALLEGRO_JOYSTICK *butt;
     butt = al_get_joystick(0);
     al_get_joystick_state(butt, &joystate);
    al_register_event_source(event_queue, al_get_joystick_event_source());
    bool buttonDown = false;
    int currentButton;
    while(!done)
    {
        al_draw_bitmap(fondo, 0, 0, ALLEGRO_FLIP_VERTICAL);

        bool get_event = al_wait_for_event_until(event_queue, &ev, &timeout);
        al_get_keyboard_state(&keystate);
        al_get_joystick_state(butt,&joystate);

        if(get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
            return 0;
        }
        else if(ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
        {
            cout<<"current button: "<<ev.joystick.button<<endl;
            buttonDown = true;
            currentButton = ev.joystick.button;
            switch(ev.joystick.button)
            {
            case 0:
                y+=velocity;
                dir = DOWN;
                break;
            case 1:
                x+=velocity;
                dir = RIGHT;
                break;
            case 2:
                x-=velocity;
                dir = LEFT;
                break;
            case 3:
                y-=velocity;
                dir = UP;

            }

        }
        else if(ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP)
        {
            buttonDown = false;
        }

        else if(ev.type == ALLEGRO_EVENT_JOYSTICK_AXIS)
        {
            cout<<"Moving an axis"<<endl;
            x+=ev.joystick.pos*10;
            y+=ev.joystick.pos*10;
        }


        else if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            //al_clear_to_color(al_map_rgb(150,0,255));
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
            else if(al_key_down(&keystate,ALLEGRO_KEY_ESCAPE))
                return 0;
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

    if(buttonDown)
    {
            active = true;
            prevDir = dir;
            switch(currentButton)
            {
            case 0:
                y+=velocity;
                dir = DOWN;
                break;
            case 1:
                x+=velocity;
                dir = RIGHT;
                break;
            case 2:
                x-=velocity;
                dir = LEFT;
                break;
            case 3:
                y-=velocity;
                dir = UP;

            }
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
        al_draw_bitmap(personajes[index], x, y, NULL);
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



    /*do
    {
        while(keypressed())
        {
            int  newkey   = readkey();
            char ASCII    = newkey & 0xff;
            char scancode = newkey >> 8;

            // a character key was pressed; add it to the string
            if(ASCII >= 32 && ASCII <= 126)
            {
                // add the new char, inserting or replacing as need be
                if(insert || iter == edittext.end())
                    iter = edittext.insert(iter, ASCII);
                else
                    edittext.replace(caret, 1, 1, ASCII);
                // increment both the caret and the iterator
                caret++;
                iter++;
                }
            // some other, "special" key was pressed; handle it here
            else
                switch(scancode)
                {
                    case KEY_DEL:
                        if(iter != edittext.end()) iter = edittext.erase(iter);
                       break;

                    case KEY_BACKSPACE:
                        if(iter != edittext.begin())
                          {
                             caret--;
                             iter--;
                             iter = edittext.erase(iter);
                          }
                       break;

                    case KEY_RIGHT:
                        if(iter != edittext.end())   caret++, iter++;
                       break;

                    case KEY_LEFT:
                        if(iter != edittext.begin()) caret--, iter--;
                       break;

                    case KEY_INSERT:
                        if(insert) insert = 0; else insert = 1;
                       break;

                    default:
                        break;
                }
            }

        // clear screen
        clear(buffer);

        // output the string to the screen
        textout(buffer, font, edittext.c_str(), 0, 10, WHITE);

        // output some stats using Allegro's printf functions
          textprintf(buffer, font,  0, 20, WHITE, "length:   %d", edittext.length());
          textprintf(buffer, font,  0, 30, WHITE, "capacity: %d", edittext.capacity());
          textprintf(buffer, font,  0, 40, WHITE, "empty?:   %d", edittext.empty());
          if(insert)
             textout(buffer, font, "Inserting", 0, 50, WHITE);
          else
             textout(buffer, font, "Replacing", 0, 50, WHITE);

          // draw the caret
          vline(buffer, caret * 8, 8, 18, WHITE);

          // blit to screen
          blit(buffer, screen, 0, 0, 0, 0, 320, 240);

        }while(!key[KEY_ESC]); // end of game loop

        // clean up
        destroy_bitmap(buffer);
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

        /*if(ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
              ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {

         bouncer_x = ev.mouse.x;
         bouncer_y = ev.mouse.y;
        }*/


        /*if(personaje==8)
            personaje=0;

        al_draw_bitmap(personajes[personaje], x, y, 100);*/
        //al_flip_display();

    }
    return 0;
}

