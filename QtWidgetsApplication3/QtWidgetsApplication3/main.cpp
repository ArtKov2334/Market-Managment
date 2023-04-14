#include "QtWidgetsApplication3.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QListWidget>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QFile>
#include <QTableWidget>
#include <QComboBox>
#include <QTimer>
#include <QSizePolicy>
#include <QFileDialog>
#include <QDir>
#include <QVector>
#include <QPrinter>
#include "qcustomplot.h"

#include <fstream>
#include <iostream>
#include <vector>
#include "windows.h"
#include <ctime>
#include <thread>
#include <sstream>
#include <time.h>

#include <curl/curl.h>

void run(int msec, QLabel* dt);

using namespace std;

class DataTime
{
public:

	string currentDateTime() {
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);

		char buffer[128];
		strftime(buffer, sizeof(buffer), "%d-%m--%Y %X", now);
		return buffer;
	}

	string currentData()
	{
		char buffer[128];
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);

		strftime(buffer, sizeof(buffer), "%d-%m-%Y", now);
		return buffer;
	}

	string currentMonth_and_Year()
	{
		char buffer[128];
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);

		strftime(buffer, sizeof(buffer), "%m-%Y", now);
		return buffer;
	}

	string currentDay()
	{
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);

		char buffer[128];
		strftime(buffer, sizeof(buffer), "%d", now);
		return buffer;
	}

	string currentTime()
	{
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);

		char buffer[128];
		strftime(buffer, sizeof(buffer), "%X", now);
		return buffer;
	}

	string getDateBefore(const std::tm& date) {
		std::time_t time = std::mktime(const_cast<std::tm*>(&date));
		const std::time_t oneDay = 24 * 60 * 60;
		std::time_t nextDay = time - oneDay;
		std::tm* nextDate = std::localtime(&nextDay);
		string next = nextDate->tm_mday + "-" + nextDate->tm_mon + '-' + nextDate->tm_year;
		return next;
	}

	string getDateAfter(const std::tm& date) {
		std::time_t time = std::mktime(const_cast<std::tm*>(&date)); 
		const std::time_t oneDay = 24 * 60 * 60; 
		std::time_t nextDay = time + oneDay; 
		std::tm* nextDate = std::localtime(&nextDay); 
		string next = nextDate->tm_mday + "-" + nextDate->tm_mon + '-' + nextDate->tm_year;
		return next;
	}

	string get_previous_month(string date_str) {
		istringstream iss(date_str);
		string month_str, year_str;
		getline(iss, month_str, '-');
		getline(iss, year_str);

		int month = stoi(month_str);
		int year = stoi(year_str);

		month -= 1;
		if (month == 0) {
			month = 12;
			year -= 1;
		}

		ostringstream oss;
		oss << setw(2) << setfill('0') << month << "-" << year;
		return oss.str();
	}

	string get_next_month(string date_str) {
		istringstream iss(date_str);
		string month_str, year_str;
		getline(iss, month_str, '-');
		getline(iss, year_str);

		int month = stoi(month_str);
		int year = stoi(year_str);

		month += 1;
		if (month == 13) {
			month = 1;
			year += 1;
		}

		ostringstream oss;
		oss << setw(2) << setfill('0') << month << "-" << year;
		return oss.str();
	}

};

class Categories
{
public:
	Categories() {
		// Initialize libcurl
		curl_global_init(CURL_GLOBAL_ALL);

		// Set up the HTTP GET request to retrieve all categories
		CURL* curl = curl_easy_init();
		if (curl) {
			// Set the API endpoint URL
			curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/categories");

			// Set the request headers
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			// Set the response body callback function
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Categories::writeCallback);

			// Set the response body buffer
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

			// Perform the HTTP request
			CURLcode res = curl_easy_perform(curl);

			// Check for errors
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}

			// Clean up
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}

		// Parse the response body and extract the categories
		QJsonDocument json_doc = QJsonDocument::fromJson(response_body.c_str());
		QJsonArray json_array = json_doc.array();
		for (int i = 0; i < json_array.size(); i++) {
			QJsonObject category_obj = json_array[i].toObject();
			//QString category_name = category_obj["name"].toString();
			categories.push_back(category_obj);
		}
	}

	Categories(string id) {
		// Initialize libcurl
		curl_global_init(CURL_GLOBAL_ALL);

		// Set up the HTTP GET request to retrieve all categories
		CURL* curl = curl_easy_init();
		if (curl) {
			// Set the API endpoint URL
			curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/categories/");

			// Set the request headers
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

			// Set the response body buffer
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

			// Perform the HTTP request
			CURLcode res = curl_easy_perform(curl);

			// Check for errors
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}

			// Clean up
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}

		// Parse the response body and extract the categories
		QJsonDocument json_doc = QJsonDocument::fromJson(response_body.c_str());
		QJsonArray json_array = json_doc.array();
		for (int i = 0; i < json_array.size(); i++) {
			QJsonObject category_obj = json_array[i].toObject();
			//QString category_name = category_obj["name"].toString();
			categories.push_back(category_obj);
		}
	}

	std::vector<QJsonObject> getCategories() {
		return categories;
	}

private:
	std::vector<QJsonObject> categories;
	std::string response_body;

	static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
		std::string* response_body = reinterpret_cast<std::string*>(userdata);
		size_t num_bytes = size * nmemb;
		response_body->append(ptr, num_bytes);
		return num_bytes;
	}
};

class User
{
public:
	User(const QString& login, const QString& password)
	{
		std::string loginStd = login.toStdString();
		std::string passwordStd = password.toStdString();

		std::ifstream logFile("Full_log.txt");
		if (!logFile.is_open())
		{
			QMessageBox::warning(nullptr, "Error", "Failed to open Full_log.txt");
			return;
		}

		std::string line;
		while (std::getline(logFile, line))
		{
			auto pos = line.find(":");
			if (pos == std::string::npos)
			{
				QMessageBox::warning(nullptr, "Error", "Unexpected format in Full_log.txt");
				logFile.close();
				return;
			}
			auto pos_t = line.find("-");

			std::string fLogin = line.substr(0, pos);
			size_t start_pos = line.find(":") + 1;
			size_t end_pos = line.find("-", start_pos);
			string fPassword = line.substr(start_pos, end_pos - start_pos - 1);
			string position = line.substr(pos_t + 1);
			duty = position;
			if (loginStd == fLogin && passwordStd == fPassword)
			{
				isAuth = true;
				break;
			}
		}

		logFile.close();
	}

	User()
	{

	}

	bool isAuthorized()
	{
		return isAuth;
	}

	string WhoisAuthorized() const
	{
		return duty;
	}

	vector<string> GetUsers()
	{
		ifstream logFile("Full_log.txt");
		if (!logFile.is_open())
		{
			QMessageBox::warning(nullptr, "Error", "Failed to open Full_log.txt");
		}

		else {
			string line;
			while (std::getline(logFile, line))
			{
				auto pos = line.find(":");
				auto pos_t = line.find("-");

				std::string fLogin = line.substr(0, pos);
				users.push_back(fLogin);
				string duty_ = line.substr(pos_t + 1);
				users.push_back(duty_);
			}
		}
		return users;
	}

private:
	bool isAuth = false;
	string duty = "";
	vector <string> users;
};

class API {
public:
	API(const std::string& base_url) : base_url_(base_url) {
		curl_global_init(CURL_GLOBAL_ALL);
		curl_ = curl_easy_init();
	}

	~API() {
		curl_easy_cleanup(curl_);
		curl_global_cleanup();
	}


	// Create a new resource
	void create(const std::string& path, const std::string& data) {
		std::string url = base_url_ + path;
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
		perform_request();
		curl_slist_free_all(headers);
	}

	// Read a single resource
	vector<QJsonObject> readSingle(const std::string& id) {


		string response_body;
		vector<QJsonObject> categories;

		CURL* curl = curl_easy_init();
		if (curl) {
			// Set the API endpoint URL
			string url = "http://localhost" + base_url_ + id;
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			//QMessageBox::warning(nullptr, "Error", url.c_str());

			// Set the request headers
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			// Set the request method to GET
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

			// Set the callback function to handle the response data
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

			// Set the pointer to the response string
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

			// Perform the request
			CURLcode res = curl_easy_perform(curl);

			// Check for errors
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}

			// Clean up
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}

		// Parse the response body and extract the category object and products array
		QJsonDocument json_doc = QJsonDocument::fromJson(response_body.c_str());
		QJsonObject json_obj = json_doc.object();
		QJsonArray json_array = json_obj["products"].toArray();
		for (int i = 0; i < json_array.size(); i++) {
			QJsonObject product_obj = json_array[i].toObject();
			categories.push_back(product_obj);
		}

		return categories;
	}

	// Update a resource
	void update(const std::string& path, const std::string& data) {
		std::string url = base_url_ + path;
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
		perform_request();
		curl_slist_free_all(headers);
	}

	// Delete a resource
	void del(const std::string& path) {
		std::string url = base_url_ + path;
		set_options(url);
		curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
		perform_request();

	}

private:
	std::string base_url_;
	CURL* curl_;
	std::string response_;

	// Set options for the request
	void set_options(const std::string& url) {
		curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_);
	}

	// Perform the request and check for errors
	void perform_request() {
		CURLcode res = curl_easy_perform(curl_);

		if (res != CURLE_OK) {
			std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
		}
	}

	// A callback function to receive the response data
	static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
		std::string* response = static_cast<std::string*>(userdata);
		response->append(ptr, size * nmemb);
		return size * nmemb;
	}
};

