#pragma once
#include<string>
#include<vector>
#include"square.h"
#include"board.h"


using namespace std;

class Evaluater {
public:
	Evaluater() {}
	/// <param name="evs">input->[10.0,5.1,0.1,5.5,-10,-5] output->[0,2,3,1,5,4]</param>
	/// <returns></returns>
	vector<int> getMaximumEvScoreIndexes(vector<float> evs) {
		vector<float> orderdlist;
		vector<int> indexes;
		for (float f : evs) {
			orderdlist = _insertToOder(orderdlist, f);
			indexes.push_back(0);
		}
		for (int j = 0; j < evs.size(); j++) {
			for (int i = 0; i < orderdlist.size(); i++) {
				if (evs[j] == orderdlist[i]) {
					if (isIncluded(i, indexes)) {
						continue;
					}
					indexes[j] = i;
				}
			}
		}
		return indexes;
	}



private:
	vector<float> _insertToOder(vector<float> list, float f) {
		vector<float>::iterator itr = list.begin();
		vector<float>::iterator sindex = list.begin();
		float max = 0.f;
		max = f;
		bool fIsLow = true;
		for (vector<float>::iterator itr = list.begin(); itr != list.end(); itr++)
		{
			if (*itr <= max) {
				max = *itr;
				sindex = itr;
				fIsLow = false;
				break;
			}
		}
		if (fIsLow) {
			list.push_back(f);
		}
		else {
			list.insert(sindex, f);
		}
		return list;
	}

	bool isIncluded(int number, vector<int> list) {
		bool isinclud = false;
		for (int i : list) {
			if (i == number) {
				isinclud = true;
			}
		}
		return isinclud;
	}

};


class boardState :public board {
private:
	material tuchedMat;
	string tuchedMatMovingDerection;
	bool isEveluated = false;
	float EveluatedScore = 0.f;
	const int pawnValue = 5;
	const int kingValue = 15;
	const int PositionValue = 2;
	const int isolatedPawns = 2;
	const int blockedDiagonalsValue = 2;
	const int peicesOnEdge = 1;
	const int ThreatValue = 5;

	string _getOpositDerection(string derection) {
		if (derection == "upright") {
			return "downleft";
		}
		else if (derection == "upleft") {
			return "downright";
		}
		else if (derection == "downright") {
			return "upleft";
		}
		else if (derection == "downleft") {
			return "upright";
		}
	}

	float EveluateScore(char turn) {
		float sumFinal = 0.f;
		for (int itr = 0; itr < w * h; itr++) {
			int pv = 0;
			int kv = 0;
			float SumOfpositionValues = 0.f;
			int IsolatedValue = 0;
			int blockedDiagonals = 0;
			int EdgeValue = 0;
			int threats = 0;
			float sum = 0.f;
			square s = sq[itr];
			material m;
			if (s.get_index_x() != 0) {
				m = s.materialContent;
			}
			if (m.getMaterialType() != 'N') {
				if (m.getMaterialType() == 'P') {
					pv += pawnValue;
				}
				else if (m.getMaterialType() == 'K') {
					kv += kingValue;
				}
				
				int y = stoi(string(1, s.get_index_y()));
				if (m.getMaterialColor() == 'W') {
					float positionScore = (float(y) / float(h)) * PositionValue;
					SumOfpositionValues += positionScore;
				}
				else {
					float positionScore = (float(h - y + 1) / float(h)) * PositionValue;
					SumOfpositionValues += positionScore;
				}

				int EdgesCount = 0;
				int isolatedCount = 0;
				for (string sj : {"upright", "upleft", "downright", "downleft"}) {
					square ss = checkNearestSquares(m, sj);
					if (ss.get_index_x() == 0) {
						EdgesCount += peicesOnEdge;
					}
					else if (ss.materialContent.getMaterialColor() == m.getMaterialColor()) {
						blockedDiagonals += blockedDiagonalsValue;
					}
					else if ((ss.materialContent.getMaterialColor() != m.getMaterialColor()) && (ss.materialContent.getMaterialType() != 'N')) {
						square oposs = checkNearestSquares(m,_getOpositDerection(sj));
						if (oposs.get_index_x()!=0) {
							if (oposs.materialContent.getMaterialType() == 'N') {
								if ((m.getMaterialColor()=='W')&&(turn=='W')) {
									threats -= ThreatValue;
								}
								else if ((m.getMaterialColor() == 'B') && (turn == 'B')) {
									threats -= ThreatValue;
								}
							}
						}
					}
					else if ((ss.materialContent.getMaterialType() == 'N') && (m.getMaterialType() != 'K')) {
						isolatedCount++;
					}
				}
				EdgeValue += EdgesCount;
				if (isolatedCount == 4) {
					IsolatedValue -= isolatedPawns;
				}

				sum = pv + kv + EdgeValue + blockedDiagonals + IsolatedValue + SumOfpositionValues + threats ;
				if (m.getMaterialColor() == 'B') {

					sum *= -1;
				}
				sumFinal += sum;
			}
		}
		return sumFinal;
	}

public:
	vector<boardState> childs;
	boardState* parent;
	int searchDepth = 1;
	float winingProbability = 0.f;
	bool isWiningProbabilitySet = false;
	string history;
	boardState(int w = 0, int h = 0) :board(w, h) {

	}
	boardState(board b) :board(b.getWidth(), b.getHeight(), b.getBoard()) {

	}
	//~boardState() : ~board();

