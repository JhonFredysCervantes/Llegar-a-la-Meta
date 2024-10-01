#include<iostream>
#include<allegro.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#define tamCuadricula 5

using namespace std;

//funciones allegro
void init();
void definit();

void cargarImagenes();
void pintarEscena();
//--------funciones programa--------------------
struct arista;
struct nodo;

struct nodo{
	int x;
	int y;
	int marca;//-2 para posibles opciones, -1 nodo usuario, 0 no hay nada, 1 nodo maquina, 2 nodo llegada
	bool especial;
	nodo *sig;
	arista *sublista;
}*PTR, *p, *q, *g, *k;

struct arista{
	arista *sigAr;
	arista *antAr;
	nodo *sigN;
}*r, *t, *h, *z;

nodo *construirTablero(nodo *&cabeza);
bool crearArista(nodo *&ini, nodo *&fin);
nodo *insertarNodo(nodo *&cabeza, int x, int y);
nodo *buscarNodo(nodo *&cabeza, int x, int y);
nodo *buscarNodoEnTablero();
nodo *buscarNodoSublis(nodo *&nod, int x, int y);
void reiniciarTablero(nodo *&cabeza);
void Aleatorio(nodo *&cabeza);
int menuPrincipal();
void modoDeJuego();
void pintarGanar();
void pintarPerder();
void pintarCuadro(nodo *&grafo);
void movimiento();
void turnoMaquina();
void NormalturnoMaquina();
void Normalmovimiento();

//-----------------------------------------------------
//variables globales

BITMAP *Buffer;
BITMAP *IMGinicio, *IMGcomenzar, *IMGinstrucciones, *IMGganar, *IMGperder, *IMGsalir;
BITMAP *modJuego, *modJuegoF, *modJuegoD;
BITMAP *instrucciones,*cursor, *fondo, *recargar;
BITMAP *sprite, *pokeUsuario, *pokeMaquina;
FONT *lucida;

bool swSelecionDeInicio;
bool turno;//true para usuario, false para maquina
int ganar; // 0 estado normal, 1 ganó el usuario, -1 ganó la maquina, 2 salir del juego
int modoJuego, turnoAlterado; //facil 1, dificil 2
double distanciaActual;
double posibleDistancia;
nodo *nodoMaquina;
nodo *nodoActual;
nodo *nodoFinal;
nodo *nodoAux;


int main(){
    PTR = construirTablero(PTR);
    Aleatorio(PTR);

    init();
    set_window_title("Pikachu Arrives Firts");
    cargarImagenes();
	text_mode(-1);

	bool salir = false;
	swSelecionDeInicio = false; //no se ha seleccionado de donde iniciar
	turno = true;//turno jugador
	ganar  =  0; //no hay ganador
    while(!salir){
        switch(menuPrincipal()){
            case 1:
                modoDeJuego();
                do{
                    pintarEscena();
                }while((!key[KEY_ESC])&&ganar==0);
                pintarCuadro(PTR);//para mostrar paso final
            break;
            case 2:
                draw_sprite(Buffer,instrucciones,0,0);
                textprintf(Buffer,font,300,460,makecol(255,255,255),"Presione cualquier tecla para continuar");
                draw_sprite(screen,Buffer,0,0);
                readkey()>>8;//funcion de allegro para esperar entrada en un formato especifico
            break;
            case 0:
                salir = true;
            break;
        }

        if(ganar==1){
            pintarGanar();
        }else if(ganar==-1){
            pintarPerder();
        }
        reiniciarTablero(PTR);

    }

    definit();
    return 0;
}
END_OF_MAIN()

void init(){
    int depth, res;
    allegro_init();
    depth = desktop_color_depth();
    if (depth==0) depth = 32;
    set_color_depth(depth);
    res= set_gfx_mode(GFX_AUTODETECT_WINDOWED,640, 480, 0, 0);
    if (res != 0){
        allegro_message(allegro_error);
        cout<<"Error al iniciar Alegro";
        exit(-1);
    }
    install_timer();
    install_keyboard();
    install_mouse();

}

