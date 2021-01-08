#ifndef ActionH
#define ActionH

struct Action
{
	enum Type
	{
		TYPE_NONE = 0,
		TYPE_PLAY_STOP,
		TYPE_PLAY_PAUSE,
		TYPE_SEEK_M3,
		TYPE_SEEK_P3,
		TYPE_SEEK_M60,
		TYPE_SEEK_P60,
		TYPE_TOGGLE_FULLSCREEN,
		TYPE_EXIT_FS_EXIT,
		TYPE_SHOW_FILE_INFO,
		TYPE_MINIMIZE,
		TYPE_TOGGLE_OSD,
		TYPE_SKIP,
		TYPE_PREV,
		TYPE_DELETE_FILE,
		TYPE_SHOW_LOG,

		TYPE_LIMITER
	} type;
	int id;

	Action(void):
		type(TYPE_NONE),
		id(0)
	{
	}
	static const char* getTypeDescription(enum Type type);

};

#endif