	boardState operator=(const boardState& b) {
		board::operator=(b);
		tuchedMat = b.tuchedMat;
		history = b.history;
		isEveluated = false;
		return *this;
	};

	void resetData() {
		isEveluated = false;
		isWiningProbabilitySet = false;
	}

	float eveluate(char turn) {
		if (!isEveluated) {
			EveluatedScore = EveluateScore(turn);
			isEveluated = true;
			return EveluatedScore;
		}
		else {
			return EveluatedScore;
		}
	}



	void setTouchedMat(material m) {
		tuchedMat = m;
	}
	material getTouchedMat() {
		return tuchedMat;
	}
	void setTouchedMatMovingDeriction(string s) {
		tuchedMatMovingDerection = s;
	}
	string getTouchedMatMovingDerection() {
		return tuchedMatMovingDerection;
	}

};

class bot {
public:
	bot() {

	}

	bool executeCmd(board& mainboard, string Cmd) {
		int code = 1;
		string ltx = " ";
		string lty = " ";
		string tmpDecoded = "";
		vector<string> tmpcaptures;
		int char_count = 0;

		while ((char_count != Cmd.length())) {
			if (Cmd[char_count] != '+') {
				tmpDecoded += Cmd[char_count];
			}
			else {
				if (tmpDecoded.size() > 0) {
					tmpcaptures.push_back(tmpDecoded);
				}
				tmpDecoded = "";
			}
			char_count++;
		}
		if ((tmpcaptures.size() == 0) && (tmpDecoded.size() > 0)) {
			tmpcaptures.push_back(tmpDecoded);
		}
		for (auto h : tmpcaptures) {
			decodeData dta = decodecmd(h);
			code *= mainboard.move(dta.fx, dta.fy, dta.tx, dta.ty, dta.capX, dta.capY);
			ltx = dta.tx;
			lty = dta.ty;
			mainboard.remove_material(dta.capX[0], dta.capY[0]);
		}
		if (ltx != " ") {
			if ((stoi(lty) == mainboard.getHeight())) {
				material m = mainboard.get_materialFromCordinates(ltx, lty);
				if ((m.getMaterialColor() == 'W') && (m.getMaterialType() == 'P')) {
					mainboard.remove_material(m);
					material King = material();
					King.setMaterial('K', 'W', ltx[0], lty[0]);
					mainboard.add_material(ltx[0], lty[0], King);
				}
			}
			else if (stoi(lty) == 1) {
				material m = mainboard.get_materialFromCordinates(ltx, lty);
				if ((m.getMaterialColor() == 'B') && (m.getMaterialType() == 'P')) {
					mainboard.remove_material(m);
					material King = material();
					King.setMaterial('K', 'B', ltx[0], lty[0]);
					mainboard.add_material(ltx[0], lty[0], King);
				}
			}
		}
		else {
			code = 0;
		}

		if (code == 0) {
			return false;
		}
		else {
			return true;
		}
	}