class Admin : public User
{
public:
	using User::User;

	vector<QJsonObject> ReadCategories()
	{
		Categories categories;
		vector<QJsonObject> categoriesList = categories.getCategories();
		return categoriesList;
	}

	void NewCategory(string name)
	{
		API api("http://localhost");
		api.create("/categories", "{\"name\":\"" + name + "\"}");
	}

	void DeleteCategory(string id)
	{
		API api("http://localhost");
		api.del("/categories/" + id);
	}

	void Update_Category(string name, string id)
	{
		API api("http://localhost");
		api.update("/categories/" + id, "{\"name\":\"" + name + "\"}");
	}

	vector<QJsonObject> ReadSingleCategory(string id)
	{
		API api("/categories/");
		vector<QJsonObject> single_cat = api.readSingle(id);
		return single_cat;
	}

	void NewProduct(string name, string price, string id_c)
	{
		API api("http://localhost");
		api.create("/products", "{\"name\":\"" + name + "\",\"price\":\"" + price + "\",\"id_category\":\"" + id_c + "\"}");
	}

	void DeleteProduct(string id)
	{
		API api("http://localhost");
		api.del("/products/" + id);
	}

	void UpdateProduct(string name, string id_c, string price, string id)
	{
		API api("http://localhost");
		api.update("/products/" + id, "{\"name\":\"" + name + "\",\"price\":\"" + price + "\",\"id_category\":\"" + id_c + "\"}");
	}
};

class string_manager
{
public:

	string get_date(string str)
	{
		auto pos_ = str.find_first_of(":");
		auto pos__ = str.find("T");

		string date = str.substr(pos_ + 1, pos__ - pos_ - 2);

		return date;
	}

	string get_month(string str)
	{
		auto pos_ = str.find_first_of("-");
		auto pos__ = str.find("T");

		string date = str.substr(pos_ + 1, pos__ - pos_ - 2);

		return date;
	}

	string get_day(string str)
	{
		auto pos_ = str.find_first_of(":");
		auto pos__ = str.find_first_of("-");

		string date = str.substr(pos_ + 1, pos__ - pos_ - 1);

		return date;
	}

	string get_time(string str)
	{
		string hour;
		auto pos = str.find("T:");
		if (pos != string::npos)
		{
			string time = str.substr(pos + 2);
			auto time_pos = time.find_first_of(":");

			hour = time.substr(0, time_pos);
		}
		return hour;
	}

	string get_price(string str)
	{
		auto pos = str.find("$");
		auto pos_ = str.find("(");

		string price = str.substr(pos_ + 1, pos - pos_ - 1);
		
		return price;
	}

	string get_amount(string str)
	{
		auto pos = str.find("*");
		auto pos_ = str.find("\t");

		string amount = str.substr(pos + 1, pos_ - pos - 1);

		return amount;
	}
};

class FileMananger
{
public:
	vector <string> get_products_from_cart(string user_name)
	{
		ifstream cart_file(folder_path + user_name + ".txt");
		vector <string> products;
		string product;
		while (!cart_file.eof())
		{
			getline(cart_file, product);
			if(product != "" || product != "\n")
			{
				products.push_back(product);
			}
		}
		return products;
	}

	vector <string> bought_by_day(string day_month_year)
	{
		vector <string> result;
		vector <string> hours;
		vector <string> prices;
		vector <string> amounts;
		ifstream file_s(buy_log_path);
		string str;
		string hour;
		string price;
		string amount;
		string date;
		string_manager sm;
		while (!file_s.eof())
		{
			getline(file_s, str);
			if (str != "" and str != "\n")
			{
				date = sm.get_date(str);
				if (stoi(date) == stoi(day_month_year))
				{
					price = sm.get_price(str);
					amount = sm.get_amount(str);
					hour = sm.get_time(str);

					hours.push_back(hour);
					prices.push_back(price);
					amounts.push_back(amount);
				}
			}
		}

		hours.push_back("60");
		prices.push_back("60");
		amounts.push_back("60");
		string data;

		if (hours.size() > 1)
		{
			int h = 1;
			float sum = stof(amounts[0]) * stof(prices[0]);
			while (true)
			{
				float sum_ = stof(amounts[h]) * stof(prices[h]);

				if (hours[h] == hours[h - 1])
				{
					sum += sum_;
				}

				else
				{
					data = hours[0] + "," + to_string(sum);
					result.push_back(data);
					break;
				}

				h++;
			}

			sum = stof(amounts[h]) * stof(prices[h]);
			h++;

			for (; h < hours.size(); h++)
			{
				float sum_ = stof(amounts[h]) * stof(prices[h]);
				if (hours[h] == hours[h - 1])
				{
					sum += sum_;
				}

				else
				{
					data = hours[h - 1] + "," + to_string(sum);
					sum = sum_;
					result.push_back(data);
				}
			}
		}
		return result;
	}

	vector <string> bought_by_month(string month_year)
	{
		vector <string> result;
		vector <string> days;
		vector <string> prices;
		vector <string> amounts;
		ifstream file_s(buy_log_path);
		string str;
		string day;
		string price;
		string amount;
		string date;
		string_manager sm;
		while (!file_s.eof())
		{
			getline(file_s, str);
			if (str != "" and str != "\n")
			{
				date = sm.get_month(str);
				if (stoi(date) == stoi(month_year))
				{
					price = sm.get_price(str);
					amount = sm.get_amount(str);
					day = sm.get_day(str);

					days.push_back(day);
					prices.push_back(price);
					amounts.push_back(amount);
				}
			}
		}

		days.push_back("60");
		prices.push_back("60");
		amounts.push_back("60");
		string data;

		if (days.size() > 1)
		{
			int h = 1;
			float sum = stof(amounts[0]) * stof(prices[0]);
			while (true)
			{
				float sum_ = stof(amounts[h]) * stof(prices[h]);

				if (days[h] == days[h - 1])
				{
					sum += sum_;
				}

				else
				{
					data = days[0] + "," + to_string(sum);
					result.push_back(data);
					break;
				}

				h++;
			}

			sum = stof(amounts[h]) * stof(prices[h]);
			h++;

			for (; h < days.size(); h++)
			{
				float sum_ = stof(amounts[h]) * stof(prices[h]);
				if (days[h] == days[h - 1])
				{
					sum += sum_;
				}

				else
				{
					data = days[h - 1] + "," + to_string(sum);
					sum = sum_;
					result.push_back(data);
				}
			}
		}
		return result;
	}

	void Delete_from_cart_file(string file_name, string str_to_delete)
	{
		ifstream cart_file(folder_path + file_name + ".txt");
		vector <string> strs;
		string str;
		while (!cart_file.eof())
		{
			getline(cart_file, str);
			auto pos = str.find(":");
			string r_str = str.substr(0, pos);
			if(r_str == str_to_delete)
			{
					continue;
			}
			else
			{
				strs.push_back(str);
			}
		}
		cart_file.close();
		ofstream cart_file_write(folder_path + file_name + ".txt");
		cart_file_write.seekp(0);
		for (int i = 0; i < strs.size(); i++)
		{
			cart_file_write << strs[i] << "\n";
		}
		cart_file_write.close();
	}

	void plus_minus_item(int amount, string user_name, string prod_name, string prod_price, bool action)
	{
		ifstream cart_file(folder_path + user_name + ".txt");
		vector <string> strs;
		string str;
		while (!cart_file.eof())
		{
			getline(cart_file, str);
			auto pos = str.find(":");
			string r_str = str.substr(0, pos);
			if (r_str == prod_name)
			{
				if (action) {
					amount++;
				}
				else
				{
					amount--;
				}
				strs.push_back(prod_name + ":" + prod_price + "::" + to_string(amount));
				continue;
			}
			else
			{
				strs.push_back(str);
			}
		}
		cart_file.close();
		ofstream cart_file_write(folder_path + user_name + ".txt");
		cart_file_write.seekp(0);
		for (int i = 0; i < strs.size(); i++)
		{
			cart_file_write << strs[i] << "\n";
		}
		cart_file_write.close();
	}
	
	void Delete_all_enters(string file_name)
	{
		ifstream cart_file(folder_path + file_name + ".txt");
		string str;
		vector <string> strs;
		while (!cart_file.eof())
		{
			getline(cart_file, str);
			if (str != "" and str != "\n" and str != " ")
			{
				strs.push_back(str);
			}
			
		}
		cart_file.close();


		ofstream carts_file(folder_path + file_name + ".txt");

		for (int i = 0; i < strs.size(); i++)
		{
			carts_file << strs[i] << "\n";
		}
	}

	vector <string> Read_CSVFile_Column(string filename, string column_number)
	{
			std::vector<std::string> result;
			std::ifstream file(filename);
			if (!file.is_open()) {
				QMessageBox::warning(nullptr, "Warning", "ECheck your CSV File!");
			}
			std::string line;
			while (std::getline(file, line)) {
				std::istringstream iss(line);
				std::string token;
				int current_column_number = 0;
				while (std::getline(iss, token, ',')) {
					if (current_column_number == stoi(column_number)) {
						result.push_back(token);
						break;
					}
					++current_column_number;
				}
			}
			file.close();
			return result;
	}
private:
	string folder_path = "C:\\Users\\admin\\source\\repos\\QtWidgetsApplication3\\QtWidgetsApplication3\\Carts\\";
	string buy_log_path = "C:\\Users\\admin\\source\\repos\\QtWidgetsApplication3\\QtWidgetsApplication3\\Buy_log.txt";
};

