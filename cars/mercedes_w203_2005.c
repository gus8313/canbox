// CAN COMFORT
static void mercedes_w203_2005my_ms_0000_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.acc = STATE_UNDEF;
		carstate.ign = STATE_UNDEF;
		return;
	}

	if (msg[0] & (1<<7))
		carstate.acc = 1;
	else
		carstate.acc = 0;

	if (msg[0] & (1<<6))
		carstate.ign = 1;
	else
		carstate.ign = 0;
}

static void mercedes_w203_2005my_ms_00B2_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.vin[0] = STATE_UNDEF;
		return;
	}

	// WAUZZZ8U4GR060231

	//                      W  A  U
	// 65F # 00 FF FF FF FF 57 41 55
	//          Z  Z  Z  8  U  4  G
	// 65F # 01 5A 5A 5A 38 55 34 47
	//          R  0  6  0  2  3  1
	// 65F # 02 52 30 36 30 32 33 31

	//    6  |     1
	// 00.36.7c.c2.01.57.41.55
	// 01.5a.5a.5a.38.55.34.47
	// 02.52.30.36.30.32.33.31
/* TODO */
	if (msg[0] == 0x00)
		memcpy(carstate.vin, msg + 5, 3);
	else if (msg[0] == 0x01)
		memcpy(carstate.vin + 3, msg + 1, 7);
	else if (msg[0] == 0x02)
		memcpy(carstate.vin + 10, msg + 1, 7);
}
/*
static void mercedes_w203_2005my_ms_009E_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.odometer = STATE_UNDEF;
		return;
	}

	uint8_t odo[3] = { (msg[3] & 0x0F), msg[2], msg[1] };
	uint32_t value = 0;

	for (int i = 0; i < 3; i++) {
		value = (value << 8) + (odo[i] & 0xFF);
	}

	carstate.odometer = value;
}*/

/*static void q3_2015_ms_571_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.voltage = STATE_UNDEF;
		return;
	}

	// 0x571 : a6 00 00 00 00 00 00 00
	// 5 + (0.05 * 0xa6) = 13.3
	carstate.voltage = 5 + (0.05 * msg[0]);
}*/

static void mercedes_w203_2005my_ms_0004_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.fl_door = STATE_UNDEF;
		carstate.fr_door = STATE_UNDEF;
		carstate.rl_door = STATE_UNDEF;
		carstate.rr_door = STATE_UNDEF;
		carstate.bonnet = STATE_UNDEF;
		carstate.tailgate = STATE_UNDEF;

		return;
	}

	// 0x470 : 00 00 24 16 20 00 00 00

	carstate.bonnet   = (msg[1] & 0x01) ? 1 : 0;
	carstate.fl_door  = (msg[0] & (1<<7)) ? 1 : 0;
	carstate.fr_door  = (msg[0] & (1<<6)) ? 1 : 0;
	carstate.rl_door  = (msg[0] & (1<<5)) ? 1 : 0;
	carstate.rr_door  = (msg[0] & (1<<4)) ? 1 : 0;
	carstate.tailgate = (msg[0] & (1<<3)) ? 1 : 0;
}

static void mercedes_w203_2005my_ms_0003_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.selector = STATE_UNDEF;
		carstate.speed = 0;
		return;
	}

	if (msg[0] & (1<<7))
		carstate.selector = e_selector_r;
	else
		carstate.selector = STATE_UNDEF;

	carstate.speed = (msg[4] * 256) + (msg[3] & 0x3F); //1/min

}

static void mercedes_w203_2005my_ms_01CA_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		key_state.key_volume = STATE_UNDEF;
		key_state.key_prev = STATE_UNDEF;
		key_state.key_next = STATE_UNDEF;
		key_state.key_mici = STATE_UNDEF;
		key_state.key_telsend = STATE_UNDEF;
		key_state.key_telend = STATE_UNDEF;
		
		return;
	}