	bool executeCmd(board& mainboard, boardState nextState) {
		string nsHistory = nextState.history;
		return executeCmd(mainboard, nsHistory);
	}

	struct decodeData {
		string fx = "";
		string fy = "";
		string tx = "";
		string ty = "";
		string capX;
		string capY;
	};

	decodeData decodecmd(string cmd) {
		decodeData dataToPass;
		int dotcount = 0;
		int anglebracketcount = 0;
		int index = 0;
		int thisStartIndex = 0;
		int capturedCount = 0;
		for (char c : cmd) {
			if (c == '.') {
				dotcount++;
				if (dotcount == 1) {
					dataToPass.fx = cmd.substr(0, index);
					thisStartIndex = index + 1;
				}
				else if (dotcount == 2) {
					dataToPass.tx = cmd.substr(thisStartIndex, index - thisStartIndex);
					thisStartIndex = index + 1;
				}
				else {
					dataToPass.capX = (cmd.substr(thisStartIndex, index - thisStartIndex));
					thisStartIndex = index + 1;
				}

			}
			else if (c == '>') {
				anglebracketcount++;
				if (anglebracketcount == 1) {
					dataToPass.fy = cmd.substr(thisStartIndex, index - thisStartIndex);
					thisStartIndex = index + 1;
				}

			}
			else if (c == 'X') {
				capturedCount++;
				if (capturedCount == 1) {
					dataToPass.ty = cmd.substr(thisStartIndex, index - thisStartIndex);
					thisStartIndex = index + 1;
				}
				else {
					dataToPass.capY = (cmd.substr(thisStartIndex, index - thisStartIndex));
					thisStartIndex = index + 1;
				}
			}
			index++;
		}
		return dataToPass;
	}

	vector<boardState> getPossibleMoves(material mat, boardState bod) {
		vector<boardState> vba;
		for (string sj : {"upright", "upleft", "downright", "downleft"}) {
			if (mat.getMaterialType() == 'P') {
				if ((mat.getMaterialColor() == 'B') && ((sj == "upright") || (sj == "upleft"))) {
					continue;
				}
				if ((mat.getMaterialColor() == 'W') && ((sj == "downright") || (sj == "downleft"))) {
					continue;
				}
				boardState tb = boardState(bod);
				material m = tb.get_materialFromCordinates(string(1, mat.x), string(1, mat.y));
				square s = tb.checkNearestSquares(m, sj);
				if (s.get_index_x() != 0) {
					if (tb.get_materialFromCordinates(string(1, s.get_index_x()), string(1, s.get_index_y())).getMaterialType() == 'N') {
						string fx = string(1, m.x);
						string fy = string(1, m.y);
						string tx = string(1, s.get_index_x());
						string ty = string(1, s.get_index_y());
						tb.saveMovement(fx, fy, tx, ty);
						tb.history = (tb.get_lastMovement());
						tb.move(fx, fy, tx, ty);
						vba.push_back(tb);
					}
				}
			}
			else if (mat.getMaterialType() == 'K') {
				boardState tb = boardState(bod);
				material m = tb.get_materialFromCordinates(string(1, mat.x), string(1, mat.y));
				square s = tb.checkNearestSquares(m, sj);
				while (s.get_index_x() != 0) {
					if (tb.get_materialFromCordinates(string(1, s.get_index_x()), string(1, s.get_index_y())).getMaterialType() == 'N') {
						tb = boardState(bod);
						m = tb.get_materialFromCordinates(string(1, mat.x), string(1, mat.y));
						string fx = string(1, m.x);
						string fy = string(1, m.y);
						string tx = string(1, s.get_index_x());
						string ty = string(1, s.get_index_y());
						tb.saveMovement(fx, fy, tx, ty);
						tb.history = (tb.get_lastMovement());
						tb.move(fx, fy, tx, ty);
						vba.push_back(tb);
						s = tb.checkNearestSquares(s.get_index_x(), s.get_index_y(), sj);
					}
					else {
						break;
					}
				}
			}

		}
		return vba;
	}

