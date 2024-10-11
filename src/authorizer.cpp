#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

class Transaction {

private:
	string card;
	string expDate;
	int CVV;
	double amount;
	double transactionId;
	string red;
	string green;
	string colorEnd;


public:
	Transaction(const Value& transaction) {
		card = transaction["card"].GetString();
		expDate = transaction["expDate"].GetString();
		CVV = transaction["CVV"].GetInt();
		amount = transaction["amount"].GetDouble();
		transactionId = transaction["transactionId"].GetDouble();
		red = "\033[31m";
		green = "\033[32m";
		colorEnd = "\033[0m";
	}

	// Validates card length
	bool validateCard() const {
		return card.length() == 16;
	}

	// Validates expiration date
	bool validateExpDate() const {
		int month, year;
		sscanf(expDate.c_str(), "%d/%d", &month, &year);

		time_t t = time(0);
		struct tm* now = localtime(&t);
		int current_month = now->tm_mon + 1;
		int current_year = now ->tm_year + 1900;
		int lastTwoDigitCurrentYear = current_year % 100;

		if (year > lastTwoDigitCurrentYear || (year == lastTwoDigitCurrentYear && month >= current_month)){
			return true;
		}
		return false;
	}

	// Validates card's CVV
	bool validateCVV() const {
		vector<int> cvvVector = {934, 237, 123, 321};

		for (int i = 0; cvvVector.size(); i++){
			if (CVV == cvvVector[i]){
				return true;
			}
		}
		return false;
	}

	// Calls validations
	bool authorizeTransaction() const {
		if (!validateCard()) {
			cout << red << "Error: Card length is not valid" << colorEnd << endl;
			return false;
		}

		if(!validateExpDate()) {
			cout << red << "Error: Card expired" << colorEnd << endl;
			return false;
		}
		if (!validateCVV()) {
			cout << red << "Error: Wrong CVV" << colorEnd << endl;
			return false;
		}

		cout << green << "Transaction authorized" << colorEnd << endl;
		return true;
	}

	// Writes to a log file
	void logTransactionOutput(ofstream& outFile, string& auth, const string& file, const string& brand) const {
		outFile << brand << " transaction with ID: " << transactionId << " - " << auth <<" \033[0m" << endl;
	}

};

class ProcessTransaction {

private:
	vector<string> jsonFiles;
	string outputFile;

public:
	ProcessTransaction(const vector<string>& _jsonFiles, const string& outputFile)
		: jsonFiles(_jsonFiles), outputFile(outputFile) {}

	void process(){
		// File object to write
		ofstream outfile(outputFile);

		if (!outfile) {
			cerr << "Error opening output file." << endl;
			return;
		}

		// Iterates through each json file(it doesn't modify files, automatically knows file type, files are references)
		for (const auto& file : jsonFiles){
			// File object to read
			ifstream inFile(file);
			if (!inFile){
				cerr << "Error opening input file: " << file << endl;
				return; 
			}
			// Saves the content of the file into "content" variable
			string content{istreambuf_iterator<char>(inFile), istreambuf_iterator<char>()};

			// RapidJSON Document
			Document transactionJson;
			// Parses JSON content
			if (transactionJson.Parse(content.c_str()).HasParseError()) {
				cerr << "Error Parsing Json in file: " << file << endl;
				continue;
			}

			// Takes brand name
			size_t lastSlash = file.find_last_of("/");
			size_t position = file.find(".");
			string brand = file.substr(lastSlash + 1, position - lastSlash - 1);
			cout << "\n\033[34m** Processing " << brand << " transactions - ";
			cout << "Number of transactions: " << transactionJson.Size() << " \033[0m**\n" << endl;

			//if it's an array, iterates through every transaction
			if (transactionJson.IsArray()){
				for (int i = 0; i < transactionJson.Size(); i++){
					const Value& transaction = transactionJson[i];
					if (transaction.HasMember("card") && transaction["card"].IsString()) {
						cout << "Card: " << transaction["card"].GetString() << endl;
					}
					if (transaction.HasMember("expDate") && transaction["expDate"].IsString()) {
						cout << "Expiry Date: " << transaction["expDate"].GetString() << endl;
					}
					if (transaction.HasMember("CVV") && transaction["CVV"].IsInt()) {
						string str_cvv = to_string(transaction["CVV"].GetInt());
						string masked_cvv(str_cvv.size(), '*');
						cout << "CVV: " << masked_cvv << endl;
					}
					if (transaction.HasMember("amount") && transaction["amount"].IsDouble()) {
						cout << "Amount: " << transaction["amount"].GetDouble() << endl;
					}
					if (transaction.HasMember("transactionId") && transaction["transactionId"].IsDouble()) {
						cout << "Transaction ID: " << transaction["transactionId"].GetDouble() << endl;
					}

					// Calls instance of Transaction class
					Transaction newTransaction(transaction);

					// Checks all validations
					bool isAuthorized = newTransaction.authorizeTransaction();
					string auth = (isAuthorized) ? "\033[32mAuthorized" : "\033[31mRejected";

					// Writes to a log output file
					newTransaction.logTransactionOutput(outfile, auth, file, brand);

					cout << "------------------------------" << endl;
					
				}
				
			} else {
				cerr << "Error: expected an array of transactions in file: " << file << endl;
			}
			inFile.close();
			cout << "Log has been created in: " << outputFile << endl;
		}
		outfile.close();
	}
};

int main(){

	// Obtains environmental variable "FTM_HOME"
	const char* env_var = getenv("FTM_HOME");
	if (env_var == nullptr) {
		cerr << "FTM_HOME variable is not defined";
		return 1;
	}

	string ftm_home = string(env_var);

	// Creates vector with json files
	vector<string> jsonFiles = {
		ftm_home + "/src/visa.json",
		ftm_home + "/src/master.json"
	};

	// Creates string with log file
	string outputFile = ftm_home + "/log/transactions.log";

	// Creates an instance of ProcessTrnasaction class
	ProcessTransaction processTran(jsonFiles, outputFile);

	// Calls procces() function
	processTran.process();

	return 0;
}