#if 0
	//up
	if ((msg[1] & (1<<3))) {

		if ((key_state.key_volume != 1) && key_state.key_cb && key_state.key_cb->inc_volume)
			key_state.key_cb->inc_volume(1);

		key_state.key_volume = 1;
	}
	//down
	else if ((msg[1] & (1<<2))) {

		if ((key_state.key_volume != 0) && key_state.key_cb && key_state.key_cb->dec_volume)
			key_state.key_cb->dec_volume(1);

		key_state.key_volume = 0;
	}
	else
		key_state.key_volume = STATE_UNDEF;
#endif

	//PREV
	uint8_t key_prev = (msg[1] & (1<<6)) ? 1 : 0;
	//1->0 short release
	if ((key_state.key_prev == 1) && (key_prev == 0) && key_state.key_cb && key_state.key_cb->prev)
		key_state.key_cb->prev();
	key_state.key_prev = key_prev;

	//NEXT
	uint8_t key_next = (msg[1]  & (1<<7)) ? 1 : 0;
	//1->0 short release
	if ((key_state.key_next == 1) && (key_next == 0) && key_state.key_cb && key_state.key_cb->next)
		key_state.key_cb->next();
	key_state.key_next = key_next;

	//Voice control
	uint8_t key_mici = (msg[4] & (1<<7)) ? 1 : 0;
	//1->0 short release
	if ((key_state.key_mici == 1) && (key_mici == 0) && key_state.key_cb && key_state.key_cb->mici)
		key_state.key_cb->mici();
	key_state.key_mici = key_mici;

	//Tel send
	uint8_t key_telsend = (msg[1] & (1<<2)) ? 1 : 0;
	//1->0 short release
	if ((key_state.key_telsend == 1) && (key_telsend == 0) && key_state.key_cb && key_state.key_cb->telsend)
		key_state.key_cb->telsend();
	key_state.key_telsend = key_telsend;

	//Tel end
	uint8_t key_telend = (msg[1] & 1) ? 1 : 0;
	//1->0 short release
	if ((key_state.key_telend == 1) && (key_telend == 0) && key_state.key_cb && key_state.key_cb->telend)
		key_state.key_cb->telend();
	key_state.key_telend = key_telend;

}


static void mercedes_w203_2005my_ms_0194_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.illum = STATE_UNDEF;
		return;
	}

	carstate.illum = msg[0]; //percents
}

static void mercedes_w203_2005my_ms_0006_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.wheel = 0;
		return;
	}

	uint8_t angle = (msg[4] * 256) + (msg[3] & 0x7); // in degrees
	uint8_t wheel = scale(angle, 0, 0x44, 0, 100); //in percent, max set to 68°, to be checked
// Steering wheel sign : offset 28 size 1
// Steering wheel sign : offset 29 size 11

	if (msg[4] & (1<<4)) {
		// turn right
		carstate.wheel = wheel;
	} else {
		// turn left
		carstate.wheel = -wheel;
	}
}

/*static void q3_2015_ms_35b_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.taho = STATE_UNDEF;
		carstate.engine = STATE_UNDEF;
		return;
	}

	// 0x35B : 08 CC 0A BA 03 19 0E 80 // 91,5 degrees 691 RPM

	carstate.taho = ((msg[2] * 256) + msg[1]) / 4; // RPM

	if (carstate.taho > 500)
		carstate.engine = 1;
	else
		carstate.engine = 0;

	carstate.temp = (msg[3] - 64) * 0.75; // coolant
}
*/
static void mercedes_w203_2005my_ms_000A_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.park_break = STATE_UNDEF;
		//carstate.low_fuel_lvl = STATE_UNDEF;
		carstate.low_washer = STATE_UNDEF;
		return;
	}

//	carstate.fuel_lvl = (msg[3] & 0x7F; // ? fuel level

// Handbrake: Bit 18 size 1
	carstate.park_break = (msg[2] & (1<<6)) ? 1 : 0;

// Washer low: Bit 25 size 1
	carstate.low_washer = (msg[3] & (1<<7)) ? 1 : 0;

//	if ((msg[3] & 0x80) == 0x80)
//		carstate.low_fuel_lvl = 1;
//	else
//		carstate.low_fuel_lvl = 0;
}

