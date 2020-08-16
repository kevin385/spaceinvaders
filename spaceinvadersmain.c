#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include<GL/freeglut.h>

unsigned char display_screen=0;  //Type Of Interface To Display

void *font = GLUT_BITMAP_9_BY_15;
void *font1 = GLUT_BITMAP_HELVETICA_10;
void *font2 = GLUT_BITMAP_HELVETICA_12;
void *font3 = GLUT_BITMAP_TIMES_ROMAN_24;

void smooth_input(void);  //Smooth Input By Taking Advantage Of glutSpecialUpFunc
unsigned char flag_left_arrow=0,flag_right_arrow=0,flag_up_arrow=0;  //Flags To Check If Keys Are Pressed
unsigned long int bullets_fired=0;
float shb_delay=0.1;
clock_t shb_time1=0;
double shb_timediff=0.0;

void display_string(int x,int y,void *font_to_use,char *string_to_display)
{
	glRasterPos2i(x,y);
	glutBitmapString(font_to_use,(unsigned char*)string_to_display);
}

void resize(int width,int height)
{
	glutReshapeWindow(500,500);
}

void render(void);
clock_t time1=0,time2=0,time3=0;
double timediff,fps=60.0,delta_time=0.0166666666666667;

void main_menu_screen(void);
unsigned char menu_option=0;

void game_screen(void);

void game_hud(void);

void paused_screen(void);
unsigned char paused_option=0;

void game_over_screen(void);

void keyPress(unsigned char key, int x, int y);
void specialKeyPress(int key, int x, int y);
void SpecialKeysUp(int key, int x, int y);

void star_background();
#define num_stars 501
struct stars
{
	float star_x_pos;
	float star_y_pos;
}star[num_stars];


int red=255,green=0,blue=0;

void shuttle(void);
float shuttle_x_pos=225.0,shuttle_y_pos=10.0;
int shuttle_hitpoints=3;
unsigned char shuttle_i_got_hit=0;

void shuttle_bullet(void);
#define max_bullets 50
struct shuttle_bullet
{
	float x_pos,y_pos;
	unsigned char allocated;
}sh_bullet[max_bullets];

float triangle_area(float x1,float y1,float x2,float y2,float x3,float y3);
unsigned char collision_with_shuttle(float x1,float y1,float x2,float y2,float x3,float y3,float x,float y);
float shuttle_area;

void circular_aliens(void);
#define num_cir_aliens 50
float alien_x_pos,alien_y_pos,r=10.0,angle;
struct circular_aliens
{
	float alien_initial_x_pos,alien_initial_y_pos,alien_min_x_pos,alien_max_x_pos;
	unsigned char alien_hitpoints,i_got_hit,alien_direction,go;
}cir_alien[num_cir_aliens];

void create_ca_alive_list(void);
unsigned char ca_alive_count;
int ca_alive[num_cir_aliens];

void ca_bullet_alloc(void);
unsigned char alien_that_will_shoot=0;
float ca_bullet_alloc_time_delay=0.5;
clock_t ca_time1=0;
double ca_timediff=0.0;

void cir_alien_bullet(void);
//USES max_bullets OF shuttle_bullets
struct cir_alien_bullet
{
	float x_pos,y_pos;
	unsigned char allocated;
}ca_bullet[max_bullets];

void new_round_initialization(void)
{
	int i,j=0;
//SHUTTLE
	red=0,green=255,blue=0;
	shuttle_x_pos=225.0,shuttle_y_pos=10.0;
	shuttle_i_got_hit=0;

//CIRCULAR ALIENS
	for(i=0;i<50;i++)
	{
		cir_alien[i].alien_initial_x_pos=(60.0+((i%10)*26.0)); //Add x amount
		cir_alien[i].alien_min_x_pos=60.0;
		cir_alien[i].alien_max_x_pos=440.0;

		if(i>9 && i%10==0)j++;
		cir_alien[i].alien_initial_y_pos=(440.0-(j*26.0)); //Add x amount
		cir_alien[i].alien_hitpoints=3;
		cir_alien[i].i_got_hit=0;
		cir_alien[i].alien_direction=cir_alien[i].go=1;
	}

//INITIALIZE BULLET SPACES BY DE-ALLOCATION FLAG
	for(i=0;i<max_bullets;i++)
	{
		sh_bullet[i].allocated=0;
		ca_bullet[i].allocated=0;
	}
}

void new_game_initialization(void)
{

//PAUSED SCREEN
	paused_option=0;

//BULLETS FIRED
	bullets_fired=0;

//GAME FACTORS
    shuttle_hitpoints=10;
    ca_bullet_alloc_time_delay=0.5;

	new_round_initialization();
}