void definit(){
clear_keybuf();
}

int menuPrincipal(){
    int opcion=-1;
    //blanquear el buffer
    clear_to_color(Buffer,makecol(0,0,0));
    while(opcion==-1){
        if((mouse_x>=230&&mouse_x<=410)&&(mouse_y>=130&&mouse_y<=190)){
            draw_sprite(Buffer,IMGcomenzar,0,0);
            if(mouse_b&1){
                opcion = 1;
            }
        }else if((mouse_x>=200&&mouse_x<=440)&&(mouse_y>=200&&mouse_y<=260)){
            draw_sprite(Buffer,IMGinstrucciones,0,0);
            if(mouse_b&1){
                opcion = 2;
            }
        }else if((mouse_x>=270&&mouse_x<=380)&&(mouse_y>=280&&mouse_y<=335)){
            draw_sprite(Buffer,IMGsalir,0,0);
            if(mouse_b&1){
                opcion = 0;
            }
        }else{
            draw_sprite(Buffer,IMGinicio,0,0);
        }
        draw_sprite(Buffer,cursor,mouse_x,mouse_y);
        draw_sprite(screen,Buffer,0,0);
    }
    return opcion;

}

void modoDeJuego(){
    modoJuego=-1;
    // para que no seleccione inmediatamente modo el facil
    mouse_b = 0;
    //limpiar el buffer
    clear_to_color(Buffer,makecol(0,0,0));
    while(modoJuego==-1){
        if((mouse_x>=190&&mouse_x<=450)&&(mouse_y>=170&&mouse_y<=230)){
            draw_sprite(Buffer,modJuegoF,0,0);
            if(mouse_b&1){
                modoJuego = 1;
            }
        }else if((mouse_x>=190&&mouse_x<=470)&&(mouse_y>=260&&mouse_y<=310)){
            draw_sprite(Buffer,modJuegoD,0,0);
            if(mouse_b&1){
                modoJuego = 2;
            }
        }else{
            draw_sprite(Buffer,modJuego,0,0);
        }
        draw_sprite(Buffer,cursor,mouse_x,mouse_y);
        draw_sprite(screen,Buffer,0,0);
    }
}

void pintarGanar(){
    textprintf(Buffer,lucida,200,200,makecol(255,0,0),"GANASTE");
    draw_sprite(screen, Buffer,0,0);
    for(int cont = 0; cont<tamCuadricula*4; cont++){//para retardar el texto "ganaste"
        rest(50);
    }
    textprintf(IMGganar,lucida,30,400,makecol(225,225,225),"Presiona cualquier tecla para ir al Menu");
    draw_sprite(screen, IMGganar,0,0);
    readkey()>>8;
}

void pintarPerder(){
    textprintf(Buffer,lucida,200,200,makecol(255,0,0),"PERDISTE");
    draw_sprite(screen, Buffer,0,0);
    for(int cont = 0; cont<tamCuadricula*4; cont++){//para retardar el texto "perder"
        rest(50);
    }
    textprintf(IMGperder,lucida,30,400,makecol(0,0,0),"Presiona cualquier tecla para ir al Menu");
    draw_sprite(screen, IMGperder,0,0);
    readkey()>>8;
}

void pintarEscena(){
    clear_bitmap(Buffer);
    clear_to_color(Buffer,makecol(255,255,255));
    draw_sprite(Buffer,fondo,0,0);
    pintarCuadro(PTR);
    //si ya se selecciono el cuadro, no mostrar mause
    if(!swSelecionDeInicio){
        buscarNodoEnTablero();
        draw_sprite(Buffer,cursor,mouse_x,mouse_y);
    }else{
        draw_sprite(screen,Buffer,0,0);
        if(turno){//si turno es verdadero para jugador
            if(modoJuego==1){
                movimiento();
            }else{
                Normalmovimiento();
            }
            if(turnoAlterado=1){
                draw_sprite(screen,recargar,480,190);
            }
        }else{
            if(modoJuego==1){
                turnoMaquina();
            }else{
                NormalturnoMaquina();
            }
            rest(50);
        }
    }
    draw_sprite(screen,Buffer,0,0);
}

