#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <map>
using namespace std;

struct User { //All user data
    string username;
    string password;
    string name;
    int age;
    double height;
    double weight;
    string activityLvl;
    int dailykcalGoal;
    int kcalRemaining;
    string lastResetDate;
};

struct Meals { //Meal data
    string food;
    int calories;
    string mealType;
    string date;
};

struct Vitals { //Vitals
    int RHR;
    int o2Lvl;
    string bloodPressure;
    string date;
};

//Function declarations
void startUI();
void signup();
void saveAcc(const User& user);
void login();
bool verifyAcc(const string& username, const string& password);
void accManager(const string& username);
void updateDetails(const string& username);
void mealLog(const string& username);
void saveMeal(const string& username, const Meals& meal);
void vitalLog(const string& username);
void saveVitals(const string& username, const Vitals& vital);
void deleteAccount(const string& username);
vector<Meals> loadMealData(const string& username);
vector<Vitals> loadVitalsData(const string& username);
void checkImprovements(const string& username);
void HRDifference(const vector<Vitals>& vitals);
void O2Difference(const vector<Vitals>& vitals);
void BPDifference(const vector<Vitals>& vitals);
bool updateDay(const string& username);
User loadData(const string& username); 
string getCurrentDate();
void WeeklyAverages(const string& username);
void MealWeeklyAverages(const string& username);
void VitalWeeklyAverages(const string& username);
string getWeekFromDate(const string& date);

int main() { //Runs the log in start UI
    startUI();
    return 0;
}

void startUI() { //Main UI
    int choice;

    do {
        cout << "\n----WELCOME TO MEDTRACK----\n";
        cout << "1. Log-in\n";
        cout << "2. Sign-up\n";
        cout << "3. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if(!cin.fail()){
            switch(choice){
            case 1:
                login();
                break;
            case 2:
                signup();
                break;
            case 3:
                cout << "Thank you for using MedTrack!\n";
                break;
            default:
                cout << "Invalid input.\n";
          }
        } else {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input! Please choose a number from 1 - 3.\n";
        }

    }while(choice != 3);
}

void signup() { //Sign-up UI
    User newUser;

    cout << "\n----SIGN UP----\n";
    cout << "\nEnter username: ";
    getline(cin, newUser.username);
    
    //Turns username into filename
    string filename = "users/" + newUser.username + ".csv";

    //check if file exists
    ifstream file(filename);
    if(file.is_open()) {
        cout << "Username already exists. Log-in or create a new username.\n";
        file.close();
        return;
    }
    file.close();

    cout << "\nEnter password: ";
    getline(cin, newUser.password);

    cout << "\nEnter name: ";
    getline(cin, newUser.name);

    cout << "\nEnter age: ";
    cin >> newUser.age;
    while(cin.fail() || newUser.age <= 0) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Age must exceed 0! Enter a valid age: ";
        cin >> newUser.age;
    }
    cin.ignore();

    cout << "\nEnter height(in cm): ";
    cin >> newUser.height;
    while(cin.fail() || newUser.height <= 0) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Height must exceed 0! Enter a valid height: ";
        cin >> newUser.height;
    }
    cin.ignore();
    
    cout << "\nEnter weight(in kg): ";
    cin >> newUser.weight;
    while(cin.fail() || newUser.weight <= 0) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Weight must exceed 0! Enter a valid weight: ";
        cin >> newUser.weight;
    }
    cin.ignore();

    cout << "\nEnter activity level(sedentary/light/moderate/active): ";
    getline(cin, newUser.activityLvl);

    cout << "\nEnter daily calorie goal: ";
    cin >> newUser.dailykcalGoal;
    while(cin.fail() || newUser.dailykcalGoal <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Calorie goal must exceed 0! Enter a valid calorie goal: ";
        cin >> newUser.dailykcalGoal;
    }
    cin.ignore();

    newUser.kcalRemaining = newUser.dailykcalGoal;
    newUser.lastResetDate = getCurrentDate();

    saveAcc(newUser);
    cout << "\nAccount created successfully!\n";
} 