int main(int argc,char **argv)
{
	int i;

//STAR BACKGROUND INTIAL POSITION GENERATION
	for(i=0;i<num_stars;i++)
	{
		star[i].star_x_pos=rand()%501;
		star[i].star_y_pos=(int)(i*(501.0/num_stars))%501;
	}

//NEW GAME
	new_game_initialization();

//AREA OF TRIANGULAR SHUTTLE
	shuttle_area=triangle_area(shuttle_x_pos,shuttle_y_pos,shuttle_x_pos+25.0,shuttle_y_pos+21.6506350946,shuttle_x_pos+50.0,shuttle_y_pos);

//OPENGL INITIALIZATIONS
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(500,500);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH))/2-250,(glutGet(GLUT_SCREEN_HEIGHT))/2-250);
	glutCreateWindow("Space Invaders");
	gluOrtho2D(0.0,500.0,0.0,500.0);
	glutDisplayFunc(render);
	glutIdleFunc(render);
	glutReshapeFunc(resize);
	glutSpecialFunc(specialKeyPress);
	glutSpecialUpFunc(SpecialKeysUp);
	glutKeyboardFunc(keyPress);

//GRAPHICS QUALITY
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();

	return 0;
}

void smooth_input(void)
{
//LEFT ARROW
	if(flag_left_arrow==1)
	{
		shuttle_x_pos=shuttle_x_pos-((60.0/fps)*4.0);
		if(shuttle_x_pos<0.0)shuttle_x_pos=0.0;
	}

//RIGHT ARROW
	if(flag_right_arrow==1)
	{
		shuttle_x_pos=shuttle_x_pos+((60.0/fps)*4.0);
		if(shuttle_x_pos>450.0)shuttle_x_pos=450.0;
	}

//UP ARROW
	shb_timediff=(double)(time2-shb_time1)/CLOCKS_PER_SEC;
	if(flag_up_arrow==1 && shb_timediff>=shb_delay)
	{
		shb_time1=clock();
		int i;
		for(i=0;i<max_bullets;i++)
		{
			if(sh_bullet[i].allocated==0)
			{
				sh_bullet[i].x_pos=shuttle_x_pos+25.0;
				sh_bullet[i].y_pos=shuttle_y_pos+21.6506350946;
				sh_bullet[i].allocated=1;
				bullets_fired++;
				break;
			}
		}
	}
}

void render(void)
{
	time2=clock();  //Common For FRAME LIMITER and FPS CALCULATOR
	timediff=(double)(time2-time1)/CLOCKS_PER_SEC;  //Common For FRAME LIMITER and FPS CALCULATOR

	if(timediff>=delta_time)
	{
		time1=clock();

		glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0,0,0.2,1);
		if(display_screen==1)game_screen();
		else if(display_screen==0)main_menu_screen();
		else if(display_screen==2)paused_screen();
		else if(display_screen==3)game_over_screen();

		fps=(1.0/timediff);
		if(fps<10.0)fps=10.0;  //FORCE 10.0 FPS

		glutSwapBuffers();
	}
}

void game_screen(void)
{
	star_background();

	shuttle_bullet();

	create_ca_alive_list();
	ca_bullet_alloc();
	cir_alien_bullet();
	circular_aliens();

	smooth_input();
	shuttle();
	game_hud();
}

void game_hud(void)
{
	char game_hud_string[25];

//INVASION LINE
	glColor3ub(red,green,blue);
	glBegin(GL_LINES);
	glVertex2f(0,50);
	glVertex2f(500,50);
	glEnd();

//HP
	sprintf(game_hud_string,"HP: %d",shuttle_hitpoints);
	if(shuttle_hitpoints>=3)glColor3ub(0,255,0);
	else if(shuttle_hitpoints==2)glColor3ub(255,255,0);
	else glColor3ub(255,0,0);
	display_string(shuttle_x_pos+55,shuttle_y_pos,font1,game_hud_string);
}

void main_menu_screen(void)
{
	char menu_string[2][30];
	strcpy(menu_string[0],"SPACE INVADERS");
	strcpy(menu_string[1],"New Game");
	strcpy(menu_string[2],"Quit");

	star_background();
	display_string(150,400,font3,menu_string[0]);

//NEW GAME
	if(menu_option==0)glColor3ub(255,255,255);
	else glColor3ub(255,0,0);
	display_string(210,250,font,menu_string[1]);

//QUIT
    if(menu_option==1)glColor3ub(255,255,255);
    else glColor3ub(255,0,0);
    display_string(225,205,font,menu_string[2]);


}