class Buy
{
public:    
	void buy_smth(string user_name, string prod_name, string prod_price, int amount, int amount_bought)
	{
		if (amount > 0)
		{
			ifstream cart_file(folder_path + user_name + ".txt");
			vector <string> strs;
			string str;
			while (!cart_file.eof())
			{
				getline(cart_file, str);
				auto pos = str.find(":");
				string r_str = str.substr(0, pos);
				if (r_str == prod_name)
				{
					strs.push_back(prod_name + ":" + prod_price + "::" + to_string(amount));
					continue;
				}
				else
				{
					strs.push_back(str);
				}
			}
			cart_file.close();
			ofstream cart_file_write(folder_path + user_name + ".txt");
			cart_file_write.seekp(0);
			for (int i = 0; i < strs.size(); i++)
			{
				cart_file_write << strs[i] << "\n";
			}
			cart_file_write.close();

			ofstream buy_log(path_log, fstream::app);
			if (!buy_log.is_open())
			{
				QMessageBox::warning(nullptr, "Error", "Check access to Buy_log.txt file!");
			}
			else
			{
				DataTime dt;
				string date = dt.currentData();
				string time = dt.currentTime();
				buy_log << "\"" << user_name << "\" " << prod_name << "(" << prod_price << "$)*" << amount_bought << "\t" << "D:" << date << " T:" << time << "\n";
				buy_log.close();
			}
		}

		else {
			FileMananger fm;
			fm.Delete_from_cart_file(user_name, prod_name);
			ofstream buy_log(path_log, fstream::app);
			if (!buy_log.is_open())
			{
				QMessageBox::warning(nullptr, "Error", "Check access to Buy_log.txt file!");
			}
			else
			{
				DataTime dt;
				string date = dt.currentData();
				string time = dt.currentTime();
				buy_log << "\"" << user_name << "\"" << prod_name << "(" << prod_price << "$)*" << amount_bought << "\t" << "D:" << date << " T:" << time << "\n";
				buy_log.close();
			}
		}
	}

	void add_to_cart(string user_name, string prod_name, string prod_price)
	{
		ifstream carts_file_get(folder_path + user_name + ".txt");
		if (!carts_file_get.is_open())
		{
			QMessageBox::warning(nullptr, "Error", "Check access to Carts.txt file!");
		}
		else
		{
			bool isWrote = false;
			vector <string> wroted_products;
			vector <string> new_f;
			
			string str;
			int amount = 1;
			while (!carts_file_get.eof())
			{
				getline(carts_file_get, str);
				auto pos = str.find(":");
				auto pos_ = str.find("::");
				string name = str.substr(0, pos);

				if (name != "" || name != "\n")
				{
					wroted_products.push_back(name);

					if (name == prod_name)
					{
						string am = str.substr(pos_ + 2);
						amount = stoi(am);
						isWrote = true;
					}
					else
					{
						new_f.push_back(str);
					}
				}
			}

			if (isWrote)
			{

			}
			carts_file_get.close();
			ofstream carts_file_write(folder_path + user_name + ".txt");
			carts_file_write.seekp(0);
			for (int i = 0; i < wroted_products.size(); i++)
			{
				if (prod_name == wroted_products[i])
				{
					amount++;
				}
			}

			new_f.push_back(prod_name + ":" + prod_price + "$" + "::" + to_string(amount));

			for (int i = 0; i < new_f.size(); i++)
			{
				carts_file_write << new_f[i] << "\n";
			} 

			carts_file_write.close();
			
			FileMananger fm;
			fm.Delete_all_enters(user_name);
		}
	}
private:
	string path_log = "Buy_log.txt";
	string folder_path = "C:\\Users\\admin\\source\\repos\\QtWidgetsApplication3\\QtWidgetsApplication3\\Carts\\";
};

class Update
{
public:

	void Update_Users_Table(QTableWidget* usersTable)
	{
		User user;
		vector<string> users = user.GetUsers();

		usersTable->setRowCount(0);

		for (int i = 0; i < users.size(); i++)
		{
			usersTable->insertRow(usersTable->rowCount());
			string name = users[i];
			QTableWidgetItem* name_ = new QTableWidgetItem();
			QTableWidgetItem* duty = new QTableWidgetItem();
			name_->setText(QString::fromStdString(name));
			i++;
			duty->setText(QString::fromStdString(users[i]));
			usersTable->setItem(usersTable->rowCount() - 1, 0, name_);
			usersTable->setItem(usersTable->rowCount() - 1, 1, duty);
			QPushButton* upd = new QPushButton("Change");
			usersTable->setCellWidget(usersTable->rowCount() - 1, 2, upd);
			QPushButton* del = new QPushButton("Delete");
			usersTable->setCellWidget(usersTable->rowCount() - 1, 3, del);

			QObject::connect(upd, &QPushButton::clicked, [usersTable, name]()
				{
					QWidget* upd = new QWidget();
			QLineEdit* name_E = new QLineEdit();
			name_E->setText(QString::fromStdString(name));
			QVBoxLayout* main = new QVBoxLayout();
			QHBoxLayout* btn = new QHBoxLayout();
			QVBoxLayout* lines = new QVBoxLayout();
			QComboBox* duty_box = new QComboBox();
			duty_box->addItem("Admin");
			duty_box->addItem("User");
			duty_box->addItem("Accountant");
			QPushButton* cancel = new QPushButton("cancel");
			QPushButton* confirm = new QPushButton("confirm");

			lines->addWidget(name_E);
			lines->addWidget(duty_box);

			btn->addWidget(cancel);
			btn->addWidget(confirm);

			main->addLayout(lines);
			main->addLayout(btn);

			upd->setLayout(main);

			QObject::connect(cancel, &QPushButton::clicked, [upd]()
				{
					upd->close();
				});
			QObject::connect(confirm, &QPushButton::clicked, [name, name_E, upd, duty_box, usersTable]()
				{
					ifstream fin("Full_log.txt");
			if (!fin.is_open())
			{
				QMessageBox::warning(nullptr, "Error", "Can't open log file!");
			}
			else
			{
				vector <string> data;
				string password;
				while (!fin.eof())
				{
					string str;
					getline(fin, str);

					auto pos = str.find(":");
					string name_old = str.substr(0, pos);
					if (name_old == name)
					{
						size_t start_pos = str.find(":") + 1;
						size_t end_pos = str.find("-", start_pos);
						password = str.substr(start_pos, end_pos - start_pos - 1);
						string data_ = name_E->text().toStdString() + ":" + password + " -" + duty_box->currentText().toStdString();
						data.push_back(data_);
						continue;
					}
					else
					{
						data.push_back(str);
					}
				}
				fin.close();
				ofstream of("Full_log.txt");
				if (!of.is_open())
				{
					QMessageBox::warning(nullptr, "Error", "Can't open log file!");
				}
				else
				{
					for (int i = 0; i < data.size(); i++)
					{
						if (i + 1 == data.size())
						{
							of << data[i];
						}
						else
						{
							of << data[i] << "\n";
						}
					}
				}
				of.close();
			}
			upd->close();
			Update upd;
			upd.Update_Users_Table(usersTable);
				});

			upd->show();
				});

			QObject::connect(del, &QPushButton::clicked, [usersTable, name]()
				{
					QWidget* delete_form = new QWidget();
			QLabel* sure = new QLabel("Are you sure you want to deletet?");
			QPushButton* cancel = new QPushButton("cancel");
			QPushButton* confirm = new QPushButton("confirm");
			QHBoxLayout* btn = new QHBoxLayout();
			QVBoxLayout* main = new QVBoxLayout();
			QVBoxLayout* inf = new QVBoxLayout();

			inf->addWidget(sure);
			btn->addWidget(cancel);
			btn->addWidget(confirm);

			main->addLayout(inf);
			main->addLayout(btn);

			QObject::connect(cancel, &QPushButton::clicked, [delete_form]()
				{
					delete_form->close();
				});
			QObject::connect(confirm, &QPushButton::clicked, [name, delete_form, usersTable]()
				{
					ifstream fin("Full_log.txt");
			if (!fin.is_open())
			{
				QMessageBox::warning(nullptr, "Error", "Can't open log file!");
			}
			else
			{
				vector <string> data;
				string password;
				while (!fin.eof())
				{
					string str;
					getline(fin, str);

					auto pos = str.find(":");
					string name_old = str.substr(0, pos);
					if (name_old == name)
					{
						continue;
					}
					else
					{
						data.push_back(str);
					}
				}
				fin.close();
				ofstream of("Full_log.txt");
				if (!of.is_open())
				{
					QMessageBox::warning(nullptr, "Error", "Can't open log file!");
				}
				else
				{
					for (int i = 0; i < data.size(); i++)
					{
						if (i + 1 == data.size())
						{
							of << data[i];
						}
						else
						{
							of << data[i] << "\n";
						}
					}
				}
				of.close();
			}
			delete_form->close();
			Update upd;
			upd.Update_Users_Table(usersTable);
				});

			delete_form->setLayout(main);
			delete_form->show();
				});
		}
		usersTable->insertRow(usersTable->rowCount());
		QPushButton* new_ = new QPushButton("New");
		usersTable->setCellWidget(usersTable->rowCount() - 1, 0, new_);

		QObject::connect(new_, &QPushButton::clicked, [usersTable]()
			{
				QWidget* new_w = new QWidget();
		QLineEdit* name_E = new QLineEdit();
		QLineEdit* password_E = new QLineEdit();
		QFormLayout* main = new QFormLayout();
		QHBoxLayout* btn = new QHBoxLayout();
		QComboBox* duty_box = new QComboBox();
		duty_box->addItem("Admin");
		duty_box->addItem("User");
		QPushButton* cancel = new QPushButton("cancel");
		QPushButton* confirm = new QPushButton("confirm");

		main->addRow("Login: ", name_E);
		main->addRow("Password: ", password_E);
		main->addRow("Duty: ", duty_box);
		btn->addWidget(cancel);
		btn->addWidget(confirm);
		main->addItem(btn);
		new_w->setLayout(main);

		QObject::connect(cancel, &QPushButton::clicked, [new_w]()
			{
				new_w->close();
			});
		QObject::connect(confirm, &QPushButton::clicked, [usersTable, new_w, name_E, password_E, duty_box]()
			{
				ofstream of("Full_log.txt", fstream::app);
		of << name_E->text().toStdString() << ":" << password_E->text().toStdString() << " -" << duty_box->currentText().toStdString() << "\n";
		of.close();

		new_w->close();

		Update upd;
		upd.Update_Users_Table(usersTable);
			});

		new_w->show();
			});
	}

