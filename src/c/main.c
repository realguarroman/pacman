

#include <pebble.h>




#include <pebble-simple-health/pebble-simple-health.h>


#define KEY_CHARACTER 0



#define CHARACTER_PACMAN 0
#define CHARACTER_MSPACMAN 1


#ifdef PBL_PLATFORM_CHALK
  #define OFFSET_X_CHALK 18
  #define HALLWAY_Y 16 + 6
	#define HALLWAY2_Y 111 + 6
  #define BIGDOT_X 120 + OFFSET_X_CHALK
	#define PACMAN_START 40 
	#define PACMAN_END 110 
  #define PACMAN_END2 15 
	#define GHOST_START -10 
	#define GHOST_END 70 
  #define GHOST_END2 5
	#define POINTS_PACMAN_X 30 + OFFSET_X_CHALK + 25

	

	#define PACMAN_FRUIT_START -20 
	#define PACMAN_FRUIT_END 180 +10 
  #define DISTANCE_FRUIT 1000
  #define TOTAL_FRUITS 8

	#define TIME_X 14 + OFFSET_X_CHALK
	#define TIME_Y 53 + 6
  #define BATTERY_X 67 
	#define BATTERY_Y 155 + 6 + 2

	#define FRUIT_X 56 + OFFSET_X_CHALK
  #define POINTS_FRUIT_X 56 + OFFSET_X_CHALK
	
	#define DATE_X 62
	#define DATE_Y 190

	#define END_X 0 + OFFSET_X_CHALK
	#define END_Y 108 + 6
#endif

#ifdef PBL_PLATFORM_BASALT
  #define HALLWAY_Y 16
	#define HALLWAY2_Y 111
  #define BIGDOT_X 120
	#define PACMAN_START 40
	#define PACMAN_END 110
  #define PACMAN_END2 15
	#define GHOST_START -10
	#define GHOST_END 70
  #define GHOST_END2 5
  #define POINTS_PACMAN_X 30

	#define PACMAN_FRUIT_START -20
	#define PACMAN_FRUIT_END 144+10
	#define DISTANCE_FRUIT 1000
  #define TOTAL_FRUITS 8

	#define TIME_X 14
	#define TIME_Y 53
  #define BATTERY_X 4
	#define BATTERY_Y 155

	#define FRUIT_X 56
  #define POINTS_FRUIT_X 56

	#define DATE_X 62
	#define DATE_Y 150

	#define END_X 0
	#define END_Y 108 
#endif

static Window *s_main_window;

static int character = CHARACTER_PACMAN;

//int steps = 0;

static GBitmap *s_background_bitmap, *s_ghost_bitmap, *s_pacman_bitmap, *s_bigdot_bitmap, *s_battery_bitmap, *s_points_pacman_bitmap, *s_fruit_bitmap, *s_points_fruit_bitmap;
static BitmapLayer *s_background_layer, *s_ghost_layer, *s_pacman_layer, *s_bigdot_layer, *s_battery_layer, *s_points_pacman_layer, *s_fruit_layer, *s_points_fruit_layer;

static TextLayer *s_time_layer, *s_date_layer, *s_end_layer;
static GFont s_time_font, s_date_font, s_end_font;

const int GHOST_LEFT[4] = {
  RESOURCE_ID_BLINKY_LEFT, RESOURCE_ID_PINKY_LEFT, RESOURCE_ID_INKY_LEFT, RESOURCE_ID_CLYDE_LEFT
};


const int GHOST_RIGHT[4] = {
  RESOURCE_ID_BLINKY_RIGHT, RESOURCE_ID_PINKY_RIGHT, RESOURCE_ID_INKY_RIGHT, RESOURCE_ID_CLYDE_RIGHT
};

const int PACMAN_RIGHT[2] = {
  RESOURCE_ID_PACMAN_RIGHT, RESOURCE_ID_MSPACMAN_RIGHT
};


const int PACMAN_LEFT[2] = {
  RESOURCE_ID_PACMAN_LEFT, RESOURCE_ID_MSPACMAN_LEFT
};


static void update_battery() {  /* mejor será suscribirse a un servicio, esto se hace cada minuto y no mola */
	
	static uint8_t battery_level;
	BatteryChargeState initial = battery_state_service_peek();
  battery_level = initial.charge_percent;	
	
	gbitmap_destroy(s_battery_bitmap);
  
	
	if (battery_level == 0 || (battery_level == 10))  {
		s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BAT0);
	} else if ((battery_level == 20) || (battery_level == 30) || (battery_level == 40)) {
  	s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BAT1);
	} else if ((battery_level == 50) || (battery_level == 60) || (battery_level == 70)) {
  	s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BAT2);
	} else if ((battery_level == 80) || (battery_level == 90) || (battery_level == 100)) {
  	s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BAT3);
	} 
		
	bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
}