void paused_screen(void)
{
	char paused_string[3][50];
	strcpy(paused_string[0],"GAME PAUSED");
	strcpy(paused_string[1],"Resume");
	strcpy(paused_string[2],"Quit To Main Menu");

	star_background();

//GAME PAUSED
	glColor3ub(255,0,0);
	display_string(200,250,font,paused_string[0]);

//RESUME
	if(paused_option==0)glColor3ub(255,255,255);
	else glColor3ub(255,0,0);
	display_string(200,235,font,paused_string[1]);

//QUIT
	if(paused_option==1)glColor3ub(255,255,255);
	else glColor3ub(255,0,0);
	display_string(200,220,font,paused_string[2]);
}

void game_over_screen(void)
{
	char game_over_string[2][25];
	strcpy(game_over_string[0],"GAME OVER!");
	strcpy(game_over_string[1],"<<< Back To Main Menu");

	star_background();

//GAME OVER
	glColor3ub(255,0,0);
	display_string(190,300,font,game_over_string[0]);

//BACK TO MAIN MENU
	glColor3ub(255,255,255);
	display_string(140,250,font,game_over_string[1]);
}

void keyPress(unsigned char key, int x, int y)
{
//GAME SCREEN
	if(display_screen==1)
	{
		switch(key)
		{
		case '1':
             delta_time=0.0666666666666667;  //For 15fps
             break;
        case '2':
        	delta_time=0.0333333333333333;  //For 30fps
             break;
        case '3':
        	delta_time=0.0166666666666667;  //For 60fps
        	break;
        case '4':
        	delta_time=0.0;  //No Frame Limit,Although glutSwapBuffers waits for vSync
        	break;

		case 27 :
            display_screen=2;  //Switch To Paused Screen (Pause Game)
		}
	}

//MAIN MENU
	else if(display_screen==0)
    {
    	switch(key)
   		{
   		case 13:  //Press Enter Key To Select Option
   			if(menu_option==0){new_game_initialization();display_screen=1;}  //New Game -> Game Screen
   			else if(menu_option==1)exit(0);  //Quit
   		}
    }

//PAUSED SCREEN
	else if(display_screen==2)
    {
    	switch(key)
    	{
    	case 13:
    		if(paused_option==0)display_screen=1;  //Paused Screen -> Game Screen (Resume Game)
    		else if(paused_option==1)display_screen=0; //Quit To Main Menu
    	}
    }

//GAME OVER SCREEN
	else if(display_screen==3)
    {
    	switch(key)
    	{
    	case 13:
    		display_screen=0;  //Back To Main Menu
    	}
    }
}

void specialKeyPress(int key, int x, int y)
{
//GAME SCREEN
	if(display_screen==1)
	{
		if(GLUT_KEY_LEFT==key)flag_left_arrow=1;
		else if(GLUT_KEY_RIGHT==key)flag_right_arrow=1;
		else if(GLUT_KEY_UP==key)flag_up_arrow=1;
	}

//MAIN MENU
	else if(display_screen==0)
	{
		switch(key)
		{
		case GLUT_KEY_UP:
			menu_option=(menu_option-1)%2;
			if(menu_option==255)menu_option=1;  //Unsigned char,(0-1)=255
			break;
		case GLUT_KEY_DOWN:
			menu_option=(menu_option+1)%2;
			break;
		}
	}

//PAUSED SCREEN
	else if(display_screen==2)
	{
		switch(key)
		{
		case GLUT_KEY_UP:
			paused_option=(paused_option-1)%2;
			if(paused_option==255)paused_option=1;  //Unsigned char,(0-1)=255
			break;
		case GLUT_KEY_DOWN:
			paused_option=(paused_option+1)%2;
			break;
		}
	}
}

void SpecialKeysUp(int key, int x, int y)
{
	 if(GLUT_KEY_LEFT==key)flag_left_arrow=0;
	 else if(GLUT_KEY_RIGHT==key)flag_right_arrow=0;
	 else if(GLUT_KEY_UP==key)flag_up_arrow=0;
}

void star_background()
{

	int i;
	glColor3ub(128,128,128); //White Color
	glPointSize(2.0);

	glBegin(GL_POINTS);
	for(i=0;i<num_stars;i++)
	{
		glVertex2f(star[i].star_x_pos,star[i].star_y_pos);  //Draw Stars
	}
	glEnd();

}