	void Update_Products_table(QTableWidget* productsTableWidget, string id_main)
	{
		Admin admin;
		vector <QJsonObject> productsList = admin.ReadSingleCategory(id_main);
		productsTableWidget->setRowCount(0);
		for (const QJsonObject& product : productsList)
		{
			string id = product["id"].toString().toStdString();

			QString product_name = product["name"].toString();
			QString product_price = product["price"].toString();
			string product_id_cat = product["id_category"].toString().toStdString();
			QTableWidgetItem* newItemName = new QTableWidgetItem(product_name);
			QTableWidgetItem* newItemPrice = new QTableWidgetItem(product_price + "$");
			productsTableWidget->insertRow(productsTableWidget->rowCount());
			newItemName->setFlags(newItemName->flags() ^ Qt::ItemIsEditable);
			productsTableWidget->setItem(productsTableWidget->rowCount() - 1, 0, newItemName);
			productsTableWidget->setItem(productsTableWidget->rowCount() - 1, 1, newItemPrice);
			QPushButton* upd = new QPushButton("Update");
			QPushButton* del = new QPushButton("Delete");
			productsTableWidget->setCellWidget(productsTableWidget->rowCount() - 1, 2, upd);
			productsTableWidget->setCellWidget(productsTableWidget->rowCount() - 1, 3, del);

			QObject::connect(upd, &QPushButton::clicked, [productsTableWidget, product_name, product_price, product_id_cat, id, id_main]()
				{
					QWidget* update_form = new QWidget;
			QLineEdit* name = new QLineEdit;
			QLineEdit* price = new QLineEdit;
			QPushButton* update_btn = new QPushButton("Update");
			QPushButton* cancel_btn = new QPushButton("Cancel");

			QVBoxLayout* lbl_eline = new QVBoxLayout;
			QHBoxLayout* btn = new QHBoxLayout;
			QVBoxLayout* main = new QVBoxLayout;

			lbl_eline->addWidget(name);
			lbl_eline->addWidget(price);

			btn->addWidget(cancel_btn);
			btn->addWidget(update_btn);

			main->addLayout(lbl_eline);
			main->addLayout(btn);

			name->setText(product_name);
			price->setText(product_price);

			update_form->setLayout(main);

			QObject::connect(cancel_btn, &QPushButton::clicked, [update_form]()
				{
					update_form->close();
				});
			QObject::connect(update_btn, &QPushButton::clicked, [update_form, price, name, id, product_id_cat, productsTableWidget, id_main]()
				{
					string pr_price = price->text().toStdString();
			string pr_name = name->text().toStdString();
			Admin admin;
			admin.UpdateProduct(pr_name, product_id_cat, pr_price, id);
			Update upd;
			upd.Update_Products_table(productsTableWidget, id_main);
			update_form->close();
				});
			update_form->show();
				});
			QObject::connect(del, &QPushButton::clicked, [productsTableWidget, id_main, id]()
				{
					QWidget* delete_form = new QWidget;
			QLabel* inform = new QLabel("Are you sure you want to delete?");
			QPushButton* delete_btn = new QPushButton("Delete");
			QPushButton* cancel_btn = new QPushButton("Cancel");

			QVBoxLayout* lbl_eline = new QVBoxLayout;
			QHBoxLayout* btn = new QHBoxLayout;
			QVBoxLayout* main = new QVBoxLayout;

			lbl_eline->addWidget(inform);

			btn->addWidget(cancel_btn);
			btn->addWidget(delete_btn);

			main->addLayout(lbl_eline);
			main->addLayout(btn);

			delete_form->setLayout(main);

			QObject::connect(cancel_btn, &QPushButton::clicked, [delete_form]()
				{
					delete_form->close();
				});

			QObject::connect(delete_btn, &QPushButton::clicked, [delete_form, productsTableWidget, id_main, id]()
				{
					Admin admin;
			admin.DeleteProduct(id);
			Update upd;
			upd.Update_Products_table(productsTableWidget, id_main);
			delete_form->close();
				});
			delete_form->show();
				});
		}
		productsTableWidget->insertRow(productsTableWidget->rowCount());
		QPushButton* new_prod = new QPushButton("New");
		productsTableWidget->setCellWidget(productsTableWidget->rowCount() - 1, 0, new_prod);
		QPushButton* import = new QPushButton("Import from .csv");
		productsTableWidget->setCellWidget(productsTableWidget->rowCount() - 1, 1, import);
		QObject::connect(new_prod, &QPushButton::clicked, [new_prod, productsTableWidget, id_main]() {
			QWidget* new_prod = new QWidget;
		QLabel* inform = new QLabel("Type name of a new product");
		QLineEdit* prod_name = new QLineEdit();
		QLabel* inform2 = new QLabel("Type price of a new product");
		QLineEdit* prod_price = new QLineEdit();
		QPushButton* confirm = new QPushButton("Confirm");
		QPushButton* cancel = new QPushButton("Cancel");

		QVBoxLayout* LinesLayout = new QVBoxLayout();
		LinesLayout->addWidget(inform);
		LinesLayout->addWidget(prod_name);
		LinesLayout->addWidget(inform2);
		LinesLayout->addWidget(prod_price);

		QHBoxLayout* btn_layout = new QHBoxLayout;
		btn_layout->addWidget(cancel);
		btn_layout->addWidget(confirm);

		QVBoxLayout* newLayout = new QVBoxLayout();

		newLayout->addLayout(LinesLayout);
		newLayout->addLayout(btn_layout);

		new_prod->setLayout(newLayout);

		new_prod->show();

		QObject::connect(cancel, &QPushButton::clicked, [new_prod]()
			{
				new_prod->close();
			});

		QObject::connect(confirm, &QPushButton::clicked, [prod_name, prod_price, new_prod, productsTableWidget, id_main]()
			{
				if (prod_name->text() == "")
				{
					QMessageBox::warning(nullptr, "Warning", "Enter name of new product");
				}
				else
				{
					string name = prod_name->text().toStdString();
					string price = prod_price->text().toStdString();
					Admin admin;
					admin.NewProduct(name, price, id_main);
					Update upd;
					upd.Update_Products_table(productsTableWidget, id_main);
				}

		new_prod->close();
			});
		new_prod->show();
			});
		QObject::connect(import, &QPushButton::clicked, [productsTableWidget, id_main]()
		{
				QString fileName = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"), QDir::homePath(), QObject::tr("CSV Files (*.csv)"));
		if (!fileName.isEmpty()) 
		{
			string file_name = fileName.toStdString();
			for (int i = 0; i < file_name.size(); i++) {
				if (file_name[i] == '/')
				{
					file_name.replace(i, 1, "\\");
					i++;
				}
			}
			ifstream f(file_name);
			if (!f.is_open()) {
				QMessageBox::warning(nullptr, "Warning", "Check your .csv file!");
			}

			string line;
			getline(f, line);
			int columns = 0;

			QComboBox* name_col = new QComboBox();
			QComboBox* price_col = new QComboBox();
			name_col->addItem("0");
			price_col->addItem("0");

			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] == ',')
				{
					columns++;
					name_col->addItem(QString::fromStdString(to_string(columns)));
					price_col->addItem(QString::fromStdString(to_string(columns)));
				}
			}

			f.close();

			QWidget* imp_w = new QWidget();
			QVBoxLayout* main_l = new QVBoxLayout();
			QLabel* inf1 = new QLabel("Choose column with names");
			QLabel* inf2 = new QLabel("Choose column with prices");
			QPushButton* confirm1 = new QPushButton("Confirm");

			main_l->addWidget(inf1);
			main_l->addWidget(name_col);
			main_l->addWidget(inf2);
			main_l->addWidget(price_col);
			main_l->addWidget(confirm1);
			imp_w->setLayout(main_l);

			QObject::connect(confirm1, &QPushButton::clicked, [inf1, inf2, confirm1, name_col, price_col, fileName, imp_w, id_main, productsTableWidget]()
				{
					if (name_col->currentText() == price_col->currentText())
					{
						QMessageBox::warning(nullptr, "Warning", "Name column and price column can't be the same!");
					}
					else {
						imp_w->close();

						QFormLayout* form_l = new QFormLayout();

						QWidget* main = new QWidget();

						main->setLayout(form_l);

						inf1->setText("Check your data:");

						QPushButton* confirm2 = new QPushButton("Confirm");
						FileMananger fm;
						vector <string> names = fm.Read_CSVFile_Column(fileName.toStdString(), name_col->currentText().toStdString());
						vector <string> prices = fm.Read_CSVFile_Column(fileName.toStdString(), price_col->currentText().toStdString());

						QListWidget* names_list = new QListWidget();
						QListWidget* prices_list = new QListWidget();
						names_list->setFixedSize(70, 100);
						prices_list->setFixedSize(70, 100);
						
						for (int i = 0; i < names.size(); i++)
						{
							QListWidgetItem* name = new QListWidgetItem();
							name->setText(QString::fromStdString(names[i]));
							names_list->addItem(name);
							QListWidgetItem* price = new QListWidgetItem();
							price->setText(QString::fromStdString(prices[i]));
							prices_list->addItem(price);
						}

						form_l->addWidget(inf1);
						inf1->setAlignment(Qt::AlignLeft);
						form_l->addRow(names_list, prices_list);
						form_l->setAlignment(names_list, Qt::AlignLeft);
						form_l->setAlignment(prices_list, Qt::AlignRight);
						form_l->addWidget(confirm2);
						form_l->setAlignment(confirm2, Qt::AlignCenter);

						QObject::connect(confirm2, &QPushButton::released, [main, id_main, names, prices, productsTableWidget]()
							{
						main->close();
						for (int i = 0; i < names.size(); i++)
						{
							Admin adm;
							adm.NewProduct(names[i], prices[i], id_main);
						}
						Update upd;
						upd.Update_Products_table(productsTableWidget, id_main);
							});

						main->show();
					}
					
				});

			imp_w->show();
		}
		else {
			QMessageBox::warning(nullptr, "Warning", "Choose .csv file!");
		}
		});
	}

	void Update_Categories_table(QTableWidget* categoriesTableWidget, QTableWidget* productsTableWidget, QPushButton* close)
	{
		Admin admin;
		vector<QJsonObject> categoriesList = admin.ReadCategories();

		categoriesTableWidget->setRowCount(0);

		for (const QJsonObject& category : categoriesList) {
			string id = category["id"].toString().toStdString();

			QString category_name = category["name"].toString();
			QTableWidgetItem* newItem = new QTableWidgetItem(category_name);
			categoriesTableWidget->insertRow(categoriesTableWidget->rowCount());
			newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
			categoriesTableWidget->setItem(categoriesTableWidget->rowCount() - 1, 0, newItem);
			QPushButton* upd = new QPushButton("Update");
			QPushButton* del = new QPushButton("Delete");
			QPushButton* show_prod = new QPushButton("Show products");
			categoriesTableWidget->setCellWidget(categoriesTableWidget->rowCount() - 1, 1, upd);
			categoriesTableWidget->setCellWidget(categoriesTableWidget->rowCount() - 1, 2, del);
			categoriesTableWidget->setCellWidget(categoriesTableWidget->rowCount() - 1, 3, show_prod);

			QObject::connect(upd, &QPushButton::clicked, [category_name, id, categoriesTableWidget, productsTableWidget, close]()
				{
					QWidget* update_form = new QWidget;
			QLineEdit* name = new QLineEdit;
			QPushButton* update_btn = new QPushButton("Update");
			QPushButton* cancel_btn = new QPushButton("Cancel");

			QVBoxLayout* lbl_eline = new QVBoxLayout;
			QHBoxLayout* btn = new QHBoxLayout;
			QVBoxLayout* main = new QVBoxLayout;

			lbl_eline->addWidget(name);

			btn->addWidget(cancel_btn);
			btn->addWidget(update_btn);

			main->addLayout(lbl_eline);
			main->addLayout(btn);

			name->setText(category_name);

			update_form->setLayout(main);

			QObject::connect(cancel_btn, &QPushButton::clicked, [update_form]()
				{
					update_form->close();
				});

			QObject::connect(update_btn, &QPushButton::clicked, [update_form, name, category_name, id, categoriesTableWidget, productsTableWidget, close]()
				{
					if (name->text().toStdString() != "")
					{
						Admin admin;
						string name_c = name->text().toStdString();
						admin.Update_Category(name_c, id);
						Update upd;
						upd.Update_Categories_table(categoriesTableWidget, productsTableWidget, close);
					}
			update_form->close();
				});

			update_form->show();
				});
			QObject::connect(del, &QPushButton::clicked, [id, categoriesTableWidget, productsTableWidget, close]()
				{
					QWidget* delete_form = new QWidget;
			QLabel* inform = new QLabel("Are you sure you want to delete?");
			QPushButton* delete_btn = new QPushButton("Delete");
			QPushButton* cancel_btn = new QPushButton("Cancel");

			QVBoxLayout* lbl_eline = new QVBoxLayout;
			QHBoxLayout* btn = new QHBoxLayout;
			QVBoxLayout* main = new QVBoxLayout;

			lbl_eline->addWidget(inform);

			btn->addWidget(cancel_btn);
			btn->addWidget(delete_btn);

			main->addLayout(lbl_eline);
			main->addLayout(btn);

			delete_form->setLayout(main);

			QObject::connect(cancel_btn, &QPushButton::clicked, [delete_form]()
				{
					delete_form->close();
				});

			QObject::connect(delete_btn, &QPushButton::clicked, [delete_form, id, categoriesTableWidget, productsTableWidget, close]()
				{
					Admin admin;
			vector <QJsonObject> category = admin.ReadSingleCategory(id);
			for (const QJsonObject& product : category)
			{
				string product_id = product["id"].toString().toStdString();

				admin.DeleteProduct(product_id);
			}

			admin.DeleteCategory(id);
			Update upd;
			upd.Update_Categories_table(categoriesTableWidget, productsTableWidget, close);
			delete_form->close();
				});
			delete_form->show();
				});
			QObject::connect(show_prod, &QPushButton::clicked, [id, categoriesTableWidget, productsTableWidget, close]()
				{
					close->setVisible(true);
					productsTableWidget->setVisible(true);
			Update upd;
			upd.Update_Products_table(productsTableWidget, id);
				});
		}
		categoriesTableWidget->insertRow(categoriesTableWidget->rowCount());
		QPushButton* NewButton = new QPushButton("New");
		QObject::connect(NewButton, &QPushButton::clicked, [categoriesTableWidget, productsTableWidget, NewButton, close]() {
			QWidget* new_cat = new QWidget;
		QLabel* inform = new QLabel("Type name of a new category");
		QLineEdit* cat_name = new QLineEdit();
		QPushButton* confirm = new QPushButton("Confirm");
		QPushButton* cancel = new QPushButton("Cancel");

		QVBoxLayout* LinesLayout = new QVBoxLayout();
		LinesLayout->addWidget(inform);
		LinesLayout->addWidget(cat_name);

		QHBoxLayout* btn_layout = new QHBoxLayout;
		btn_layout->addWidget(cancel);
		btn_layout->addWidget(confirm);

		QVBoxLayout* newLayout = new QVBoxLayout();

		newLayout->addLayout(LinesLayout);
		newLayout->addLayout(btn_layout);

		new_cat->setLayout(newLayout);

		new_cat->show();

		QObject::connect(cancel, &QPushButton::clicked, [new_cat]()
			{
				new_cat->close();
			});

		QObject::connect(confirm, &QPushButton::clicked, [cat_name, new_cat, categoriesTableWidget, NewButton, productsTableWidget, close]()
			{
				if (cat_name->text() == "")
				{
					QMessageBox::warning(nullptr, "Warning", "Enter name of new category");
				}
				else
				{
					string name = cat_name->text().toStdString();
					Admin admin;
					admin.NewCategory(name);
					Update upd;
					upd.Update_Categories_table(categoriesTableWidget, productsTableWidget, close);
				}

		new_cat->close();
			});
			});
		categoriesTableWidget->setCellWidget(categoriesTableWidget->rowCount() - 1, 0, NewButton);
	}

	void Update_Products_table_user(QTableWidget* productsTableWidget, string id_main, QString login)
	{
		Admin admin;
		vector <QJsonObject> productsList = admin.ReadSingleCategory(id_main);
		productsTableWidget->setRowCount(0);
		for (const QJsonObject& product : productsList)
		{
			string id = product["id"].toString().toStdString();

			QString prod_name = product["name"].toString();
			QString prod_price = product["price"].toString();
			QTableWidgetItem* newItemName = new QTableWidgetItem(prod_name);
			QTableWidgetItem* newItemPrice = new QTableWidgetItem(prod_price + "$");
			productsTableWidget->insertRow(productsTableWidget->rowCount());
			newItemName->setFlags(newItemName->flags() ^ Qt::ItemIsEditable);
			productsTableWidget->setItem(productsTableWidget->rowCount() - 1, 0, newItemName);
			productsTableWidget->setItem(productsTableWidget->rowCount() - 1, 1, newItemPrice);
			QPushButton* buy = new QPushButton("Buy");
			productsTableWidget->setCellWidget(productsTableWidget->rowCount() - 1, 2, buy);
			QPushButton* add_to_cart = new QPushButton("Add to cart");
			productsTableWidget->setCellWidget(productsTableWidget->rowCount() - 1, 3, add_to_cart);

			QObject::connect(buy, &QPushButton::clicked, [productsTableWidget, prod_name, prod_price, login, id_main]() {
				QWidget* count = new QWidget();
			QFormLayout* main = new QFormLayout();
			QLineEdit* countE = new QLineEdit();
			QPushButton* cancel = new QPushButton("Cancel");
			QPushButton* confirm = new QPushButton("Confirm");
			QLabel* inf = new QLabel(QString::fromStdString("How many ") + prod_name + QString::fromStdString(" do you want to buy?"));
			main->addWidget(inf);
			main->addWidget(countE);
			main->addRow(cancel, confirm);
			count->setLayout(main);

			QObject::connect(cancel, &QPushButton::released, [count]()
				{
					count->close();
				});
			QObject::connect(confirm, &QPushButton::released, [count, prod_name, prod_price, login, productsTableWidget, countE, id_main]()
				{
					int amount_buy = countE->text().toInt();
			count->close();
			QWidget* sure_form = new QWidget();
			QHBoxLayout* btn = new QHBoxLayout();
			QPushButton* cancel = new QPushButton("Cancel");
			QPushButton* confirm = new QPushButton("Confirm");
			QVBoxLayout* main = new QVBoxLayout();
			QLabel* inf = new QLabel(QString::fromStdString("Are you sure you want to buy ") + prod_name + " for " + prod_price);
			main->addWidget(inf);
			btn->addWidget(cancel);
			btn->addWidget(confirm);
			main->addLayout(btn);

			QObject::connect(cancel, &QPushButton::clicked, [sure_form]()
				{
					sure_form->close();
				});

			QObject::connect(confirm, &QPushButton::clicked, [sure_form, prod_name, prod_price, login, amount_buy, productsTableWidget, id_main]()
				{
					Buy b;
			b.buy_smth(login.toStdString(), prod_name.toStdString(), prod_price.toStdString(), 0, amount_buy);
			sure_form->close();
			Update upd;
			upd.Update_Products_table_user(productsTableWidget, id_main, login);
				});
			sure_form->setLayout(main);
			sure_form->show();
				});

			count->show();
				});

			QObject::connect(add_to_cart, &QPushButton::clicked, [prod_name, prod_price, login]()
				{
					Buy b;
			b.add_to_cart(login.toStdString(), prod_name.toStdString(), prod_price.toStdString());
				});
		}
	}

	void Update_Categories_table_user(QTableWidget* categoriesTableWidget, QTableWidget* productsTableWidget, QString login, QPushButton* close)
	{
		Admin admin;
		vector<QJsonObject> categoriesList = admin.ReadCategories();

		categoriesTableWidget->setRowCount(0);

		for (const QJsonObject& category : categoriesList) {
			string id = category["id"].toString().toStdString();

			QString category_name = category["name"].toString();
			QTableWidgetItem* newItem = new QTableWidgetItem(category_name);
			categoriesTableWidget->insertRow(categoriesTableWidget->rowCount());
			newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
			categoriesTableWidget->setItem(categoriesTableWidget->rowCount() - 1, 0, newItem);
			QPushButton* show_prod = new QPushButton("->");
			categoriesTableWidget->setCellWidget(categoriesTableWidget->rowCount() - 1, 1, show_prod);

			QObject::connect(show_prod, &QPushButton::clicked, [id, categoriesTableWidget, productsTableWidget, login, close]()
				{
					close->setVisible(true);
					productsTableWidget->setVisible(true);
			Update upd;
			upd.Update_Products_table_user(productsTableWidget, id, login);
				});
		}
	}

	void update_cart_table(QTableWidget* cart_table, string user_name)
	{
		cart_table->setVisible(true);
		cart_table->setRowCount(0);
		FileMananger fm;
		vector <string> products = fm.get_products_from_cart(user_name);
		string product;
		for (const string product_name : products)
		{
			auto pos = product_name.find(":");
			auto pos_ = product_name.find("::");
			string prod_name = product_name.substr(0, pos);

			if (prod_name == "" || prod_name == "\n")
			{
				continue;
			}
			else {
				cart_table->insertRow(cart_table->rowCount());
				string prod_price = product_name.substr(pos + 1, pos_ - pos - 1);
				string amount = product_name.substr(pos_ + 2);
				QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(prod_name));
				QTableWidgetItem* priceItem = new QTableWidgetItem(QString::fromStdString(prod_price));
				QTableWidgetItem* amountItem = new QTableWidgetItem(QString::fromStdString(amount));
				cart_table->setColumnWidth(4, 10);
				cart_table->setColumnWidth(5, 7);
				cart_table->setColumnWidth(6, 10);
				cart_table->setColumnWidth(1, 50);
				cart_table->setColumnWidth(2, 15);

				cart_table->setItem(cart_table->rowCount() - 1, 0, nameItem);
				cart_table->setItem(cart_table->rowCount() - 1, 1, priceItem);
				cart_table->setItem(cart_table->rowCount() - 1, 5, amountItem);

				QPushButton* buy = new QPushButton("Buy");
				cart_table->setCellWidget(cart_table->rowCount() - 1, 2, buy);
				QPushButton* delete_from_cart = new QPushButton("Delete from cart");
				cart_table->setCellWidget(cart_table->rowCount() - 1, 3, delete_from_cart);
				QPushButton* plus_item = new QPushButton("+");
				cart_table->setCellWidget(cart_table->rowCount() - 1, 6, plus_item);
				QPushButton* minus_item = new QPushButton("-");
				cart_table->setCellWidget(cart_table->rowCount() - 1, 4, minus_item);

				QObject::connect(buy, &QPushButton::clicked, [cart_table, prod_name, prod_price, user_name, amount]() {
					QWidget* count = new QWidget();
				QFormLayout* main = new QFormLayout();
				QLineEdit* countE = new QLineEdit();
				QPushButton* cancel = new QPushButton("Cancel");
				QPushButton* confirm = new QPushButton("Confirm");
				QLabel* inf = new QLabel(QString::fromStdString("How many " + prod_name + " do you want to buy?"));
				main->addWidget(inf);
				main->addWidget(countE);
				main->addRow(cancel, confirm);
				count->setLayout(main);

				QObject::connect(cancel, &QPushButton::released, [count]()
					{
						count->close();
					});
				QObject::connect(confirm, &QPushButton::released, [count, prod_name, prod_price, user_name, cart_table, amount, countE]()
					{
						int amount_buy = countE->text().toInt();
				count->close();
				QWidget* sure_form = new QWidget();
				QHBoxLayout* btn = new QHBoxLayout();
				QPushButton* cancel = new QPushButton("Cancel");
				QPushButton* confirm = new QPushButton("Confirm");
				QVBoxLayout* main = new QVBoxLayout();
				QLabel* inf = new QLabel(QString::fromStdString("Are you sure you want to buy " + prod_name + " for " + prod_price));
				main->addWidget(inf);
				btn->addWidget(cancel);
				btn->addWidget(confirm);
				main->addLayout(btn);

				QObject::connect(cancel, &QPushButton::clicked, [sure_form]()
					{
						sure_form->close();
					});

				QObject::connect(confirm, &QPushButton::clicked, [sure_form, prod_name, prod_price, user_name, cart_table, amount, amount_buy]()
					{
						Buy b;
				b.buy_smth(user_name, prod_name, prod_price, stoi(amount) - amount_buy, amount_buy);
				sure_form->close();
				Update upd;
				upd.update_cart_table(cart_table, user_name);
					});
				sure_form->setLayout(main);
				sure_form->show();
					});

				count->show();
					});
				QObject::connect(delete_from_cart, &QPushButton::released, [cart_table, prod_name, prod_price, user_name, amount]()
					{
						FileMananger fm;
				fm.Delete_from_cart_file(user_name, prod_name);
				Update upd;
				upd.update_cart_table(cart_table, user_name);
					});
				QObject::connect(plus_item, &QPushButton::released, [cart_table, prod_name, prod_price, user_name, amount]()
					{
						FileMananger fm;
				fm.plus_minus_item(stoi(amount), user_name, prod_name, prod_price, true);
				Update upd;
				upd.update_cart_table(cart_table, user_name);
					});
				QObject::connect(minus_item, &QPushButton::released, [cart_table, prod_name, prod_price, user_name, amount]()
					{
						FileMananger fm;
				fm.plus_minus_item(stoi(amount), user_name, prod_name, prod_price, false);
				Update upd;
				upd.update_cart_table(cart_table, user_name);
					});
			}
		}
	}

	void update_report_for_day(QCustomPlot* wGraphic, string day)
	{
		FileMananger fm;
		vector <string> report_vector = fm.bought_by_day(day);

		QVector <double> y;
		QVector <double> x;
		for (int i = 0; i < report_vector.size(); i++)
		{
			string data = report_vector[i];
			auto pos = data.find(',');
			string hour = data.substr(0, pos);
			string income = data.substr(pos + 1);
			y.push_back(stod(income));
			x.push_back(stod(hour));
		}
		
		wGraphic->addGraph();

		wGraphic->graph(0)->setData(x, y);
		wGraphic->xAxis->setRange(0, 24);
		wGraphic->xAxis->setLabel("Time(24-hours system)");
		wGraphic->yAxis->setLabel("Income($)");
		wGraphic->yAxis->setRange(0, 500);

		wGraphic->replot();
	}

	void update_report_for_month(QCustomPlot* wGraphic, string month)
	{
		FileMananger fm;
		vector <string> report_vector = fm.bought_by_month(month);

		QVector <double> y;
		QVector <double> x;
		for (int i = 0; i < report_vector.size(); i++)
		{
			string data = report_vector[i];
			auto pos = data.find(',');
			string day = data.substr(0, pos);
			string income = data.substr(pos + 1);
			y.push_back(stod(income));
			x.push_back(stod(day));
		}

		wGraphic->addGraph();

		wGraphic->graph(0)->setData(x, y);
		wGraphic->xAxis->setRange(0, 31);
		wGraphic->xAxis->setLabel("Day");
		wGraphic->yAxis->setLabel("Income($)");
		wGraphic->yAxis->setRange(0, 5000);

		wGraphic->replot();
	}
};