/*
static void q3_2015_ms_3E3_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		car_air_state.l_seat = STATE_UNDEF;
		car_air_state.r_seat = STATE_UNDEF;
		car_air_state.powerfull = STATE_UNDEF;
		return;
	}

	/*
0x3E3 : 00 00 97 XY 00 00 00 00

97 - temp ?

XY 36 - 0011 0110

Y - left seat heating
0000 - 0 - off
0110 - 6 - 3 leds
0100 - 4 - 2 leds
0010 - 2 - 1 led

X - right seat heating
0011 - 3 - 3 leads
0010 - 2 - 2 leads
0001 - 1 - 1 lead
0000 - 0 - off

0100 - 4 - turbo front window airheating
*/
/*
	switch ((msg[3] >> 4) & 0x03) {
		case 0x01:
			car_air_state.r_seat = 1;
			break;
		case 0x02:
			car_air_state.r_seat = 2;
			break;
		case 0x03:
			car_air_state.r_seat = 3;
			break;
		case 0x00:
			car_air_state.r_seat = 0;
			break;
		default:
			car_air_state.r_seat = 0;
			break;
	}

	switch ((msg[3] >> 1) & 0x03) {
		case 0x01:
			car_air_state.l_seat = 1;
			break;
		case 0x02:
			car_air_state.l_seat = 2;
			break;
		case 0x03:
			car_air_state.l_seat = 3;
			break;
		case 0x00:
			car_air_state.l_seat = 0;
			break;
		default:
			car_air_state.l_seat = 0;
			break;
	}

	if ((msg[3] >> 6) & 0x01)
		car_air_state.powerfull = 1;
	else
		car_air_state.powerfull = 0;
}
*/
static void mercedes_w203_2005my_ms_02FF_handler(const uint8_t * msg, struct msg_desc_t * desc)
{
	if (is_timeout(desc)) {

		carstate.tpms.state = e_tpms_off;
		return;
	}
	if (msg[0] & 0xF0) {
		carstate.tpms.state = e_tpms_on;
    
		carstate.tpms.fl = msg[4]; // Bar *100
		carstate.tpms.fr = msg[5];
		carstate.tpms.rl = msg[6];
		carstate.tpms.rr = msg[7];

		if (msg[2] & 0x01) //Spare tire available ?
			carstate.tpms.spr = msg[8];
	}
	else
		carstate.tpms.state = e_tpms_off;

}

//ID period tick num
static struct msg_desc_t mercedes_w203_2005my_ms[] =
{
	{ 0x0000,  100, 0, 0, mercedes_w203_2005my_ms_0000_handler }, // ACC
	{ 0x00B2,  200, 0, 0, mercedes_w203_2005my_ms_00B2_handler }, // VIN
//	{ 0x009E, 1000, 0, 0, mercedes_w203_2005my_ms_009E_handler }, // Odometer
//	{ 0x571,  600, 0, 0, mercedes_w203_2005my_ms_571_handler }, // Voltage
	{ 0x0004,   50, 0, 0, mercedes_w203_2005my_ms_0004_handler }, // Doors
	{ 0x0003,  100, 0, 0, mercedes_w203_2005my_ms_0003_handler }, // Gear selector
	{ 0x01CA,  100, 0, 0, mercedes_w203_2005my_ms_01CA_handler }, // Keys
	{ 0x0194,  100, 0, 0, mercedes_w203_2005my_ms_0194_handler }, // Illum
	{ 0x0006,  100, 0, 0, mercedes_w203_2005my_ms_0006_handler }, // Steering angle
//	{ 0x35b,  100, 0, 0, mercedes_w203_2005my_ms_35b_handler }, // Taho
	{ 0x000A,  100, 0, 0, mercedes_w203_2005my_ms_000A_handler }, // Break
//	{ 0x3E3,  500, 0, 0, mercedes_w203_2005my_ms_3E3_handler }, // Seat heating
	{ 0x02FF,  500, 0, 0, mercedes_w203_2005my_ms_02FF_handler }, // Tire pressure
};