	vector<boardState> possibleCapturesForPawn(material mat, boardState bod) {
		vector<boardState> boardSeqArr = _possibleCapturesForPawn(mat, bod);
		if (boardSeqArr.size() > 0) {
			vector<boardState> tmp;
			for (auto a : boardSeqArr) {
				vector<boardState> tmp1 = possibleCapturesForPawn(a.getTouchedMat(), a);
				tmp.insert(tmp.end(), tmp1.begin(), tmp1.end());
			}
			if (tmp.size() == 0) {
				tmp.insert(tmp.end(), boardSeqArr.begin(), boardSeqArr.end());
			}
			return tmp;
		}
		else {
			return boardSeqArr;
		}
	}

	void visualiseCaptues(vector<boardState> boardX, boardState bassBoard) {
		for (boardState bd : boardX) {
			bassBoard.printBoard();

			cout << bd.history << endl;

			bd.printBoard();
			cout << endl << endl;
		}
	}

	vector<boardState> possibleCapturesForKing(material mat, boardState bod, string movingDerection = "Unknown") {
		vector<boardState> boardSeqArr = _possibleCapturesForKing(mat, bod, movingDerection);
		if (boardSeqArr.size() > 0) {
			vector<boardState> tmp;
			for (auto a : boardSeqArr) {
				vector<boardState> tmp1 = possibleCapturesForKing(a.getTouchedMat(), a, a.getTouchedMatMovingDerection());
				tmp.insert(tmp.end(), tmp1.begin(), tmp1.end());
			}
			if (tmp.size() == 0) {
				tmp.insert(tmp.end(), boardSeqArr.begin(), boardSeqArr.end());
			}
			return tmp;
		}
		else {
			return boardSeqArr;
		}
	}

	vector<boardState> allPossibilities(char turn, board mainboard, /*bot checkerPlayer,*/ bool mustcapture = true, bool mustCaptureMaximumPieces = true) {
		square* ss = mainboard.getBoard();
		vector<boardState> tempboardMoves;
		vector<boardState> tempboardCap;
		int indexCounter = 0;
		int detectedMaxCaptureCount = 0;
		vector<int> maxCaptureMovementIndex;
		for (int i = 0; i < mainboard.getWidth() * mainboard.getHeight(); i++) {
			material m = ss[i].materialContent;
			if (m.getMaterialColor() == turn) {
				if (m.getMaterialType() == 'P') {
					boardState bss = boardState(mainboard);
					vector<boardState> tempboard0 = possibleCapturesForPawn(m, bss);
					if (tempboard0.size() > 0) {

						for (int j = 0; j < tempboard0.size(); j++) {
							int captures = tempboard0[j].history.size();
							if (detectedMaxCaptureCount < captures) {
								detectedMaxCaptureCount = captures;
								maxCaptureMovementIndex.clear();
								maxCaptureMovementIndex.push_back(indexCounter);
							}
							else if (detectedMaxCaptureCount == captures) {
								maxCaptureMovementIndex.push_back(indexCounter);
							}

							tempboardCap.insert(tempboardCap.end(), tempboard0.begin(), tempboard0.end());
							indexCounter++;
						}
					}
				}
				else if (m.getMaterialType() == 'K') {
					boardState bss = boardState(mainboard);
					vector<boardState> tempboard0 = possibleCapturesForKing(m, bss);
					if (tempboard0.size() > 0) {
						for (int j = 0; j < tempboard0.size(); j++) {
							int captures = tempboard0[j].history.size();
							if (detectedMaxCaptureCount < captures) {
								detectedMaxCaptureCount = captures;
								maxCaptureMovementIndex.clear();
								maxCaptureMovementIndex.push_back(indexCounter);
							}
							else if (detectedMaxCaptureCount == captures) {
								maxCaptureMovementIndex.push_back(indexCounter);
							}

							tempboardCap.insert(tempboardCap.end(), tempboard0.begin(), tempboard0.end());
							indexCounter++;
						}
					}
				}
				if (m.getMaterialType() != 'N') {
					boardState bss = boardState(mainboard);
					vector<boardState> tempboard0 = getPossibleMoves(m, bss);
					if (tempboard0.size() > 0) {
						tempboardMoves.insert(tempboardMoves.end(), tempboard0.begin(), tempboard0.end());
					}
				}
			}
		}
		vector<boardState> tempboardCap0;
		if (mustCaptureMaximumPieces && (indexCounter > 0)) {

			for (int i = 0; i < tempboardCap.size(); i++) {
				for (int itr = 0; itr < maxCaptureMovementIndex.size(); itr++) {
					if (i == maxCaptureMovementIndex[itr]) {
						tempboardCap0.push_back(tempboardCap[maxCaptureMovementIndex[itr]]);
					}
				}
			}
		}
		else {
			tempboardCap0 = tempboardCap;
		}

		if (mustcapture) {
			if (tempboardCap0.size() > 0) {
				return tempboardCap0;
			}
			else {
				return tempboardMoves;
			}
		}
		else {
			tempboardMoves.insert(tempboardMoves.end(), tempboardCap0.begin(), tempboardCap0.end());
			return tempboardMoves;
		}

	}