void saveAcc(const User& user) { //Creates account file containing details
    filesystem::create_directory("users"); //Creates directory in current directory of the project file
    string filename = "users/" + user.username + ".csv";
    ofstream file(filename); //Inputs values into the file
        if(file.is_open()) {
            file << "username" << "," << user.username << "," << "\n"
                 << "password" << "," << user.password << "," << "\n"
                 << "name" << "," << user.name << "," << "\n"
                 << "age" << "," << user.age << "," << "\n"
                 << "height" << "," << user.height << "," << "\n"
                 << "weight" << "," << user.weight << "," << "\n"
                 << "activity level" << "," << user.activityLvl << "," << "\n"
                 << "daily calorie goal" << "," << user.dailykcalGoal << "," << "\n"
                 << "remaining calories" << "," << user.kcalRemaining << "," << "\n"
                 << "last reset date" << "," << user.lastResetDate << "," << "\n";
            file.close();
        }
}

void login() { //Gets log in details
    string username, password;

    cout << "\n----LOG IN----\n";
    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    if(verifyAcc(username, password)) {
        cout << "\nLogged in succesfully!\n";
        updateDay(username);
        accManager(username);
    } else {
        cout << "\nInvalid username or password.\n";
    }
}

bool verifyAcc(const string& username, const string& password) { //Checks if file exists and details are correct
    string filename = "users/" + username + ".csv"; //Turns username into filename
    ifstream file(filename);
    if(!file.is_open()) { //Checks if file exists
        return false;
    }

    string line;
    string savedPassword;

    while(getline(file, line)) { // Reads file line by line, stores in 'line' variable
        stringstream ss(line);
        string label, value;

        getline(ss, label, ','); //Reads until first comma, stores as labe
        getline(ss, value, ','); //Reads until the second comma, stores as value

        if(label == "password") { //If the label is password, gets value
            savedPassword = value;
            break;
        }
    }
    
    file.close();
    if (savedPassword == password) { //Compares the saved password from inputted password
        return true;
    }
    else {
        return false;
    }
}

void accManager(const string& username) { //Shows the actions the user can take
    int choice;

     do {
        cout << "\n----ACCOUNT MANAGEMENT----\n";
        cout << "1. Update personal details\n";
        cout << "2. Input meals\n";
        cout << "3. Input vitals\n";
        cout << "4. Check improvements\n";
        cout << "5. Delete account\n";
        cout << "6. Log out\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (!cin.fail()){
            switch(choice) {
                case 1:
                    updateDetails(username);
                    break;
                case 2:
                    mealLog(username);
                    break;
                case 3:
                    vitalLog(username);
                    break;
                case 4:
                    checkImprovements(username);
                    break;
                case 5:
                    deleteAccount(username);
                    return;
                case 6:
                    cout << "\nLogged out successfully!\n";
                    break;
                default:
                    cout << "\nInvalid choice. Try again.\n";
                }
        } else {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input! Please choose a number from 1 - 3.\n";
        }
    }while(choice != 6);
}

User loadData(const string& username) { //Reads file and stores the user datainto a structure
    User user; 
    string filename = "users/" + username + ".csv";
    ifstream file(filename);
    if(file.is_open()) {
        string line;
        while(getline(file, line)) { //Looping to read every line in csv file
            stringstream ss(line);
            string label, value;
            
            getline(ss, label, ',');
            getline(ss, value, ',');    

                if(label == "username") { //Storing of values in each field
                    user.username = value;}
                else if(label ==  "password") {
                    user.password = value;}
                else if(label ==  "name") {
                    user.name = value;}
                else if(label ==  "age") {
                    user.age = stoi(value);} 
                else if(label ==  "height") {
                    user.height = stod(value);}  
                else if(label ==  "weight") {
                    user.weight = stod(value);}
                else if(label ==  "activity level") {
                    user.activityLvl = value;}
                else if(label ==  "daily calorie goal") {
                    user.dailykcalGoal = stoi(value);}
                else if(label ==  "remaining calories") {
                    user.kcalRemaining = stoi(value);}
                else if(label ==  "last reset date") {
                    user.lastResetDate = value;}
        }
        file.close();
    }
    return user;
}

