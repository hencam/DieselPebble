/*

   Digital watch based on my old Diesel watch

   A digital watch with large digits, seconds and date.

*/


#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "resource_ids.auto.h"

// be914b28-c46a-4045-821f-4fbc22aafb25
#define MY_UUID {0xbe, 0x91, 0x4b, 0x28, 0xc4, 0x6a, 0x40, 0x45, 0x82, 0x1f, 0x4f, 0xbc, 0x22, 0xaa, 0xfb, 0x25}
PBL_APP_INFO(MY_UUID, "D", "Pebble Technology", 0x5, 0x0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

int first_run = 1;

//
// There's only enough memory to load about 6 of 10 required images
// so we have to swap them in & out...
//
// We have one "slot" per digit location on screen.
//
// Because layers can only have one parent we load a digit for each
// slot--even if the digit image is already in another slot.
//
//
#define TOTAL_IMAGE_SLOTS 15

// DEFINE SLOTS - this is the rough layout on-screen
// 0/1:	month	 (01 - 12)
// 2:	dot		 (.)
// 3/4:	day		 (01 - 31)
// 5:	text day (Mon - Sun)
// 6:	secs	 (0-5)
// 7:	secs	 (0-9)
// 8:	am/pm	 (am - pm)
// 9/10: hours	 (01 - 12)
// 11:	colon	 (:)
// 12/13: mins	 (00 - 59)
// 14: 	TOPBAR

// This are the different font chars (images) required for each slot
// 0/1/2/3/4:	INV 0-9 :		(11)
// 5:			INV days		(7)
// 6/7:			SMALL secs 0-9	(10)
// 8:			TINY am/pm		(2)
// 9/10/11/12/13:BIG nn:nn		(12)

#define NUMBER_OF_IMAGES 44

// images
const int IMAGE_RESOURCE_IDS[NUMBER_OF_IMAGES] = {
	RESOURCE_ID_IMAGE_AM,
	RESOURCE_ID_IMAGE_PM,
	RESOURCE_ID_IMAGE_BIG0,
	RESOURCE_ID_IMAGE_BIG1,
	RESOURCE_ID_IMAGE_BIG2,
	RESOURCE_ID_IMAGE_BIG3,
	RESOURCE_ID_IMAGE_BIG4,
	RESOURCE_ID_IMAGE_BIG5,
	RESOURCE_ID_IMAGE_BIG6,
	RESOURCE_ID_IMAGE_BIG7,
	RESOURCE_ID_IMAGE_BIG8,
	RESOURCE_ID_IMAGE_BIG9,
	RESOURCE_ID_IMAGE_BIGSPACER,
	RESOURCE_ID_IMAGE_BIGTHIN1,
	RESOURCE_ID_IMAGE_DAYSUN,
	RESOURCE_ID_IMAGE_DAYMON,
	RESOURCE_ID_IMAGE_DAYTUE,
	RESOURCE_ID_IMAGE_DAYWED,
	RESOURCE_ID_IMAGE_DAYTHU,
	RESOURCE_ID_IMAGE_DAYFRI,
	RESOURCE_ID_IMAGE_DAYSAT,
	RESOURCE_ID_IMAGE_SMALL0,
	RESOURCE_ID_IMAGE_SMALL1,
	RESOURCE_ID_IMAGE_SMALL2,
	RESOURCE_ID_IMAGE_SMALL3,
	RESOURCE_ID_IMAGE_SMALL4,
	RESOURCE_ID_IMAGE_SMALL5,
	RESOURCE_ID_IMAGE_SMALL6,
	RESOURCE_ID_IMAGE_SMALL7,
	RESOURCE_ID_IMAGE_SMALL8,
	RESOURCE_ID_IMAGE_SMALL9,
	RESOURCE_ID_IMAGE_SMALLREV0,
	RESOURCE_ID_IMAGE_SMALLREV1,
	RESOURCE_ID_IMAGE_SMALLREV2,
	RESOURCE_ID_IMAGE_SMALLREV3,
	RESOURCE_ID_IMAGE_SMALLREV4,
	RESOURCE_ID_IMAGE_SMALLREV5,
	RESOURCE_ID_IMAGE_SMALLREV6,
	RESOURCE_ID_IMAGE_SMALLREV7,
	RESOURCE_ID_IMAGE_SMALLREV8,
	RESOURCE_ID_IMAGE_SMALLREV9,
	RESOURCE_ID_IMAGE_SMALLREVSPACER,
	RESOURCE_ID_IMAGE_TOPBAR,
	RESOURCE_ID_IMAGE_BLANKPIXEL
};

BmpContainer image_containers[TOTAL_IMAGE_SLOTS];



void update_seconds(int newval, int mode)
{
	int tens = newval / 10 % 10;
	int ones = newval % 10;

	if ((ones == 0) || (mode == 1))
	{
		if (mode == 0)
		{
			layer_remove_from_parent(&image_containers[6].layer.layer);
			bmp_deinit_container(&image_containers[6]);
		}
		
		bmp_init_container(IMAGE_RESOURCE_IDS[tens + 21], &image_containers[6]);
		image_containers[6].layer.layer.frame.origin.x = 90;
		image_containers[6].layer.layer.frame.origin.y = 40;
		layer_add_child(&window.layer, &image_containers[6].layer.layer);
	}

	if (mode == 0)
	{
		layer_remove_from_parent(&image_containers[7].layer.layer);
		bmp_deinit_container(&image_containers[7]);
	}
	
	bmp_init_container(IMAGE_RESOURCE_IDS[ones + 21], &image_containers[7]);
	image_containers[7].layer.layer.frame.origin.x = 120;
	image_containers[7].layer.layer.frame.origin.y = 40;
	layer_add_child(&window.layer, &image_containers[7].layer.layer);
}

void update_minutes(int newval, int mode)
{
	int tens = newval / 10 % 10;
	int ones = newval % 10;
	
	if ((ones == 0) || (mode == 1))
	{
		if (mode == 0)
		{
			layer_remove_from_parent(&image_containers[12].layer.layer);
			bmp_deinit_container(&image_containers[12]);
		}

		bmp_init_container(IMAGE_RESOURCE_IDS[tens + 2], &image_containers[12]);
		image_containers[12].layer.layer.frame.origin.x = 80;
		image_containers[12].layer.layer.frame.origin.y = 93;
		layer_add_child(&window.layer, &image_containers[12].layer.layer);
	}
	
	if (mode == 0)
	{
		layer_remove_from_parent(&image_containers[13].layer.layer);
		bmp_deinit_container(&image_containers[13]);
	}
	
	bmp_init_container(IMAGE_RESOURCE_IDS[ones + 2], &image_containers[13]);
	image_containers[13].layer.layer.frame.origin.x = 115;
	image_containers[13].layer.layer.frame.origin.y = 93;
	layer_add_child(&window.layer, &image_containers[13].layer.layer);

}

void update_hour(int newval, int mode)
{
	int tens = newval / 10 % 10;
	int ones = newval % 10;
	
	if (mode == 0)
	{
		layer_remove_from_parent(&image_containers[9].layer.layer);
		bmp_deinit_container(&image_containers[9]);
		
		layer_remove_from_parent(&image_containers[10].layer.layer);
		bmp_deinit_container(&image_containers[10]);
	}
	
	
	if (tens == 0)
	{
		bmp_init_container(IMAGE_RESOURCE_IDS[43], &image_containers[9]);
		image_containers[9].layer.layer.frame.origin.x = 0;
		image_containers[9].layer.layer.frame.origin.y = 93;
		layer_add_child(&window.layer, &image_containers[9].layer.layer);
	}
	else
	{
		bmp_init_container(IMAGE_RESOURCE_IDS[tens + 2], &image_containers[9]);
		image_containers[9].layer.layer.frame.origin.x = 0;
		image_containers[9].layer.layer.frame.origin.y = 93;
		layer_add_child(&window.layer, &image_containers[9].layer.layer);
	}
	
	bmp_init_container(IMAGE_RESOURCE_IDS[ones + 2], &image_containers[10]);
	image_containers[10].layer.layer.frame.origin.x = 35;
	image_containers[10].layer.layer.frame.origin.y = 93;
	layer_add_child(&window.layer, &image_containers[10].layer.layer);
}

void update_day(int newval, int newdayval, int mode)
{
	int tens = newval / 10 % 10;
	int ones = newval % 10;
	
	if (mode == 0)
	{
		layer_remove_from_parent(&image_containers[3].layer.layer);
		bmp_deinit_container(&image_containers[3]);

		layer_remove_from_parent(&image_containers[4].layer.layer);
		bmp_deinit_container(&image_containers[4]);
	}
	
	bmp_init_container(IMAGE_RESOURCE_IDS[tens + 31], &image_containers[3]);
	image_containers[3].layer.layer.frame.origin.x = 0;
	image_containers[3].layer.layer.frame.origin.y = 0;
	layer_add_child(&window.layer, &image_containers[3].layer.layer);
	
	bmp_init_container(IMAGE_RESOURCE_IDS[ones + 31], &image_containers[4]);
	image_containers[4].layer.layer.frame.origin.x = 30;
	image_containers[4].layer.layer.frame.origin.y = 0;
	layer_add_child(&window.layer, &image_containers[4].layer.layer);

	// DAY TEXT
	if (mode == 0)
	{
		layer_remove_from_parent(&image_containers[5].layer.layer);
		bmp_deinit_container(&image_containers[5]);
	}
	
	bmp_init_container(IMAGE_RESOURCE_IDS[newdayval + 14], &image_containers[5]);
	image_containers[5].layer.layer.frame.origin.x = 0;
	image_containers[5].layer.layer.frame.origin.y = 40;
	layer_add_child(&window.layer, &image_containers[5].layer.layer);

}

void update_month(int newval, int mode)
{
	newval++;
	int tens = newval / 10 % 10;
	int ones = newval % 10;
	
	if (mode == 0)
	{
		layer_remove_from_parent(&image_containers[0].layer.layer);
		bmp_deinit_container(&image_containers[0]);
		
		layer_remove_from_parent(&image_containers[1].layer.layer);
		bmp_deinit_container(&image_containers[1]);
	}
	
	bmp_init_container(IMAGE_RESOURCE_IDS[tens + 31], &image_containers[0]);
	image_containers[0].layer.layer.frame.origin.x = 75;
	image_containers[0].layer.layer.frame.origin.y = 0;
	layer_add_child(&window.layer, &image_containers[0].layer.layer);
	
	bmp_init_container(IMAGE_RESOURCE_IDS[ones + 31], &image_containers[1]);
	image_containers[1].layer.layer.frame.origin.x = 105;
	image_containers[1].layer.layer.frame.origin.y = 0;
	layer_add_child(&window.layer, &image_containers[1].layer.layer);
}


// watch layout and pixel positions
/*
<body>
    <div id="canvas" width="144" height="168" style="background-color: #000000;width:144px;height:168px;position:absolute; top:0px; left:0px;">


        <div style='position:absolute; top:0px; left:0px;'><img src='img\TopBar.png'></div>
        
        <div style='position:absolute; top:0px; left:0px;'><img src='img\SmallRev0.png'></div>

        <div style='position:absolute; top:0px; left:30px;'><img src='img\SmallRev2.png'></div>

        <div style='position:absolute; top:0px; left:55px;'><img src='img\SmallRevSpacer.png'></div>

        <div style='position:absolute; top:0px; left:75px;'><img src='img\SmallRev0.png'></div>

        <div style='position:absolute; top:0px; left:105px;'><img src='img\SmallRev3.png'></div>


        <div style='position:absolute; top:40px; left:0px;'><img src='img\DayTue.png'></div>

        <div style='position:absolute; top:40px; left:90px;'><img src='img\Small3.png'></div>

        <div style='position:absolute; top:40px; left:120px;'><img src='img\Small7.png'></div>


        <div style='position:absolute; top:95px; left:0px;'><img src='img\am.png'></div>

        <div style='position:absolute; top:95px; left:20px;'><img src='img\BigThin1.png'></div>

        <div style='position:absolute; top:95px; left:35px;'><img src='img\Big2.png'></div>

        <div style='position:absolute; top:115px; left:70px;'><img src='img\BigSpacer.png'></div>

        <div style='position:absolute; top:95px; left:80px;'><img src='img\Big3.png'></div>

        <div style='position:absolute; top:95px; left:115px;'><img src='img\Big8.png'></div>
        
    </div>
</body>
*/


// Data Fields
// int	 tm_hour	Hours since midnight. 
// int	 tm_isdst	Daylight Saving Time flag (not implemented) 
// int	 tm_mday	Day of the month. 
// int	 tm_min	 	Minutes after the hour. 
// int	 tm_mon	 	Months since January. 
// int	 tm_sec	 	Seconds after the minute. 
// int	 tm_wday	Day since Sunday. 
// int	 tm_yday	Days since January 1. 
// int	 tm_year	Years since 1900.


void display_time(PblTm *tick_time, int mode)
{
	update_seconds(tick_time->tm_sec, mode);
	
	if ((tick_time->tm_sec == 0) || (mode == 1))
	{
		update_minutes(tick_time->tm_min, mode);
		
		if ((tick_time->tm_min == 0) || (mode == 1))
		{
			update_hour(tick_time->tm_hour, mode);   
   
			if ((tick_time->tm_hour == 0) || (mode == 1))
			{
				update_day(tick_time->tm_mday, tick_time->tm_wday, mode);
				
				if ((tick_time->tm_mday == 1) || (mode == 1))
				{
					update_month(tick_time->tm_mon, mode);
				}
			}
		}
	}
}


void handle_seconds_tick(AppContextRef actxr, PebbleTickEvent *t)
{
	display_time(t->tick_time, 0);
}

void handle_init(AppContextRef actxr)
{
	resource_init_current_app(&APP_RESOURCES);
	window_init(&window, "Diesel Watchface");
	window_stack_push(&window, true);
	
	window_set_background_color(&window, GColorBlack);
	
	// init face (needs images placing for items that don't change)
	
	// PIXEL BLOCK TOPBAR
	bmp_init_container(IMAGE_RESOURCE_IDS[42], &image_containers[14]);
	image_containers[14].layer.layer.frame.origin.x = 0;
	image_containers[14].layer.layer.frame.origin.y = 0;
	layer_add_child(&window.layer, &image_containers[14].layer.layer);
	
	// BIG MAIN TIME DIGITS SEPERATOR
	bmp_init_container(IMAGE_RESOURCE_IDS[12], &image_containers[11]);
	image_containers[11].layer.layer.frame.origin.x = 70;
	image_containers[11].layer.layer.frame.origin.y = 113;
	layer_add_child(&window.layer, &image_containers[11].layer.layer);
	
	// TOPBAR DATE SEPERATOR
	bmp_init_container(IMAGE_RESOURCE_IDS[41], &image_containers[2]);
	image_containers[2].layer.layer.frame.origin.x = 55;
	image_containers[2].layer.layer.frame.origin.y = 0;
	layer_add_child(&window.layer, &image_containers[2].layer.layer);
	
	
	PblTm tick_time;
	get_time(&tick_time);
	display_time(&tick_time, 1);
	
}

void handle_deinit(AppContextRef actxr)
{
	(void)actxr;
	
	layer_remove_from_parent(&image_containers[6].layer.layer);
	bmp_deinit_container(&image_containers[6]);

	layer_remove_from_parent(&image_containers[7].layer.layer);
	bmp_deinit_container(&image_containers[7]);
	
	layer_remove_from_parent(&image_containers[14].layer.layer);
	bmp_deinit_container(&image_containers[14]);

	layer_remove_from_parent(&image_containers[5].layer.layer);
	bmp_deinit_container(&image_containers[5]);
	
	layer_remove_from_parent(&image_containers[9].layer.layer);
	bmp_deinit_container(&image_containers[9]);
	
	layer_remove_from_parent(&image_containers[10].layer.layer);
	bmp_deinit_container(&image_containers[10]);
	
	layer_remove_from_parent(&image_containers[11].layer.layer);
	bmp_deinit_container(&image_containers[11]);
	
	layer_remove_from_parent(&image_containers[12].layer.layer);
	bmp_deinit_container(&image_containers[12]);
	
	layer_remove_from_parent(&image_containers[13].layer.layer);
	bmp_deinit_container(&image_containers[13]);
	
	layer_remove_from_parent(&image_containers[0].layer.layer);
	bmp_deinit_container(&image_containers[0]);
	
	layer_remove_from_parent(&image_containers[1].layer.layer);
	bmp_deinit_container(&image_containers[1]);
	
	layer_remove_from_parent(&image_containers[2].layer.layer);
	bmp_deinit_container(&image_containers[2]);
	
	layer_remove_from_parent(&image_containers[3].layer.layer);
	bmp_deinit_container(&image_containers[3]);
	
	layer_remove_from_parent(&image_containers[4].layer.layer);
	bmp_deinit_container(&image_containers[4]);
	
}

void pbl_main(void *params)
{
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		
		.tick_info = {
			.tick_handler = &handle_seconds_tick,
			.tick_units = SECOND_UNIT
		}
		
	};
	app_event_loop(params, &handlers);
}