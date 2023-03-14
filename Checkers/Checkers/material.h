#pragma once
class material {
private:
	bool check_validity(char type) {
		bool validity = false;
		for (char mt : _materialTypes) {
			if (type == mt) {
				validity = true;
				break;
			}
		}
		return validity;
	}
	const char _materialTypes[3] = { 'N','P','K' };
	char materialType = ' ';
	char materialColor = ' ';

public:
	char x = ' ';		//change into strings #dev
	char y = ' ';
	material(char type = 'N', char color = 'N') {

		if (check_validity(type)) {
			materialType = char(type);
			materialColor = char(color);

		}
	}

	material(const material& orgM) {
		materialColor = orgM.materialColor;
		materialType = orgM.materialType;
		x = orgM.x;
		y = orgM.y;
	}


	material operator=(const material& orgM) {

		materialColor = orgM.materialColor;
		materialType = orgM.materialType;
		x = orgM.x;
		y = orgM.y;
		return *this;
	}


	void setMaterial(char mtype, char mcolor, char posX, char posY) {
		materialType = mtype;
		materialColor = mcolor;
		x = posX;
		y = posY;
	}
	char getMaterialColor() {
		return materialColor;
	}
	char getMaterialType() {
		return materialType;
	}
};

