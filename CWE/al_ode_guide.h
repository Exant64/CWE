#pragma once

enum ButtonGuideFlags {
	SELECT = BIT_0,
	CONFIRM = BIT_1,
	BACK = BIT_2
};
void CreateButtonGuide(int _flags);