void pintarCuadro(nodo *&grafo){
    q = grafo;
    while(q!=NULL){
        switch(q->marca){
            case -2:
                rectfill(Buffer, q->y*70+2, q->x*70+2, q->y*70+70, q->x*70+70, makecol(100,140,100));
            break;

            case -1:
                rectfill(Buffer, q->y*70+2, q->x*70+2, q->y*70+70, q->x*70+70, makecol(100,180,100));
                draw_sprite(Buffer,pokeUsuario, q->y*70+5, q->x*70+5);
            break;

            case 1:
                rectfill(Buffer, q->y*70+2, q->x*70+2, q->y*70+70, q->x*70+70, makecol(100,180,100));
                draw_sprite(Buffer,pokeMaquina, q->y*70+5, q->x*70+5);
            break;

            case 2:
                rectfill(Buffer, q->y*70+2, q->x*70+2, q->y*70+70, q->x*70+70, makecol(50,50,120));
            break;

            default:
                rectfill(Buffer, q->y*70+2, q->x*70+2, q->y*70+70, q->x*70+70, makecol(20,140,50));
            break;
        }
        q = q->sig;
    }
}

void turnoMaquina(){
    h = nodoMaquina->sublista;
    distanciaActual = sqrt(pow(nodoMaquina->x-nodoFinal->x,2)+pow(nodoMaquina->y-nodoFinal->y,2));
    while((h!=NULL)&&(!turno)){
        posibleDistancia = sqrt(pow(h->sigN->x - nodoFinal->x,2)+ pow(h->sigN->y - nodoFinal->y,2));
        if((h->sigN->especial)&&(h->sigN->marca!=-1)&&(distanciaActual > posibleDistancia)){
            nodoMaquina->marca = 0;
            nodoMaquina = h->sigN;
            nodoMaquina->marca = 1;
            turno = true;
        }
        h = h->sigAr;
    }
    h = nodoMaquina->sublista;
    while((h!=NULL)&&(!turno)){
        posibleDistancia = sqrt(pow(h->sigN->x - nodoFinal->x,2)+ pow(h->sigN->y - nodoFinal->y,2));
        if((h->sigN->marca!=-1)&&(distanciaActual > posibleDistancia)){
            nodoMaquina->marca = 0;
            nodoMaquina = h->sigN;
            nodoMaquina->marca = 1;
            turno = true;
        }
        h = h->sigAr;
    }

    if(nodoMaquina==nodoFinal){
        ganar = -1;
    }
}

void NormalturnoMaquina(){

    h = nodoMaquina->sublista;
    distanciaActual = sqrt(pow(nodoMaquina->x-nodoFinal->x,2)+pow(nodoMaquina->y-nodoFinal->y,2));
    while((h!=NULL)&&(!turno)){
        posibleDistancia = sqrt(pow(h->sigN->x - nodoFinal->x,2)+ pow(h->sigN->y - nodoFinal->y,2));
        if(((nodoMaquina->x==h->sigN->x)&&(nodoMaquina->y!=h->sigN->y))||((nodoMaquina->x!=h->sigN->x)&&(nodoMaquina->y==h->sigN->y))){
            if((h->sigN->especial)&&(h->sigN->marca!=-1)&&(distanciaActual > posibleDistancia)){
                nodoMaquina->marca = 0;
                nodoMaquina = h->sigN;
                nodoMaquina->marca = 1;
                turno = true;
            }
        }
        h = h->sigAr;
    }

    h = nodoMaquina->sublista;
    while((h!=NULL)&&(!turno)){
        posibleDistancia = sqrt(pow(h->sigN->x - nodoFinal->x,2)+ pow(h->sigN->y - nodoFinal->y,2));
        if(((nodoMaquina->x==h->sigN->x)&&(nodoMaquina->y!=h->sigN->y))||((nodoMaquina->x!=h->sigN->x)&&(nodoMaquina->y==h->sigN->y))){
            if((h->sigN->marca!=-1)&&(distanciaActual > posibleDistancia)){
                nodoMaquina->marca = 0;
                nodoMaquina = h->sigN;
                nodoMaquina->marca = 1;
                turno = true;
            }
        }
        h = h->sigAr;
    }

    if(nodoMaquina==nodoFinal){
        ganar = -1;
    }
}