static void update_time() {

  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	static char buffer[]= "00:00";	

	static char bufferdate[]= "25 MAR";
	strftime(bufferdate, sizeof("25 MAR"), "%e %b", tick_time);
	text_layer_set_text(s_date_layer,bufferdate);
	
	
	 // Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
	//APP_LOG(APP_LOG_LEVEL_INFO, "Time %s",buffer);
	

		
	text_layer_set_text(s_time_layer, buffer);


	
	int ghost = (tick_time->tm_mday) % 4;
	
	
	int steps = health_get_metric_sum(HealthMetricStepCount);
	int goal = health_get_metric_goal(HealthMetricStepCount);
	
	
	
//  int goal = 10000;
//	steps=steps + 200;
	

	
	
	if (steps < (goal/2)) {
		
		//pacman va hacia la derecha huyendo de Inky

		//mostramos bigdot
		bitmap_layer_set_bitmap(s_bigdot_layer, s_bigdot_bitmap);
		
		//no mostramos end
		text_layer_set_text(s_end_layer, "");

		//mostramos la fruta (la cereza)
		gbitmap_destroy(s_fruit_bitmap);
		s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CEREZAS);
		layer_set_frame(bitmap_layer_get_layer(s_fruit_layer),GRect(FRUIT_X, HALLWAY2_Y, 29, 29));
		bitmap_layer_set_compositing_mode(s_fruit_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_fruit_layer, s_fruit_bitmap);

			
	
		
		
		float factor = (float)steps / ((float)goal / 2);
		//mostramos a pacman
		gbitmap_destroy(s_pacman_bitmap);
		int pos_x_pacman = PACMAN_START + factor * (PACMAN_END - PACMAN_START);	
		#ifdef PBL_PLATFORM_CHALK
			pos_x_pacman = pos_x_pacman + OFFSET_X_CHALK;
		#endif
		s_pacman_bitmap = gbitmap_create_with_resource(PACMAN_RIGHT[character]);
		layer_set_frame(bitmap_layer_get_layer(s_pacman_layer),GRect(pos_x_pacman, HALLWAY_Y, 28, 28));
		bitmap_layer_set_compositing_mode(s_pacman_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_pacman_layer, s_pacman_bitmap);
		
		//mostramos a inky
		gbitmap_destroy(s_ghost_bitmap);
		int pos_x_ghost = GHOST_START + factor * (GHOST_END - GHOST_START);
		#ifdef PBL_PLATFORM_CHALK
			pos_x_ghost = pos_x_ghost + OFFSET_X_CHALK;
		#endif
		s_ghost_bitmap = gbitmap_create_with_resource(GHOST_RIGHT[ghost]);
		layer_set_frame(bitmap_layer_get_layer(s_ghost_layer),GRect(pos_x_ghost, HALLWAY_Y, 28, 28));
		bitmap_layer_set_compositing_mode(s_ghost_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_ghost_layer, s_ghost_bitmap);
		
		
		
		
		
	}
	
	
	if ((steps >= (goal/2)) && (steps <= goal)) {
		
		//pacman va hacia la izquierda persiguiendo a Inky asustado
		
	//	gbitmap_destroy(s_pacman_bitmap);
	//  gbitmap_destroy(s_bigdot_bitmap);
	//	gbitmap_destroy(s_ghost_bitmap);
		
	//	int steps=steps-goal/2;
		
		
		//no mostramos bigdot
		bitmap_layer_set_bitmap(s_bigdot_layer, NULL);
		
		//no mostramos end
		text_layer_set_text(s_end_layer, "");
		
		//mostramos la fruta (la cereza)
		gbitmap_destroy(s_fruit_bitmap);
		s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CEREZAS);
		layer_set_frame(bitmap_layer_get_layer(s_fruit_layer),GRect(FRUIT_X, HALLWAY2_Y, 29, 29));
		bitmap_layer_set_compositing_mode(s_fruit_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_fruit_layer, s_fruit_bitmap);
		
		float factor = (float)(steps-goal/2) / ((float)goal / 2);	
		//mostramos a pacman
		gbitmap_destroy(s_pacman_bitmap);
		int pos_x_pacman = PACMAN_END + factor * (PACMAN_END2 - PACMAN_END);
		#ifdef PBL_PLATFORM_CHALK
			pos_x_pacman = pos_x_pacman + OFFSET_X_CHALK;
		#endif
		s_pacman_bitmap = gbitmap_create_with_resource(PACMAN_LEFT[character]);
		layer_set_frame(bitmap_layer_get_layer(s_pacman_layer),GRect(pos_x_pacman, HALLWAY_Y, 28, 28));
		bitmap_layer_set_compositing_mode(s_pacman_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_pacman_layer, s_pacman_bitmap);
		
		//mostramos a inky
		gbitmap_destroy(s_ghost_bitmap);
		int pos_x_ghost = GHOST_END + factor * (GHOST_END2 - GHOST_END);
		#ifdef PBL_PLATFORM_CHALK
			pos_x_ghost = pos_x_ghost + OFFSET_X_CHALK;
		#endif
		s_ghost_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SCARED);
		layer_set_frame(bitmap_layer_get_layer(s_ghost_layer),GRect(pos_x_ghost, HALLWAY_Y, 28, 28));
		bitmap_layer_set_compositing_mode(s_ghost_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_ghost_layer, s_ghost_bitmap);
		
	
	}
	
	
	if ((steps > (goal)) && (steps <= (goal+200))){  //sacamos la puntuación
		//pacman se ha comido a Inky, hacemos desaparecer a los dos y mostramos la puntuación
		//no mostramos big dot
		bitmap_layer_set_bitmap(s_bigdot_layer, NULL);
		//mostramos la puntuación
		bitmap_layer_set_bitmap(s_points_pacman_layer, s_points_pacman_bitmap);
		//no mostramos a pacman
		bitmap_layer_set_bitmap(s_pacman_layer, NULL);
		//no mostramos a Inky
		bitmap_layer_set_bitmap(s_ghost_layer, NULL);
		//mostramos la fruta (la cereza)
		gbitmap_destroy(s_fruit_bitmap);
		s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CEREZAS);
		layer_set_frame(bitmap_layer_get_layer(s_fruit_layer),GRect(FRUIT_X, HALLWAY2_Y, 29, 29));
		bitmap_layer_set_compositing_mode(s_fruit_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_fruit_layer, s_fruit_bitmap);
		
	} else {
		//no mostramos la puntuación
		bitmap_layer_set_bitmap(s_points_pacman_layer, NULL);
	}
	
  if ((steps > (goal)) && (steps <= (goal + DISTANCE_FRUIT * TOTAL_FRUITS))) {  //a comer fruta
		//pacman va por el pasillo inferior a comerse la fruta
		
		//no mostramos bigdot
		bitmap_layer_set_bitmap(s_bigdot_layer, NULL);
		
		//no mostramos a Inky
		bitmap_layer_set_bitmap(s_ghost_layer, NULL);
		
		//no mostramos end
		text_layer_set_text(s_end_layer, "");
		
		
		//mostramos a pacman
		gbitmap_destroy(s_pacman_bitmap);
		float factor = (float)((steps-goal) % DISTANCE_FRUIT) / DISTANCE_FRUIT;
		int pos_x_pacman = PACMAN_FRUIT_START + factor * (PACMAN_FRUIT_END - PACMAN_FRUIT_START);
		s_pacman_bitmap = gbitmap_create_with_resource(PACMAN_RIGHT[character]);
		layer_set_frame(bitmap_layer_get_layer(s_pacman_layer),GRect(pos_x_pacman, HALLWAY2_Y, 28, 28));
		bitmap_layer_set_compositing_mode(s_pacman_layer, GCompOpSet);
		bitmap_layer_set_bitmap(s_pacman_layer, s_pacman_bitmap);
		
		//mostramos la fruta
		int fruit_number = (steps-goal) / DISTANCE_FRUIT;
		
		bitmap_layer_set_bitmap(s_fruit_layer, NULL);
		if (((steps-goal) % DISTANCE_FRUIT) < (DISTANCE_FRUIT/2)) { //pintar fruta
			//int fruit_number = (steps-goal) / DISTANCE_FRUIT;
			gbitmap_destroy(s_fruit_bitmap);	
			switch(fruit_number) {
				case 0:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CEREZAS); 
				break;				
				case 1:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FRESA); 
				break;		
				case 2:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MELOCOTON); 
				break;
				case 3:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MANZANA); 
				break;
				case 4:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHIRIMOYA); 
				break;
				case 5:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GALAXIAN); 
				break;
				case 6:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMPANA); 
				break;
				case 7:
					s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LLAVE); 
				break;			
			}
			bitmap_layer_set_bitmap(s_fruit_layer, s_fruit_bitmap);
		}	
		
		
		//mostramos la puntuación
		if ((((steps-goal) % DISTANCE_FRUIT) > (DISTANCE_FRUIT/2)) && (((steps-goal) % DISTANCE_FRUIT) < ((DISTANCE_FRUIT/2)+300))) { 
			gbitmap_destroy(s_points_fruit_bitmap);
			switch(fruit_number) {
				case 0:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_100); 
				break;				
				case 1:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_300); 
				break;		
				case 2:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_500); 
				break;
				case 3:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_700); 
				break;
				case 4:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_1000); 
				break;
				case 5:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_2000); 
				break;
				case 6:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_3000); 
				break;
				case 7:
					s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_5000); 
				break;			
			}
			bitmap_layer_set_bitmap(s_points_fruit_layer, s_points_fruit_bitmap);
		}	else {
			//no mostramos la puntuación
			bitmap_layer_set_bitmap(s_points_fruit_layer, NULL);
		}
		
		
	}
	
	if (steps > (goal + DISTANCE_FRUIT * TOTAL_FRUITS)) {
		//FINAL
		//no mostramos big dot
		bitmap_layer_set_bitmap(s_bigdot_layer, NULL);
		//no mostramos a pacman
		bitmap_layer_set_bitmap(s_pacman_layer, NULL);
		//no mostramos a Inky
		bitmap_layer_set_bitmap(s_ghost_layer, NULL);
		//no mostramos la fruta 
		bitmap_layer_set_bitmap(s_fruit_layer, NULL);
		//monstramos end 
		text_layer_set_text(s_end_layer, "YOU WIN!");

		
		
	}
	
	
	int minutes = tick_time->tm_min;  //batería cada hora
	
	if(minutes == 0) {	
		update_battery();
	}
	