void updateDetails(const string& username) { //Reads user details file and allows the user to update it
    User user = loadData(username);
    int choice;
    
    do { 
        cout << "\n----UPDATE PERSONAL DETAILS----\n";
        cout << "1. Update name\n";
        cout << "2. Update age\n";
        cout << "3. Update height\n";
        cout << "4. Update weight\n";
        cout << "5. Update activity level\n";
        cout << "6. Update daily calorie goal\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (!cin.fail()){
            switch(choice) { //Break; means that user can only update one at a time
                case 1:
                    cout << "Enter new name: ";
                    getline(cin, user.name);
                    break;
                case 2:
                    cout << "\nEnter new age: ";
                    cin >> user.age;
                    while(cin.fail() || user.age <= 0) {
                        cin.clear();
                        cin.ignore(100, '\n');
                        cout << "Age must exceed 0! Enter a valid age: ";
                        cin >> user.age;
                    }
                    cin.ignore();
                    break;
                case 3:
                    cout << "\nEnter new height(in cm): ";
                    cin >> user.height;
                    while(cin.fail() || user.height <= 0) {
                        cin.clear();
                        cin.ignore(100, '\n');
                        cout << "Height must exceed 0! Enter a valid height: ";
                        cin >> user.height;
                    }
                    cin.ignore();
                    break;
                case 4:
                    cout << "Enter new weight (kg): ";
                    cin >> user.weight;
                    while(cin.fail() || user.weight <= 0) {
                        cin.clear();
                        cin.ignore(100, '\n');
                        cout << "Weight must exceed 0! Enter a valid weight: ";
                        cin >> user.weight;
                    }
                    cin.ignore();
                    break;
                case 5:
                    cout << "Enter new activity level: ";
                    getline(cin, user.activityLvl);
                    break;
                case 6:
                    cout << "Enter new daily calorie goal: ";
                    cin >> user.dailykcalGoal;
                    while(cin.fail() || user.dailykcalGoal <= 0) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Calorie goal must exceed 0! Enter a valid calorie goal: ";
                        cin >> user.dailykcalGoal;
                    }
                    user.kcalRemaining = user.dailykcalGoal;
                    cin.ignore();
                    break;
                case 7:
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } else {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input! Please choose a number from 1 - 3.\n";
        }
        
        if(choice >= 1 && choice <= 6) { //Saving details if user chose to update any detail
            saveAcc(user);
            cout << "Details updated successfully!\n";
        }
    } while(choice != 7);
}

string getCurrentDate() { //Function to return date in DD/MM/YYYY form
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    stringstream ss;
    ss << setw(2) << setfill('0') << ltm->tm_mday << "/"
       << setw(2) << setfill('0') << 1 + ltm->tm_mon << "/"
       << 1900 + ltm->tm_year;
    
    return ss.str();
}

void mealLog(const string& username) { // Gets meal info from user and calls the save meals function
    Meals meal;

    cout << "\n----MEALS----\n";
    cout << "Enter food: ";
    getline(cin, meal.food);

    while (1) {
        cout << "Enter calories: ";
        cin >> meal.calories;

        if (cin.fail() || meal.calories < 0) {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input!\n";
        } else {
            cin.ignore(100, '\n');
            break;
        }
    }

    cout << "Enter meal type(Breakfast/Lunch/Dinner/Snack): ";
    getline(cin, meal.mealType);

    meal.date = getCurrentDate();

    saveMeal(username, meal);

    User user = loadData(username);
    updateDay(username); //checks if day is diferent
    user = loadData(username);

    user.kcalRemaining -= meal.calories; //displays remaining calories, and if it's already exceeded
    if(user.kcalRemaining < 0) {
        cout << "Warning! You've exceeded your daily calorie goal by " 
             << abs(user.kcalRemaining) << " calories.\n";}
    saveAcc(user); 
    cout << "Meal logged successfully!\n";
    cout << "Remaining calories for today: " << user.kcalRemaining << endl;
}

void saveMeal(const string& username, const Meals& meal) { //Inserts meals into a new csv file
    string filename = "users/" + username + "_meals.csv";
    ofstream mealFile(filename, ios::app);
    if(!mealFile.is_open()) {  //If can't open file with the username
        cerr << "Error: Cannot open meal file for " << username << endl;
        return;
    }
    mealFile.seekp(0, ios::end); //Checks for header labels, puts one if there's none
    if(mealFile.tellp() == 0) {
        mealFile << "Date" << "," << "Food" << "," << "Calories" << "," << "MealType" << "\n";
    }
    mealFile << meal.date << ","
             << meal.food << ","
             << meal.calories << ","
             << meal.mealType << "\n";
    mealFile.close();
}