void Normalmovimiento(){
    if(turno){
        nodoAux = NULL;
        t = nodoActual->sublista;
        distanciaActual = sqrt(pow(nodoFinal->x-nodoActual->x,2)+pow(nodoFinal->y-nodoActual->y,2));
        turnoAlterado = 0;
        while((t!=NULL)){
            posibleDistancia = sqrt(pow(nodoFinal->x-t->sigN->x,2)+pow(nodoFinal->y-t->sigN->y,2));
            if(((nodoActual->x==t->sigN->x)&&(nodoActual->y!=t->sigN->y))||((nodoActual->x!=t->sigN->x)&&(nodoActual->y==t->sigN->y))){
                if((t->sigN->marca!=1)&&(distanciaActual>posibleDistancia)){
                    turnoAlterado = 1;
                }
            }
            t = t->sigAr;
        }

        if(turnoAlterado==1){

            switch(readkey()>>8){
                //arriba
                case KEY_UP:
                    nodoAux = buscarNodoSublis(nodoActual, nodoActual->x-1, nodoActual->y);
                break;
                //abajo
                case KEY_DOWN:
                    nodoAux = buscarNodoSublis(nodoActual, nodoActual->x+1, nodoActual->y);
                break;
                //derecha
                case KEY_RIGHT:
                    nodoAux = buscarNodoSublis(nodoActual, nodoActual->x, nodoActual->y+1);
                break;
                //izquierda
                case KEY_LEFT:
                    nodoAux = buscarNodoSublis(nodoActual, nodoActual->x, nodoActual->y-1);
                break;
            }
            if(nodoAux!=NULL){
                distanciaActual = sqrt(pow(nodoFinal->x-nodoActual->x,2)+pow(nodoFinal->y-nodoActual->y,2));
                posibleDistancia = sqrt(pow(nodoFinal->x-nodoAux->x,2)+pow(nodoFinal->y-nodoAux->y,2));
                if((nodoAux->marca!=1)&&(distanciaActual>posibleDistancia)){
                    nodoActual->marca = 0;
                    nodoActual = nodoAux;
                    nodoActual->marca = -1;
                    turno = false;
                }
            }

            if(nodoAux==nodoFinal){
                ganar = 1;
            }
        }else{
            turno = false;
        }
    }
}

void movimiento(){
    nodoAux = NULL;
    switch(readkey()>>8){
        //arriba
        case KEY_UP:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x-1, nodoActual->y);
        break;
        //abajo
        case KEY_DOWN:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x+1, nodoActual->y);
        break;
        //derecha
        case KEY_RIGHT:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x, nodoActual->y+1);
        break;
        //izquierda
        case KEY_LEFT:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x, nodoActual->y-1);
        break;
        //diagonal sup iz
        case KEY_Q:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x-1, nodoActual->y-1);
        break;
        //diagonal inf der
        case KEY_S:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x+1, nodoActual->y+1);
        break;
        //diagonal sup der
        case KEY_W:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x-1, nodoActual->y+1);
        break;
        //diagonal inf iz
        case KEY_A:
            nodoAux = buscarNodoSublis(nodoActual, nodoActual->x+1, nodoActual->y-1);
        break;
    }

    if(nodoAux!=NULL){
        distanciaActual = sqrt(pow(nodoFinal->x-nodoActual->x,2)+pow(nodoFinal->y-nodoActual->y,2));
        posibleDistancia = sqrt(pow(nodoFinal->x-nodoAux->x,2)+pow(nodoFinal->y-nodoAux->y,2));
        if((nodoAux->marca!=1)&&(distanciaActual>posibleDistancia)){
            nodoActual->marca = 0;
            nodoActual = nodoAux;
            nodoActual->marca = -1;
            turno = false;
        }
    }

    if(nodoAux==nodoFinal){
        ganar = 1;
    }
}