	const int MaximumSearchDepth = 7;
	const int MaxSearchLines = 4;
	int _goToDepth(boardState* bs, char turn, bool isFirstCall = true) {
		vector<int> selectedIndexes;
		//turn = GetTheOponentColor(turn);
		Evaluater E;
		vector<boardState> aps;
		if (bs->searchDepth <= MaximumSearchDepth) {
			aps = allPossibilities(turn, *bs);
			vector<float> evs;
			for (int jj = 0; jj < aps.size();jj++) {
				evs.push_back(aps[jj].eveluate(turn));
			}
			vector<int> allIndexes = E.getMaximumEvScoreIndexes(evs);
			int itr = 0;
			for (int x : allIndexes) {
				if (turn == 'W') {
					if (x < MaxSearchLines) {
						selectedIndexes.push_back(itr);
					}
				}
				else {
					int tmpint = 0;
					if (allIndexes.size() < MaxSearchLines) {
						tmpint = -1;
					}
					else {
						tmpint = (allIndexes.size() - MaxSearchLines - 1);
					}
					if (tmpint < x) {
						selectedIndexes.push_back(itr);
					}
				}
				itr++;
			}
			turn = GetTheOponentColor(turn);
			for (int x : selectedIndexes) {
				aps[x].parent = bs;
				aps[x].searchDepth = aps[x].parent->searchDepth + 1;
				_goToDepth(&aps[x], turn, false);
				bs->childs.push_back(aps[x]);
			}

		}
		else {
			return 0;
		}

		if (isFirstCall && (aps.size() > 0)) {
			char turn0 = turn;
			while (!bs->isWiningProbabilitySet) {
				boardState* tb = bs;
				bool loop = true;
				while (loop) {

					//set maximum iteration count to 3 if there are more than 3 posibilities
					int loopcounts = MaxSearchLines;
					if (tb->childs.size() < MaxSearchLines) {
						loopcounts = tb->childs.size();
					}

					//check each childs' wining probability is set
					for (int i = 0; i < loopcounts; i++) {
						if ((tb->childs[i].childs.size() == 0)) {
							if (!tb->childs[i].isWiningProbabilitySet) {
								tb->childs[i].winingProbability = tb->childs[i].eveluate(GetTheOponentColor(turn0));
								tb->childs[i].isWiningProbabilitySet = true;
								loop = false;
							}
						}
						if (!tb->childs[i].isWiningProbabilitySet) {
							tb = &tb->childs[i];
							turn0 = GetTheOponentColor(turn0);
							break;
						}

						else {
							//set the parent wining probability if every child has already set the wining probabilities
							if (i == loopcounts - 1) {
								float sum = 0.f;
								for (boardState ttb : tb->childs) {
									sum += ttb.winingProbability;
								}
								sum /= tb->childs.size();
								tb->winingProbability = sum;
								tb->isWiningProbabilitySet = true;
								loop = false;
								break;
							}
							else {
								continue;
							}
						}

					}
				}
			}
		}
		return 0;
	}