void shuttle(void)
{
	int i;
	for(i=0;i<max_bullets;i++)
	{
		if(ca_bullet[i].allocated==1 && ca_bullet[i].y_pos<50.0 && collision_with_shuttle(shuttle_x_pos,shuttle_y_pos,shuttle_x_pos+25.0,shuttle_y_pos+21.6506350946,shuttle_x_pos+50.0,shuttle_y_pos,ca_bullet[i].x_pos,ca_bullet[i].y_pos))
		{
			ca_bullet[i].allocated=0;

			shuttle_hitpoints=shuttle_hitpoints-1;
			if(shuttle_hitpoints<1)display_screen=3;  //GAME OVER

			shuttle_i_got_hit=shuttle_i_got_hit+5;
		}
	}

	if(shuttle_i_got_hit>0)
	{
		glColor3ub(255,255,255);
		shuttle_i_got_hit--;
	}
	else glColor3ub(red,green,blue);

	glBegin(GL_TRIANGLES);
	glVertex2f(shuttle_x_pos,shuttle_y_pos);
	glVertex2f(shuttle_x_pos+25.0,shuttle_y_pos+21.6506350946);
	glVertex2f(shuttle_x_pos+50.0,shuttle_y_pos);
	glEnd();
}

void shuttle_bullet(void)
{
	int i;
	glColor3ub(0,255,0);

	for(i=0;i<max_bullets;i++)
	{
		if(sh_bullet[i].allocated==1)
		{
			sh_bullet[i].y_pos=sh_bullet[i].y_pos+((60.0/fps)*5.0);
			if(sh_bullet[i].y_pos<500.0)
			{
				glBegin(GL_LINES);
				glVertex2f(sh_bullet[i].x_pos,sh_bullet[i].y_pos+10);
				glVertex2f(sh_bullet[i].x_pos,sh_bullet[i].y_pos);
				glVertex2f(sh_bullet[i].x_pos+1,sh_bullet[i].y_pos);
				glVertex2f(sh_bullet[i].x_pos+1,sh_bullet[i].y_pos+10);
				glEnd();
			}
			else sh_bullet[i].allocated=0;
		}
	}
}

float triangle_area(float x1,float y1,float x2,float y2,float x3,float y3)
{
	return fabs((x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2))/2.0);
}

unsigned char collision_with_shuttle(float x1,float y1,float x2,float y2,float x3,float y3,float x,float y)
{
	float area1,area2,area3,area_sum,area_diff;

	area1=triangle_area(x,y,x2,y2,x3,y3);
	area2=triangle_area(x1,y1,x,y,x3,y3);
	area3=triangle_area(x1,y1,x2,y2,x,y);

	area_sum=area1+area2+area3;
	area_diff=shuttle_area-area_sum;

	return ((area_diff)>-1.0 && (area_diff)<1.0);
}