void cargarImagenes(){
    Buffer = create_bitmap(640,480);
    IMGinicio = load_bitmap("inicios.bmp",NULL);
    IMGcomenzar = load_bitmap("inicios_c.bmp",NULL);
    IMGinstrucciones = load_bitmap("inicios_instruc.bmp",NULL);
    IMGsalir = load_bitmap("inicios_salir.bmp",NULL);
    IMGganar = load_bitmap("ganar.bmp",NULL);
    IMGperder = load_bitmap("perder.bmp",NULL);
    IMGinstrucciones = load_bitmap("inicios_instruc.bmp",NULL);
    instrucciones = load_bitmap("instrucciones_o.bmp",NULL);
    modJuego = load_bitmap("modo_juego.bmp",NULL);
    modJuegoD = load_bitmap("modo_dificil.bmp",NULL);
    modJuegoF = load_bitmap("modo_facil.bmp",NULL);
    fondo = load_bitmap("fondo2.bmp",NULL);
    cursor = load_bitmap("cursor.bmp",NULL);
    pokeUsuario = load_bitmap("pikachu.bmp",NULL);
    sprite = load_bitmap("sprite_o.bmp",NULL);
    recargar = load_bitmap("recargar_o.bmp",NULL);
    lucida = load_font("lucida.pcx",NULL,NULL);
    srand(time(NULL));
    pokeMaquina = create_sub_bitmap(sprite,(1+rand()%4)*60,(1+rand()%4)*60,60,60);
}

bool crearArista(nodo *&ini, nodo *&fin){
	r = new arista;
	r->sigN = fin;
	r->antAr = NULL;
	if(ini->sublista==NULL){
		ini->sublista = r;
		r->sigAr = NULL;
	}else{
		r->sigAr = ini->sublista;
		ini->sublista->antAr = r;
		ini->sublista = r;
	}
}

nodo *construirTablero(nodo *&cabeza){
	for(int i=1; i<=tamCuadricula; i++){
		for(int j = 1; j<=tamCuadricula; j++){
			cabeza = insertarNodo(cabeza, i, j);
		}
	}

	for(int i=1; i<=tamCuadricula; i++){
		for(int j = 1; j<=tamCuadricula; j++){

            q = buscarNodo(cabeza, i, j);

            if((i%2!=0)&&(j%2!=0)){//para marcar nodos claves
                q->especial = true;
            }
			g = buscarNodo(cabeza,i,j+1);
			if(g!=NULL){
				crearArista(q,g);
				crearArista(g,q);
			}
			g = buscarNodo(cabeza,i+1,j);
			if(g!=NULL){
				crearArista(q,g);
				crearArista(g,q);
			}
			g = buscarNodo(cabeza,i+1,j+1);
			if(g!=NULL){
				crearArista(q,g);
				crearArista(g,q);
			}
			g =  buscarNodo(cabeza,i+1,j-1);
			if(g!=NULL){
				crearArista(q,g);
				crearArista(g,q);
			}
		}
	}

	return cabeza;
}

nodo *insertarNodo(nodo *&cabeza, int x_, int y_){
	k = new nodo;
	k->x = x_;
	k->y = y_;
	k->especial = false;
	if( ((k->x == 1)||(k->x == 5) )&&( (k->y ==1 )||(k->y == 5) )){
        k->marca = -2;
	}else{
        k->marca = 0;
	}
	k->sublista = NULL;
	if(cabeza==NULL){
		cabeza=k;
		k->sig = NULL;
	}else{
		k->sig = cabeza;
		cabeza = k;
	}
	return cabeza;

}

