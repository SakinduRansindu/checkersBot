#pragma once
#include "material.h";
class square {
public:

	material materialContent;
	bool IsBlackSquare;

	square operator=(const square& s) {
		index_x = s.index_x;
		index_y = s.index_y;
		materialContent = s.materialContent;
		IsBlackSquare = s.IsBlackSquare;
		return *this;
	}

	void set_index_x(char x) {
		index_x = x;
	}

	void set_index_y(char y) {
		index_y = y;
	}

	char get_index_x() {
		return index_x;
	}
	char get_index_y() {
		return index_y;
	}

private:
	char index_x;
	char index_y;
};
