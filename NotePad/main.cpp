#include "NotePad.h"

int main()
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	NotePad app(GetModuleHandleW(NULL));
	if (!app.Create())
		return -1;

	return app.Run();
}
