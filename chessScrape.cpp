#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

template<typename Map>
void print_map(Map& m)
{
   std::cout << '{';
   for(auto& p: m)
        std::cout << p.first << ':' << p.second << ' ';
   std::cout << "}\n";
}

struct Game {

	int result;
	int record;
	string opening;
	string playerWhite;
	string playerBlack;
	int whiteElo;
	int blackElo;
	int numMoves;
	
	Game(string playerWhite, string playerBlack, int whiteElo, int blackElo, int record, string opening, int numMoves, int result) {

		this->result = result;
		this->opening = opening;
		this->whiteElo = whiteElo;
		this->blackElo = blackElo;	
		this->record = record;
		this->playerWhite = playerWhite;
		this->playerBlack = playerBlack;
		this->numMoves = numMoves;

	}

};

struct Player {

	string name;
	map<string, int> opponentMap;

	Player(string name) {
		this->name = name;
	}

};

string parseRecord(string input, string delimiterBegin, string delimiterEnd) {

	string token = input.substr(input.find(delimiterBegin), input.find(delimiterEnd));
	token = token.substr(2,token.length()-4);
	return token;

}

vector<string> split(string str, char delimiter) {

	vector<string> internal;
	stringstream ss(str);
	string tok;
	while(getline(ss,tok,delimiter)) {
		internal.push_back(tok);
	}
	return internal;
}

string returnGameKey(string str) {
	string gameKey;
	vector<string> fields = split(str, ',');
	gameKey = fields.at(2);
	gameKey = gameKey + fields.at(3);
	gameKey = gameKey + fields.at(5);
	gameKey = gameKey + fields.at(6);
	return gameKey;
}

int main(int argc, char* argv[]) {
	ifstream file(argv[1]);
	ifstream names("names.txt");
	ifstream gamesFile("gamesFile.csv");

	ofstream log;
	log.open("logFile.txt", ios::app);

	string str;
	string token;
	string opening;
	string delimiterBegin;
	string delimiterEnd;
	string nameWhite;
	string nameBlack;
	string playerToCheck;
	int eloWhite;
	int eloBlack;
	int record;
	int result;
	int numMoves;
	vector<Game*> games;
	bool validGame;

	string gameKey;
	set<string> gameSet;

	while (getline(gamesFile,str)) {
		gameKey = returnGameKey(str);
		gameSet.insert(gameKey);
	}

	gamesFile.close();

	string inputString(argv[1]);
	string name = inputString.substr(31, inputString.length()-35);
	cout << name << endl;
	for (unsigned i = 1; i < name.length(); ++i) {
		if (isupper(name.at(i))) {
			name = name.substr(i, name.length()-i) + ", " + name.substr(0,i);
			cout << name << endl;
			break;
		}
	}
	Player currentPlayer = Player(name);

	set<string> setOfPlayers;
	while (getline(names,str)) {
		setOfPlayers.insert(str);
	}

	names.close();

	while (getline(file,str)) {

		delimiterBegin = " \"";
		delimiterEnd = "\"]";
		nameWhite = parseRecord(str, delimiterBegin, delimiterEnd);
		getline(file,str);
		nameBlack = parseRecord(str, delimiterBegin, delimiterEnd);
		getline(file,str);
		eloWhite = stoi(str.substr(11, str.length()-13));
		getline(file,str);
		eloBlack = stoi(str.substr(11, str.length()-13));
		getline(file,str);
		token = parseRecord(str, delimiterBegin, delimiterEnd);
		token = token.substr(0, token.length());
		
		if (token == "0-1") {
			result = -1;
		}
		else if(token == "1-0") {
			result = 1;
		}
		else {
			result = 0;
		}
		if (name == nameWhite) {
			playerToCheck = nameBlack;
			auto search = currentPlayer.opponentMap.find(nameBlack);
			if (search != currentPlayer.opponentMap.end()) {
				record = currentPlayer.opponentMap[nameBlack];
				currentPlayer.opponentMap[nameBlack] = currentPlayer.opponentMap[nameBlack] + result;
			}
			else {
				record = 0;
				currentPlayer.opponentMap[nameBlack] = result;
			}
		}
		else {	
			playerToCheck = nameWhite;
			auto search = currentPlayer.opponentMap.find(nameWhite);
			if (search != currentPlayer.opponentMap.end()) {
				record = -1*currentPlayer.opponentMap[nameWhite];
				currentPlayer.opponentMap[nameWhite] = currentPlayer.opponentMap[nameWhite] - result;
			}
			else {
				record = 0;
				currentPlayer.opponentMap[nameBlack] = -1*result;
			}
		}

		getline(file,str);
		getline(file,str);

		if (str.length() > 1) {
			token = str.substr(3, str.length());
			delimiterEnd = "2";
			opening = token.substr(0, token.find(delimiterEnd)-1);
			vector<string> sep = split(str, ' ');
			numMoves = sep.size()/3;
			validGame = true;
		}
		else {
			validGame = false;
		}
		getline(file,str);
		getline(file,str);
		getline(file,str);
		getline(file,str);
		auto search = setOfPlayers.find(playerToCheck);
		size_t found1 = nameWhite.find(",");
		size_t found2 = nameBlack.find(",");
		if (found1 != string::npos) {
			nameWhite.erase(found1, 1);
		}
		if (found2 != string::npos) {
			nameBlack.erase(found2, 1);
		}
		gameKey = to_string(eloWhite) + to_string(eloBlack) + opening + to_string(numMoves);
		auto searchGame = gameSet.find(gameKey);
		if (searchGame != gameSet.end()) {
			log << nameWhite + nameBlack + to_string(eloWhite) + to_string(eloBlack) + opening + to_string(numMoves) + to_string(result) << endl;
		}
		if (min(eloWhite, eloBlack) > 2600 && search == setOfPlayers.end() && searchGame == gameSet.end() && validGame) {

			games.push_back(new Game(nameWhite, nameBlack, eloWhite, eloBlack, record, opening, numMoves, result));

		}
	}
	file.close();

	ofstream outputFile;
	outputFile.open("gamesFile.csv", ios::app);
	for (vector<Game*>::iterator it = games.begin(); it != games.end(); ++it) {
		outputFile << (*it)->playerWhite << "," << (*it)->playerBlack << "," << (*it)->whiteElo << "," << (*it)->blackElo << "," << (*it)-> record << "," << (*it)->opening << "," << (*it)->numMoves << "," << (*it)->result << endl;
	}
	ofstream out("names.txt", ios::app);
	out << name << endl;
	vector<string> nameSeparation = split(name, ' ');
	name = nameSeparation.at(0) + '-' + nameSeparation.at(1);
}