//	APP_LOG(APP_LOG_LEVEL_INFO, "steps %d",steps);
//	APP_LOG(APP_LOG_LEVEL_INFO, "goal %d",goal);

}
		



void process_tuple(Tuple *t)
{
	
 
 //Get key
	//APP_LOG(APP_LOG_LEVEL_INFO, "viendo tupla");
	
	

  int key = t->key;
	
  //Decide what to do
  switch(key) {	
		  	
			case KEY_CHARACTER:
			   
			
				if(strcmp(t->value->cstring, "p") == 0)
				{
					//Set and save
					character = CHARACTER_PACMAN;
					persist_write_int(KEY_CHARACTER, CHARACTER_PACMAN);
					update_time();
				}
				else if(strcmp(t->value->cstring, "m") == 0)
				{
					//Set and save 
				
					character = CHARACTER_MSPACMAN;
					persist_write_int(KEY_CHARACTER, CHARACTER_MSPACMAN);
					update_time();
				}
				
      break;
		
	}		
				
			

}


static void in_received_handler(DictionaryIterator *iter, void *context) 
{
		
    (void) context;
    //Get data
    Tuple *t = dict_read_first(iter);
    while(t != NULL)
    {		
			process_tuple(t);      
      //Get next
      t = dict_read_next(iter);
    }
}