void circular_aliens(void)
{
	int i,j;

    for(i=0;i<num_cir_aliens;i++)  //Scan Through All Aliens
    {
    	if(cir_alien[i].alien_hitpoints>0)  //Only Aliens With HP>0
    	{
//COLLISION WITH BULLET
    		for(j=0;j<max_bullets;j++)  //Scan Through All Bullets
    		{
    			if(sh_bullet[j].allocated==1 && (pow(cir_alien[i].alien_initial_x_pos-sh_bullet[j].x_pos,2)+pow(cir_alien[i].alien_initial_y_pos-sh_bullet[j].y_pos,2))<=pow(r,2))  //Thank You Pythagoras :)
    			{
    				cir_alien[i].alien_hitpoints=cir_alien[i].alien_hitpoints-1;
    				cir_alien[i].i_got_hit=cir_alien[i].i_got_hit+5;
    		    	sh_bullet[j].allocated=0;
    		    }
    		}
//DRAW CIRCULAR ALIENS

    		if(cir_alien[i].i_got_hit>0)
    		{
    			glColor3ub(255,255,255);
    			cir_alien[i].i_got_hit--;
    		}
    		else if(cir_alien[i].alien_hitpoints==3)glColor3ub(255,0,0);
    		else if(cir_alien[i].alien_hitpoints==2)glColor3ub(255,255,0);
    		else if(cir_alien[i].alien_hitpoints==1)glColor3ub(0,255,0);

    		glBegin(GL_POLYGON);  //FILLED POLYGONS
    		for(angle=0.0;angle<=6.28318531;angle=angle+0.1) //360 degrees
    		{
    			alien_x_pos=(r*cosf(angle))+cir_alien[i].alien_initial_x_pos;//calculate the x component
    			alien_y_pos=(r*sinf(angle))+cir_alien[i].alien_initial_y_pos;//calculate the y component
    			glVertex2f(alien_x_pos,alien_y_pos);//output vertex
    		}
    		glEnd();
    	}
    }

//ALIEN MOVEMENT
//ALL ALIENS HORIZONTAL MOVEMENT
//LEFT -> RIGHT
	for(i=0;i<num_cir_aliens;i++)
    {
        if(cir_alien[i].alien_initial_x_pos<=cir_alien[i].alien_max_x_pos && cir_alien[i].go==1) //Food Is On The Right:D
        {
        	if(cir_alien[i].alien_direction==1)cir_alien[i].alien_initial_x_pos=cir_alien[i].alien_initial_x_pos+((60.0/fps)*0.8); //Go Get The Food Buddy!
        	if(cir_alien[i].alien_initial_x_pos>=cir_alien[i].alien_max_x_pos)
        	{
        		cir_alien[i].alien_initial_x_pos=cir_alien[i].alien_max_x_pos;
        		cir_alien[i].go=0; //Throw Food To The Left
        		cir_alien[i].alien_direction=!cir_alien[i].alien_direction; //Hamster Changes Direction
        		cir_alien[i].alien_initial_y_pos=cir_alien[i].alien_initial_y_pos-((60.0/fps)*5.0);  //Aliens Hop Down
        		if(cir_alien[i].alien_initial_y_pos<=50.0)display_screen=3; //Invasion Barrier Reached
       		}
       	}
//RIGHT -> LEFT
       	if(cir_alien[i].alien_initial_x_pos>=cir_alien[i].alien_min_x_pos && cir_alien[i].go==0) //Food Is On The Left :D
       	{
       		if(cir_alien[i].alien_direction==0)cir_alien[i].alien_initial_x_pos=cir_alien[i].alien_initial_x_pos-((60.0/fps)*0.8); //Go Get The Food Buddy!
       		if(cir_alien[i].alien_initial_x_pos<=cir_alien[i].alien_min_x_pos)
       		{
       			cir_alien[i].alien_initial_x_pos=cir_alien[i].alien_min_x_pos;
       			cir_alien[i].go=1; //Throw Food To The Right
       			cir_alien[i].alien_direction=!cir_alien[i].alien_direction; //Hamster Changes Direction
       			cir_alien[i].alien_initial_y_pos=cir_alien[i].alien_initial_y_pos-((60.0/fps)*5.0);  //Aliens Hop Down
       			if(cir_alien[i].alien_initial_y_pos<=50.0)display_screen=3;  //Invasion Barrier Reached
    		}
    	}
    }
}

void create_ca_alive_list(void)
{
	int i,j;

//CREATE LIST OF CIRULAR ALIENS THAT ARE ALIVE
	for(i=0,j=0,ca_alive_count=0;i<num_cir_aliens;i++)
	{
		if(cir_alien[i].alien_hitpoints>0)
		{
			ca_alive[j]=i;
			j++;
			ca_alive_count++;
		}
	}
	if(ca_alive_count==0)display_screen=3;  //GAME COMPLETED :)
}

void ca_bullet_alloc(void)
{
	ca_timediff=(double)(time2-ca_time1)/CLOCKS_PER_SEC;

	if(ca_timediff>=ca_bullet_alloc_time_delay && ca_alive_count>0)
	{
		ca_time1=clock();

		alien_that_will_shoot=ca_alive[rand()%ca_alive_count];

		if(ca_bullet[alien_that_will_shoot].allocated==0)
		{
			ca_bullet[alien_that_will_shoot].x_pos=cir_alien[alien_that_will_shoot].alien_initial_x_pos;
			ca_bullet[alien_that_will_shoot].y_pos=cir_alien[alien_that_will_shoot].alien_initial_y_pos-r;
			ca_bullet[alien_that_will_shoot].allocated=1;
		}
	}
}

void cir_alien_bullet(void)
{
	int i;
	glColor3ub(255,255,0);

//BULLET DRAW AND MOVEMENT
	for(i=0;i<max_bullets;i++)
	{
		if(ca_bullet[i].allocated==1)
		{
			ca_bullet[i].y_pos=ca_bullet[i].y_pos-((60.0/fps)*5.0);
			if(ca_bullet[i].y_pos>0.0)
			{
				glBegin(GL_LINES);
				glVertex2f(ca_bullet[i].x_pos,ca_bullet[i].y_pos);
				glVertex2f(ca_bullet[i].x_pos,ca_bullet[i].y_pos-10);
				glVertex2f(ca_bullet[i].x_pos-1,ca_bullet[i].y_pos);
				glVertex2f(ca_bullet[i].x_pos-1,ca_bullet[i].y_pos-10);
				glEnd();
			}
			else ca_bullet[i].allocated=0;
		}
	}
}