class UI
{
public:
	void ShowLoginScreen()
	{
		QWidget* logInForm = new QWidget();
		QFormLayout* formLayout = new QFormLayout();
		logInForm->setLayout(formLayout);

		QLineEdit* loginEdit = new QLineEdit();
		QLineEdit* passwordEdit = new QLineEdit();
		passwordEdit->setEchoMode(QLineEdit::Password);

		formLayout->addRow("Login:", loginEdit);
		formLayout->addRow("Password:", passwordEdit);

		QPushButton* logInButton = new QPushButton();
		logInButton->setText("Log In");
		formLayout->addRow(logInButton);

		QObject::connect(logInButton, &QPushButton::clicked, [this, loginEdit, passwordEdit, logInForm, formLayout]() {
			QString login = loginEdit->text();
		QString password = passwordEdit->text();
		User user(login, password);
		if (user.isAuthorized())
		{
			logInForm->close();
			string duty = user.WhoisAuthorized();
			if (duty == "Admin")
			{
				ShowMainScreen(login);
			}
			else if (duty == "User")
			{
				ShowUserScreen(login);
			}
			else if (duty == "Accountant")
			{
				ShowAccountantScreen(login);
			}
			else
			{
				QMessageBox::warning(nullptr, "Error", "Check out your current duty");
			}
		}
		else
		{
			QMessageBox::warning(nullptr, "Error", "Incorrect Login or Password");
		}
			});

		logInForm->show();
	}