static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



static void main_window_load(Window *window) {
	

	
	if (persist_exists(KEY_CHARACTER)) {
		character = persist_read_int(KEY_CHARACTER);		
	} else {
		character = CHARACTER_PACMAN;
	}

  //Load bitmaps into GBitmap structures

	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	
	#ifdef PBL_PLATFORM_BASALT 
		s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  #endif
  #ifdef PBL_PLATFORM_CHALK 
		s_background_layer = bitmap_layer_create(GRect(0, 0, 180, 180));
  #endif
	
	
	
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	
	s_bigdot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BIG_DOT);
	s_bigdot_layer = bitmap_layer_create(GRect(BIGDOT_X, HALLWAY_Y, 16, 28));
	bitmap_layer_set_bitmap(s_bigdot_layer, s_bigdot_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bigdot_layer));
	
	
	
	s_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CEREZAS);
	s_fruit_layer = bitmap_layer_create(GRect(FRUIT_X, HALLWAY2_Y, 29, 29));
	bitmap_layer_set_bitmap(s_fruit_layer, s_fruit_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_fruit_layer));
	
	 //points_fruit
	s_points_fruit_bitmap = gbitmap_create_with_resource(RESOURCE_ID_100);
	s_points_fruit_layer = bitmap_layer_create(GRect(POINTS_FRUIT_X, HALLWAY2_Y, 36, 28));
	bitmap_layer_set_bitmap(s_points_fruit_layer, NULL);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_points_fruit_layer));
	
	
	s_ghost_bitmap = gbitmap_create_with_resource(RESOURCE_ID_INKY_RIGHT);
	s_ghost_layer = bitmap_layer_create(GRect(8, HALLWAY_Y, 28, 28));
	bitmap_layer_set_bitmap(s_ghost_layer, s_ghost_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_ghost_layer));
	
	
	s_pacman_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PACMAN_RIGHT);
	s_pacman_layer = bitmap_layer_create(GRect(58, HALLWAY_Y, 28, 28));
	bitmap_layer_set_bitmap(s_pacman_layer, s_pacman_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_pacman_layer));
	

	
	s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BAT3);
	s_battery_layer = bitmap_layer_create(GRect(BATTERY_X, BATTERY_Y, 42, 11));
	bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_layer));
	
	
	//números
	
	s_time_layer= text_layer_create(GRect(TIME_X, TIME_Y, 117, 35)); //19, 63, 107, 35
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite); 
  text_layer_set_text(s_time_layer, "00:00");
	 //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOYSTIX_35));
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
//	text_layer_set_overflow_mode(s_time_layer, GTextOverflowModeFill);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	
  //fecha
	s_date_layer= text_layer_create(GRect(DATE_X, DATE_Y, 79, 18)); // 0,0,144,84
  //	text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text(s_date_layer, "25 MAR");
	//Create GFont
  s_date_font =  fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOYSTIX_18));
  //Apply to TextLayer
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_date_layer));
	
	
	 //end
	s_end_layer= text_layer_create(GRect(END_X, END_Y, 144, 30)); // 0,0,144,84
  //	text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_end_layer, GColorWhite);
	text_layer_set_background_color(s_end_layer, GColorClear);
  text_layer_set_text(s_end_layer, "YOU WIN!");
	//Create GFont
  s_end_font =  fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOYSTIX_25));
  //Apply to TextLayer
  text_layer_set_font(s_end_layer, s_end_font);
  text_layer_set_text_alignment(s_end_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_end_layer));
	
	
	 //points_pacman
	
	
	
	s_points_pacman_bitmap = gbitmap_create_with_resource(RESOURCE_ID_200);
	s_points_pacman_layer = bitmap_layer_create(GRect(GHOST_END2, HALLWAY_Y, POINTS_PACMAN_X, 28));
	bitmap_layer_set_bitmap(s_points_pacman_layer, NULL);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_points_pacman_layer));
	
	
	
	


  #ifdef PBL_PLATFORM_CHALK
  