	boardState searchToDepth(boardState bs, char turn,bool* isGameOver) {
		_goToDepth(&bs, turn);
		vector<float> list;
		for (boardState b : bs.childs) {
			list.push_back(b.winingProbability);
		}
		if(bs.childs.size()==0){
			*isGameOver = true;
		}
		Evaluater E;
		vector<int> il = E.getMaximumEvScoreIndexes(list);
		int index = 0;
		boardState nextmove;
		int comparisonint = 0;
		if (turn == 'B') {
			comparisonint = il.size() - 1;
		}
		for (int k : il) {
			if (k == comparisonint) {
				nextmove = bs.childs[index];
				nextmove.winingProbability = bs.childs[index].winingProbability;
				break;
			}
			index++;
		}

		//debug code
	/*	for (boardState b:bs.childs) {
			b.printBoard("                       ");
			cout<<"          " << b.winingProbability << endl;
		}*/
		return nextmove;
	}

	void tempory() {
		Evaluater E;
		boardState myboard = board(8, 8);
		vector<string> s = {"BK","WP","WP","WP"};
		vector<int> i = {0,18,22,15};
		myboard.setupTroopsCustomOrder(i,s);
		bool isover = false;
		myboard.printBoard();
		boardState f1 = searchToDepth(myboard, 'W', &isover);
		f1.printBoard();
	}