void vitalLog(const string& username) { //Asks for user vitals one by one and calls save vitals function
    int choice;
    Vitals vital;
    vital.date = getCurrentDate();
    
    do {
        cout << "\n----VITALS----\n";
        cout << "1. Log resting heart rate\n";
        cout << "2. Log O2 levels\n";
        cout << "3. Log blood pressure\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        
        Vitals vital; 
        vital.date = getCurrentDate();
        vital.RHR = 0;     
        vital.o2Lvl = 0;     
        vital.bloodPressure = "";

        if (!cin.fail()){
            switch(choice) {
                case 1:
                    while (1) {
                        cout << "Enter resting heart rate (bpm): ";
                        cin >> vital.RHR;

                        if (cin.fail() || vital.RHR < 0) {
                            cin.clear();
                            cin.ignore(100, '\n');
                            cout << "Invalid input!\n";
                        } else {
                            cin.ignore(100, '\n');
                            break;
                        }
                    }
                    saveVitals(username, vital);
                    cout << "Resting heart rate logged successfully!\n";
                    break;
                case 2:
                    while (1) {
                        cout << "Enter O2 level (%): ";
                        cin >> vital.o2Lvl;

                        if (cin.fail() || vital.o2Lvl < 0) {
                            cin.clear();
                            cin.ignore(100, '\n');
                            cout << "Invalid input!\n";
                        } else {
                            cin.ignore(100, '\n');
                            break;
                        }
                    }
                    saveVitals(username, vital);
                    cout << "O2 level logged successfully!\n";
                    break;
                case 3:
                    cout << "Enter blood pressure (e.g., 120/80): ";
                    getline(cin, vital.bloodPressure);
                    saveVitals(username, vital);
                    cout << "Blood pressure logged successfully!\n";
                    break;
                case 4:
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } else {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input! Please choose a number from 1 - 3.\n";
        }
    } while(choice != 4);
}

void saveVitals(const string& username, const Vitals& newVital) { //Saves vitals info into csv file
    filesystem::create_directory("users");
    
    string filename = "users/" + username + "_vitals.csv";
    
    vector<Vitals> allVitals = loadVitalsData(username);
    
    bool foundToday = false; // Check if we already have an entry for today
    for(auto& vital : allVitals) {
        if(vital.date == newVital.date) { // Update existing entry with new data
            if(newVital.RHR != 0) vital.RHR = newVital.RHR;
            if(newVital.o2Lvl != 0) vital.o2Lvl = newVital.o2Lvl;
            if(!newVital.bloodPressure.empty()) vital.bloodPressure = newVital.bloodPressure;
            foundToday = true;
            break;
        }
    }
   
    if(!foundToday) {  // If no entry for today, add the new one
        allVitals.push_back(newVital);
    }
    
    ofstream vitalFile(filename); // Rewrite the entire file with updated data
    if(!vitalFile.is_open()) {  
        cerr << "Error: Cannot open vitals file for " << username << "\n";
        return;
    }
    
    vitalFile << "Date" << "," << "RHR" << "," << "O2 Lvl" << "," << "BP" << "\n";
    
    for(const auto& vital : allVitals) { //Outputs all vitals in one line
        vitalFile << vital.date << ","
                 << vital.RHR << ","
                 << vital.o2Lvl << ","
                 << vital.bloodPressure << "\n";
    }
    
    vitalFile.close();
}
void deleteAccount(const string& username) { //deletes user account if they wish
    char input;
    string password;
    
    cout << "\n----DELETE ACCOUNT----\n";
    cout << "Are you sure you want to delete this account? (Y/N): ";
    cin >> input;
    cin.ignore();
    
    if(input == 'Y' || input == 'y') { //Asks for passw. if user inputs Y/y
        cout << "Enter password to confirm: ";
        getline(cin, password);
        
        if(verifyAcc(username, password)) { //If password is correct, deletes all files
            string userFile = "users/" + username + ".csv";
            string mealFile = "users/" + username + "_meals.csv";
            string vitalFile = "users/" + username + "_vitals.csv";
            
            remove(userFile.c_str());
            remove(mealFile.c_str());
            remove(vitalFile.c_str());
            
            cout << "Account deleted successfully!\n";
        } else {
            cout << "Incorrect password! Account deletion cancelled.\n";
        }
    } else {
        cout << "Account deletion cancelled.\n";
    }
}

vector<Meals> loadMealData(const string& username) { //Reads meal file and stores value in a vector
    vector<Meals> meals;
    string filename = "users/" + username + "_meals.csv";
    ifstream file(filename);
    
    if(file.is_open()) {
        string line;
        getline(file, line);
        
        while(getline(file, line)) { //Reading line by line
            stringstream ss(line);
            string item;
            Meals meal;
            
            getline(ss, meal.date, ',');
            getline(ss, meal.food, ',');
            
            getline(ss, item, ',');
            if(!item.empty()) {
                meal.calories = stoi(item);
            } else {
                meal.calories = 0;
            }
            
            getline(ss, meal.mealType, ',');
            
            meals.push_back(meal); //Stores structure in a vector
        }
        file.close();
    }
    return meals;
}

vector<Vitals> loadVitalsData(const string& username) { //Reads vitals file and stores it in vector(Similar to loadMealData)
    vector<Vitals> vitals;
    string filename = "users/" + username + "_vitals.csv";
    ifstream file(filename);
    
    if(file.is_open()) {
        string line;
        getline(file, line);
        
        while(getline(file, line)) {
            stringstream ss(line);
            string item;
            Vitals vital;
            
            getline(ss, vital.date, ',');
            
            getline(ss, item, ',');
            vital.RHR = item.empty() ? 0 : stoi(item);
            
            getline(ss, item, ',');
            vital.o2Lvl = item.empty() ? 0 : stoi(item);
            
            getline(ss, vital.bloodPressure, ',');
            
            vitals.push_back(vital);
        }
        file.close();
    }
    return vitals;
}

void checkImprovements(const string& username) { //Displays options for checking improvements, calls functions
    vector<Vitals> vitals = loadVitalsData(username);
    vector<Meals> meals = loadMealData(username);
    
    if(vitals.empty() && meals.empty()) {
        cout << "\nNo data available for analysis.\n";
        return;
    }
    
    int choice;
    do {
        cout << "\n----CHECK IMPROVEMENTS----\n";
        cout << "1. Show RHR difference\n";
        cout << "2. Show O2 lvl difference\n";
        cout << "3. Show BP difference\n";
        cout << "4. Show weekly averages\n";
        cout << "5. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        if (!cin.fail()){
                switch(choice) {
                case 1:
                    HRDifference(vitals);
                    break;
                case 2:
                    O2Difference(vitals);
                    break;
                case 3:
                    BPDifference(vitals);
                    break;
                case 4:
                    WeeklyAverages(username);
                    break;
                case 5:
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } else {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input! Please choose a number from 1 - 3.\n";
        }
    } while(choice != 5);
}

void HRDifference(const vector<Vitals>& vitals) { //Displays heart rate difference from first log
    if(vitals.size() < 2) { //checks for content
        cout << "Need at least 2 readings to show difference.\n";
        return;
    }
    Vitals first = vitals.front(); //Assigning the first log and the last log 
    Vitals last = vitals.back();
    int difference = last.RHR - first.RHR; //Difference
    
    cout << "\n----RESTING HEART RATE DIFFERENCE----\n";
    cout << "First reading (" << first.date << "): " << first.RHR << " bpm\n";
    cout << "Last reading (" << last.date << "): " << last.RHR << " bpm\n";
    cout << "Difference: " << (difference > 0 ? "+" : "") << difference << " bpm\n"; //shows if positive or negative change
}

void O2Difference(const vector<Vitals>& vitals) { //Shows O2 levels difference like in RHR difference
    if(vitals.size() < 2) {
        cout << "Need at least 2 readings to show difference.\n";
        return;
    }
    Vitals first = vitals.front();
    Vitals last = vitals.back();
    int difference = last.o2Lvl - first.o2Lvl;
    
    cout << "\n----O2 LEVEL DIFFERENCE----\n";
    cout << "First reading (" << first.date << "): " << first.o2Lvl << "%\n";
    cout << "Last reading (" << last.date << "): " << last.o2Lvl << "%\n";
    cout << "Difference: " << (difference > 0 ? "+" : "") << difference << "%\n";
}

void BPDifference(const vector<Vitals>& vitals) { //Shows Blood pressure difference like in RHR difference
    vector<Vitals> bpReadings;
    for(const auto& v : vitals) { //Checks for value since bp is a string type, not like O2 and RHR which have default of 0(int)
        if(!v.bloodPressure.empty()) {
            bpReadings.push_back(v);
        }
    }
    
    if(bpReadings.size() < 2) {
        cout << "Need at least 2 blood pressure readings to show difference.\n";
        return;
    }
    
    Vitals first = bpReadings.front();
    Vitals last = bpReadings.back();
    
    cout << "\n----BLOOD PRESSURE DIFFERENCE----\n";
    cout << "First reading (" << first.date << "): " << first.bloodPressure << "\n";
    cout << "Last reading (" << last.date << "): " << last.bloodPressure << "\n";
}

bool updateDay(const string& username) { //CHecks if date changed
    User user = loadData(username);
    string today = getCurrentDate();
    bool dayChanged = false;
    
    if (user.lastResetDate != today) {
        user.kcalRemaining = user.dailykcalGoal;
        user.lastResetDate = today;
        saveAcc(user);
        dayChanged = true;
    }
    
    return dayChanged; //Returns true or false
}

void WeeklyAverages(const string& username) { //Displays choices for displaying weekly average
    int choice;
    do {
        cout << "\n----WEEKLY AVERAGES----\n";
        cout << "1. Meal averages\n";
        cout << "2. Vital averages\n";
        cout << "3. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        if (!cin.fail()){
            switch(choice) {
                case 1:
                    MealWeeklyAverages(username);
                    break;
                case 2:
                    VitalWeeklyAverages(username);
                    break;
                case 3:
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } else {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Invalid input! Please choose a number from 1 - 3.\n";
        }
    } while(choice != 3);
}

string getWeekFromDate(const string& date) { //Returns week string
    int year, month, day;
    char slash;
    stringstream ss(date); //Reads date in the arguments
    if (!(ss >> day >> slash >> month >> slash >> year)) {
        return "Unknown-Week";
    }
    
    if (month < 1 || month > 12 || day < 1 || day > 31 || year < 1900) {
        return "Invalid-Date";
    }

    tm timeinfo = {}; //Stores years and dates in a structure
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    mktime(&timeinfo); //FUnction to calculate information

    int week = (timeinfo.tm_yday / 7) + 1;
    if (week < 1) week = 1;
    if (week > 53) week = 53;
    
    stringstream weekSS;
    weekSS << year << "-W" << setw(2) << setfill('0') << week;
    return weekSS.str();
}

void MealWeeklyAverages(const string& username) { //Calculates and displays weekly average of meals
    vector<Meals> meals = loadMealData(username);
    
    if(meals.empty()) { //Checks for content
        cout << "\nNo meal data available.\n";
        return;
    }

    map<string, vector<int>> weeklyCalories; //Maps values into weeks (Key: week string)
    
    for(const auto& meal : meals) { //Storing calories in a vector for each week
        string week = getWeekFromDate(meal.date);
        weeklyCalories[week].push_back(meal.calories); 
    }
    
    cout << "\n----WEEKLY MEAL AVERAGES----\n";
    cout << "Week\t\tAvg Calories\tMeal Count\n";
    
    for(const auto& week : weeklyCalories) { //Computes average for each week
        double avgCalories = 0;
        for(int cal : week.second) {
            avgCalories += cal;
        }
        avgCalories /= week.second.size();
        
        cout << week.first << "\t" << fixed << setprecision(1) << avgCalories << " kcal\t" 
             << week.second.size() << " meals\n";
    }
}

void VitalWeeklyAverages(const string& username) { //Calculates and displays weekly average for vitals
    vector<Vitals> vitals = loadVitalsData(username); //Calls to read file
    
    if(vitals.empty()) { //Check for content
        cout << "\nNo vital data available.\n";
        return;
    }
    
    map<string, vector<int>> weeklyRHR; //Maps weeks into vector (Key: week string)
    map<string, vector<int>> weeklyO2;
    map<string, vector<string>> weeklyBP;
    
    for(const auto& vital : vitals) { //Sorts data by week into vectors mapped
        string week = getWeekFromDate(vital.date);
        if(!vital.RHR == 0) {
            weeklyRHR[week].push_back(vital.RHR);
        }
        if(!vital.o2Lvl == 0) {
            weeklyO2[week].push_back(vital.o2Lvl);
        }
        if(!vital.bloodPressure.empty()) { 
            weeklyBP[week].push_back(vital.bloodPressure);
        }
    }
    
    cout << "\n----WEEKLY VITAL AVERAGES----\n";
    cout << "Week\t\tAvg RHR\tAvg O2\tBP Readings\n";
    
    for(const auto& week : weeklyRHR) { //Calculating weekly average
        double avgRHR = 0, avgO2 = 0;
        
        for(int rhr : week.second) {
            avgRHR += rhr;
        }
        avgRHR /= week.second.size();
       
        if(weeklyO2.find(week.first) != weeklyO2.end()) {
            for(int o2 : weeklyO2[week.first]) {
                avgO2 += o2;
            }
            avgO2 /= weeklyO2[week.first].size();
        }
        
    
        int bpCount = 0;
        if(weeklyBP.find(week.first) != weeklyBP.end()) { // Count BP readings
            bpCount = weeklyBP[week.first].size();
        }
        
        cout << week.first << "\t" << fixed << setprecision(1) << avgRHR << " bpm\t"
             << (avgO2 > 0 ? to_string((int)avgO2) + "%" : "N/A") << "\t"
             << bpCount << " readings\n";
    }
}