//    minute_layer_unit = bitmap_layer_create(GRect(72 + 18, 84 + 6, 72, 84));
  #else
    
//    minute_layer_unit = bitmap_layer_create(GRect(72, 84, 72, 84));
  #endif
    
  update_time();
	update_battery();

	
//	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(minute_layer_unit));		
//	window_set_background_color (window, GColorBlack);
}


static void tick_minutos(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}


	
static void main_window_unload(Window *window) {
	

	
		//Destroy GBitmaps
	gbitmap_destroy(s_background_bitmap);
	gbitmap_destroy(s_ghost_bitmap);
	gbitmap_destroy(s_pacman_bitmap);
	gbitmap_destroy(s_bigdot_bitmap);
	gbitmap_destroy(s_battery_bitmap);
	gbitmap_destroy(s_points_pacman_bitmap);
	gbitmap_destroy(s_fruit_bitmap);
	gbitmap_destroy(s_points_fruit_bitmap);
	
	
	// Destroy TextLayers
  text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_end_layer);


	//Destroy  layers
	bitmap_layer_destroy(s_background_layer);
	bitmap_layer_destroy(s_ghost_layer);
	bitmap_layer_destroy(s_pacman_layer);
	bitmap_layer_destroy(s_bigdot_layer);
	bitmap_layer_destroy(s_battery_layer);
	bitmap_layer_destroy(s_points_pacman_layer);
	bitmap_layer_destroy(s_fruit_layer);
	bitmap_layer_destroy(s_points_fruit_layer);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
 
	// Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT , tick_minutos);

	// Register callbacks
	app_message_register_inbox_received((AppMessageInboxReceived) in_received_handler);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	app_message_open(784, 784);
	
}
	


static void deinit(void) {
	// Destroy the window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
