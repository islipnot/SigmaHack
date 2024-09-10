struct weapon_info
{
	char weapon_name[23];      //0x0000
	char title[42];            //0x0017
	char padding;              //0x0041
	uint16_t sound;            //0x0042
	uint16_t reload;           //0x0044
	uint16_t reload_time;      //0x0046
	uint16_t atk_delay;        //0x0048
	uint16_t damage;           //0x004A
	uint16_t piercing;         //0x004C
	uint16_t projspeed;        //0x004E
	uint16_t part;             //0x0050
	uint16_t spread;           //0x0052
	uint16_t recoil;           //0x0054
	uint16_t mag_size;         //0x0056
	uint16_t mdl_kick_rot;     //0x0058
	uint16_t mdl_kick_back;    //0x005A
	uint16_t recoil_increase;  //0x005C
	uint16_t recoil_base;      //0x005E
	uint16_t max_recoil;       //0x0060
	uint16_t recoil_back_fade; //0x0062
	uint16_t push_factor;      //0x0064
	uint16_t is_auto;          //0x0066
};

struct wpn_class
{
	uint32_t weapon_id;         //0x0000
	char pad_0004[8];           //0x0004
	weapon_info* weapon_info;   //0x000C
	char pad_0010[12];          //0x0010
	uint32_t consecutive_shots; //0x001C
	uint32_t reload_delay;      //0x0020
	char pad_0024[4];           //0x0024
	uint8_t scoped;             //0x0028
	char pad_0029[3];           //0x0029
	int scoped_since;           //0x002C
};

struct player_entity
{
	char pad_0000[12];         //0x0000
	float N00000052;           //0x000C
	char pad_0010[24];         //0x0010
	float x;                   //0x0028
	float y;                   //0x002C
	float z;                   //0x0030
	float yaw;                 //0x0034
	float pitch;               //0x0038
	float roll;                //0x003C
	char pad_0040[172];        //0x0040
	uint32_t health;           //0x00EC
	uint32_t armor;            //0x00F0
	char pad_00F4[12];         //0x00F4
	bool akimbo;               //0x0100
	char pad_0101[7];          //0x0101
	uint32_t pistol_reserves;  //0x0108
	uint32_t carbine_reserves; //0x010C
	uint32_t shotgun_reserves; //0x0110
	uint32_t smg_reserves;     //0x0114
	uint32_t sniper_reserves;  //0x0118
	uint32_t ar_reserves;      //0x011C
	char pad_0120[4];          //0x0120
	uint32_t akimbo_reserves;  //0x0124
	char pad_0128[4];          //0x0128
	uint32_t pistol_ammo;      //0x012C
	uint32_t carbine_ammo;     //0x0130
	uint32_t shotgun_ammo;     //0x0134
	uint32_t smg_ammo;         //0x0138
	uint32_t sniper_ammo;      //0x013C
	uint32_t ar_ammo;          //0x0140
	uint32_t grenades;         //0x0144
	uint32_t akimbo_delay;     //0x0148
	uint32_t knife_delay;      //0x014C
	char pad_0150[180];        //0x0150
	uint8_t dead;              //0x0204
	char name[15];             //0x0205
	char pad_0214[248];        //0x0214
	uint32_t team;             //0x030C
	uint32_t swap_delay;       //0x0310
	char pad_0314[40];         //0x0314
	wpn_class* knife;          //0x033C
	wpn_class* pistol;         //0x0340
	wpn_class* carbine;        //0x0344
	wpn_class* shotty;         //0x0348
	wpn_class* subgun;         //0x034C
	wpn_class* sniper;         //0x0350
	wpn_class* assault;        //0x0354
	wpn_class* grenade;        //0x0358
	wpn_class* second_wpn;     //0x035C
	wpn_class* other_wpn;      //0x0360
	wpn_class* equiped_wpn;    //0x0364
};

extern UINT* player_count;
extern player_entity*** player_list_ptr;
extern player_entity* local_player;