	void gameHandle(char humenPlayerColor) {
		//tempory();
		board myboard = board(8, 8);
		Evaluater E;
		myboard.readyTroops(3);
		myboard.printBoard();
		bool isGameOver = false;

		char turn = 'B';
		while (!isGameOver) {
			if (turn == humenPlayerColor) {
				string hinput = "";
				vector<boardState> tbss = allPossibilities(turn, myboard);
				if(tbss.size()==0){
					isGameOver = true;
					break;
				}
				cout << "Enter the Movement Code: " << endl;
				bool isValiedMove = false;
				bool preValidation = false;
				for (int q = 0; q < 5; q++) {
					

					//debug data
					if (tbss.size() > 0) {
						for (auto tmpboard : tbss) {
							cout << tmpboard.history << endl;
						}
						
					}

					cin >> hinput;
					for (auto bodSt : tbss) {
						if (bodSt.history == hinput) {
							preValidation = true;
						}
					}
					if (preValidation) {
						isValiedMove = executeCmd(myboard, hinput);
						if (isValiedMove) {
							myboard.printBoard();
							break;
						}
					}
					if (!isValiedMove) {
						cout << "invalied movement" << endl;
					}
				}
			}
			else {
				boardState bsb = boardState(myboard);
				boardState exeBoard = searchToDepth(bsb, turn,&isGameOver);
				if (isGameOver) {
					break;
				}
				else {
					executeCmd(myboard, exeBoard);
				}
				myboard.printBoard();
				cout << (bsb.eveluate(turn)) << endl << endl;

			}
			turn = GetTheOponentColor(turn);

		}
		if (turn == 'B') {
			cout << "White won\n";
		}
		else {
			cout << "Black won\n";
		}
	}

private:
	char GetTheOponentColor(char turn) {
		if (turn == 'B') {
			return 'W';
		}
		else {
			return 'B';
		}
	}
	vector<boardState> _possibleCapturesForKing(material mat, boardState bod, string movingDerection) {
		boardState tb = boardState(bod);
		material m = tb.get_materialFromCordinates(string(1, mat.x), string(1, mat.y));
		vector<boardState> vba;

		for (string sj : {"upright", "upleft", "downright", "downleft"}) {
			if (movingDerection == _getOpositDerection(sj)) {
				continue;
			}
			square nsq = tb.checkNearestSquares(m, sj);
			bool run = true;
			while ((nsq.get_index_x() != 0) && (run)) {
				if (((nsq.materialContent.getMaterialType() == 'P') || (nsq.materialContent.getMaterialType() == 'K')) && (nsq.materialContent.getMaterialColor() != mat.getMaterialColor())) {
					square nsq2 = tb.checkNearestSquares(nsq.get_index_x(), nsq.get_index_y(), sj);
					while (nsq2.get_index_x() != 0) {
						if (nsq2.materialContent.getMaterialColor() != mat.getMaterialColor()) {}
						if (nsq2.materialContent.getMaterialType() == 'N') {
							tb = boardState(bod);
							m = tb.get_materialFromCordinates(string(1, mat.x), string(1, mat.y));
							tb.move(string(1, m.x), string(1, m.y), string(1, nsq2.get_index_x()), string(1, nsq2.get_index_y()), string(1, nsq.get_index_x()), string(1, nsq.get_index_y()));
							tb.remove_material(nsq.materialContent);
							tb.saveMovement(string(1, m.x), string(1, m.y), string(1, nsq2.get_index_x()), string(1, nsq2.get_index_y()), string(1, nsq.get_index_x()), string(1, nsq.get_index_y()));
							tb.history += (tb.get_lastMovement() + "+");
							tb.setTouchedMat(tb.get_materialFromCordinates(string(1, nsq2.get_index_x()), string(1, nsq2.get_index_y())));
							tb.setTouchedMatMovingDeriction(sj);
							vba.push_back(tb);
							nsq2 = tb.checkNearestSquares(nsq2.get_index_x(), nsq2.get_index_y(), sj);
						}
						else {
							run = false;
							break;
						}
					}
				}
				nsq = tb.checkNearestSquares(nsq.get_index_x(), nsq.get_index_y(), sj);
			}
		}
		return vba;
	}

	string _getOpositDerection(string derection) {
		if (derection == "upright") {
			return "downleft";
		}
		else if (derection == "upleft") {
			return "downright";
		}
		else if (derection == "downright") {
			return "upleft";
		}
		else if (derection == "downleft") {
			return "upright";
		}
	}

	vector<boardState> _possibleCapturesForPawn(material mat, boardState bod) {
		boardState tb = boardState(bod);
		material m = tb.get_materialFromCordinates(string(1, mat.x), string(1, mat.y));
		vector<boardState> vba;
		for (string sj : {"upright", "upleft", "downright", "downleft"})
		{
			square nearSq = tb.checkNearestSquares(m, sj);
			square nextSq = tb.checkNearestSquares(nearSq.materialContent, sj);
			if ((nearSq.materialContent.getMaterialType() != 'N') && (nextSq.get_index_x() != 0) && (nearSq.get_index_x() != 0) && (nextSq.materialContent.getMaterialType() == 'N') && (nearSq.materialContent.getMaterialColor() != m.getMaterialColor())) {
				tb = boardState(bod);
				string fx = string(1, m.x);
				string fy = string(1, m.y);
				string tx = string(1, nextSq.get_index_x());
				string ty = string(1, nextSq.get_index_y());
				string CapX = string(1, nearSq.get_index_x());
				string CapY = string(1, nearSq.get_index_y());
				tb.move(fx, fy, tx, ty);
				tb.remove_material(nearSq.materialContent);
				//material captured
				tb.saveMovement(fx, fy, tx, ty, CapX, CapY);
				tb.history += tb.get_lastMovement() + "+";
				tb.setTouchedMat(tb.get_materialFromCordinates(tx, ty));
				vba.push_back(tb);
			}
		}
		return vba;
	}
};

