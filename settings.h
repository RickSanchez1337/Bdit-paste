#pragma once


namespace settings
{

	namespace player
	{
		int enabled = true;
		int name = true;
		int health = true;
		int distance = true;
		int max_distance = 300;
		int held_item = true;
		int hotbar = true;
	}

	namespace ore
	{
		int enabled = true;
		int name = true;
		int distance = true;
		int max_distance = 100;
		int stone = true;
		int metal = true;
		int sulfur = true;
	}

	namespace collectable
	{
		int enabled = true;
		int name = true;
		int distance = true;
		int max_distance = 100;
	}

	namespace dropped
	{
		int enabled = true;
		int name = true;
		int distance = true;
		int max_distance = 100;
	}

	namespace misc
	{
		int spider = true;
		int debug_cam = true;
		int automatic = true;
		int no_recoil = false;
		int fast_reload = true;
		int fat_bullet = false;
		int always_day = true;
	}

	namespace aimbot
	{
		int enabled = true;
		int fov = 100;
		int max_distance = 300;
		int aim_key = 2;
		int silent_key = 1;
		int silent = true;
		int use_mouse = false;
		int smooth = 10;
	}
}