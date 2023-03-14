#pragma once
#include<string>
#include<vector>
#include<iostream>
#include"square.h"

using namespace std;
const char alpha[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
class board {
public:

	board(int width, int height) {
		w = width;
		h = height;
		sq = new square[w * h];
		int i = 0;
		for (int y = 1; y <= h; y++) {
			for (int x = 1; x <= w; x++) {
				sq[i].set_index_x(alpha[x - 1]);
				string c = to_string(y);
				sq[i].set_index_y(c[0]);
				i++;
			}

		}
		ColorTheBoard();
	}

	board(int width, int height, square* tsqs) {
		w = width;
		h = height;
		sq = new square[w * h];
		int i = 0;
		for (int y = 1; y <= h; y++) {
			for (int x = 1; x <= w; x++) {
				sq[i].set_index_x(alpha[x - 1]);
				string c = to_string(y);
				sq[i].set_index_y(c[0]);
				i++;
				memcpy(sq, tsqs, sizeof(square) * w * h);
			}

		}
		ColorTheBoard();
	}

	~board() {
		delete[] sq;
	}

	board operator=(const board& b) {
		w = b.w;
		h = b.h;
		sq = new square[w * h];
		last_movement = b.last_movement;
		for (int k = 0; k < (h * w); k++) {
			sq[k] = b.sq[k];
		}
		return *this;
	};


	board(const board& b) {
		w = b.w;
		h = b.h;
		sq = new square[w * h];
		last_movement = b.last_movement;
		for (int k = 0; k < (h * w); k++) {
			sq[k] = b.sq[k];
		}

	}

	int getHeight() {
		return h;
	}
	int getWidth() {
		return w;
	}

	square* getBoard() {
		return sq;
	}

	//BP
	void setupTroopsCustomOrder(vector<int> index, vector<string> code) {
		int i = 0;
		for (int y = 1; y <= h; y++) {
			for (int x = 1; x <= w; x++) {
				for (int j = 0; j < index.size(); j++)
				{
					if (index[j] == i) {
						sq[i].materialContent.setMaterial(code[j][1], code[j][0], alpha[x - 1], y + 48);
					}
				}
				i++;
			}
		}
	}

	void readyTroops(int trooplines)
	{

		if ((trooplines * 2) < h) {
			int i = 0;
			for (int y = 1; y <= h; y++) {
				for (int x = 1; x <= w; x++) {

					if ((trooplines >= convertCharYs2int(sq[i].get_index_y())) && (sq[i].IsBlackSquare)) {
						sq[i].materialContent.setMaterial('P', 'W', alpha[x - 1], y + 48);
					}
					else if ((h - (trooplines - 1) <= convertCharYs2int(sq[i].get_index_y())) && (sq[i].IsBlackSquare)) {
						sq[i].materialContent.setMaterial('P', 'B', alpha[x - 1], y + 48);
					}
					else {
						sq[i].materialContent.setMaterial('N', 'N', alpha[x - 1], y + 48);
					}
					i++;

				}
			}
		}
	}

	int move(string fx, string fy, string tx, string ty, string CapX = " ", string CapY = " ") {
		int acceptcode = 0;
		if (CapX == " ") {
			acceptcode = move_material(fx, fy, tx, ty);
		}
		else {
			acceptcode = move_material(fx, fy, tx, ty, CapX, CapY);
		}
		return acceptcode;
	}

	void printBoard(string indent = "") {
		int i = 0;
		for (int y = 1; y <= h; y++) {
			cout << indent + "    --------------------------------------" << endl << indent;
			cout << (h + 1) - y << "  |";
			std::string tmp = "";
			for (int x = 1; x <= w; x++) {
				string col(1, sq[(h * w - 1) - i].materialContent.getMaterialColor());
				string typ(1, sq[(h * w - 1) - i].materialContent.getMaterialType());
				if ((col == "N") && (typ == "N")) {
					col = " ";
					typ = " ";
				}
				tmp = col + typ + " | " + tmp;
				i++;
			}
			cout << tmp << endl;

		}
		cout << indent + "    --------------------------------------" << endl << indent;
		for (int x = 0; x < w; x++) {
			cout << "    " << alpha[x];
		}
		cout << endl << endl;
	}

	vector<string> posibleMovesForMaterial(material m) {
		vector<string> codes;
		for (string sj : {"upright", "upleft", "downright", "downleft"}) {
			square st = checkNearestSquares(m, sj);
			if (st.get_index_x() != 0) {
				if (st.materialContent.getMaterialType() == 'N' && st.get_index_x() != 0) {
					string cd = string(1, m.x) + string(1, convertCharYs2int(m.y)) + string(1, m.getMaterialType()) + string(1, st.get_index_x()) + string(1, convertCharYs2int(st.get_index_y()));
					codes.push_back(cd);
				}
			}
		}
		return codes;
	}
	square checkNearestSquares(char x, char y, string Squareside) {
		char nextXChar = x + 1;
		char previousXChar = x - 1;
		char upY = y + 1;
		char downY = y - 1;
		bool upright = false;
		bool upleft = false;
		bool downright = false;
		bool downleft = false;

		if (nextXChar <= alpha[w - 1]) {
			upright = true;
			downright = true;
		}
		if (previousXChar >= 'a') {
			upleft = true;
			downleft = true;
		}
		if (upY - 48 > h) {
			upright = upright && false;
			upleft = upleft && false;
		}
		if (downY - 48 < 1) {
			downright = downright && false;
			downleft = downleft && false;
		}

		//limit output to requested square
		if (Squareside != "upright") {
			upright = false;
		}
		if (Squareside != "upleft") {
			upleft = false;
		}
		if (Squareside != "downright") {
			downright = false;
		}
		if (Squareside != "downleft") {
			downleft = false;
		}

		square s;
		if (upright) {
			s = getSquareFromCordinates(nextXChar, upY);

		}
		else if (upleft) {
			s = getSquareFromCordinates(previousXChar, upY);

		}
		else if (downright) {
			s = getSquareFromCordinates(nextXChar, downY);

		}
		else if (downleft) {
			s = getSquareFromCordinates(previousXChar, downY);

		}
		else {
			s.set_index_x(0);
			s.set_index_y(0);
			s.materialContent = 'N';
		}
		return s;
	}

	square checkNearestSquares(material m, string Squareside) {
		return checkNearestSquares(m.x, m.y, Squareside);
	}

	void remove_material(material m) {
		remove_material(m.x, m.y);
	}

	void saveMovement(string fx, string fy, string tx, string ty, string capturedMaterialX = " ", string capturedMaterialY = " ") {
		if (capturedMaterialX == " ") {
			last_movement = fx + "." + fy + ">" + tx + "." + ty + "X";
		}
		else {
			last_movement = fx + "." + fy + ">" + tx + "." + ty + "X" + capturedMaterialX + "." + capturedMaterialY + "X";
		}
	}

	string get_lastMovement() {
		return last_movement;
	}

	material get_materialFromCordinates(string x, string y) {
		return getSquareFromCordinates(x[0], y[0]).materialContent;   //#dev
	}

	void remove_material(char x, char y) {
		for (int i = 0; i <= w * h; i++) {
			if ((sq[i].get_index_x() == x) && (convertCharYs2int(sq[i].get_index_y()) == int(y - 48))) {
				sq[i].materialContent.setMaterial('N', 'N', x, y);
				break;
			}
		}
	}

	void add_material(char x, char y, material m) {
		for (int i = 0; i <= w * h; i++) {
			if ((sq[i].get_index_x() == x) && (convertCharYs2int(sq[i].get_index_y()) == int(y - 48))) {
				sq[i].materialContent.setMaterial(m.getMaterialType(), m.getMaterialColor(), x, y);
				break;
			}
		}
	}

protected:
	int w, h;
	square* sq;

private:
	square getSquareFromCordinates(char x, char y) {
		square tsq;
		for (int i = 0; i <= w * h; i++) {
			if ((sq[i].get_index_x() == x) && ((convertCharYs2int(sq[i].get_index_y())) == int(y - 48))) {
				tsq = sq[i];
				break;
			}
		}
		return tsq;
	}

	string last_movement = " ";
	int move_material(string fx, string fy, string tx, string ty, string capX = " ", string capY = " ") {
		material tm = get_materialFromCordinates(tx, ty);
		material fm = get_materialFromCordinates(fx, fy);
		if ((tm.getMaterialType() == 'N') && (fm.getMaterialType() != 'N')) {
			add_material(tx[0], ty[0], fm);
			remove_material(fx[0], fy[0]);
			saveMovement(fx, fy, tx, ty, capX, capY);
			return 1;
		}
		else { return 0; }

	}

	int convertCharYs2int(char y) {
		return ((int)y - 48);
	}

	void ColorTheBoard() {
		bool isPreLineStartFromBlack = false;
		bool isNewLine = true;
		bool isCurrentSquBlack = false;
		int i = 0;
		for (int y = 1; y <= h; y++) {
			for (int x = 1; x <= w; x++) {
				if (isNewLine) {
					if (!isPreLineStartFromBlack) {
						//start put colors from black
						isCurrentSquBlack = true;
						sq[i].IsBlackSquare = true;
					}
					else {
						isCurrentSquBlack = false;
						sq[i].IsBlackSquare = false;
					}
					isPreLineStartFromBlack = !isPreLineStartFromBlack;

				}
				else {
					isCurrentSquBlack = !isCurrentSquBlack;
					sq[i].IsBlackSquare = isCurrentSquBlack;
				}
				isNewLine = false;
				i++;
			}
			isNewLine = true;
		}
	}


};