	void ShowMainScreen(QString login)
	{
		QWidget* main_screen = new QWidget();
		main_screen->size();

		QPushButton* log_out = new QPushButton("Log out");
		QObject::connect(log_out, &QPushButton::released, [main_screen]()
			{
				main_screen->close();
		UI ui;
		ui.ShowLoginScreen();
			});

		QLabel* messageLabel = new QLabel();
		messageLabel->setAlignment(Qt::AlignLeft);
		messageLabel->setText("You are logged in.");

		QLabel* loginLabel = new QLabel();
		loginLabel->setAlignment(Qt::AlignRight);
		loginLabel->setText("Login: " + login);

		QLabel* data_time = new QLabel();
		run(1000, data_time);

		QPushButton* categoriesButton = new QPushButton("Categories");
		QPushButton* usersButton = new QPushButton("Users");
		usersButton->setMaximumSize(50, 30); 
		categoriesButton->setMaximumSize(100, 30);

		QTableWidget* categoriesTableWidget = new QTableWidget(0, 4);
		QTableWidget* productsTableWidget = new QTableWidget(0, 4);
		QTableWidget* usersTable = new QTableWidget(0, 4);
		QPushButton* close = new QPushButton("<-");
		close->setVisible(false);
		close->setMaximumSize(30, 30);
		QObject::connect(close, &QPushButton::clicked, [productsTableWidget, main_screen, close]()
			{
				productsTableWidget->setVisible(false);
				close->setVisible(false);
			});

		QVBoxLayout* menuLayout = new QVBoxLayout();
		menuLayout->addWidget(categoriesButton);
		menuLayout->addSpacing(5);
		menuLayout->addWidget(usersButton);

		QHBoxLayout* headerLayout = new QHBoxLayout();
		headerLayout->addWidget(messageLabel);
		headerLayout->addWidget(loginLabel);
		headerLayout->addWidget(data_time);
		headerLayout->addWidget(log_out);

		QHBoxLayout* layout = new QHBoxLayout();
		layout->addLayout(menuLayout);
		layout->addWidget(categoriesTableWidget);
		layout->addWidget(close);
		layout->addWidget(productsTableWidget);
		layout->addWidget(usersTable);
		layout->setStretch(0, 1);

		categoriesTableWidget->setVisible(false);
		productsTableWidget->setVisible(false);
		usersTable->setVisible(false);

		QVBoxLayout* mainLayout = new QVBoxLayout();
		mainLayout->addLayout(headerLayout);
		mainLayout->addLayout(layout);
		mainLayout->addSpacing(10);
		main_screen->setLayout(mainLayout);

		QObject::connect(categoriesButton, &QPushButton::clicked, [usersTable, layout, categoriesTableWidget, productsTableWidget, close, main_screen]()
			{
				categoriesTableWidget->setVisible(true);
		main_screen->resize(1000, 400);
		usersTable->setVisible(false);
		Update upd;
		upd.Update_Categories_table(categoriesTableWidget, productsTableWidget, close);
			});

		QObject::connect(usersButton, &QPushButton::clicked, [categoriesTableWidget, productsTableWidget, usersTable, main_screen, close]() {
		categoriesTableWidget->setVisible(false);
		productsTableWidget->setVisible(false);
		usersTable->setVisible(true);
		close->setVisible(false);
		main_screen->resize(520, 300);
		Update upd;
		upd.Update_Users_Table(usersTable);
			});

		main_screen->show();
	}