nodo *buscarNodo(nodo *&cabeza, int x, int y){
	p = cabeza;
	while(p!=NULL){
		if((p->x==x)&&(p->y==y)){
			return p;
		}
		p=p->sig;
	}
	return NULL;
}

nodo *buscarNodoSublis(nodo *&nod, int x, int y){
    if(nod!=NULL){
        z = nod->sublista;
        while(z!=NULL){
            if((x == z->sigN->x)&&(y == z->sigN->y)){
                return z->sigN;
            }
            z = z->sigAr;
        }
    }
    return NULL;
}
//tener en cuenta que swSelecionDeInicio me indica si por primera vez se selecciona nodo inicial o de partida
nodo *buscarNodoEnTablero(){
    if((mouse_b&1)&&(turno==true)){
        k = PTR;
        while(k!=NULL){
            //ubicando nodo
            if(((k->y*70<mouse_x)&&(k->y*70+70>mouse_x))&&((k->x*70<mouse_y)&&(k->x*70+70>mouse_y))){
                //validado que sea el primer nodo y esté en una esquina
                if(!swSelecionDeInicio&&(((k->x == 1)||(k->x == 5) )&&( (k->y ==1 )||(k->y == 5) )) ){
                    nodoActual = buscarNodo(PTR,k->x, k->y);
                    //posicionar nodo de la maquina
                    nodoMaquina = nodoActual->sublista->sigN;
                    //posicionando nodo actual y final
                    if((k->x==1)&&(k->y==1)){
                        nodoFinal = buscarNodo(PTR,5,5);
                    }else if((k->x==5)&&(k->y==5)){
                        nodoFinal = buscarNodo(PTR,1,1);
                    }else{
                        nodoFinal = buscarNodo(PTR,nodoActual->y, nodoActual->x);
                    }

                    nodoFinal->marca = 2;
                    nodoActual->marca = -1;
                    nodoMaquina->marca = 1;
                    swSelecionDeInicio = true;

                    //desmarcar opciones
                    p=PTR;
                    while(p!=NULL){
                        if(p->marca==-2){
                            p->marca = 0;
                        }
                        p=p->sig;
                    }
                }
                return k;
            }
            k = k->sig;
        }
    }
	return NULL;
}

void reiniciarTablero(nodo *&cabeza){
    for(int i=1; i<=tamCuadricula; i++){
		for(int j = 1; j<=tamCuadricula; j++){
            q = buscarNodo(cabeza, i, j);
			if(((q->x == 1)||(q->x == 5) )&&( (q->y ==1 )||(q->y == 5) )){
            q->marca = -2;
            }else{
                q->marca = 0;
            }
		}
	}
	swSelecionDeInicio = false;
	turno = true;
	ganar  =  0;
	Aleatorio(PTR);
	masked_blit(sprite,pokeMaquina,(1+rand()%4)*60,(1+rand()%4)*60,0,0,60,60);//cambiar de pokemon
}

void Aleatorio(nodo *&cabeza){
    int i2;
    srand(time(NULL));
    k = cabeza;
    while(k!=NULL){

        for(int i=0; i<tamCuadricula; i++){
            z=k->sublista;
            i2 = (1+rand()%8);
            while((z!=NULL)&&(i2>0)){
                z=z->sigAr;
                i2--;
            }
            if((z!=NULL)&&(z!=k->sublista)){
                t = z->antAr;
                h = z->sigAr;
                if(h!=NULL){
                    h->antAr = t;
                    t->sigAr = h;
                }else{
                    t->sigAr = NULL;
                }
                z->antAr = NULL;
                z->sigAr = k->sublista;
                k->sublista->antAr = z;
                k->sublista = z;
            }
        }

        k=k->sig;
    }
}