	void ShowUserScreen(QString login)
	{
		QWidget* main_screen = new QWidget();
		main_screen->size();

		QPushButton* log_out = new QPushButton("Log out");
		QObject::connect(log_out, &QPushButton::released, [main_screen]()
			{
				main_screen->close();
		UI ui;
		ui.ShowLoginScreen();
			});

		QLabel* messageLabel = new QLabel();
		messageLabel->setAlignment(Qt::AlignLeft);
		messageLabel->setText("You are logged in.");

		QLabel* loginLabel = new QLabel();
		loginLabel->setAlignment(Qt::AlignRight);
		loginLabel->setText("Login: " + login);

		QLabel* data_time = new QLabel();
		run(1000, data_time);

		QPushButton* categoriesButton = new QPushButton("Categories");
		categoriesButton->setMaximumSize(100, 30);
		QPushButton* cartButton = new QPushButton("Cart");
		cartButton->setMaximumSize(50, 30);

		QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QTableWidget* categoriesTableWidget = new QTableWidget(0, 2);
		categoriesTableWidget->setSizePolicy(policy);
		QTableWidget* productsTableWidget = new QTableWidget(0, 4);
		productsTableWidget->setSizePolicy(policy);
		QTableWidget* cartTableWidget = new QTableWidget(0, 7);
		cartTableWidget->setSizePolicy(policy);
		QPushButton* close = new QPushButton("<-");
		close->setVisible(false);
		close->setMaximumSize(30, 30);
		QObject::connect(close, &QPushButton::clicked, [productsTableWidget, main_screen, close]()
			{
		productsTableWidget->setVisible(false);
		close->setVisible(false);
			});

		QVBoxLayout* menuLayout = new QVBoxLayout();
		menuLayout->addWidget(categoriesButton);
		menuLayout->addWidget(cartButton);
		menuLayout->addSpacing(5);

		QHBoxLayout* headerLayout = new QHBoxLayout();
		headerLayout->addWidget(messageLabel);
		headerLayout->addWidget(loginLabel);
		headerLayout->addWidget(data_time);
		headerLayout->addWidget(log_out);

		QHBoxLayout* layout = new QHBoxLayout();
		layout->addLayout(menuLayout);
		layout->addWidget(categoriesTableWidget);
		layout->addWidget(close);
		layout->addWidget(productsTableWidget);
		layout->addWidget(cartTableWidget);
		layout->setStretch(0, 1);

		categoriesTableWidget->setVisible(false);
		productsTableWidget->setVisible(false);
		cartTableWidget->setVisible(false);

		QHBoxLayout* CRUDLayout = new QHBoxLayout();

		QVBoxLayout* mainLayout = new QVBoxLayout();
		mainLayout->addLayout(headerLayout);
		mainLayout->addLayout(layout);
		mainLayout->addLayout(CRUDLayout);
		mainLayout->addSpacing(10);
		main_screen->setLayout(mainLayout);

		QObject::connect(categoriesButton, &QPushButton::clicked, [cartTableWidget, layout, categoriesTableWidget, productsTableWidget, login, close, main_screen]()
			{
				main_screen->resize(1000, 400);
		categoriesTableWidget->setVisible(true);
		Update upd;
		upd.Update_Categories_table_user(categoriesTableWidget, productsTableWidget, login, close);
		cartTableWidget->setVisible(false);
			});

		QObject::connect(cartButton, &QPushButton::clicked, [categoriesTableWidget, productsTableWidget, login, cartTableWidget, main_screen, close]()
			{
				close->setVisible(false);
				main_screen->resize(520, 300);
				categoriesTableWidget->setVisible(false);
				productsTableWidget->setVisible(false);
				Update upd;
				upd.update_cart_table(cartTableWidget, login.toStdString());
			});

		main_screen->show();
	}

	void ShowAccountantScreen(QString login)
	{
		QWidget* main_screen = new QWidget();
		main_screen->size();

		QLabel* messageLabel = new QLabel();
		messageLabel->setAlignment(Qt::AlignLeft);
		messageLabel->setText("You are logged in.");

		QPushButton* log_out = new QPushButton("Log out");
		QObject::connect(log_out, &QPushButton::released, [main_screen]()
			{
				main_screen->close();
				UI ui;
				ui.ShowLoginScreen();
			});

		QLabel* loginLabel = new QLabel();
		loginLabel->setAlignment(Qt::AlignCenter);
		loginLabel->setText("Login: " + login);

		QLabel* data_time = new QLabel();
		data_time->setAlignment(Qt::AlignRight);
		run(1000, data_time);

		QPushButton* report = new QPushButton("Report");
		QPushButton* report_by_day = new QPushButton("Report for current day");
		QPushButton* report_by_month = new QPushButton("Report for current month");
		report_by_day->setVisible(false);
		report_by_month->setVisible(false);
		QPushButton* close = new QPushButton("<-");
		close->setVisible(false);

		QHBoxLayout* headerLayout = new QHBoxLayout();
		headerLayout->addWidget(messageLabel);
		headerLayout->addWidget(loginLabel);
		headerLayout->addWidget(data_time);
		headerLayout->addWidget(log_out);

		QVBoxLayout* main_layout = new QVBoxLayout();
		main_layout->addLayout(headerLayout);
		main_layout->addWidget(close);
		main_layout->addWidget(report);
		main_layout->addWidget(report_by_day);
		main_layout->addWidget(report_by_month);

		QObject::connect(report, &QPushButton::clicked, [report, report_by_day, close, report_by_month]()
			{
		report->setVisible(false);
		report_by_day->setVisible(true);
		report_by_month->setVisible(true);
		close->setVisible(true);

		QObject::connect(close, &QPushButton::clicked, [report, report_by_day, close, report_by_month]()
			{
		report->setVisible(true);
		report_by_day->setVisible(false);
		report_by_month->setVisible(false);
		close->setVisible(false);
			});

		QObject::connect(report_by_day, &QPushButton::clicked, [report, report_by_day, close]()
		{
			DataTime dt;
			string day = dt.currentData();
			
			QCustomPlot* wGraphic = new QCustomPlot();

			Update upd;
			upd.update_report_for_day(wGraphic, day);

			QWidget* report_day_widget = new QWidget();
			report_day_widget->setMinimumSize(500,500);
			QVBoxLayout* main_layout = new QVBoxLayout();
			QHBoxLayout* header_layout = new QHBoxLayout();
			QPushButton* export_ = new QPushButton("Export to CSV file");
			QPushButton* day_back = new QPushButton("<");
			header_layout->addWidget(day_back);
			day_back->setStyleSheet("text-align: left;");
			day_back->setFixedSize(20, 20);
			QLabel* day_label = new QLabel(QString::fromStdString(dt.currentData()));
			header_layout->addWidget(day_label);
			day_label->setStyleSheet("text-align: center;");
			QPushButton* day_forward = new QPushButton(">");
			header_layout->addWidget(day_forward);
			day_forward->setStyleSheet("text-align: right;");
			day_forward->setFixedSize(20, 20);

			main_layout->addLayout(header_layout);
			main_layout->addWidget(wGraphic);
			main_layout->addWidget(export_);
			
			report_day_widget->setLayout(main_layout);
			report_day_widget->show();

			QObject::connect(export_, &QPushButton::clicked, [report, report_by_day, export_]()
				{
					DataTime d_t;
			string date = d_t.currentData();
			QString default_name = QString::fromStdString("report(" + date + ")");
			QString fileName = QFileDialog::getSaveFileName(nullptr, "Document creation", default_name, "Text files (*.txt);;CSV Files (*.csv)");

			if (!fileName.isEmpty())
			{
				string file_name = fileName.toStdString();
				for (int i = 0; i < file_name.size(); i++) {
					if (file_name[i] == '/')
					{
						file_name.replace(i, 1, "\\");
						i++;
					}
				}
				ofstream file(file_name);

				DataTime dt;
				FileMananger fm;
				string day = dt.currentDay();
				int day_i = stoi(day);
				day = to_string(day_i);
				vector <string> report = fm.bought_by_day(day);

				for (int i = 0; i < report.size(); i++)
				{
					file << report[i] << "\n";
				}
			}
				});

			QObject::connect(day_back, &QPushButton::clicked, [report, report_by_day, export_, day_label, wGraphic]()
				{
			string str = day_label->text().toStdString();
			std::tm tm = {}; 
			std::istringstream ss(str);
			ss >> std::get_time(&tm, "%d-%m-%Y");
			DataTime dt;
			string day = dt.getDateBefore(tm);
			Update upd;
			upd.update_report_for_day(wGraphic, day);
			QString newDate = QString::fromStdString(day);
			day_label->setText(newDate);
				});

			QObject::connect(day_forward, &QPushButton::clicked, [report, report_by_day, export_, day_label, wGraphic]()
				{
			string str = day_label->text().toStdString();
			std::tm tm = {};
			std::istringstream ss(str);
			ss >> std::get_time(&tm, "%d-%m-%Y");
			DataTime dt;
			string day = dt.getDateAfter(tm);
			Update upd;
			upd.update_report_for_day(wGraphic, day);
			QString newDate = QString::fromStdString(day);
			day_label->setText(newDate);
				});
			});

		QObject::connect(report_by_month, &QPushButton::clicked, [report, report_by_day, close]()
			{
		DataTime dt;
		string month = dt.currentMonth_and_Year();

		QCustomPlot* wGraphic = new QCustomPlot();

		Update upd;
		upd.update_report_for_month(wGraphic, month);

		QWidget* report_month_widget = new QWidget();
		report_month_widget->setMinimumSize(500, 500);
		QVBoxLayout* main_layout = new QVBoxLayout();
		QHBoxLayout* header_layout = new QHBoxLayout();
		QPushButton* export_ = new QPushButton("Export to CSV file");
		QPushButton* month_back = new QPushButton("<");
		header_layout->addWidget(month_back);
		month_back->setStyleSheet("text-align: left;");
		month_back->setFixedSize(20, 20);
		QLabel* month_label = new QLabel(QString::fromStdString(dt.currentMonth_and_Year()));
		header_layout->addWidget(month_label);
		month_label->setStyleSheet("text-align: center;");
		QPushButton* month_forward = new QPushButton(">");
		header_layout->addWidget(month_forward);
		month_forward->setStyleSheet("text-align: right;");
		month_forward->setFixedSize(20, 20);

		main_layout->addLayout(header_layout);
		main_layout->addWidget(wGraphic);
		main_layout->addWidget(export_);

		report_month_widget->setLayout(main_layout);
		report_month_widget->show();

		QObject::connect(export_, &QPushButton::clicked, [report, report_by_day, export_]()
			{
				DataTime d_t;
		string date = d_t.currentMonth_and_Year();
		QString default_name = QString::fromStdString("report(" + date + ")");
		QString fileName = QFileDialog::getSaveFileName(nullptr, "Document creation", default_name, "Text files (*.txt);;CSV Files (*.csv)");

		if (!fileName.isEmpty())
		{
			string file_name = fileName.toStdString();
			for (int i = 0; i < file_name.size(); i++) {
				if (file_name[i] == '/')
				{
					file_name.replace(i, 1, "\\");
					i++;
				}
			}
			ofstream file(file_name);

			DataTime dt;
			FileMananger fm;
			string month = dt.currentMonth_and_Year();
			vector <string> report = fm.bought_by_month(month);

			for (int i = 0; i < report.size(); i++)
			{
				file << report[i] << "\n";
			}
		}
			});

		QObject::connect(month_back, &QPushButton::clicked, [report, report_by_day, export_, month_label, wGraphic]()
			{
		string str = month_label->text().toStdString();
		DataTime dt;
		string month = dt.get_previous_month(str);
		Update upd;
		upd.update_report_for_month(wGraphic, month);
		QString newDate = QString::fromStdString(month);
		month_label->setText(newDate);
			});

		QObject::connect(month_forward, &QPushButton::clicked, [report, report_by_day, export_, month_label, wGraphic]()
			{
		string str = month_label->text().toStdString();
		DataTime dt;
		string month = dt.get_next_month(str);
		Update upd;
		upd.update_report_for_month(wGraphic, month);
		QString newDate = QString::fromStdString(month);
		month_label->setText(newDate);
			});
			});
		});

		main_screen->setLayout(main_layout);
		main_screen->show();
	}
};

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	UI ui;
	ui.ShowLoginScreen();

	return a.exec();

}

void run(int msec, QLabel* dt) {
	auto endless = [=]() {
		while (true) {
			DataTime dt_c;
			dt->setText(QString::fromStdString(dt_c.currentDateTime()));
			std::this_thread::sleep_for(std::chrono::milliseconds(msec));
		}
	};

	std::thread thread(endless);

	thread.detach();
}